#include "checkbox.h"

#include <array>

using namespace duoplot;

constexpr size_t kNumRectanglesPerCorner = 10U;
constexpr size_t kNumPointsPerCorner = kNumRectanglesPerCorner * 2U * 3U;
constexpr size_t kNumTrianglesForRectangularParts = 2U * 3U;
constexpr size_t kTotalNumPoints = kNumPointsPerCorner * 4U + kNumTrianglesForRectangularParts * 4U;

constexpr float kCornerRadius = 30.0f;
constexpr float kEdgeThickness = 20.0f;
namespace
{
const RGBTripletf kCheckMarkColor{0.0f, 0.6f, 0.0f};
const RGBTripletf kButtonColor{0.6f, 0.5f, 0.4f};
constexpr float kClickedFactor = 0.8f;
const RGBTripletf kButtonPressedColor{
    kClickedFactor * 88.0f / 255.0f, kClickedFactor * 203.0f / 255.0f, kClickedFactor * 221.0f / 255.0f};


constexpr size_t kNumPointsPerRectangle = 6U;
constexpr size_t kNumFromRectangles = 2U * kNumPointsPerRectangle;
constexpr size_t kNumCircleTriangles = 10U;
constexpr size_t kNumPointsFromCircleEdge = kNumCircleTriangles * 3U;
constexpr size_t kNumPointsFromEdges = kNumPointsFromCircleEdge * 2U;
constexpr size_t kNumPointsFromCheckMark = kNumPointsFromEdges + kNumFromRectangles;
}  // namespace

Checkbox::Checkbox(const float x,
                   const float y,
                   const float width,
                   const float height,
                   const std::string label,
                   const RGBTripletf& color,
                   const std::function<void(RGBTripletf)>& set_shader_color,
                   const std::function<void(const uint64_t, const bool)>& checkbox_state_changed_callback)
    : GuiElement(x, y, width, height, "CHECKBOX", color),
      label_(label),
      checkbox_state_changed_callback_(checkbox_state_changed_callback),
      checkbox_buffer_{kTotalNumPoints * 2U},
      check_mark_buffer_{kNumPointsFromCheckMark * 2U},
      set_shader_color_(set_shader_color),
      vertex_buffer_2_(OGLPrimitiveType::TRIANGLES),
      check_mark_vb_{OGLPrimitiveType::TRIANGLES}
{
    is_checked_ = false;
    shader_mode_ = ShaderMode::CHECKBOX;
    color_ = kButtonColor;

    std::memset(checkbox_buffer_.data(), 0, kTotalNumPoints * 2U * sizeof(float));

    changeCheckboxPoints();
    changeCheckMarkPoints();

    vertex_buffer_2_.addBuffer(checkbox_buffer_.data(), num_points_to_render_, 2, GL_DYNAMIC_DRAW);
    check_mark_vb_.addBuffer(check_mark_buffer_.data(), num_points_to_render_check_mark_, 2, GL_DYNAMIC_DRAW);
}

Checkbox::~Checkbox() {}

void Checkbox::changeCheckboxPoints()
{
    checkbox_buffer_.reset();

    PutRoundedRectangleEdgeIntoBuffer(
        checkbox_buffer_, x_, y_, width_, width_, kEdgeThickness, kCornerRadius, kNumRectanglesPerCorner);

    num_points_to_render_ = checkbox_buffer_.idx() / 2U;
}

void Checkbox::changeCheckMarkPoints()
{
    check_mark_buffer_.reset();
    const float first_rect_width = width_ / 5.0f;
    const float first_rect_height = 2.0f * width_ / 5.0f;

    const float second_rect_width = width_ / 5.0f;
    const float second_rect_height = 5.0f * width_ / 5.0f;

    const float o_x = x_ + width_ / 4.0f;
    const float o_y = y_ + width_ / 2.0f;

    PutRotatedRectangleIntoBuffer(
        check_mark_buffer_, o_x, o_y, first_rect_width, first_rect_height, M_PI / 4.0f);

    constexpr float sqrt_2 = 1.41421356237f;

    PutRotatedRectangleIntoBuffer(
        check_mark_buffer_,
        o_x + first_rect_width * sqrt_2 + 5.0f,
        o_y + first_rect_width * sqrt_2 + 5.0f,
        second_rect_width, second_rect_height, M_PI / 4.0f + M_PI / 2.0f);

    num_points_to_render_check_mark_ = check_mark_buffer_.idx() / 2U;
}

void Checkbox::mousePressed(wxMouseEvent& event)
{
    color_ = kButtonPressedColor;
}

void Checkbox::mouseReleased(wxMouseEvent& event)
{
    color_ = kButtonColor;
    is_checked_ = !is_checked_;

    if (checkbox_state_changed_callback_)
    {
        checkbox_state_changed_callback_(id_, is_checked_);
    }
}

void Checkbox::updateVertexBuffer()
{
    changeCheckboxPoints();
    changeCheckMarkPoints();

    vertex_buffer_2_.updateBufferData(0, checkbox_buffer_.data(), num_points_to_render_, 2);
    check_mark_vb_.updateBufferData(0, check_mark_buffer_.data(), num_points_to_render_check_mark_, 2);
}

void Checkbox::childRender() const
{
    set_shader_color_(color_);
    vertex_buffer_2_.render(num_points_to_render_);
    // if(is_checked_)
    {
        set_shader_color_(kCheckMarkColor);
        check_mark_vb_.render(num_points_to_render_check_mark_);
    }
}
