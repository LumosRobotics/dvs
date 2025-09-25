#ifndef MAIN_APPLICATION_MAIN_WINDOW_H_
#define MAIN_APPLICATION_MAIN_WINDOW_H_

#include <wx/button.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/window.h>
#include <wx/wx.h>

#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "buffered_writer.h"
#include "close_button.h"
#include "cmdl_output_window.h"
#include "communication/data_receiver.h"
#include "communication/received_data.h"
#include "events.h"
#include "gui_element.h"
#include "input_data.h"
#include "plot_pane.h"
#include "project_state/configuration_agent.h"
#include "project_state/project_settings.h"
#include "project_state/save_manager.h"
#include "serial_interface/object_types.h"
#include "serial_interface/serial_interface.h"
#include "tab_button.h"
#include "topic_text_output_window.h"
#include "tray_icon.h"
#include "window_button.h"

class GuiWindow;
class ScrollingTextGuiElement;

enum class GuiTransferState : uint8_t
{
    Idle = 0U,
    SendingGuiData = 1U,
    WaitingForControlMessage = 2U
};

constexpr size_t kSerialSendBufferSize{1024U};

class MainWindow : public wxFrame
{
private:
    DataReceiver data_receiver_;
    SerialInterface serial_interface_;
    SaveManager* save_manager_;
    ConfigurationAgent* configuration_agent_;
    std::mutex receive_mtx_;

    CmdlOutputWindow* cmdl_output_window_;
    TopicTextOutputWindow* topic_text_output_window_;

    std::thread* tcp_receive_thread_;
    std::map<std::string, ApplicationGuiElement*> plot_panes_;
    std::map<std::string, ApplicationGuiElement*> gui_elements_;
    std::map<GuiElementId, std::shared_ptr<GuiElementState>> gui_elements_states_;
    std::map<std::string, ScrollingTextGuiElement*> scrolling_text_elements_;

    std::map<std::string, std::queue<std::unique_ptr<InputData>>> queued_data_;

    uint8_t* serial_array_;
    GuiTransferState gui_transfer_state_;
    uint32_t idle_counter_;
    uint32_t num_iterations_without_control_message_;

    CloseButton* close_button_;
    CloseButton* minimize_button_;

    bool shutdown_in_progress_;

    std::atomic<bool> open_project_file_queued_;
    std::string queued_project_file_name_;

    std::atomic<bool> new_window_queued_;

    std::string current_element_name_;

    wxTimer receive_timer_;
    wxTimer refresh_timer_;

    std::vector<GuiWindow*> windows_;
    int current_window_num_;
    CustomTaskBarIcon* task_bar_;
    int window_callback_id_;
    std::map<TopicId, std::vector<ScrollingTextGuiElement*>> stream_of_strings_subscriptions_;
    std::map<TopicId, std::vector<PlotPane*>> plot_pane_subscriptions_;
    std::map<TopicId, std::vector<std::shared_ptr<objects::BaseObject>>> objects_temporary_storage_;
    std::map<TopicId, std::vector<std::pair<uint64_t, std::string>>> streams_of_strings_;

    bool window_initialization_in_progress_;

    std::function<void(const char key)> notification_from_gui_element_key_pressed_;
    std::function<void(const char key)> notification_from_gui_element_key_released_;
    std::function<std::vector<std::string>(void)> get_all_element_names_;
    std::function<void(const std::string&)> notify_main_window_element_deleted_;
    std::function<void(const std::string&, const std::string&)> notify_main_window_element_name_changed_;
    std::function<void(const std::string&, const std::string&)> notify_main_window_name_changed_;
    std::function<void()> notify_main_window_about_modification_;
    std::function<void(const Color_t, const std::string&)> push_text_to_cmdl_output_window_;
    std::function<void(void)> print_gui_callback_code_;

    uint64_t time_at_start_;
    uint64_t time_of_receive_control_message_;
    uint64_t time_of_sending_gui_data_;
    uint8_t control_state_;

    void printGuiCallbackCode();

    void OnReceiveTimer(wxTimerEvent&);
    void OnRefreshTimer(wxTimerEvent&);
    void setActiveView(const ReceivedData& received_data);
    void receiveData();

    void mouseLeftPressed(wxMouseEvent& event);

    void mouseLeftReleased(wxMouseEvent& event);
    void mouseMoved(wxMouseEvent& event);

    bool mouse_left_pressed_;
    wxPoint mouse_pos_at_press_;
    int first_window_button_offset_;

    std::vector<WindowButton*> window_buttons_;
    WindowButton* new_window_button_;

    bool hasWindowWithName(const std::string& window_name);
    void preferencesCallback(wxCommandEvent& event);
    void preferences();

    void handleTopicOutputWindow(wxCommandEvent& event);

    void windowNameChanged(const std::string& old_name, const std::string& new_name);

    void notifyChildrenOnKeyPressed(const char key);
    void notifyChildrenOnKeyReleased(const char key);

    void saveProject();
    void saveProjectCallback(wxCommandEvent& event);
    void newProjectCallback(wxCommandEvent& event);
    void newProject();
    void saveProjectAs();
    void saveProjectAsCallback(wxCommandEvent& event);
    void openExistingFile(const std::string& file_path);

    void updateClientApplicationAboutGuiState() const;

    void performScreenshot(const std::string& screenshot_base_path);

    void removeAllWindows();
    void setupWindows(const ProjectSettings& project_settings);
    void fileModified();
    bool currentGuiElementSet() const;
    void tcpReceiveThreadFunction();
    void manageReceivedData(ReceivedData& received_data);
    void handleGuiManipulation(ReceivedData& received_data);

    void mainWindowFlushMultipleElements(const ReceivedData& received_data);
    void addActionToQueue(ReceivedData& received_data);
    void setIsFileSavedForAllWindows(const bool file_saved);
    void pushNewDataToQueue(const TopicId topic_id, const std::shared_ptr<objects::BaseObject>& obj);
    void handleSerialData();

    wxMenuBar* createMainMenuBar();

    wxMenuBar* menu_bar_;
    wxMenu* windows_menu_;

    void toggleWindowVisibilityCallback(wxCommandEvent& event);
    void exitApplication(wxCommandEvent& event);
    void updateSerialDeviceAboutGuiState();

public:
    MainWindow();
    MainWindow(const std::vector<std::string>& cmdl_args);
    ~MainWindow();

    void OnChildDestroy(wxCloseEvent& event);

    std::vector<std::string> getAllElementNames() const;

    void deleteWindow(wxCommandEvent& event);
    void toggleWindowVisibility(const std::string& window_name);
    void openExistingFile(wxCommandEvent& event);
    void openExistingFileCallback(wxCommandEvent& WXUNUSED(event));
    void openExistingFile();
    void newWindowCallback(wxCommandEvent& WXUNUSED(event));
    void newWindow();
    void newWindowWithoutFileModification();
    void newWindowWithoutFileModification(const std::string& element_handle_string);

    void elementDeleted(const std::string& element_handle_string);
    void elementNameChanged(const std::string& old_name, const std::string& new_name);

    void destroy();
};

#endif  // MAIN_APPLICATION_MAIN_WINDOW_H_
