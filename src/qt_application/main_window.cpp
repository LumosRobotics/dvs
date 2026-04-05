#include "main_window.h"

#include <QCloseEvent>
#include <QFileInfo>
#include <QMessageBox>
#include <cstring>
#include <fstream>
#include <nlohmann/json.hpp>

#include <QFile>
#include <QIcon>

#include "lumos/plotting/enumerations.h"
#include "lumos/logging/logging.h"
#include "platform_paths.h"
#include "plot_objects/plot_object_base/plot_object_base.h"
#include "plot_objects/plot_objects.h"
#include "serial_interface/raw_data_frame.h"

using namespace lumos::internal;

// Forward declaration for data conversion function
std::shared_ptr<const ConvertedDataBase> convertPlotObjectData(const CommunicationHeader& hdr,
                                                               const ReceivedData& received_data,
                                                               const PlotObjectAttributes& attributes,
                                                               const UserSuppliedProperties& user_supplied_properties);

MainWindow::MainWindow(const std::vector<std::string>& cmdl_args)
    : QMainWindow(nullptr),
      settings_(std::make_unique<SettingsHandler>("Duoplot")),
      data_receiver_(),
      serial_interface_(nullptr),
      tcp_receive_thread_(nullptr),
      receive_timer_(nullptr),
      refresh_timer_(nullptr),
      current_window_num_(0),
      window_callback_id_(0),
      shutdown_in_progress_(false),
      open_project_file_queued_(false),
      new_window_queued_(false),
      current_element_name_(""),
      window_initialization_in_progress_(false)
{
    // Load settings
    settings_->loadSettings();

    // Hide this window - it's just a data router
    hide();

    // Setup callbacks
    setupCallbacks();

    // Setup timers
    setupTimers();

    // Create default project
    ProjectSettings project_settings;

    // Create one default window with one tab
    WindowSettings window_settings;
    window_settings.name = "main";
    window_settings.x = 100;
    window_settings.y = 100;
    window_settings.width = 1200;
    window_settings.height = 800;

    TabSettings tab_settings;
    tab_settings.name = "tab0";
    window_settings.tabs.push_back(tab_settings);

    project_settings.pushBackWindowSettings(window_settings);

    // Setup windows
    setupWindows(project_settings);

    // Setup tray icon
    tray_icon_ = new TrayIcon(this);
    tray_icon_->setOnFileNew([this]()  { newProject(); });
    tray_icon_->setOnFileOpen([this]() {
        const QString path = QFileDialog::getOpenFileName(nullptr, "Open Project", "",
                                                          "Duoplot Project (*.dvs)");
        if (!path.isEmpty())
            openExistingFile(path.toStdString());
    });
    tray_icon_->setOnFileSave([this]()   { saveProject(); });
    tray_icon_->setOnFileSaveAs([this]() { saveProjectAs(); });
    tray_icon_->setOnNewWindow([this]()  { newWindow(); });
    tray_icon_->setOnShowWindow([this](const std::string& name) {
        for (auto* w : windows_)
        {
            if (w->getName().toStdString() == name)
            {
                w->show();
                w->raise();
                w->activateWindow();
                break;
            }
        }
    });
    // Register existing windows in the tray menu
    for (auto* w : windows_)
        tray_icon_->addWindow(w->getName().toStdString());
    // Set tray icon image
    const QString icon_path = QString::fromStdString(getResourcesPathString()) + "images/apple.ico";
    if (QFile::exists(icon_path))
        tray_icon_->setIcon(QIcon(icon_path));

    tray_icon_->show();

    // Start TCP receive thread
    tcp_receive_thread_ = new std::thread(&MainWindow::tcpReceiveThreadFunction, this);

    LUMOS_LOG_INFO() << "MainWindow initialized (hidden data router)";
}

MainWindow::~MainWindow()
{
    destroy();
}

void MainWindow::destroy()
{
    if (shutdown_in_progress_)
    {
        return;
    }

    shutdown_in_progress_ = true;

    LUMOS_LOG_INFO() << "MainWindow shutting down";

    // Stop timers
    if (receive_timer_)
    {
        receive_timer_->stop();
    }
    if (refresh_timer_)
    {
        refresh_timer_->stop();
    }

    // Wait for TCP thread to finish
    if (tcp_receive_thread_ && tcp_receive_thread_->joinable())
    {
        tcp_receive_thread_->join();
        delete tcp_receive_thread_;
        tcp_receive_thread_ = nullptr;
    }

    // Clean up windows
    removeAllWindows();

    LUMOS_LOG_INFO() << "MainWindow destroyed";
}

void MainWindow::setupCallbacks()
{
    notification_from_gui_element_key_pressed_ = [this](const char key) {
        notifyChildrenOnKeyPressed(key);
    };

    notification_from_gui_element_key_released_ = [this](const char key) {
        notifyChildrenOnKeyReleased(key);
    };

    get_all_element_names_ = [this]() {
        return getAllElementNames();
    };

    notify_main_window_element_deleted_ = [this](const std::string& element_handle_string) {
        elementDeleted(element_handle_string);
    };

    notify_main_window_element_name_changed_ = [this](const std::string& old_name, const std::string& new_name) {
        elementNameChanged(old_name, new_name);
    };

    notify_main_window_name_changed_ = [this](const std::string& old_name, const std::string& new_name) {
        windowNameChanged(old_name, new_name);
    };

    notify_main_window_about_modification_ = [this]() {
        LUMOS_LOG_DEBUG() << "Project modified";
        // TODO: Mark project as modified
    };
}

void MainWindow::setupTimers()
{
    // Receive timer - checks for incoming data
    receive_timer_ = new QTimer(this);
    connect(receive_timer_, &QTimer::timeout, this, &MainWindow::onReceiveTimer);
    receive_timer_->start(16);  // ~60 Hz

    // Refresh timer - updates display
    refresh_timer_ = new QTimer(this);
    connect(refresh_timer_, &QTimer::timeout, this, &MainWindow::onRefreshTimer);
    refresh_timer_->start(33);  // ~30 Hz
}

void MainWindow::setupWindows(const ProjectSettings& project_settings)
{
    window_initialization_in_progress_ = true;

    for (const WindowSettings& ws : project_settings.getWindows())
    {
        GuiWindow* window = new GuiWindow(
            nullptr,  // No parent - top-level window
            ws,
            QString("Duoplot"),
            window_callback_id_,
            true,  // project_is_saved
            notification_from_gui_element_key_pressed_,
            notification_from_gui_element_key_released_,
            get_all_element_names_,
            notify_main_window_element_deleted_,
            notify_main_window_element_name_changed_,
            notify_main_window_name_changed_,
            notify_main_window_about_modification_);

        connect(window, &GuiWindow::serialConnectRequested, this, &MainWindow::connectSerialPort);
        connect(window, &GuiWindow::guiElementCreated,     this, &MainWindow::onGuiElementCreated);
        connect(window, &GuiWindow::newProjectRequested,   this, &MainWindow::newProject);
        connect(window, &GuiWindow::saveProjectRequested,  this, &MainWindow::saveProject);
        connect(window, &GuiWindow::saveProjectAsRequested,this, &MainWindow::saveProjectAs);
        connect(window, &GuiWindow::newWindowRequested,    this, &MainWindow::newWindow);
        connect(window, &GuiWindow::openProjectRequested,  this, [this]() {
            const QString path = QFileDialog::getOpenFileName(
                nullptr, tr("Open Project"), QString(),
                tr("Duoplot Project (*.dvs);;All Files (*)"));
            if (!path.isEmpty())
                openExistingFile(path.toStdString());
        });

        windows_.push_back(window);
        window->show();
        window_callback_id_++;
        current_window_num_++;

        // Register in tray (tray_icon_ may be null during initial setup — added separately)
        if (tray_icon_)
            tray_icon_->addWindow(ws.name);

        // Collect all plot panes and GUI elements from this window
        for (ApplicationGuiElement* element : window->getAllGuiElements())
        {
            const std::string handle = element->getHandleString();
            gui_elements_[handle] = element;

            // Check if it's a PlotPane and add to plot_panes_ map
            PlotPane* plot_pane = dynamic_cast<PlotPane*>(element);
            if (plot_pane != nullptr)
            {
                plot_panes_[handle] = plot_pane;
            }
        }
    }

    window_initialization_in_progress_ = false;

    LUMOS_LOG_INFO() << "Created " << windows_.size() << " windows";
}

void MainWindow::removeAllWindows()
{
    for (auto* window : windows_)
    {
        delete window;
    }
    windows_.clear();
    plot_panes_.clear();
    gui_elements_.clear();
}

void MainWindow::tcpReceiveThreadFunction()
{
    LUMOS_LOG_INFO() << "TCP receive thread started";

    while (!shutdown_in_progress_)
    {
        ReceivedData received_data = data_receiver_.receiveAndGetDataFromTcp();

        if (received_data.rawData() == nullptr)
        {
            continue;
        }

        if (!shutdown_in_progress_)
        {
            std::lock_guard<std::mutex> lock(receive_mtx_);
            manageReceivedData(received_data);
        }
    }

    LUMOS_LOG_INFO() << "TCP receive thread stopped";
}

void MainWindow::manageReceivedData(ReceivedData& received_data)
{
    // NOTE: called from tcpReceiveThreadFunction which already holds receive_mtx_.
    const Function fcn = received_data.getFunction();

    if (fcn == Function::OPEN_PROJECT_FILE)
    {
        const CommunicationHeader& hdr = received_data.getCommunicationHeader();
        queued_project_file_name_ =
            hdr.get(CommunicationHeaderObjectType::PROJECT_FILE_NAME).as<properties::Label>().data;
        open_project_file_queued_ = true;
    }
    else if (fcn == Function::SCREENSHOT)
    {
        const CommunicationHeader& hdr = received_data.getCommunicationHeader();
        const std::string base_path =
            hdr.get(CommunicationHeaderObjectType::SCREENSHOT_BASE_PATH).as<properties::Label>().data;
        performScreenshot(base_path);
    }
    else if (fcn == Function::QUERY_FOR_SYNC_OF_GUI_DATA)
    {
        updateClientApplicationAboutGuiState();
    }
    else if (fcn == Function::SET_GUI_ELEMENT_LABEL)
    {
        handleGuiManipulation(received_data);
    }
    else
    {
        addActionToQueue(received_data);
    }
}

void MainWindow::handleGuiManipulation(ReceivedData& received_data)
{
    const CommunicationHeader& hdr = received_data.getCommunicationHeader();
    const std::string handle_string =
        hdr.get(CommunicationHeaderObjectType::HANDLE_STRING).as<properties::Label>().data;
    const std::string label =
        hdr.get(CommunicationHeaderObjectType::LABEL).as<properties::Label>().data;

    if (gui_elements_.count(handle_string) > 0)
    {
        gui_elements_[handle_string]->setLabel(label);
    }
}

void MainWindow::performScreenshot(const std::string& screenshot_base_path)
{
    LUMOS_LOG_INFO() << "Screenshot requested: " << screenshot_base_path;
    for (auto* w : windows_)
        w->screenshot(screenshot_base_path);
}

void MainWindow::updateClientApplicationAboutGuiState()
{
    // Collect non-PlotPane GUI elements that have interactive state
    std::vector<ApplicationGuiElement*> interactive_elements;
    for (const auto& kv : gui_elements_)
    {
        if (plot_panes_.count(kv.first) == 0)  // skip plot panes
            interactive_elements.push_back(kv.second);
    }

    // Wire format: num_elements (uint8) + for each element: sendGuiData() binary block
    // Compute total size
    std::uint64_t total_bytes = sizeof(std::uint8_t);  // num_elements
    for (const auto* elem : interactive_elements)
    {
        const std::uint8_t hl = static_cast<std::uint8_t>(elem->getHandleString().size());
        total_bytes += sizeof(std::uint8_t) +   // type
                       sizeof(std::uint8_t) +   // handle length
                       hl +                     // handle string
                       sizeof(std::uint32_t) +  // payload size
                       elem->getGuiPayloadSize();
    }

    FillableUInt8Array arr{total_bytes};
    arr.fillWithStaticType(static_cast<std::uint8_t>(interactive_elements.size()));

    for (const auto* elem : interactive_elements)
    {
        const std::string& hs = elem->getHandleString();
        const std::uint8_t hl = static_cast<std::uint8_t>(hs.size());
        arr.fillWithStaticType(static_cast<std::uint8_t>(elem->getElementSettings()->type));
        arr.fillWithStaticType(hl);
        arr.fillWithDataFromPointer(hs.data(), hl);
        arr.fillWithStaticType(static_cast<std::uint32_t>(elem->getGuiPayloadSize()));
        elem->fillGuiPayload(arr);
    }

    lumos::internal::sendThroughTcpInterface(arr.view(), lumos::internal::kGuiTcpPortNum);
}

void MainWindow::addActionToQueue(ReceivedData& received_data)
{
    const Function fcn = received_data.getFunction();

    if (fcn == Function::SET_CURRENT_ELEMENT)
    {
        setActiveView(received_data);
    }
    else if (fcn == Function::FLUSH_MULTIPLE_ELEMENTS)
    {
        mainWindowFlushMultipleElements(received_data);
    }
    else if (isPlotDataFunction(fcn))
    {
        const CommunicationHeader& hdr{received_data.getCommunicationHeader()};
        const PlotObjectAttributes plot_object_attributes{hdr};
        const UserSuppliedProperties user_supplied_properties{hdr};

        std::shared_ptr<const ConvertedDataBase> converted_data =
            convertPlotObjectData(hdr, received_data, plot_object_attributes, user_supplied_properties);

        queued_data_[current_element_name_].push(std::make_unique<InputData>(
            received_data, converted_data, plot_object_attributes, user_supplied_properties));
    }
    else if (fcn == Function::PROPERTIES_EXTENSION || fcn == Function::PROPERTIES_EXTENSION_MULTIPLE)
    {
        const CommunicationHeader& hdr{received_data.getCommunicationHeader()};
        const PlotObjectAttributes plot_object_attributes{hdr};
        const UserSuppliedProperties user_supplied_properties{hdr};
        queued_data_[current_element_name_].push(
            std::make_unique<InputData>(received_data, plot_object_attributes, user_supplied_properties));
    }
    else
    {
        // AXES, VIEW, CLEAR, FLUSH_ELEMENT, etc.
        queued_data_[current_element_name_].push(std::make_unique<InputData>(received_data));
    }
}

void MainWindow::mainWindowFlushMultipleElements(const ReceivedData& received_data)
{
    const CommunicationHeader& hdr = received_data.getCommunicationHeader();
    const uint8_t num_names = hdr.get(CommunicationHeaderObjectType::NUM_NAMES).as<uint8_t>();

    const VectorConstView<uint8_t> name_lengths{received_data.payloadData(), static_cast<size_t>(num_names)};

    std::vector<std::string> names;
    size_t idx = num_names;

    for (size_t k = 0; k < num_names; k++)
    {
        names.push_back("");
        std::string& current_elem = names.back();
        const uint8_t current_element_length = name_lengths(k);
        for (size_t i = 0; i < current_element_length; i++)
        {
            current_elem += static_cast<char>(received_data.payloadData()[idx]);
            idx++;
        }
    }

    // Build a fake FLUSH_ELEMENT packet and queue it for each named element
    CommunicationHeader faked_hdr{Function::FLUSH_ELEMENT};
    const uint64_t num_bytes_hdr = faked_hdr.numBytes();
    const uint64_t num_bytes = num_bytes_hdr + 1 + 2 * sizeof(uint64_t);

    FillableUInt8Array fillable_array{num_bytes};
    fillable_array.fillWithStaticType(isBigEndian());
    fillable_array.fillWithStaticType(kMagicNumber);
    fillable_array.fillWithStaticType(num_bytes);
    faked_hdr.fillBufferWithData(fillable_array);

    const UInt8ArrayView array_view{fillable_array.data(), fillable_array.size()};

    for (const std::string& name : names)
    {
        ReceivedData fake_received_data{array_view.size()};
        std::memcpy(fake_received_data.rawData(), array_view.data(), array_view.size());
        fake_received_data.parseHeader();
        queued_data_[name].push(std::make_unique<InputData>(fake_received_data));
    }
}

void MainWindow::setActiveView(const ReceivedData& received_data)
{
    const CommunicationHeader& hdr = received_data.getCommunicationHeader();
    const std::string name =
        hdr.get(CommunicationHeaderObjectType::ELEMENT_NAME).as<properties::Label>().data;

    if (name.empty())
    {
        LUMOS_LOG_WARNING() << "SET_CURRENT_ELEMENT with empty name";
        return;
    }

    current_element_name_ = name;

    if (plot_panes_.count(current_element_name_) == 0)
    {
        new_window_queued_ = true;
    }

    LUMOS_LOG_DEBUG() << "Set current element: " << current_element_name_;
}

void MainWindow::receiveData()
{
    std::lock_guard<std::mutex> lock(receive_mtx_);

    if (open_project_file_queued_)
    {
        open_project_file_queued_ = false;
        openExistingFile(queued_project_file_name_);
    }

    if (new_window_queued_)
    {
        new_window_queued_ = false;
        newWindowWithoutFileModification(current_element_name_);
    }

    // Dispatch queued data to matching plot panes
    for (auto& qa : queued_data_)
    {
        if (!qa.second.empty())
        {
            const std::string element_handle_string = qa.first;

            if (plot_panes_.count(element_handle_string) > 0U)
            {
                plot_panes_[element_handle_string]->pushQueue(qa.second);
            }
        }
    }
}

void MainWindow::onReceiveTimer()
{
    if (!shutdown_in_progress_)
    {
        handleSerialData();
        receiveData();
    }
}

void MainWindow::onRefreshTimer()
{
    if (!shutdown_in_progress_)
    {
        // Update all plot panes
        for (auto& ge : plot_panes_)
        {
            PlotPane* plot_pane = ge.second;
            plot_pane->update();
        }
    }
}

void MainWindow::newProject()
{
    LUMOS_LOG_INFO() << "New project";
    current_project_file_.clear();
    removeAllWindows();

    ProjectSettings ps;
    WindowSettings ws;
    ws.name = "main";
    ws.x = 100;
    ws.y = 100;
    ws.width = 1200;
    ws.height = 800;
    TabSettings ts;
    ts.name = "tab0";
    ws.tabs.push_back(ts);
    ps.pushBackWindowSettings(ws);

    setupWindows(ps);
    if (tray_icon_)
    {
        for (auto* w : windows_)
            tray_icon_->addWindow(w->getName().toStdString());
    }
}

void MainWindow::saveProject()
{
    if (current_project_file_.isEmpty())
    {
        saveProjectAs();
        return;
    }

    ProjectSettings ps;
    for (const auto* w : windows_)
        ps.pushBackWindowSettings(w->getWindowSettings());

    std::ofstream out_file(current_project_file_.toStdString());
    if (!out_file.is_open())
    {
        LUMOS_LOG_ERROR() << "Failed to open file for writing: " << current_project_file_.toStdString();
        QMessageBox::critical(nullptr, tr("Save Failed"),
                              tr("Could not open file for writing:\n") + current_project_file_);
        return;
    }

    out_file << ps.toJson().dump(4);
    LUMOS_LOG_INFO() << "Project saved to: " << current_project_file_.toStdString();

    for (auto* w : windows_)
    {
        w->setIsFileSavedForLabel(true);
        w->setProjectName(QFileInfo(current_project_file_).baseName());
    }
}

void MainWindow::saveProjectAs()
{
    const QString path = QFileDialog::getSaveFileName(
        nullptr, tr("Save Project As"), current_project_file_,
        tr("Duoplot Project (*.dvs);;All Files (*)"));
    if (path.isEmpty())
        return;

    current_project_file_ = path;
    saveProject();
}

void MainWindow::openExistingFile(const std::string& file_path)
{
    LUMOS_LOG_INFO() << "Opening project: " << file_path;
    try
    {
        ProjectSettings ps(file_path);
        if (ps.getWindows().empty())
        {
            QMessageBox::warning(nullptr, tr("Open Failed"),
                                 tr("The file does not contain any windows:\n")
                                 + QString::fromStdString(file_path));
            return;
        }

        removeAllWindows();
        current_project_file_ = QString::fromStdString(file_path);
        setupWindows(ps);

        if (tray_icon_)
        {
            for (auto* w : windows_)
                tray_icon_->addWindow(w->getName().toStdString());
        }

        const QString project_name = QFileInfo(current_project_file_).baseName();
        for (auto* w : windows_)
        {
            w->setIsFileSavedForLabel(true);
            w->setProjectName(project_name);
        }
    }
    catch (const std::exception& e)
    {
        LUMOS_LOG_ERROR() << "Failed to load project: " << e.what();
        QMessageBox::critical(nullptr, tr("Open Failed"),
                              tr("Failed to load project:\n") + QString::fromStdString(e.what()));
    }
}

void MainWindow::newWindow()
{
    newWindowWithoutFileModification();
    // TODO: Mark as modified
}

void MainWindow::newWindowWithoutFileModification()
{
    newWindowWithoutFileModification("");
}

void MainWindow::newWindowWithoutFileModification(const std::string& element_handle_string)
{
    WindowSettings window_settings;
    window_settings.name = "Window " + std::to_string(current_window_num_);
    window_settings.x = 30 + current_window_num_ * 30;
    window_settings.y = 30 + current_window_num_ * 30;
    window_settings.width = 900;
    window_settings.height = 700;

    TabSettings tab_settings;
    tab_settings.name = "tab0";
    window_settings.tabs.push_back(tab_settings);

    GuiWindow* window = new GuiWindow(
        nullptr,
        window_settings,
        QString("Duoplot"),
        window_callback_id_,
        true,
        notification_from_gui_element_key_pressed_,
        notification_from_gui_element_key_released_,
        get_all_element_names_,
        notify_main_window_element_deleted_,
        notify_main_window_element_name_changed_,
        notify_main_window_name_changed_,
        notify_main_window_about_modification_);

    connect(window, &GuiWindow::serialConnectRequested, this, &MainWindow::connectSerialPort);
    connect(window, &GuiWindow::guiElementCreated,     this, &MainWindow::onGuiElementCreated);
    connect(window, &GuiWindow::newProjectRequested,   this, &MainWindow::newProject);
    connect(window, &GuiWindow::saveProjectRequested,  this, &MainWindow::saveProject);
    connect(window, &GuiWindow::saveProjectAsRequested,this, &MainWindow::saveProjectAs);
    connect(window, &GuiWindow::newWindowRequested,    this, &MainWindow::newWindow);
    connect(window, &GuiWindow::openProjectRequested,  this, [this]() {
        const QString path = QFileDialog::getOpenFileName(
            nullptr, tr("Open Project"), QString(),
            tr("Duoplot Project (*.dvs);;All Files (*)"));
        if (!path.isEmpty())
            openExistingFile(path.toStdString());
    });

    windows_.push_back(window);
    window->show();
    window_callback_id_++;
    current_window_num_++;

    if (tray_icon_)
        tray_icon_->addWindow(window_settings.name);

    // If a specific element was requested, create a plot pane for it immediately
    if (!element_handle_string.empty())
    {
        window->createNewPlotPane(element_handle_string);

        for (ApplicationGuiElement* element : window->getAllGuiElements())
        {
            const std::string handle = element->getHandleString();
            gui_elements_[handle] = element;

            PlotPane* plot_pane = dynamic_cast<PlotPane*>(element);
            if (plot_pane != nullptr)
            {
                plot_panes_[handle] = plot_pane;
            }
        }
    }

    LUMOS_LOG_INFO() << "Created new window: " << window_settings.name;
}

void MainWindow::handleSerialData()
{
    if (serial_interface_ == nullptr)
    {
        return;
    }

    const std::vector<RawDataFrame> frames = serial_interface_->extractRawDataFrames();
    if (frames.empty())
    {
        return;
    }
    // TODO Phase 2: feed frames through the subscription system (topic IDs + object types).
    LUMOS_LOG_DEBUG() << "Received " << frames.size() << " serial frames (not yet processed)";
}

std::vector<std::string> MainWindow::getAllElementNames() const
{
    std::vector<std::string> names;

    for (const auto* window : windows_)
    {
        auto window_names = window->getElementNames();
        names.insert(names.end(), window_names.begin(), window_names.end());
    }

    return names;
}

void MainWindow::windowNameChanged(const std::string& old_name, const std::string& new_name)
{
    LUMOS_LOG_INFO() << "Window name changed: " << old_name << " -> " << new_name;
}

void MainWindow::elementDeleted(const std::string& element_handle_string)
{
    LUMOS_LOG_INFO() << "Element deleted: " << element_handle_string;

    // Remove from maps
    plot_panes_.erase(element_handle_string);
    gui_elements_.erase(element_handle_string);
    queued_data_.erase(element_handle_string);
}

void MainWindow::elementNameChanged(const std::string& old_name, const std::string& new_name)
{
    LUMOS_LOG_INFO() << "Element name changed: " << old_name << " -> " << new_name;

    // Update maps
    if (plot_panes_.count(old_name) > 0)
    {
        plot_panes_[new_name] = plot_panes_[old_name];
        plot_panes_.erase(old_name);
    }

    if (gui_elements_.count(old_name) > 0)
    {
        gui_elements_[new_name] = gui_elements_[old_name];
        gui_elements_.erase(old_name);
    }

    if (queued_data_.count(old_name) > 0)
    {
        queued_data_[new_name] = std::move(queued_data_[old_name]);
        queued_data_.erase(old_name);
    }
}

void MainWindow::notifyChildrenOnKeyPressed(const char key)
{
    for (auto* window : windows_)
    {
        window->notifyChildrenOnKeyPressed(key);
    }
}

void MainWindow::notifyChildrenOnKeyReleased(const char key)
{
    for (auto* window : windows_)
    {
        window->notifyChildrenOnKeyReleased(key);
    }
}

void MainWindow::connectSerialPort(const QString& port, int baudrate)
{
    // Build platform-specific port path
    const std::string port_name = port.toStdString();
    std::string port_path;

#ifdef __APPLE__
    if (port_name.find("/dev/") == std::string::npos)
    {
        port_path = "/dev/tty." + port_name;
    }
    else
    {
        port_path = port_name;
    }
#else
    if (port_name.find("/dev/") == std::string::npos)
    {
        port_path = "/dev/" + port_name;
    }
    else
    {
        port_path = port_name;
    }
#endif

    LUMOS_LOG_INFO() << "Connecting serial port: " << port_path << " @ " << baudrate;

    // Replace previous interface (previous thread runs until process exit).
    serial_interface_ = new SerialInterface(port_path, static_cast<int32_t>(baudrate));
    serial_interface_->start();
}

void MainWindow::onGuiElementCreated(ApplicationGuiElement* element)
{
    const std::string handle = element->getHandleString();
    gui_elements_[handle] = element;
    LUMOS_LOG_INFO() << "GUI element created and registered: " << handle;
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Save window geometry to settings
    settings_->saveSettings();

    destroy();
    event->accept();

    // Quit the application when MainWindow closes
    qApp->quit();
}
