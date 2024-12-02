#ifndef DUOPLOT_BUTTON_H
#define DUOPLOT_BUTTON_H

#include "constants.h"
#include "gui_element.h"
#include "rgbtriplet.h"

enum class ButtonStyle : uint8_t
{
    RECTANGULAR,
    ROUNDED_RECTANGULAR,
    CIRCULAR,
    ROUNDED_BAR
};

struct BaseButtonConfiguration
{
    ButtonStyle style;

    float x;
    float y;

    float edge_thickness;
    RGBTripletf edge_color;

    bool with_edge;

    BaseButtonConfiguration() = delete;
    BaseButtonConfiguration(const float x_in, const float y_in, const ButtonStyle style_in);
    BaseButtonConfiguration(const float x_in,
                            const float y_in,
                            const ButtonStyle style_in,
                            const float edge_thickness_in,
                            const RGBTripletf& edge_color_in,
                            const bool with_edge_in);

    void setEdge(const RGBTripletf& edge_color_in, const float edge_thickness_in)
    {
        if (RGBTripletf{-1.0f, -1.0f, -1.0f} == edge_color_in)
        {
            with_edge = false;
        }
        else
        {
            edge_color = edge_color_in;
            edge_thickness = edge_thickness_in;
            with_edge = true;
        }
    }
};

struct RectangularButtonConfiguration : public BaseButtonConfiguration
{
    float width;
    float height;

    RectangularButtonConfiguration() = delete;
    RectangularButtonConfiguration(const float x_in,
                                   const float y_in,
                                   const float width_in,
                                   const float height_in,
                                   const RGBTripletf& edge_color = RGBTripletf{-1.0f, -1.0f, -1.0f},
                                   const float edge_thickness_in = 3.0f);
};

struct RoundedRectangleButtonConfiguration : public BaseButtonConfiguration
{
    float width;
    float height;
    float corner_radius;

    RoundedRectangleButtonConfiguration() = delete;
    RoundedRectangleButtonConfiguration(const float x_in,
                                        const float y_in,
                                        const float width_in,
                                        const float height_in,
                                        const float corner_radius_in,
                                        const RGBTripletf& edge_color = RGBTripletf{-1.0f, -1.0f, -1.0f},
                                        const float edge_thickness_in = 3.0f);
};

struct RoundedBarButtonConfiguration : public BaseButtonConfiguration
{
    float width;
    float height;

    RoundedBarButtonConfiguration() = delete;
    RoundedBarButtonConfiguration(const float x_in,
                                  const float y_in,
                                  const float width_in,
                                  const float height_in,
                                  const RGBTripletf& edge_color = RGBTripletf{-1.0f, -1.0f, -1.0f},
                                  const float edge_thickness_in = 3.0f);
};

struct CircularButtonConfiguration : public BaseButtonConfiguration
{
    float radius;

    CircularButtonConfiguration() = delete;
    CircularButtonConfiguration(const float x_in,
                                const float y_in,
                                const float radius_in,
                                const RGBTripletf& edge_color = RGBTripletf{-1.0f, -1.0f, -1.0f},
                                const float edge_thickness_in = 3.0f);
};

struct ButtonConfiguration : public BaseButtonConfiguration
{
    float width;
    float height;
    float radius;
    float corner_radius;

    ButtonConfiguration() = delete;

    ButtonConfiguration(const RectangularButtonConfiguration& button_config)
        : BaseButtonConfiguration{button_config.x,
                                  button_config.y,
                                  button_config.style,
                                  button_config.edge_thickness,
                                  button_config.edge_color,
                                  button_config.with_edge}
    {
        width = button_config.width;
        height = button_config.height;
    }

    ButtonConfiguration(const RoundedRectangleButtonConfiguration& button_config)
        : BaseButtonConfiguration{button_config.x,
                                  button_config.y,
                                  button_config.style,
                                  button_config.edge_thickness,
                                  button_config.edge_color,
                                  button_config.with_edge}
    {
        width = button_config.width;
        height = button_config.height;
        corner_radius = button_config.corner_radius;
    }

    ButtonConfiguration(const RoundedBarButtonConfiguration& button_config)
        : BaseButtonConfiguration{button_config.x,
                                  button_config.y,
                                  button_config.style,
                                  button_config.edge_thickness,
                                  button_config.edge_color,
                                  button_config.with_edge}
    {
        width = button_config.width;
        height = button_config.height;
    }

    ButtonConfiguration(const CircularButtonConfiguration& button_config)
        : BaseButtonConfiguration{button_config.x,
                                  button_config.y,
                                  button_config.style,
                                  button_config.edge_thickness,
                                  button_config.edge_color,
                                  button_config.with_edge}
    {
        radius = button_config.radius;
    }
};

class Button : public GuiElement
{
private:
    std::string label_;
    std::function<void(uint64_t)> button_pressed_callback_;
    std::function<void(uint64_t)> button_released_callback_;

    VertexBuffer vertex_buffer_2_;
    VertexBuffer edge_vertex_buffer_;
    size_t num_points_to_render_;
    float corner_radius_;
    float edge_thickness_;
    float radius_;
    bool with_edge_;
    RGBTripletf edge_color_;

    BufferedVector button_buffer_;
    BufferedVector button_edge_buffer_;
    ButtonStyle style_;
    std::function<void(RGBTripletf)> set_shader_color_;

    size_t getNumPointsToRender(const ButtonStyle style) const;
    void UpdatePositionOrSize(const float delta_x,
                              const float delta_y,
                              const float delta_width,
                              const float delta_height,
                              const ChangeDirection change_direction) override;

    void changeButtonPoints();

public:
    Button(const ButtonConfiguration& button_configuration,
           const std::string label,
           const RGBTripletf& color,
           const std::function<void(RGBTripletf)>& set_shader_color,
           const std::function<void(uint64_t)>& button_pressed_callback,
           const std::function<void(uint64_t)>& button_released_callback);
    ~Button();
    void mousePressed(wxMouseEvent& event) override;
    void mouseReleased(wxMouseEvent& event) override;

    void render() const override;
    void updateVertexBuffer() override;
};

#endif  // DUOPLOT_BUTTON_H
