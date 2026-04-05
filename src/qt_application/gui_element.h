#ifndef QT_APPLICATION_GUI_ELEMENT_H_
#define QT_APPLICATION_GUI_ELEMENT_H_

#include <QJsonObject>
#include <QPoint>
#include <QRect>
#include <QSize>

#include <functional>
#include <memory>
#include <string>

#include "lumos/plotting/constants.h"
#include "lumos/plotting/fillable_uint8_array.h"
#include "lumos/plotting/internal.h"
#include "project_state/project_settings.h"

enum class CursorSquareState {
    DEFAULT = 0, MOVE,
    RESIZE_UPPER_LEFT, RESIZE_UPPER_CENTER, RESIZE_UPPER_RIGHT,
    RESIZE_CENTER_LEFT, RESIZE_CENTER_RIGHT,
    RESIZE_LOWER_LEFT, RESIZE_LOWER_CENTER, RESIZE_LOWER_RIGHT
};

// Pure interface for GUI elements - does not inherit from QWidget to avoid multiple QObject inheritance
class ApplicationGuiElement
{
protected:
    std::shared_ptr<ElementSettings> element_settings_;

    std::function<void(const char key)> notify_main_window_key_pressed_;
    std::function<void(const char key)> notify_main_window_key_released_;
    std::function<void()> notify_main_window_about_modification_;
    std::function<void(const QPoint&, const std::string&)> notify_parent_right_click_;
    std::function<void(const std::string&)> on_text_output_;

public:
    ApplicationGuiElement() = delete;

    // 3-callback constructor (backward compat)
    ApplicationGuiElement(
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void()>& notify_main_window_about_modification)
        : element_settings_(element_settings),
          notify_main_window_key_pressed_(notify_main_window_key_pressed),
          notify_main_window_key_released_(notify_main_window_key_released),
          notify_main_window_about_modification_(notify_main_window_about_modification),
          notify_parent_right_click_(),
          on_text_output_()
    {
    }

    // 5-callback constructor
    ApplicationGuiElement(
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void()>& notify_main_window_about_modification,
        const std::function<void(const QPoint&, const std::string&)>& notify_parent_right_click,
        const std::function<void(const std::string&)>& on_text_output)
        : element_settings_(element_settings),
          notify_main_window_key_pressed_(notify_main_window_key_pressed),
          notify_main_window_key_released_(notify_main_window_key_released),
          notify_main_window_about_modification_(notify_main_window_about_modification),
          notify_parent_right_click_(notify_parent_right_click),
          on_text_output_(on_text_output)
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

    // Bounding rect support
    virtual QRect getBoundingRect() const { return QRect(); }
    virtual void setBoundingRect(const QRect& r, const QSize& parent_size)
    {
        element_settings_->x = static_cast<float>(r.x()) / parent_size.width();
        element_settings_->y = static_cast<float>(r.y()) / parent_size.height();
        element_settings_->width = static_cast<float>(r.width()) / parent_size.width();
        element_settings_->height = static_cast<float>(r.height()) / parent_size.height();
    }

    // Serialization
    virtual QJsonObject serializeToJson() const { return QJsonObject{}; }
    virtual void deserializeFromJson(const QJsonObject&) {}

    // GUI data sending to client via TCP (port 9758)
    // Each interactive element overrides these to describe its payload.
    virtual std::uint64_t getGuiPayloadSize() const { return 0; }
    virtual void fillGuiPayload(FillableUInt8Array& /*output*/) const {}

    void sendGuiData() const
    {
        const std::uint8_t handle_len =
            static_cast<std::uint8_t>(element_settings_->handle_string.length());

        // Wire format: type(1) + handle_len(1) + handle(n) + payload_size(4) + payload
        const std::uint64_t payload_size = getGuiPayloadSize();
        const std::uint64_t total =
            sizeof(std::uint8_t) +   // type
            sizeof(std::uint8_t) +   // handle length
            handle_len +             // handle string
            sizeof(std::uint32_t) +  // payload size field
            payload_size;

        FillableUInt8Array arr{total};
        arr.fillWithStaticType(static_cast<std::uint8_t>(element_settings_->type));
        arr.fillWithStaticType(handle_len);
        arr.fillWithDataFromPointer(element_settings_->handle_string.data(), handle_len);
        arr.fillWithStaticType(static_cast<std::uint32_t>(payload_size));
        fillGuiPayload(arr);

        lumos::internal::sendThroughTcpInterface(arr.view(), lumos::internal::kGuiTcpPortNum);
    }
};

#endif  // QT_APPLICATION_GUI_ELEMENT_H_
