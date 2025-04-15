#ifndef GUI_ELEMENT_CHECKBOX_H
#define GUI_ELEMENT_CHECKBOX_H

#include "constants.h"
#include "gui_element.h"
#include "rgbtriplet.h"

class Checkbox : public GuiElement
{
private:
    std::string label_;
    std::function<void(const uint64_t, const bool)> checkbox_state_changed_callback_;

    VertexBuffer vertex_buffer_2_;
    VertexBuffer check_mark_vb_;
    size_t num_points_to_render_;
    size_t num_points_to_render_check_mark_;

    float* points_;
    BufferedVector checkbox_buffer_;
    BufferedVector check_mark_buffer_;
    bool is_checked_;
    std::function<void(RGBTripletf)> set_shader_color_;

    void changeCheckboxPoints();
    void changeCheckMarkPoints();

public:
    Checkbox(const float x,
             const float y,
             const float width,
             const float height,
             const std::string label,
             const RGBTripletf& color,
             const std::function<void(RGBTripletf)>& set_shader_color,
             const std::function<void(const uint64_t, const bool)>& checkbox_state_changed_callback);
    ~Checkbox();
    void mousePressed(wxMouseEvent& event) override;
    void mouseReleased(wxMouseEvent& event) override;

    void childRender() const override;
    void updateVertexBuffer() override;
};

#endif  // GUI_ELEMENT_CHECKBOX_H
