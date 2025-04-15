#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

#include <wx/wx.h>

#include <functional>
#include <string>

#include "opengl_primitives.h"
#include "rgbtriplet.h"
#include "vertex_buffer.h"

constexpr float kMinWidth = 10.0f;
constexpr float kMinHeight = 10.0f;

enum class ChangeDirection : uint8_t
{
    UP,
    DOWN,
    LEFT,
    RIGHT,
    LEFT_UP,
    LEFT_DOWN,
    RIGHT_UP,
    RIGHT_DOWN,
    MIDDLE,
    NONE
};

enum class ShaderMode : int32_t
{
    NORMAL,
    BUTTON,
    SLIDER,
    CHECKBOX
};

class GuiElement
{
protected:
    float x_;
    float y_;
    float width_;
    float height_;

    uint64_t id_;

    std::string name_;

    ShaderMode shader_mode_{ShaderMode::NORMAL};

    float parent_width_;
    float parent_height_;
    bool render_sizing_rectangles_{false};

    VertexBuffer vertex_buffer_;
    VertexBuffer vertex_buffer_sizing_rectangles_;
    BufferedVector sizing_rect_buffer_;
    RGBTripletf color_;

    void setSizingRectangles();

    virtual void updateVertexBuffer();

    virtual void UpdatePositionOrSize(const float delta_x,
                                      const float delta_y,
                                      const float delta_width,
                                      const float delta_height,
                                      const ChangeDirection change_direction);

    void renderSizingRectangles() const;

public:
    GuiElement() = default;
    GuiElement(const float x,
               const float y,
               const float width,
               const float height,
               const std::string& name,
               const RGBTripletf& color);

    void UpdateSizeFromParent(const wxSize new_size);
    bool PointIsWithin(const wxPoint pt) const;

    void preRender() const;
    virtual void render() const;
    virtual void childRender() const = 0;

    void setRenderSizingRectangles(const bool value)
    {
        render_sizing_rectangles_ = value;
    }

    void mouseEntered(wxMouseEvent& event);
    void mouseExited(wxMouseEvent& event);
    void mouseExited(wxPoint& exit_point);
    virtual void mouseMoved(wxMouseEvent& event, const wxPoint& delta_vec);
    virtual void mouseDragged(wxMouseEvent& event, const wxPoint& delta_vec);
    virtual void mousePressed(wxMouseEvent& event);
    virtual void mouseReleased(wxMouseEvent& event);
    ChangeDirection GetDirectionFromMouse(const wxPoint pt) const;
    virtual void ChangePositionOrSize(const wxPoint delta_vec, const ChangeDirection change_direction);

    ShaderMode GetShaderMode() const
    {
        return shader_mode_;
    }

    RGBTripletf getColor() const
    {
        return color_;
    }
};

#endif
