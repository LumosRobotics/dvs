#include "button.h"

#include <array>

using namespace duoplot;

constexpr size_t kNumTrianglesPerCorner = 10U;
constexpr size_t kNumPointsPerCorner = (kNumTrianglesPerCorner)*3U;
constexpr size_t kNumTrianglesForRectangularParts = 2U * 3U;
constexpr size_t kTotalNumPoints = kNumPointsPerCorner * 4U + 3U * kNumTrianglesForRectangularParts;

constexpr size_t kNumTrianglesForCircularButton = 100U;
constexpr size_t kNumPointsForCircularButton = kNumTrianglesForCircularButton * 3U;

constexpr size_t kNumTrianglesForBarEdges = 100U;
constexpr size_t kNumPointsForBarEdges = kNumTrianglesForBarEdges * 3U * 2U;

constexpr float kCornerRadius = 6.0f;
namespace
{
const RGBTripletf kButtonColor{0x58CBDDU};
constexpr float kClickedFactor = 0.8f;
const RGBTripletf kButtonPressedColor{
    kClickedFactor * 88.0f / 255.0f, kClickedFactor * 203.0f / 255.0f, kClickedFactor * 221.0f / 255.0f};

}  // namespace

Button::Button(const ButtonConfiguration& button_config,
               const std::string label,
               const RGBTripletf& color,
               const std::function<void(RGBTripletf)>& set_shader_color,
               const std::function<void(uint64_t)>& button_pressed_callback,
               const std::function<void(uint64_t)>& button_released_callback)
    : GuiElement(button_config.x, button_config.y, button_config.width, button_config.height, "BUTTON", color),
      label_(label),
      edge_color_(button_config.edge_color),
      style_{button_config.style},
      edge_thickness_(button_config.edge_thickness),
      with_edge_(button_config.with_edge),
      button_buffer_(getNumPointsToRender(button_config.style)),
      button_edge_buffer_(getNumPointsToRender(button_config.style)),
      button_pressed_callback_(button_pressed_callback),
      button_released_callback_(button_released_callback),
      vertex_buffer_2_(OGLPrimitiveType::TRIANGLES),
      edge_vertex_buffer_(OGLPrimitiveType::TRIANGLES),
      set_shader_color_(set_shader_color)
{
    shader_mode_ = ShaderMode::BUTTON;
    color_ = kButtonColor;

    if (button_config.style == ButtonStyle::RECTANGULAR)
    {
        width_ = button_config.width;
        height_ = button_config.height;
    }
    else if (button_config.style == ButtonStyle::ROUNDED_RECTANGULAR)
    {
        width_ = button_config.width;
        height_ = button_config.height;
        corner_radius_ = button_config.corner_radius;
    }
    else if (button_config.style == ButtonStyle::CIRCULAR)
    {
        radius_ = button_config.radius;
        width_ = 2.0f * radius_;
        height_ = 2.0f * radius_;
    }
    else if (button_config.style == ButtonStyle::ROUNDED_BAR)
    {
        width_ = button_config.width;
        height_ = button_config.height;
    }

    button_buffer_.setAllValuesToZero();
    button_edge_buffer_.setAllValuesToZero();

    changeButtonPoints();

    vertex_buffer_2_.addBuffer(button_buffer_.data(), num_points_to_render_, 2, GL_DYNAMIC_DRAW);
    edge_vertex_buffer_.addBuffer(button_edge_buffer_.data(), num_points_to_render_, 2, GL_DYNAMIC_DRAW);
}

Button::~Button() {}

size_t Button::getNumPointsToRender(const ButtonStyle style) const
{
    switch (style)
    {
        case ButtonStyle::RECTANGULAR:
            return 2U * 6U;
        case ButtonStyle::ROUNDED_RECTANGULAR:
            return 2U * (kNumPointsPerCorner * 4U + 3U * kNumTrianglesForRectangularParts);
        case ButtonStyle::CIRCULAR:
            return 2U * kNumPointsForCircularButton;
        case ButtonStyle::ROUNDED_BAR:
            return 2U * (kNumPointsForBarEdges + 6U);
        default:
            return 0U;
    }
}

void Button::changeButtonPoints()
{
    button_buffer_.reset();
    button_edge_buffer_.reset();

    if (style_ == ButtonStyle::RECTANGULAR)
    {
        if (with_edge_)
        {
            PutRectangleIntoBuffer(button_edge_buffer_, x_, y_, width_, height_);
            PutRectangleIntoBuffer(button_buffer_,
                                   x_ + edge_thickness_,
                                   y_ + edge_thickness_,
                                   width_ - 2.0f * edge_thickness_,
                                   height_ - 2.0f * edge_thickness_);
        }
        else
        {
            PutRectangleIntoBuffer(button_buffer_, x_, y_, width_, height_);
        }
    }
    else if (style_ == ButtonStyle::ROUNDED_RECTANGULAR)
    {
        PutRoundedRectangleIntoBuffer(button_buffer_, x_, y_, width_, height_, corner_radius_, kNumTrianglesPerCorner);
    }
    else if (style_ == ButtonStyle::CIRCULAR)
    {
        if (with_edge_)
        {
            PutCircleIntoBuffer(
                button_edge_buffer_, x_ + radius_, y_ + radius_, radius_, kNumTrianglesForCircularButton);

            PutCircleIntoBuffer(
                button_buffer_, x_ + radius_, y_ + radius_, radius_ - edge_thickness_, kNumTrianglesForCircularButton);
        }
        else
        {
            PutCircleIntoBuffer(button_buffer_, x_ + radius_, y_ + radius_, radius_, kNumTrianglesForCircularButton);
        }
    }
    else if (style_ == ButtonStyle::ROUNDED_BAR)
    {
        /*if (with_edge_)
        {
            PutRoundedBarIntoBuffer(button_buffer_,
                                    x_ + edge_thickness_ * 1.05f,
                                    y_ + edge_thickness_ * 1.05f,
                                    width_ - 2.0f * edge_thickness_ * 1.05f,
                                    height_ - 2.0f * edge_thickness_ * 1.05f,
                                    (height_ - 2.0f * edge_thickness_ * 1.05f) / 2.0f,
                                    kNumTrianglesForBarEdges);

            PutRoundedBarIntoBuffer(
                button_edge_buffer_, x_, y_, width_, height_, height_ / 2.0f, kNumTrianglesForBarEdges);
        }
        else
        {
            PutRoundedBarIntoBuffer(button_buffer_, x_, y_, width_, height_, height_ / 2.0f, kNumTrianglesForBarEdges);
        }*/
    }

    num_points_to_render_ = button_buffer_.idx() / 2U;
}

void Button::UpdatePositionOrSize(const float delta_x,
                                  const float delta_y,
                                  const float delta_width,
                                  const float delta_height,
                                  const ChangeDirection change_direction)
{
    const float x_before = x_;
    const float y_before = y_;

    if (style_ == ButtonStyle::CIRCULAR)
    {
        x_ += delta_x;
        y_ += delta_y;
        if (delta_width != 0.0f)
        {
            radius_ += delta_width * 0.5f;
        }
        else if (delta_height != 0.0f)
        {
            radius_ += delta_height * 0.5f;
        }

        if (radius_ < kMinWidth)
        {
            radius_ = kMinWidth;
            x_ = x_before;
            y_ = y_before;
        }

        width_ = 2.0f * radius_;
        height_ = 2.0f * radius_;
    }
    else
    {
        x_ += delta_x;
        y_ += delta_y;
        width_ += delta_width;
        height_ += delta_height;
    }

    if (width_ < kMinWidth)
    {
        width_ = kMinWidth;
        x_ = x_before;
    }
    if (height_ < kMinHeight)
    {
        height_ = kMinHeight;
        y_ = y_before;
    }
}

void Button::mousePressed(wxMouseEvent& event)
{
    color_ = kButtonPressedColor;

    if (button_pressed_callback_)
    {
        button_pressed_callback_(id_);
    }
}

void Button::mouseReleased(wxMouseEvent& event)
{
    color_ = kButtonColor;

    if (button_released_callback_)
    {
        button_released_callback_(id_);
    }
}

void Button::updateVertexBuffer()
{
    changeButtonPoints();

    vertex_buffer_2_.updateBufferData(0, button_buffer_.data(), num_points_to_render_, 2);
    edge_vertex_buffer_.updateBufferData(0, button_edge_buffer_.data(), num_points_to_render_, 2);
}

void Button::render() const
{
    if (with_edge_)
    {
        set_shader_color_(edge_color_);
        edge_vertex_buffer_.render(num_points_to_render_);
    }
    set_shader_color_(color_);
    vertex_buffer_2_.render(num_points_to_render_);
}

// Button configuration

BaseButtonConfiguration::BaseButtonConfiguration(const float x_in, const float y_in, const ButtonStyle style_in)
    : x{x_in}, y{y_in}, style{style_in}, with_edge{false}, edge_thickness{0.0f}, edge_color{0.0f, 0.0f, 0.0f}
{
}

BaseButtonConfiguration::BaseButtonConfiguration(const float x_in,
                                                 const float y_in,
                                                 const ButtonStyle style_in,
                                                 const float edge_thickness_in,
                                                 const RGBTripletf& edge_color_in,
                                                 const bool with_edge_in)
    : x{x_in},
      y{y_in},
      style{style_in},
      edge_thickness{edge_thickness_in},
      edge_color{edge_color_in},
      with_edge{with_edge_in}
{
}

RectangularButtonConfiguration::RectangularButtonConfiguration(const float x_in,
                                                               const float y_in,
                                                               const float width_in,
                                                               const float height_in,
                                                               const RGBTripletf& edge_color_in,
                                                               const float edge_thickness_in)
    : BaseButtonConfiguration{x_in, y_in, ButtonStyle::RECTANGULAR}, width{width_in}, height{height_in}
{
    setEdge(edge_color_in, edge_thickness_in);
}

RoundedRectangleButtonConfiguration::RoundedRectangleButtonConfiguration(const float x_in,
                                                                         const float y_in,
                                                                         const float width_in,
                                                                         const float height_in,
                                                                         const float corner_radius_in,
                                                                         const RGBTripletf& edge_color_in,
                                                                         const float edge_thickness_in)
    : BaseButtonConfiguration{x_in, y_in, ButtonStyle::ROUNDED_RECTANGULAR},
      width{width_in},
      height{height_in},
      corner_radius{corner_radius_in}
{
    setEdge(edge_color_in, edge_thickness_in);
}

RoundedBarButtonConfiguration::RoundedBarButtonConfiguration(const float x_in,
                                                             const float y_in,
                                                             const float width_in,
                                                             const float height_in,
                                                             const RGBTripletf& edge_color_in,
                                                             const float edge_thickness_in)
    : BaseButtonConfiguration{x_in, y_in, ButtonStyle::ROUNDED_BAR}, width{width_in}, height{height_in}
{
    setEdge(edge_color_in, edge_thickness_in);
}

CircularButtonConfiguration::CircularButtonConfiguration(const float x_in,
                                                         const float y_in,
                                                         const float radius_in,
                                                         const RGBTripletf& edge_color_in,
                                                         const float edge_thickness_in)
    : BaseButtonConfiguration{x_in, y_in, ButtonStyle::CIRCULAR}, radius{radius_in}
{
    setEdge(edge_color_in, edge_thickness_in);
}
