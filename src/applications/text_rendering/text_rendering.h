#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

// Forward declarations — keeps GL/impl headers out of the public interface.
class label_text_store;
class shader;

class text_renderer
{
public:
    text_renderer();
    ~text_renderer();
    text_renderer(const text_renderer&)            = delete;
    text_renderer& operator=(const text_renderer&) = delete;

    // Load font file.  Call before add_text / build.
    bool init(const std::string& font_path, uint32_t pixel_size = 128);

    void add_text(const std::string& label,
                  glm::vec2 text_loc,
                  glm::vec3 text_color,
                  float     font_angle,
                  float     font_size);

    // Shape all labels, build atlas, upload GPU buffers.  Call once after add_text.
    void build();

    // Bind shader, draw, unbind.  Call every frame.
    void render(float zoom_scale = 1.0f);

private:
    label_text_store* store_;
    shader*           text_shader_;
    bool              built_ = false;

    bool init_shader();
};
