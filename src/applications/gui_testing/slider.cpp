#include "slider.h"

#include <array>

using namespace duoplot;

constexpr size_t kNumTrianglesPerEdge = 10U;
constexpr size_t kNumPointsPerEdge = (kNumTrianglesPerEdge)*3U;
constexpr size_t kNumTrianglesForRectangularParts = 2U;
constexpr size_t kTotalNumPoints = kNumPointsPerEdge * 2U + 3U * kNumTrianglesForRectangularParts;

constexpr size_t kNumCircleTriangles = 20U;
constexpr size_t kNumCirclePoints = kNumCircleTriangles * 3U;

constexpr float kEdgeRadius = 20.0f;
constexpr float kEdgeMargin = 20.0f;
constexpr float kCircleRadius = 7.0f;
constexpr float kMinSliderThickness = 5.0f;
constexpr float kMinSliderHeight = 20.0f;

namespace
{
const RGBTripletf kButtonColor{0x58CBDDU};
constexpr float kClickedFactor = 0.8f;
const RGBTripletf kButtonPressedColor{
    kClickedFactor * 88.0f / 255.0f, kClickedFactor * 203.0f / 255.0f, kClickedFactor * 221.0f / 255.0f};

}  // namespace

Slider::Slider(const float x,
               const float y,
               const float width,
               const float height,
               const int32_t min_val,
               const int32_t max_val,
               const int32_t init_val,
               const SliderDirection slider_direction,
               const std::function<void(RGBTripletf)> set_shader_color,
               const std::string label,
               const RGBTripletf& color,
               const std::function<void(const uint64_t, const int32_t)>& slider_changed_value_callback,
               const std::function<void(const uint64_t, const int32_t)>& slider_final_value_callback)
    : GuiElement(x, y, width, height, "SLIDER", color),
      label_(label),
      slider_changed_value_callback_(slider_changed_value_callback),
      slider_final_value_callback_(slider_final_value_callback),
      set_shader_color_(set_shader_color),
      min_val_(min_val),
      max_val_(max_val),
      circle_buffer_(kNumCirclePoints * 2U),
      line_buffer_{kTotalNumPoints * 2U},
      slider_direction_(slider_direction),
      current_val_(init_val),
      vertex_buffer_line_(OGLPrimitiveType::TRIANGLES),
      vertex_buffer_circle_(OGLPrimitiveType::TRIANGLES)
{
    current_val_ = std::clamp(current_val_, min_val_, max_val_);
    bar_thickness_ = kMinSliderThickness;

    height_ = std::max(bar_thickness_ + 20.0f, kMinSliderHeight);

    /*if (type_ == SliderDirection::VERTICAL)
    {
        width_ = 30.0f;
    }
    else if (type_ == SliderDirection::HORIZONTAL)
    {
        height_ = 30.0f;
    }
    else
    {
        throw std::runtime_error("Invalid slider type!");
    }*/

    shader_mode_ = ShaderMode::SLIDER;

    color_ = kButtonColor;

    std::memset(line_buffer_.data(), 0, kTotalNumPoints * 2U * sizeof(float));

    updatePoints();
    updateCirclePoints();

    vertex_buffer_line_.addBuffer(line_buffer_.data(), num_line_points_to_render_, 2, GL_DYNAMIC_DRAW);
    vertex_buffer_circle_.addBuffer(circle_buffer_.data(), num_circle_points_to_render_, 2, GL_DYNAMIC_DRAW);
}

Slider::~Slider() {}

void Slider::updateCirclePoints()
{
    circle_buffer_.reset();
    const float x_offset = static_cast<float>(current_val_ - min_val_) / static_cast<float>(max_val_ - min_val_);

    const float edge_radius = bar_thickness_ / 2.0f;
    PutCircleIntoBuffer(circle_buffer_,
                        x_ + edge_radius + kEdgeMargin + (width_ - (edge_radius + kEdgeMargin) * 2.0f) * x_offset,
                        y_ + height_ / 2.0f,
                        kCircleRadius,
                        kNumCircleTriangles);

    num_circle_points_to_render_ = circle_buffer_.idx() / 2U;
}

void Slider::UpdatePositionOrSize(const float delta_x,
                                  const float delta_y,
                                  const float delta_width,
                                  const float delta_height,
                                  const ChangeDirection change_direction)
{
    const float x_before = x_;
    const float y_before = y_;

    if (slider_direction_ == SliderDirection::HORIZONTAL)
    {
        x_ += delta_x;
        y_ += delta_y;
        width_ += delta_width;
        bar_thickness_ += delta_height;

        if (width_ < kMinWidth)
        {
            width_ = kMinWidth;
            x_ = x_before;
        }

        if (bar_thickness_ < kMinSliderThickness)
        {
            bar_thickness_ = kMinSliderThickness;
            y_ = y_before;
        }

        height_ = std::max(bar_thickness_ + 20.0f, kMinSliderHeight);
    }
    else
    {
    }
}

void Slider::updatePoints()
{
    line_buffer_.reset();

    const float edge_radius = bar_thickness_ / 2.0f;

    if (slider_direction_ == SliderDirection::HORIZONTAL)
    {
        PutRoundedBarIntoBuffer(line_buffer_,
                                x_ + kEdgeMargin,
                                y_ + height_ / 2.0f - edge_radius,
                                width_ - 2.0f * kEdgeMargin,
                                bar_thickness_,
                                true,
                                kNumTrianglesPerEdge);
    }
    else
    {
    }

    num_line_points_to_render_ = line_buffer_.idx() / 2U;
}

void Slider::mouseDragged(wxMouseEvent& event, const wxPoint& delta_vec)
{
    const wxPoint pos = event.GetPosition();

    const float edge_radius = bar_thickness_ / 2.0f;
    const float normalized_pos = std::clamp(
        static_cast<float>(pos.x - x_ - (edge_radius + kEdgeMargin)) / (width_ - (edge_radius + kEdgeMargin) * 2.0f),
        0.0f,
        1.0f);

    const int32_t new_val = static_cast<int32_t>(normalized_pos * static_cast<float>(max_val_ - min_val_)) + min_val_;
    if (new_val == current_val_)
    {
        return;
    }
    current_val_ = new_val;

    if (slider_changed_value_callback_)
    {
        slider_changed_value_callback_(id_, current_val_);
    }

    updateCirclePoints();
    vertex_buffer_circle_.updateBufferData(0, circle_buffer_.data(), num_circle_points_to_render_, 2);
}

void Slider::mousePressed(wxMouseEvent& event)
{
    const wxPoint pos = event.GetPosition();

    const float edge_radius = bar_thickness_ / 2.0f;
    const float normalized_pos = std::clamp(
        static_cast<float>(pos.x - x_ - (edge_radius + kEdgeMargin)) / (width_ - (edge_radius + kEdgeMargin) * 2.0f),
        0.0f,
        1.0f);

    const int32_t new_val = static_cast<int32_t>(normalized_pos * static_cast<float>(max_val_ - min_val_)) + min_val_;
    if (new_val == current_val_)
    {
        return;
    }
    current_val_ = new_val;

    if (slider_changed_value_callback_)
    {
        slider_changed_value_callback_(id_, current_val_);
    }

    updateCirclePoints();
    vertex_buffer_circle_.updateBufferData(0, circle_buffer_.data(), num_circle_points_to_render_, 2);
}

void Slider::mouseReleased(wxMouseEvent& event)
{
    if (slider_final_value_callback_)
    {
        slider_final_value_callback_(id_, current_val_);
    }
}

void Slider::updateVertexBuffer()
{
    updatePoints();
    updateCirclePoints();

    vertex_buffer_line_.updateBufferData(0, line_buffer_.data(), num_line_points_to_render_, 2);
    vertex_buffer_circle_.updateBufferData(0, circle_buffer_.data(), num_circle_points_to_render_, 2);
}

void Slider::childRender() const
{
    set_shader_color_(color_);
    vertex_buffer_line_.render(num_line_points_to_render_);

    set_shader_color_(RGBTripletf(1.0f, 1.0f, 1.0f));
    vertex_buffer_circle_.render(num_circle_points_to_render_);
}
