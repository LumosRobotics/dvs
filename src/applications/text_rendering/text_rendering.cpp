// text_rendering.cpp — implementation of text_renderer.
// Include GLEW first so the buffer headers see __GLEW_H__ and skip gl3.h.
#include "opengl_textrendering/shaders/shader.h"
#include "opengl_textrendering/label_text_store.h"
#include "text_rendering.h"

namespace {

constexpr char kVertSrc[] = R"glsl(
#version 330 core

uniform float zoomscale;

layout(location = 0) in vec2 position;
layout(location = 1) in vec2 origin;
layout(location = 2) in vec2 textureCoord;
layout(location = 3) in vec3 color;

out vec3 v_textureColor;
out vec2 v_textureCoord;

void main()
{
    mat4 scalingMatrix = mat4(1.0) * zoomscale;
    scalingMatrix[3][3] = 1.0;

    vec4 finalPosition   = scalingMatrix * vec4(position, 0.0, 1.0);
    vec4 finalTextOrigin = scalingMatrix * vec4(origin,   0.0, 1.0);

    // Offset from origin, un-zoomed, so text size stays fixed in screen space.
    vec2 scaled_pt = vec2(finalPosition.x - finalTextOrigin.x,
                          finalPosition.y - finalTextOrigin.y) / zoomscale;

    gl_Position    = vec4(finalPosition.x + scaled_pt.x,
                          finalPosition.y + scaled_pt.y, 0.0, 1.0);
    v_textureCoord = textureCoord;
    v_textureColor = color;
}
)glsl";

constexpr char kFragSrc[] = R"glsl(
#version 330 core
uniform sampler2D u_Texture;

in vec3 v_textureColor;
in vec2 v_textureCoord;

out vec4 f_Color;

void main()
{
    vec4 texColor = vec4(1.0, 1.0, 1.0, texture(u_Texture, v_textureCoord).r);
    f_Color = vec4(v_textureColor, 1.0f) * texColor;
}
)glsl";

} // namespace

text_renderer::text_renderer()
    : store_(new label_text_store{}), text_shader_(new shader{})
{
}

text_renderer::~text_renderer()
{
    delete store_;
    delete text_shader_;
}

bool text_renderer::init(const std::string& font_path, uint32_t pixel_size)
{
    // Initialize GLEW if it hasn't been already (safe to call multiple times).
    glewInit();
    store_->init(font_path, pixel_size);
    return init_shader();
}

bool text_renderer::init_shader()
{
    text_shader_->create_shader_from_source(kVertSrc, kFragSrc);
    text_shader_->setUniform("u_Texture", 0);
    return text_shader_->get_shader_id() != 0;
}

void text_renderer::add_text(const std::string& label, glm::vec2 loc,
                              glm::vec3 color, float angle, float size)
{
    store_->add_text(label, loc, color, angle, size);
}

void text_renderer::build()
{
    store_->set_buffers();
    built_ = true;
}

void text_renderer::render(float zoom_scale)
{
    if (!built_) return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    text_shader_->Bind();
    // Use glUniform directly — shader::setUniform() calls Bind/UnBind internally
    // which would unbind the shader before paint_text() runs.
    glUniform1f(glGetUniformLocation(text_shader_->get_shader_id(), "zoomscale"), zoom_scale);
    store_->paint_text();
    text_shader_->UnBind();
}
