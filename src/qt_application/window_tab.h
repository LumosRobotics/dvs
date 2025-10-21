#ifndef QT_APPLICATION_WINDOW_TAB_H_
#define QT_APPLICATION_WINDOW_TAB_H_

#include <QWidget>

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "gui_element.h"
#include "misc/rgb_triplet.h"
#include "plot_pane.h"
#include "project_state/project_settings.h"

class WindowTab
{
private:
    std::string name_;
    std::vector<PlotPane*> plot_panes_;
    std::vector<ApplicationGuiElement*> gui_elements_;

    QWidget* parent_widget_;

    std::function<void(const char key)> notify_main_window_key_pressed_;
    std::function<void(const char key)> notify_main_window_key_released_;
    std::function<void(const QPoint& pos, const std::string& elem_name)> notify_parent_window_right_mouse_pressed_;
    std::function<void(const std::string&)> notify_main_window_element_deleted_;
    std::function<void()> notify_main_window_about_modification_;

    int current_element_idx_;
    RGBTripletf background_color_;
    int element_x_offset_;

public:
    WindowTab(QWidget* parent_widget,
              const TabSettings& tab_settings,
              const int element_x_offset,
              const std::function<void(const char key)>& notify_main_window_key_pressed,
              const std::function<void(const char key)>& notify_main_window_key_released,
              const std::function<void(const QPoint& pos, const std::string& elem_name)>&
                  notify_parent_window_right_mouse_pressed,
              const std::function<void(const std::string&)>& notify_main_window_element_deleted,
              const std::function<void()>& notify_main_window_about_modification);

    ~WindowTab();

    std::vector<ApplicationGuiElement*> getPlotPanes() const;
    std::vector<ApplicationGuiElement*> getGuiElements() const;
    std::vector<ApplicationGuiElement*> getAllGuiElements() const;

    void updateAllElements();
    void setMinXPos(const int min_x_pos);

    void createNewPlotPane();
    void createNewPlotPane(const std::string& element_handle_string);
    void createNewPlotPane(const std::shared_ptr<ElementSettings>& element_settings);

    void show();
    void hide();
    void updateSizeFromParent(const QSize new_size) const;

    RGBTripletf getBackgroundColor() const;
    std::string getName() const;
    TabSettings getTabSettings() const;

    ApplicationGuiElement* getGuiElement(const std::string& element_handle_string) const;

    void notifyChildrenOnKeyPressed(const char key);
    void notifyChildrenOnKeyReleased(const char key);

    bool deleteElementIfItExists(const std::string& element_handle_string);
    bool elementWithNameExists(const std::string& element_handle_string);

    std::vector<std::string> getElementNames() const;
    void setName(const std::string& new_name);
};

#endif  // QT_APPLICATION_WINDOW_TAB_H_
