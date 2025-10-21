#ifndef QT_APPLICATION_GUI_ELEMENT_H_
#define QT_APPLICATION_GUI_ELEMENT_H_

#include <QPoint>
#include <QSize>

#include <functional>
#include <memory>
#include <string>

#include "project_state/project_settings.h"

// Pure interface for GUI elements - does not inherit from QWidget to avoid multiple QObject inheritance
class ApplicationGuiElement
{
protected:
    std::shared_ptr<ElementSettings> element_settings_;

    std::function<void(const char key)> notify_main_window_key_pressed_;
    std::function<void(const char key)> notify_main_window_key_released_;
    std::function<void()> notify_main_window_about_modification_;

public:
    ApplicationGuiElement() = delete;
    ApplicationGuiElement(
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void()>& notify_main_window_about_modification)
        : element_settings_(element_settings),
          notify_main_window_key_pressed_(notify_main_window_key_pressed),
          notify_main_window_key_released_(notify_main_window_key_released),
          notify_main_window_about_modification_(notify_main_window_about_modification)
    {
    }

    virtual ~ApplicationGuiElement() {}

    std::string getHandleString() const
    {
        return element_settings_->handle_string;
    }

    virtual void setHandleString(const std::string& new_name)
    {
        element_settings_->handle_string = new_name;
    }

    std::shared_ptr<ElementSettings> getElementSettings() const
    {
        return element_settings_;
    }

    // Pure virtual methods that must be implemented
    virtual void updateSizeFromParent(const QSize& parent_size) = 0;
    virtual void keyPressedElementSpecific(const char key) = 0;
    virtual void keyReleasedElementSpecific(const char key) = 0;

    // Optional overridable methods
    virtual void setMinXPos(const int min_x_pos) {}
    virtual void updateElementSettings(const std::map<std::string, std::string>& new_settings) {}
    virtual void setLabel(const std::string& new_label) {}
};

#endif  // QT_APPLICATION_GUI_ELEMENT_H_
