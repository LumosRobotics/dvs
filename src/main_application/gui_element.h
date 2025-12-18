#ifndef MAIN_APPLICATION_GUI_ELEMENT_H_
#define MAIN_APPLICATION_GUI_ELEMENT_H_

#include <wx/notebook.h>
#include <wx/wx.h>

#include <functional>
#include <memory>
#include <queue>

#include "axes/structures/grid_vectors.h"
#include "color.h"
#include "communication/received_data.h"
#include "lumos/plotting/enumerations.h"
#include "lumos/math.h"
#include "gui_element_state.h"
#include "input_data.h"
#include "project_state/project_settings.h"

using namespace lumos;

enum class CursorSquareState
{
    INSIDE,
    OUTSIDE,
    LEFT,
    RIGHT,
    TOP,
    BOTTOM,
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT
};

struct Bound2D
{
    float x_min;
    float x_max;
    float y_min;
    float y_max;

    Bound2D() = default;

    bool pointIsWithin(const wxPoint& point) const
    {
        return (x_min <= point.x) && (point.x <= x_max) && (y_min <= point.y) && (point.y <= y_max);
    }
};

class ApplicationGuiElement
{
protected:
    std::shared_ptr<ElementSettings> element_settings_;

    std::function<void(const char key)> notify_main_window_key_pressed_;
    std::function<void(const char key)> notify_main_window_key_released_;
    std::function<void(const wxPoint pos, const std::string& elem_name)> notify_parent_window_right_mouse_pressed_;
    std::function<void()> notify_main_window_about_modification_;
    std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)> notify_tab_about_editing_;
    std::function<void(const Color_t, const std::string&)> push_text_to_cmdl_output_window_;

    GuiElementId id_;

    int minimum_x_pos_;
    int minimum_y_pos_;

    int min_x_size_;
    int max_x_size_;
    int min_y_size_;
    int max_y_size_;

    bool mouse_is_inside_;

    wxPoint current_mouse_pos_;
    wxPoint previous_mouse_pos_;

    wxPoint mouse_pos_at_press_;

    wxPoint element_pos_at_press_;
    wxSize element_size_at_press_;

    bool shift_pressed_at_mouse_press_;
    bool control_pressed_at_mouse_press_;
    bool left_mouse_pressed_;

    CursorSquareState cursor_state_at_press_;

    float edit_size_margin_;

    void fillWithBasicData(FillableUInt8Array& output_array) const;
    std::uint64_t basicDataSize() const;
    virtual std::uint64_t getGuiPayloadSize() const = 0;
    virtual void fillGuiPayload(FillableUInt8Array& output_array) const = 0;

    void sendGuiData();

    std::pair<Bound2D, Bound2D> getBounds() const
    {
        Bound2D bnd;
        bnd.x_min = 0.0f;
        bnd.x_max = this->getSize().GetWidth();
        bnd.y_min = 0.0f;
        bnd.y_max = this->getSize().GetHeight();

        Bound2D bnd_with_margin;
        bnd_with_margin.x_min = bnd.x_min + edit_size_margin_;
        bnd_with_margin.x_max = bnd.x_max - edit_size_margin_;
        bnd_with_margin.y_min = bnd.y_min + edit_size_margin_;
        bnd_with_margin.y_max = bnd.y_max - edit_size_margin_;

        return std::make_pair(bnd, bnd_with_margin);
    }

public:
    ApplicationGuiElement() = delete;
    ApplicationGuiElement(
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void(const wxPoint pos, const std::string& elem_name)>&
            notify_parent_window_right_mouse_pressed,
        const std::function<void()>& notify_main_window_about_modification,
        const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
            notify_tab_about_editing,
        const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window);

    virtual ~ApplicationGuiElement() {}

    virtual void setMinXPos(const int min_x_pos) = 0;

    virtual void hide()
    {
        LUMOS_LOG_WARNING() << "hide() not implemented!";
    }

    virtual void show()
    {
        LUMOS_LOG_WARNING() << "show() not implemented!";
    }

    std::string getHandleString() const
    {
        return element_settings_->handle_string;
    }

    virtual void setHandleString(const std::string& new_name)
    {
        element_settings_->handle_string = new_name;
    }

    virtual void updateElementSettings(const std::map<std::string, std::string>& new_settings) = 0;

    std::shared_ptr<ElementSettings> getElementSettings() const
    {
        return element_settings_;
    }

    virtual void updateSizeFromParent(const wxSize& parent_size) = 0;

    virtual wxPoint getPosition() const = 0;

    virtual wxSize getSize() const = 0;

    virtual wxWindow* getParent() const = 0;

    void setCursor(const wxCursor& cursor);

    virtual void setPosition(const wxPoint& new_pos) = 0;

    virtual void setSize(const wxSize& new_size) = 0;

    virtual void setLabel(const std::string& new_label)
    {
        LUMOS_LOG_WARNING() << "setLabel() not implemented!";
    }
    virtual std::shared_ptr<GuiElementState> getGuiElementState() const
    {
        LUMOS_LOG_WARNING() << "getGuiElementState() not implemented!";
        return std::make_shared<GuiElementState>();
    }

    CursorSquareState getCursorSquareState(const Bound2D bound, const Bound2D bound_margin, const wxPoint mouse_pos);

    void setCursorDependingOnMousePos(const wxPoint& current_mouse_position);

    void adjustPaneSizeOnMouseMoved();
    virtual void setElementPositionAndSize();

    // Keyboard functions
    virtual void keyPressedElementSpecific(const char key) = 0;
    virtual void keyReleasedElementSpecific(const char key) = 0;

    void keyPressed(const char key);
    void keyReleased(const char key);

    void keyPressedCallback(wxKeyEvent& evt);
    void keyReleasedCallback(wxKeyEvent& evt);

    // Mouse functions
    virtual void mouseLeftPressedGuiElementSpecific(wxMouseEvent& event) {}
    virtual void mouseMovedGuiElementSpecific(wxMouseEvent& event) {}
    virtual void mouseLeftReleasedGuiElementSpecific(wxMouseEvent& event) {}

    virtual void mouseRightPressedGuiElementSpecific(wxMouseEvent& event) {}
    virtual void mouseRightLeftGuiElementSpecific(wxMouseEvent& event) {}

    void mouseEnteredElement(wxMouseEvent& event);
    void mouseLeftElement(wxMouseEvent& event);

    void mouseMovedOverItem(wxMouseEvent& event);

    void mouseLeftReleased(wxMouseEvent& event);
    void mouseLeftPressed(wxMouseEvent& event);

    void mouseRightReleased(wxMouseEvent& event);
    void mouseRightPressed(wxMouseEvent& event);
};

#endif  // MAIN_APPLICATION_GUI_ELEMENT_H_
