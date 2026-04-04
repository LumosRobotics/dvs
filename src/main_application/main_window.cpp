#include "main_window.h"

#include <unistd.h>
#include <wx/wfstream.h>
#include <wx/wxprec.h>

#include <chrono>
#include <csignal>
#include <iostream>
#include <stdexcept>

#include "lumos/plotting/constants.h"
#include "lumos/math.h"
#include "events.h"
#include "filesystem.h"
#include "globals.h"
#include "gui_window.h"
#include "platform_paths.h"


namespace element_number_counter
{
int counter = 0;
int getNextFreeElementNumber()
{
    int counter_tmp_val = counter;
    counter++;

    return counter_tmp_val;
}
}  // namespace element_number_counter

using namespace lumos::internal;

MainWindow::MainWindow(const std::vector<std::string>& cmdl_args)
    : wxFrame(NULL, wxID_ANY, "", wxPoint(30, 130), wxSize(kMainWindowWidth, 150), wxNO_BORDER),
      data_receiver_{},
      serial_interface_{"/dev/tty.usbmodem142102", 115200}
{
#ifndef PLATFORM_APPLE_M
    Show();
#endif

    serial_interface_.start();
    shutdown_in_progress_ = false;

    first_window_button_offset_ =
        kNewWindowButtonHeight + kNewWindowButtonPreOffset + kNewWindowButtonPostOffset + kMainWindowTopMargin;

    SetBackgroundColour(RGBTripletfToWxColour(kMainWindowBackgroundColor));

    serial_array_ = new uint8_t[kSerialSendBufferSize];
    gui_transfer_state_ = GuiTransferState::Idle;
    idle_counter_ = 0;
    num_iterations_without_control_message_ = 0;

    static_cast<void>(cmdl_args);
    window_initialization_in_progress_ = true;
    open_project_file_queued_ = false;
    new_window_queued_ = false;

    window_callback_id_ = lumos_ids::WINDOW_TOGGLE;

    configuration_agent_ = new ConfigurationAgent();

    wxIcon* icon = new wxIcon();

    icon->LoadFile(getResourcesPathString() + "images/apple.ico", wxBITMAP_TYPE_ICO, 32, 32);

    task_bar_ = new CustomTaskBarIcon();
    if (!task_bar_->SetIcon(*icon, "wxTaskBarIcon Sample\n"))
    {
        wxLogError("Could not set icon.");
    }
    this->SetPosition(wxPoint(10, 130));

    menu_bar_ = createMainMenuBar();

    cmdl_output_window_ = new CmdlOutputWindow();
    cmdl_output_window_->Hide();

    topic_text_output_window_ = new TopicTextOutputWindow();
    topic_text_output_window_->Show();

    push_text_to_cmdl_output_window_ = [this](const Color_t col, const std::string& text) -> void {
        cmdl_output_window_->pushNewText(col, text);
    };

    print_gui_callback_code_ = [this]() { printGuiCallbackCode(); };

    time_at_start_ = static_cast<uint64_t>(
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())
            .count());
    time_of_receive_control_message_ = 0;
    time_of_sending_gui_data_ = 0;
    control_state_ = 0U;

    SetMenuBar(menu_bar_);
#ifdef PLATFORM_APPLE_M
    wxMenuBar::MacSetCommonMenuBar(menu_bar_);
#endif

    Bind(wxEVT_MENU, &MainWindow::newProjectCallback, this, wxID_NEW);
    Bind(wxEVT_MENU, &MainWindow::saveProjectCallback, this, wxID_SAVE);
    Bind(wxEVT_MENU, &MainWindow::openExistingFileCallback, this, wxID_OPEN);
    Bind(wxEVT_MENU, &MainWindow::saveProjectAsCallback, this, wxID_SAVEAS);
    // Bind(wxEVT_MENU, &MainWindow::exitApplication, this, wxID_HELP_INDEX);
    Bind(wxEVT_MENU, &MainWindow::newWindowCallback, this, wxID_HELP_CONTENTS);
    Bind(wxEVT_MENU, &MainWindow::handleTopicOutputWindow, this, wxID_HELP_PROCEDURES);

    task_bar_->setOnMenuExitCallback([this]() -> void { this->destroy(); });
    task_bar_->setOnMenuFileNew([this]() -> void { newProject(); });
    task_bar_->setOnMenuFileOpen([this]() -> void { openExistingFile(); });
    task_bar_->setOnMenuFileSave([this]() -> void { saveProject(); });
    task_bar_->setOnMenuFileSaveAs([this]() -> void { saveProjectAs(); });
    task_bar_->setOnMenuSubWindow(
        [this](const std::string& window_name) -> void { toggleWindowVisibility(window_name); });
    task_bar_->setOnMenuPreferences([this]() -> void { preferences(); });
    task_bar_->setOnMenuFileNewWindow([this]() -> void { this->newWindow(); });

    notification_from_gui_element_key_pressed_ = [this](const char key) { notifyChildrenOnKeyPressed(key); };
    notification_from_gui_element_key_released_ = [this](const char key) { notifyChildrenOnKeyReleased(key); };
    get_all_element_names_ = [this]() -> std::vector<std::string> { return getAllElementNames(); };
    notify_main_window_element_deleted_ = [this](const std::string& element_handle_string) -> void {
        elementDeleted(element_handle_string);
    };
    notify_main_window_element_name_changed_ = [this](const std::string& old_name,
                                                      const std::string& new_name) -> void {
        elementNameChanged(old_name, new_name);
    };
    notify_main_window_name_changed_ = [this](const std::string& old_name, const std::string& new_name) -> void {
        windowNameChanged(old_name, new_name);
    };
    notify_main_window_about_modification_ = [this]() -> void { fileModified(); };

#ifdef PLATFORM_LINUX_M
    int argc = 1;
    char* argv[1] = {"noop"};
    glutInit(&argc, argv);
#endif

    if (configuration_agent_->hasKey("last_opened_file") &&
        lumos::filesystem::exists(configuration_agent_->readValue<std::string>("last_opened_file")))
    {
        save_manager_ = new SaveManager(configuration_agent_->readValue<std::string>("last_opened_file"));
    }
    else
    {
        save_manager_ = new SaveManager();
    }

    int visualization_period_ms = 10;
    if (configuration_agent_->hasKey("visualization_period_ms"))
    {
        visualization_period_ms = configuration_agent_->readValue<int>("visualization_period_ms");
        visualization_period_ms = std::max(1, std::min(100, visualization_period_ms));
    }

    current_window_num_ = 0;

    setupWindows(save_manager_->getCurrentProjectSettings());

    for (auto we : windows_)
    {
        windows_menu_->Append(we->getCallbackId(), we->getName());
        Bind(wxEVT_MENU, &MainWindow::toggleWindowVisibilityCallback, this, we->getCallbackId());
    }

    tcp_receive_thread_ = new std::thread(&MainWindow::tcpReceiveThreadFunction, this);

    receive_timer_.Bind(wxEVT_TIMER, &MainWindow::OnReceiveTimer, this);
    receive_timer_.Start(visualization_period_ms);

    // refresh_timer_.Bind(wxEVT_TIMER, &MainWindow::OnRefreshTimer, this);  // TODO: Remove?
    window_initialization_in_progress_ = false;

    TabSettings tab_settings;
    tab_settings.name = "New window";

    new_window_button_ = new WindowButton(this,
                                          tab_settings,
                                          wxPoint(0, kMainWindowTopMargin + kNewWindowButtonPreOffset),
                                          wxSize(kMainWindowWidth, kNewWindowButtonHeight),
                                          0,
                                          [this](const std::string& f) -> void { newWindow(); });

    mouse_left_pressed_ = false;

    close_button_ = new CloseButton(
        this,
        wxPoint(0, 0),
        wxSize(25, 25),
        RGBTripletfToWxColour(kMainWindowBackgroundColor),
        [this]() -> void { this->destroy(); },
        [](wxPaintDC& dc, const int button_size) -> void {
            const int diff = 7;
            const wxPoint p00{diff, diff};
            const wxPoint p01{button_size - diff, button_size - diff};

            dc.DrawLine(p00, p01);

            const wxPoint p10{diff, button_size - diff};
            const wxPoint p11{button_size - diff, diff};

            dc.DrawLine(p10, p11);
        });
    minimize_button_ = new CloseButton(
        this,
        wxPoint(25, 0),
        wxSize(25, 25),
        RGBTripletfToWxColour(kMainWindowBackgroundColor),
        [this]() -> void { this->Iconize(); },
        [](wxPaintDC& dc, const int button_size) -> void {
            const int diff = 7;
            const int y_offset = 7;
            const wxPoint p00{diff, button_size - y_offset};
            const wxPoint p01{button_size - diff, button_size - y_offset};

            dc.DrawLine(p00, p01);
        });
    Bind(wxEVT_LEFT_DOWN, &MainWindow::mouseLeftPressed, this);
    Bind(wxEVT_LEFT_UP, &MainWindow::mouseLeftReleased, this);
    Bind(wxEVT_MOTION, &MainWindow::mouseMoved, this);
}

void MainWindow::handleTopicOutputWindow(wxCommandEvent& WXUNUSED(event))
{
    if (topic_text_output_window_->IsShown())
    {
        topic_text_output_window_->Hide();
    }
    else
    {
        topic_text_output_window_->Show();
    }
}

void MainWindow::exitApplication(wxCommandEvent& WXUNUSED(event))
{
    this->destroy();
}

std::string guiElementTypeToGuiHandleString(const GuiElementType tp)
{
    switch (tp)
    {
        case GuiElementType::Slider:
            return "SliderHandle";
        case GuiElementType::Button:
            return "ButtonHandle";
        case GuiElementType::Checkbox:
            return "CheckboxHandle";
        case GuiElementType::TextLabel:
            return "TextLabelHandle";
        case GuiElementType::Unknown:
            throw std::runtime_error("guiElementTypeToGuiHandleString: Unknown GuiElementType!");
            return "";
        default:
            return "unknown";
    }
}

std::string getCallbackFunctionUseFromType(const GuiElementType tp)
{
    std::string s = "";

    if (tp == GuiElementType::Slider)
    {
        s = "        const std::int32_t min_value = gui_element_handle.getMinValue();\n"
            "        const std::int32_t max_value = gui_element_handle.getMaxValue();\n"
            "        const std::int32_t step_size = gui_element_handle.getStepSize();\n"
            "        const std::int32_t value = gui_element_handle.getValue();\n";
    }
    else if (tp == GuiElementType::Button)
    {
        s = "        const bool is_pressed = gui_element_handle.getIsPressed();\n";
    }
    else if (tp == GuiElementType::Checkbox)
    {
        s = "        const bool is_checked = gui_element_handle.getIsChecked();\n";
    }
    else if (tp == GuiElementType::TextLabel)
    {
        s = "TextLabelHandle";
    }
    else
    {
        throw std::runtime_error("getCallbackFunctionUseFromType: Unknown GuiElementType!");
    }

    return s;
}

void MainWindow::printGuiCallbackCode()
{
    // clang-format off
    cmdl_output_window_->clear();
    cmdl_output_window_->Show();

    push_text_to_cmdl_output_window_(Color_t::BLACK, "\n");

    push_text_to_cmdl_output_window_(Color_t::BLACK, "void userFunction()\n");
    push_text_to_cmdl_output_window_(Color_t::BLACK, "{\n");

    for (const GuiWindow* gw : windows_)
    {
        const WindowSettings ws{gw->getWindowSettings()};

        const std::string window_str = "    /// Window: " + ws.name + "\n";

        push_text_to_cmdl_output_window_(Color_t::BLACK, window_str);

        for (const TabSettings& ts : ws.tabs)
        {
            const std::string tab_str = "    /// Tab: " + ts.name + "\n";

            push_text_to_cmdl_output_window_(Color_t::BLACK, tab_str);

            for (const std::shared_ptr<ElementSettings>& es : ts.elements)
            {
                const lumos::GuiElementType type{es->type};
                if(type == GuiElementType::Unknown || type == GuiElementType::PlotPane)
                {
                    continue;
                }
                const std::string handle_string{es->handle_string};

                const std::string get_function_text = "    const lumos::gui::" +
                    guiElementTypeToGuiHandleString(type) + " " + handle_string + " = lumos::gui::getGuiElementHandle<lumos::gui::" + guiElementTypeToGuiHandleString(type) + ">(\"" + handle_string + "\");\n";

                push_text_to_cmdl_output_window_(Color_t::BLACK, get_function_text);
            }

            push_text_to_cmdl_output_window_(Color_t::BLACK, "\n");
        }
    }

    push_text_to_cmdl_output_window_(Color_t::BLACK, "}\n\n\n");

    push_text_to_cmdl_output_window_(Color_t::BLACK, "int main(int argc, char** argv)\n{\n");

    for (const GuiWindow* gw : windows_)
    {
        const WindowSettings ws{gw->getWindowSettings()};

        const std::string window_str = "    /// Window: " + ws.name + "\n";

        push_text_to_cmdl_output_window_(Color_t::BLACK, window_str);

        for (const TabSettings& ts : ws.tabs)
        {
            const std::string tab_str = "    /// Tab: " + ts.name + "\n";

            push_text_to_cmdl_output_window_(Color_t::BLACK, tab_str);

            for (const std::shared_ptr<ElementSettings>& es : ts.elements)
            {
                const lumos::GuiElementType type{es->type};
                if(type == GuiElementType::Unknown || type == GuiElementType::PlotPane || type == GuiElementType::TextLabel)
                {
                    continue;
                }
                const std::string handle_string{es->handle_string};

                const std::string cb_function_text = "    lumos::gui::registerGuiCallback(\"" + handle_string + "\", [](const " + guiElementTypeToGuiHandleString(type) + "& gui_element_handle) -> void {\n"
                + getCallbackFunctionUseFromType(type) +
                    "    });\n\n";
                push_text_to_cmdl_output_window_(Color_t::BLACK, cb_function_text);
            }
        }
    }

    push_text_to_cmdl_output_window_(Color_t::BLACK, "    lumos::gui::startGuiReceiveThread();\n");

    push_text_to_cmdl_output_window_(Color_t::BLACK, "    // Other client code here...\n");

    push_text_to_cmdl_output_window_(Color_t::BLACK, "}\n");
    // clang-format on
}

void MainWindow::mouseMoved(wxMouseEvent& event)
{
    if (mouse_left_pressed_)
    {
        const wxPoint current_mouse_position = event.GetPosition();
        const wxPoint delta_pos = current_mouse_position - mouse_pos_at_press_;

        const wxPoint current_position = this->GetPosition();
        this->SetPosition(current_position + delta_pos);
    }
}

void MainWindow::mouseLeftReleased(wxMouseEvent& event)
{
    mouse_left_pressed_ = false;
}

void MainWindow::mouseLeftPressed(wxMouseEvent& event)
{
    const wxPoint current_mouse_position = event.GetPosition();

    if (current_mouse_position.y < kMainWindowTopMargin)
    {
        mouse_left_pressed_ = true;
        mouse_pos_at_press_ = current_mouse_position;
    }
}

void MainWindow::elementNameChanged(const std::string& old_name, const std::string& new_name)
{
    for (auto& local_ge : plot_panes_)
    {
        if (local_ge.first == old_name)
        {
            ApplicationGuiElement* ge = local_ge.second;
            plot_panes_.erase(local_ge.first);
            plot_panes_[new_name] = ge;
            break;
        }
    }

    for (auto& local_ge : gui_elements_)
    {
        if (local_ge.first == old_name)
        {
            ApplicationGuiElement* ge = local_ge.second;
            gui_elements_.erase(local_ge.first);
            gui_elements_[new_name] = ge;
            break;
        }
    }
}

void MainWindow::elementDeleted(const std::string& element_handle_string)
{
    if (!shutdown_in_progress_)
    {
        if (plot_panes_.count(element_handle_string) > 0)
        {
            plot_panes_.erase(element_handle_string);
        }
        else if (gui_elements_.count(element_handle_string) > 0)
        {
            gui_elements_.erase(element_handle_string);
        }
    }
}

wxMenuBar* MainWindow::createMainMenuBar()
{
    wxMenuBar* menu_bar_tmp = new wxMenuBar();

    // File Menu
    wxMenu* file_menu_ = new wxMenu();
    file_menu_->Append(wxID_NEW, _T("&New"));
    file_menu_->AppendSeparator();
    file_menu_->Append(wxID_OPEN, _T("&Open..."));
    file_menu_->AppendSeparator();
    file_menu_->Append(wxID_SAVE, _T("&Save"));
    file_menu_->Append(wxID_SAVEAS, _T("&Save As..."));

    menu_bar_tmp->Append(file_menu_, _T("&File"));

    /*wxMenu* project_settings = new wxMenu();
    project_settings->AppendCheckItem(wxID_NEW, wxT("&Listen to serial port"));
    project_settings->Append(wxID_NEW, _T("&Serial port settings..."));
    file_menu_->AppendSeparator();
    project_settings->AppendCheckItem(wxID_NEW, wxT("&Listen to local IPC"));

    menu_bar_tmp->Append(project_settings, _T("&Project Settings"));*/

    windows_menu_ = new wxMenu();
    windows_menu_->Append(wxID_HELP_PROCEDURES, "Command line output");
    windows_menu_->AppendSeparator();
    windows_menu_->Append(wxID_HELP_CONTENTS, "New window");
    windows_menu_->AppendSeparator();

    menu_bar_tmp->Append(windows_menu_, _T("&Window"));

    return menu_bar_tmp;
}

bool MainWindow::hasWindowWithName(const std::string& window_name)
{
    auto q = std::find_if(windows_.begin(), windows_.end(), [&window_name](const GuiWindow* const w) -> bool {
        return window_name == w->getName();
    });
    return q != windows_.end();
}

void MainWindow::toggleWindowVisibilityCallback(wxCommandEvent& event)
{
    for (auto we : windows_)
    {
        if (we->getCallbackId() == event.GetId())
        {
            if (!we->IsShown())
            {
                we->Show();
                fileModified();
            }
            we->Raise();

            break;
        }
    }
}

void MainWindow::toggleWindowVisibility(const std::string& window_name)
{
    for (auto we : windows_)
    {
        if (we->getName() == window_name)
        {
            if (!(we->IsShown()))
            {
                we->Show();
            }

            we->Raise();
        }
    }
}

void MainWindow::setupWindows(const ProjectSettings& project_settings)
{
    for (const WindowSettings& ws : project_settings.getWindows())
    {
        windows_.emplace_back(new GuiWindow(this,
                                            ws,
                                            save_manager_->getCurrentFileName(),
                                            window_callback_id_,
                                            save_manager_->isSaved(),
                                            notification_from_gui_element_key_pressed_,
                                            notification_from_gui_element_key_released_,
                                            get_all_element_names_,
                                            notify_main_window_element_deleted_,
                                            notify_main_window_element_name_changed_,
                                            notify_main_window_name_changed_,
                                            notify_main_window_about_modification_,
                                            push_text_to_cmdl_output_window_,
                                            print_gui_callback_code_));
        window_callback_id_ += 1;
        current_window_num_++;
        task_bar_->addNewWindow(ws.name);
    }

    int pos_y = first_window_button_offset_;
    this->SetSize(wxSize(kMainWindowWidth, kMainWindowButtonHeight * windows_.size() + first_window_button_offset_));
    for (auto we : windows_)
    {
        // Plot Panes
        std::vector<ApplicationGuiElement*> pps = we->getPlotPanes();
        for (const auto& ge : pps)
        {
            const std::shared_ptr<ElementSettings> element_settings = ge->getElementSettings();
            std::shared_ptr<PlotPaneSettings> plot_pane_settings =
                std::dynamic_pointer_cast<PlotPaneSettings>(element_settings);

            for (const SubscribedStreamSettings& subscribed_stream : plot_pane_settings->subscribed_streams)
            {
                if (plot_pane_subscriptions_.count(subscribed_stream.topic_id) > 0)
                {
                    std::cout << "Topic " << subscribed_stream.topic_id << " already exists in list, adding "
                              << ge->getHandleString() << " to it." << std::endl;
                    plot_pane_subscriptions_[subscribed_stream.topic_id].push_back(dynamic_cast<PlotPane*>(ge));
                }
                else
                {
                    std::cout << "Topic " << subscribed_stream.topic_id
                              << " doesn't exist in list, creating new list and adding " << ge->getHandleString()
                              << " to it." << std::endl;
                    std::vector<PlotPane*> pps;
                    pps.push_back(dynamic_cast<PlotPane*>(ge));
                    plot_pane_subscriptions_[subscribed_stream.topic_id] = pps;
                }

                objects_temporary_storage_[subscribed_stream.topic_id] =
                    std::vector<std::shared_ptr<objects::BaseObject>>();
            }

            plot_panes_[ge->getHandleString()] = ge;
        }

        // Scrolling texts elements

        std::vector<ScrollingTextGuiElement*> scrolling_text_gui_elements = we->getScrollingTexts();

        for (const auto& ge : scrolling_text_gui_elements)
        {
            const std::shared_ptr<ElementSettings> element_settings = ge->getElementSettings();
            std::shared_ptr<ScrollingTextSettings> scrolling_text_settings =
                std::dynamic_pointer_cast<ScrollingTextSettings>(element_settings);

            for (const SubscribedTextStreamSettings& subscribed_stream : scrolling_text_settings->subscribed_streams)
            {
                if (stream_of_strings_subscriptions_.count(subscribed_stream.topic_id) > 0)
                {
                    std::cout << "Topic " << subscribed_stream.topic_id << " already exists in list, adding "
                              << ge->getHandleString() << " to it." << std::endl;
                    stream_of_strings_subscriptions_[subscribed_stream.topic_id].push_back(ge);
                }
                else
                {
                    std::cout << "Topic " << subscribed_stream.topic_id
                              << " doesn't exist in list, creating new list and adding " << ge->getHandleString()
                              << " to it." << std::endl;
                    std::vector<ScrollingTextGuiElement*> stges;
                    stges.push_back(ge);
                    stream_of_strings_subscriptions_[subscribed_stream.topic_id] = stges;
                }

                streams_of_strings_[subscribed_stream.topic_id] = std::vector<std::pair<uint64_t, std::string>>();
            }

            scrolling_text_elements_[ge->getHandleString()] = ge;
        }

        // General GUI Elements
        std::vector<ApplicationGuiElement*> ges = we->getGuiElements();
        for (const auto& ge : ges)
        {
            gui_elements_[ge->getHandleString()] = ge;
        }

        TabSettings tab_settings;
        tab_settings.name = we->getName();

        window_buttons_.push_back(
            new WindowButton(this,
                             tab_settings,
                             wxPoint(0, pos_y),
                             wxSize(kMainWindowWidth, kMainWindowButtonHeight),
                             0,
                             [this](const std::string& f) -> void { toggleWindowVisibility(f); }));
        pos_y += kMainWindowButtonHeight;
    }

    updateClientApplicationAboutGuiState();
}

void MainWindow::updateClientApplicationAboutGuiState() const
{
    std::vector<std::shared_ptr<GuiElementState>> gui_elements_state;

    std::uint64_t total_num_bytes = 0U;

    for (const auto& ge : gui_elements_)
    {
        gui_elements_state.push_back(ge.second->getGuiElementState());

        total_num_bytes += gui_elements_state.back()->getTotalNumBytes();
    }

    total_num_bytes += sizeof(std::uint8_t);  // Number of gui elements

    FillableUInt8Array output_array{total_num_bytes};

    output_array.fillWithStaticType(static_cast<std::uint8_t>(gui_elements_state.size()));

    for (const auto& ges : gui_elements_state)
    {
        ges->serializeToBuffer(output_array);
    }

    sendThroughTcpInterface(output_array.view(), kGuiTcpPortNum);
}

std::vector<std::string> MainWindow::getAllElementNames() const
{
    std::vector<std::string> element_names;

    for (auto we : windows_)
    {
        const std::vector<std::string> window_element_names = we->getElementNames();

        element_names.insert(element_names.end(), window_element_names.begin(), window_element_names.end());
    }

    return element_names;
}

void MainWindow::performScreenshot(const std::string& screenshot_base_path)
{
    const std::string final_path =
        screenshot_base_path.back() == '/' ? screenshot_base_path : screenshot_base_path + "/";

    if (!lumos::filesystem::exists(final_path))
    {
        lumos::filesystem::create_directory(final_path);
    }

    for (auto we : windows_)
    {
        we->screenshot(final_path);
    }
}

void MainWindow::newWindowCallback(wxCommandEvent& WXUNUSED(event))
{
    newWindow();
}

void MainWindow::newWindow()
{
    window_initialization_in_progress_ = true;
    newWindowWithoutFileModification();

    window_initialization_in_progress_ = false;

    fileModified();
}

void MainWindow::newWindowWithoutFileModification()
{
    WindowSettings window_settings;
    window_settings.name = "Window " + std::to_string(current_window_num_);
    window_settings.x = 30;
    window_settings.y = 30;
    window_settings.width = 600;
    window_settings.height = 628;

    GuiWindow* gui_window = new GuiWindow(this,
                                          window_settings,
                                          save_manager_->getCurrentFileName(),
                                          window_callback_id_,
                                          save_manager_->isSaved(),
                                          notification_from_gui_element_key_pressed_,
                                          notification_from_gui_element_key_released_,
                                          get_all_element_names_,
                                          notify_main_window_element_deleted_,
                                          notify_main_window_element_name_changed_,
                                          notify_main_window_name_changed_,
                                          notify_main_window_about_modification_,
                                          push_text_to_cmdl_output_window_,
                                          print_gui_callback_code_);

    windows_menu_->Append(gui_window->getCallbackId(), gui_window->getName());
    Bind(wxEVT_MENU, &MainWindow::toggleWindowVisibilityCallback, this, gui_window->getCallbackId());

    task_bar_->addNewWindow(window_settings.name);
    current_window_num_++;
    window_callback_id_ += 1;

    windows_.push_back(gui_window);

    int pos_y = first_window_button_offset_ + kMainWindowButtonHeight * (windows_.size() - 1);
    this->SetSize(wxSize(kMainWindowWidth, kMainWindowButtonHeight * windows_.size() + first_window_button_offset_));

    TabSettings tab_settings;
    tab_settings.name = gui_window->getName();

    window_buttons_.push_back(new WindowButton(this,
                                               tab_settings,
                                               wxPoint(0, pos_y),
                                               wxSize(kMainWindowWidth, kMainWindowButtonHeight),
                                               0,
                                               [this](const std::string& f) -> void { toggleWindowVisibility(f); }));
}

void MainWindow::newWindowWithoutFileModification(const std::string& element_handle_string)
{
    WindowSettings window_settings;
    window_settings.name = "Window " + std::to_string(current_window_num_);
    window_settings.x = 30;
    window_settings.y = 30;
    window_settings.width = 600;
    window_settings.height = 628;

    GuiWindow* gui_window = new GuiWindow(this,
                                          window_settings,
                                          save_manager_->getCurrentFileName(),
                                          window_callback_id_,
                                          save_manager_->isSaved(),
                                          notification_from_gui_element_key_pressed_,
                                          notification_from_gui_element_key_released_,
                                          get_all_element_names_,
                                          notify_main_window_element_deleted_,
                                          notify_main_window_element_name_changed_,
                                          notify_main_window_name_changed_,
                                          notify_main_window_about_modification_,
                                          push_text_to_cmdl_output_window_,
                                          print_gui_callback_code_);

    windows_menu_->Append(gui_window->getCallbackId(), gui_window->getName());
    Bind(wxEVT_MENU, &MainWindow::toggleWindowVisibilityCallback, this, gui_window->getCallbackId());

    task_bar_->addNewWindow(window_settings.name);
    current_window_num_++;
    window_callback_id_ += 1;

    windows_.push_back(gui_window);

    int pos_y = first_window_button_offset_ + kMainWindowButtonHeight * (windows_.size() - 1);
    this->SetSize(wxSize(kMainWindowWidth, kMainWindowButtonHeight * windows_.size() + first_window_button_offset_));

    TabSettings tab_settings;
    tab_settings.name = gui_window->getName();

    window_buttons_.push_back(new WindowButton(this,
                                               tab_settings,
                                               wxPoint(0, pos_y),
                                               wxSize(kMainWindowWidth, kMainWindowButtonHeight),
                                               0,
                                               [this](const std::string& f) -> void { toggleWindowVisibility(f); }));

    gui_window->createNewPlotPane(element_handle_string);

    std::vector<ApplicationGuiElement*> pps = gui_window->getPlotPanes();

    for (const auto& ge : pps)
    {
        if (plot_panes_.count(ge->getHandleString()) == 0)
        {
            plot_panes_[ge->getHandleString()] = ge;
        }
    }
}

void MainWindow::windowNameChanged(const std::string& old_name, const std::string& new_name)
{
    (*std::find_if(window_buttons_.begin(), window_buttons_.end(), [old_name](const WindowButton* const wb) -> bool {
        return wb->getButtonLabel() == old_name;
    }))->setButtonLabel(new_name);
}

MainWindow::~MainWindow()
{
    delete configuration_agent_;
}

void MainWindow::setIsFileSavedForAllWindows(const bool file_saved)
{
    for (auto we : windows_)
    {
        we->setIsFileSavedForLabel(file_saved);
    }
}

void MainWindow::fileModified()
{
    if (window_initialization_in_progress_)
    {
        return;
    }
    save_manager_->setIsModified();

    setIsFileSavedForAllWindows(false);
}

void MainWindow::saveProjectAsCallback(wxCommandEvent& WXUNUSED(event))
{
    saveProjectAs();
}

void MainWindow::saveProjectAs()
{
    wxFileDialog open_file_dialog(
        this, _("Choose file to save to"), "", "", "duoplot files (*.duoplot)|*.duoplot", wxFD_SAVE);
    if (open_file_dialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    const std::string new_save_path = std::string(open_file_dialog.GetPath().mb_str());

    configuration_agent_->writeValue("last_opened_file", new_save_path);

    if (new_save_path == save_manager_->getCurrentFilePath())
    {
        saveProject();
        return;
    }

    ProjectSettings ps;

    for (const GuiWindow* gw : windows_)
    {
        ps.pushBackWindowSettings(gw->getWindowSettings());
    }

    save_manager_->saveToNewFile(new_save_path, ps);

    for (auto we : windows_)
    {
        we->setProjectName(save_manager_->getCurrentFileName());
    }
    setIsFileSavedForAllWindows(true);
}

void MainWindow::saveProject()
{
    if (!save_manager_->savePathIsSet())
    {
        saveProjectAs();
    }
    else
    {
        if (save_manager_->isSaved())
        {
            return;
        }
        ProjectSettings ps;

        for (const GuiWindow* gw : windows_)
        {
            ps.pushBackWindowSettings(gw->getWindowSettings());
        }

        configuration_agent_->writeValue("last_opened_file", save_manager_->getCurrentFilePath());

        save_manager_->save(ps);
        setIsFileSavedForAllWindows(true);
    }
}

void MainWindow::newProjectCallback(wxCommandEvent& WXUNUSED(event))
{
    newProject();
}

void MainWindow::newProject()
{
    window_initialization_in_progress_ = true;
    if (!save_manager_->isSaved())
    {
        if (wxMessageBox(_("Current content has not been saved! Proceed?"),
                         _("Please confirm"),
                         wxICON_QUESTION | wxYES_NO,
                         this) == wxNO)
        {
            return;
        }
    }

    removeAllWindows();

    save_manager_->reset();

    current_window_num_ = 0;
    newWindowWithoutFileModification();

    window_initialization_in_progress_ = false;
}

void MainWindow::removeAllWindows()
{
    plot_panes_.clear();
    gui_elements_.clear();

    for (auto we : windows_)
    {
        Unbind(wxEVT_MENU, &MainWindow::toggleWindowVisibilityCallback, this, we->getCallbackId());
        const int menu_id = windows_menu_->FindItem(we->getName());
        windows_menu_->Destroy(menu_id);

        we->deleteAllTabs();
        we->Destroy();
    }

    windows_.clear();
}

void MainWindow::saveProjectCallback(wxCommandEvent& WXUNUSED(event))
{
    saveProject();
}

void MainWindow::preferencesCallback(wxCommandEvent& WXUNUSED(event))
{
    preferences();
}

void MainWindow::preferences()
{
    std::cout << "Preferences!" << std::endl;
}

void MainWindow::openExistingFileCallback(wxCommandEvent& WXUNUSED(event))
{
    openExistingFile();
}

void MainWindow::openExistingFile(const std::string& file_path)
{
    window_initialization_in_progress_ = true;
    if (save_manager_->getCurrentFilePath() == file_path)
    {
        return;
    }

    removeAllWindows();

    configuration_agent_->writeValue("last_opened_file", file_path);

    save_manager_->openExistingFile(file_path);

    setupWindows(save_manager_->getCurrentProjectSettings());

    for (auto we : windows_)
    {
        windows_menu_->Append(we->getCallbackId(), we->getName());
        Bind(wxEVT_MENU, &MainWindow::toggleWindowVisibilityCallback, this, we->getCallbackId());
    }

    window_initialization_in_progress_ = false;
}

void MainWindow::openExistingFile()
{
    if (!save_manager_->isSaved())
    {
        if (wxMessageBox(_("Current content has not been saved! Proceed?"),
                         _("Please confirm"),
                         wxICON_QUESTION | wxYES_NO,
                         this) == wxNO)
        {
            return;
        }
    }

    wxFileDialog open_file_dialog(
        this, _("Open duoplot file"), "", "", "duoplot files (*.duoplot)|*.duoplot", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
    if (open_file_dialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    openExistingFile(std::string(open_file_dialog.GetPath().mb_str()));
}

void MainWindow::OnRefreshTimer(wxTimerEvent&)
{
    refresh_timer_.Stop();

    SendSizeEvent();
    Refresh();
}

void MainWindow::notifyChildrenOnKeyPressed(const char key)
{
    for (auto& we : windows_)
    {
        we->notifyChildrenOnKeyPressed(key);
    }

    if ((wxGetKeyState(WXK_COMMAND) || wxGetKeyState(WXK_CONTROL)) &&
        (wxGetKeyState(static_cast<wxKeyCode>('s')) || wxGetKeyState(static_cast<wxKeyCode>('S'))))
    {
        saveProject();
    }
    /*else if ((wxGetKeyState(WXK_COMMAND) || wxGetKeyState(WXK_CONTROL)) &&
             (wxGetKeyState(static_cast<wxKeyCode>('q')) || wxGetKeyState(static_cast<wxKeyCode>('Q'))))
    {
        this->destroy();
    }*/
}

void MainWindow::notifyChildrenOnKeyReleased(const char key)
{
    for (auto& we : windows_)
    {
        we->notifyChildrenOnKeyReleased(key);
    }
}

void MainWindow::deleteWindow(wxCommandEvent& event)
{
    auto q = std::find_if(windows_.begin(), windows_.end(), [&event](const GuiWindow* const gw) -> bool {
        return gw->getCallbackId() == (event.GetId() - 1);
    });

    if (q != windows_.end())
    {
        const std::vector<ApplicationGuiElement*> pps = (*q)->getPlotPanes();
        for (const auto& ge : pps)
        {
            plot_panes_.erase(ge->getHandleString());
        }
        const std::vector<ApplicationGuiElement*> ges = (*q)->getGuiElements();
        for (const auto& ge : ges)
        {
            gui_elements_.erase(ge->getHandleString());
        }

        const std::string deleted_window_name = (*q)->getName();

        Unbind(wxEVT_MENU, &MainWindow::toggleWindowVisibilityCallback, this, (*q)->getCallbackId());
        const int menu_id = windows_menu_->FindItem((*q)->getName());
        windows_menu_->Destroy(menu_id);

        task_bar_->removeWindow((*q)->getName());
        delete (*q);
        windows_.erase(q);

        auto qb = std::find_if(window_buttons_.begin(),
                               window_buttons_.end(),
                               [&deleted_window_name](const WindowButton* const w) -> bool {
                                   return w->getButtonLabel() == deleted_window_name;
                               });

        if (qb != window_buttons_.end())
        {
            delete (*qb);
            window_buttons_.erase(qb);
        }

        int pos_y = first_window_button_offset_;
        this->SetSize(
            wxSize(kMainWindowWidth, kMainWindowButtonHeight * windows_.size() + first_window_button_offset_));
        for (auto wb : window_buttons_)
        {
            wb->SetPosition(wxPoint(0, pos_y));
            pos_y += kMainWindowButtonHeight;
        }

        fileModified();
    }
}

void MainWindow::destroy()
{
    shutdown_in_progress_ = true;
    this->Close();
}
