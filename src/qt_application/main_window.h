#ifndef QT_APPLICATION_MAIN_WINDOW_H_
#define QT_APPLICATION_MAIN_WINDOW_H_

#include <QMainWindow>
#include <QString>
#include <QTimer>
#include <QFileDialog>

#include <atomic>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "communication/data_receiver.h"
#include "communication/received_data.h"
#include "gui_element.h"
#include "gui_window.h"
#include "input_data.h"
#include "plot_pane.h"
#include "project_state/project_settings.h"
#include "serial_interface/serial_interface.h"
#include "settings_handler/settings_handler.h"
#include "tray_icon.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    // Settings
    std::unique_ptr<SettingsHandler> settings_;

    // Tray icon (Phase 3)
    TrayIcon* tray_icon_{nullptr};

    // Communication
    DataReceiver data_receiver_;
    SerialInterface* serial_interface_;  // null until user selects a port
    std::thread* tcp_receive_thread_;
    std::mutex receive_mtx_;

    // Timers
    QTimer* receive_timer_;
    QTimer* refresh_timer_;

    // Windows (visible to user)
    std::vector<GuiWindow*> windows_;
    int current_window_num_;
    int window_callback_id_;

    // Data structures
    std::map<std::string, PlotPane*> plot_panes_;  // Map element_name -> PlotPane*
    std::map<std::string, ApplicationGuiElement*> gui_elements_;  // Map element_name -> GuiElement*
    std::map<std::string, std::queue<std::unique_ptr<InputData>>> queued_data_;  // Per-element data queues

    // Project file path (empty = unsaved new project)
    QString current_project_file_;

    // State
    std::atomic<bool> shutdown_in_progress_;
    std::atomic<bool> open_project_file_queued_;
    std::atomic<bool> new_window_queued_;
    std::string queued_project_file_name_;
    std::string current_element_name_;
    bool window_initialization_in_progress_;

    // Callbacks that GuiWindows use to communicate back to MainWindow
    std::function<void(const char key)> notification_from_gui_element_key_pressed_;
    std::function<void(const char key)> notification_from_gui_element_key_released_;
    std::function<std::vector<std::string>(void)> get_all_element_names_;
    std::function<void(const std::string&)> notify_main_window_element_deleted_;
    std::function<void(const std::string&, const std::string&)> notify_main_window_element_name_changed_;
    std::function<void(const std::string&, const std::string&)> notify_main_window_name_changed_;
    std::function<void()> notify_main_window_about_modification_;

    // Helper methods
    void setupTimers();
    void setupCallbacks();
    void tcpReceiveThreadFunction();
    void receiveData();
    void manageReceivedData(ReceivedData& received_data);
    void addActionToQueue(ReceivedData& received_data);
    void setActiveView(const ReceivedData& received_data);
    void handleGuiManipulation(ReceivedData& received_data);
    void mainWindowFlushMultipleElements(const ReceivedData& received_data);
    void handleSerialData();
    void performScreenshot(const std::string& screenshot_base_path);
    void updateClientApplicationAboutGuiState();

    // Window management
    void setupWindows(const ProjectSettings& project_settings);
    void removeAllWindows();
    void windowNameChanged(const std::string& old_name, const std::string& new_name);
    void elementDeleted(const std::string& element_handle_string);
    void elementNameChanged(const std::string& old_name, const std::string& new_name);

    // Project management
    void newProject();
    void saveProject();
    void saveProjectAs();
    void openExistingFile(const std::string& file_path);

    void notifyChildrenOnKeyPressed(const char key);
    void notifyChildrenOnKeyReleased(const char key);

private slots:
    void onReceiveTimer();
    void onRefreshTimer();
    void connectSerialPort(const QString& port, int baudrate);
    void onGuiElementCreated(ApplicationGuiElement* element);

public:
    MainWindow(const std::vector<std::string>& cmdl_args);
    ~MainWindow();

    void destroy();
    std::vector<std::string> getAllElementNames() const;

    void newWindow();
    void newWindowWithoutFileModification();
    void newWindowWithoutFileModification(const std::string& element_handle_string);

protected:
    void closeEvent(QCloseEvent* event) override;
};

#endif  // QT_APPLICATION_MAIN_WINDOW_H_
