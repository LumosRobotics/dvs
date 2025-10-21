#ifndef QT_APPLICATION_GUI_WINDOW_H_
#define QT_APPLICATION_GUI_WINDOW_H_

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QTabWidget>
#include <QCloseEvent>

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "gui_element.h"
#include "project_state/project_settings.h"
#include "window_tab.h"

class GuiWindow : public QMainWindow
{
    Q_OBJECT

private:
    std::vector<WindowTab*> tabs_;
    QTabWidget* tab_widget_;
    int element_x_offset_;
    int callback_id_;
    std::string name_;
    std::string project_name_;
    bool project_is_saved_;

    // Callbacks to MainWindow
    std::function<void(const char key)> notify_main_window_key_pressed_;
    std::function<void(const char key)> notify_main_window_key_released_;
    std::function<void(const std::string&)> notify_main_window_element_deleted_;
    std::function<std::vector<std::string>(void)> get_all_element_names_;
    std::function<void(const std::string&, const std::string&)> notify_main_window_element_name_changed_;
    std::function<void(const std::string&, const std::string&)> notify_main_window_name_changed_;
    std::function<void()> notify_main_window_about_modification_;

    int current_tab_num_;
    std::string last_clicked_item_;

    // UI elements
    QMenuBar* menu_bar_;
    QMenu* new_element_menu_;
    QMenu* popup_menu_;

    // Helper methods
    void setupMenuBar();
    void tabChanged(int index);

    // Menu callbacks
    void onNewTab();
    void onNewPlotPane();
    void onEditWindowName();
    void onEditTabName();
    void onDeleteTab();
    void onEditElementName();
    void onDeleteElement();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

public:
    GuiWindow() = delete;
    GuiWindow(
        QWidget* parent,
        const WindowSettings& window_settings,
        const QString& project_name,
        const int callback_id,
        const bool project_is_saved,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<std::vector<std::string>(void)>& get_all_element_names,
        const std::function<void(const std::string&)>& notify_main_window_element_deleted,
        const std::function<void(const std::string&, const std::string&)>& notify_main_window_element_name_changed,
        const std::function<void(const std::string&, const std::string&)>& notify_main_window_name_changed,
        const std::function<void()>& notify_main_window_about_modification);

    ~GuiWindow();

    int getCallbackId() const;
    void setName(const QString& new_name);
    void updateLabel();
    WindowSettings getWindowSettings() const;
    QString getName() const;
    void setIsFileSavedForLabel(const bool is_saved);
    void setProjectName(const QString& project_name);
    void deleteAllTabs();

    ApplicationGuiElement* getGuiElement(const std::string& element_handle_string) const;

    void notifyChildrenOnKeyPressed(const char key);
    void notifyChildrenOnKeyReleased(const char key);

    void createNewPlotPane();
    void createNewPlotPane(const std::string& handle_string);

    void updateAllElements();
    std::vector<ApplicationGuiElement*> getGuiElements() const;
    std::vector<ApplicationGuiElement*> getPlotPanes() const;
    std::vector<ApplicationGuiElement*> getAllGuiElements() const;

    std::vector<std::string> getElementNames() const;
};

#endif  // QT_APPLICATION_GUI_WINDOW_H_
