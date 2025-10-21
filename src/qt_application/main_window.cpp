#include "main_window.h"

#include <QCloseEvent>

#include "lumos/plotting/enumerations.h"
#include "lumos/logging/logging.h"
#include "plot_objects/plot_object_base/plot_object_base.h"
#include "plot_objects/plot_objects.h"

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
      tcp_receive_thread_(nullptr),
      receive_timer_(nullptr),
      refresh_timer_(nullptr),
      current_window_num_(0),
      window_callback_id_(0),
      shutdown_in_progress_(false),
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

        windows_.push_back(window);
        window->show();
        window_callback_id_++;
        current_window_num_++;

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
    const Function fcn = received_data.getFunction();

    // TODO: Handle special functions like OPEN_PROJECT_FILE, SCREENSHOT, etc.

    // For now, just add to queue
    addActionToQueue(received_data);
}

void MainWindow::addActionToQueue(ReceivedData& received_data)
{
    const Function fcn = received_data.getFunction();

    if (fcn == Function::SET_CURRENT_ELEMENT)
    {
        setActiveView(received_data);
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
    else
    {
        // Other commands (AXES, VIEW, CLEAR, etc.)
        queued_data_[current_element_name_].push(std::make_unique<InputData>(received_data));
    }
}

void MainWindow::setActiveView(const ReceivedData& received_data)
{
    const CommunicationHeader& hdr = received_data.getCommunicationHeader();
    current_element_name_ = hdr.get(CommunicationHeaderObjectType::ELEMENT_NAME).as<properties::Label>().data;

    LUMOS_LOG_DEBUG() << "Set current element: " << current_element_name_;
}

void MainWindow::receiveData()
{
    std::lock_guard<std::mutex> lock(receive_mtx_);

    // Process queued data for each element
    for (auto& qa : queued_data_)
    {
        if (!qa.second.empty())
        {
            const std::string element_handle_string = qa.first;

            if (plot_panes_.count(element_handle_string) > 0U)
            {
                PlotPane* plot_pane = plot_panes_[element_handle_string];
                plot_pane->pushQueue(qa.second);
            }
        }
    }
}

void MainWindow::onReceiveTimer()
{
    if (!shutdown_in_progress_)
    {
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
    LUMOS_LOG_INFO() << "New project requested";
    // TODO: Implement new project functionality
}

void MainWindow::saveProject()
{
    LUMOS_LOG_INFO() << "Save project requested";
    // TODO: Implement project saving
}

void MainWindow::saveProjectAs()
{
    LUMOS_LOG_INFO() << "Save project as requested";
    // TODO: Implement project saving
}

void MainWindow::openExistingFile(const std::string& file_path)
{
    LUMOS_LOG_INFO() << "Opening project: " << file_path;
    // TODO: Implement project loading
}

void MainWindow::newWindow()
{
    newWindowWithoutFileModification();
    // TODO: Mark as modified
}

void MainWindow::newWindowWithoutFileModification()
{
    WindowSettings window_settings;
    window_settings.name = "window_" + std::to_string(windows_.size());
    window_settings.x = 100 + windows_.size() * 50;
    window_settings.y = 100 + windows_.size() * 50;
    window_settings.width = 1200;
    window_settings.height = 800;

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

    windows_.push_back(window);
    window->show();
    window_callback_id_++;
    current_window_num_++;

    LUMOS_LOG_INFO() << "Created new window: " << window_settings.name;
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

void MainWindow::closeEvent(QCloseEvent* event)
{
    // Save window geometry to settings
    settings_->saveSettings();

    destroy();
    event->accept();

    // Quit the application when MainWindow closes
    qApp->quit();
}
