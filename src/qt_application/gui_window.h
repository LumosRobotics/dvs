#ifndef QT_APPLICATION_GUI_WINDOW_H_
#define QT_APPLICATION_GUI_WINDOW_H_

#include <QMainWindow>
#include <QMenuBar>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QCloseEvent>
#include <QResizeEvent>

#include <functional>
#include <map>
#include <string>
#include <vector>

#include "cmdl_output_window.h"
#include "editing_overlay.h"
#include "gui_element.h"
#include "help_pane.h"
#include "project_state/project_settings.h"
#include "topic_text_output_window.h"
#include "window_tab.h"

class GuiWindow : public QMainWindow
{
    Q_OBJECT

private:
    std::vector<WindowTab*> tabs_;

    // Custom tab strip (vertical buttons on the left, hidden when only 1 tab)
    QWidget*     tab_button_panel_;
    QVBoxLayout* tab_button_layout_;
    std::vector<QPushButton*> tab_buttons_qt_;   // label buttons (checkable)
    std::vector<QWidget*>     tab_row_widgets_;  // container per tab (label btn + × btn)

    // Content area: parent for all WindowTab element widgets
    QWidget* content_area_;

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
    std::string last_clicked_item_;  // handle of element that received the last right-click

    // Menu bar
    QMenuBar* menu_bar_;

    // Popup menus (right-click context menus)
    QMenu* popup_menu_window_;   // right-click on window background
    QMenu* popup_menu_element_;  // right-click on a GUI element

    // Output & help windows (owned; Phase 3)
    CmdlOutputWindow*      cmdl_output_window_;
    TopicTextOutputWindow* topic_output_window_;
    HelpPane*              help_pane_;

    // Edit-mode overlay (Phase 5)
    EditingOverlay* editing_overlay_;

    // Edit-mode drag state
    bool                   ctrl_drag_active_{false};
    ApplicationGuiElement* element_being_dragged_{nullptr};
    CursorSquareState      drag_cursor_state_{CursorSquareState::DEFAULT};
    QRect                  element_rect_at_press_;
    QPoint                 mouse_content_at_press_;

    // --- private methods ---
    void setupMenuBar();
    void setupPopupMenus();
    void updateTabButtonPanel();
    void switchToTab(int index);

    // Edit-mode helpers
    void installEditFilter(ApplicationGuiElement* elem);
    void installEditFiltersOnAllElements();
    static CursorSquareState computeCursorState(const QRect& widget_rect, const QPoint& local_pos);

    // Menu / action handlers
    void onNewTab();
    void onNewPlotPane();
    void onEditWindowName();
    void onEditTabName();
    void onDeleteTab();
    void onEditElementName();
    void onDeleteElement();
    void onOpenSerialPort();

    void onNewButton();
    void onNewSlider();
    void onNewCheckbox();
    void onNewTextLabel();
    void onNewEditableText();
    void onNewDropdownMenu();
    void onNewRadioButtonGroup();
    void onNewListBox();
    void onNewScrollingText();

    void onShowCmdlOutput();
    void onShowTopicOutput();
    void onShowHelp();

signals:
    void serialConnectRequested(const QString& port, int baudrate);
    void guiElementCreated(ApplicationGuiElement* element);
    void newProjectRequested();
    void openProjectRequested();
    void saveProjectRequested();
    void saveProjectAsRequested();
    void newWindowRequested();

protected:
    void closeEvent(QCloseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void contextMenuEvent(QContextMenuEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

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

    void screenshot(const std::string& base_path);

    void createNewPlotPane();
    void createNewPlotPane(const std::string& handle_string);

    ApplicationGuiElement* createNewButtonForTab(const std::shared_ptr<ButtonSettings>& settings);
    ApplicationGuiElement* createNewSliderForTab(const std::shared_ptr<SliderSettings>& settings);
    ApplicationGuiElement* createNewCheckboxForTab(const std::shared_ptr<CheckboxSettings>& settings);
    ApplicationGuiElement* createNewTextLabelForTab(const std::shared_ptr<TextLabelSettings>& settings);
    ApplicationGuiElement* createNewEditableTextForTab(const std::shared_ptr<EditableTextSettings>& settings);
    ApplicationGuiElement* createNewDropdownMenuForTab(const std::shared_ptr<DropdownMenuSettings>& settings);
    ApplicationGuiElement* createNewRadioButtonGroupForTab(const std::shared_ptr<RadioButtonGroupSettings>& settings);
    ApplicationGuiElement* createNewListBoxForTab(const std::shared_ptr<ListBoxSettings>& settings);
    ApplicationGuiElement* createNewScrollingTextForTab(const std::shared_ptr<ScrollingTextSettings>& settings);

    void updateAllElements();
    std::vector<ApplicationGuiElement*> getGuiElements() const;
    std::vector<ApplicationGuiElement*> getPlotPanes() const;
    std::vector<ApplicationGuiElement*> getAllGuiElements() const;

    std::vector<std::string> getElementNames() const;
};

#endif  // QT_APPLICATION_GUI_WINDOW_H_
