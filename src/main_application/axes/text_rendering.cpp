// text_rendering.cpp — HarfBuzz/atlas-based text renderer for duoplot.
// opengl_header.h must come first so GL symbols are defined before any other
// header tries to use them.
#include "opengl_low_level/opengl_header.h"

#include <hb.h>
#include <hb-ft.h>

#include "axes/text_rendering_impl/label_text_store.h"
#include "platform_paths.h"
#include "text_rendering.h"

#include <iostream>

using namespace lumos;

namespace {

constexpr float kTextScaleParameter = 1000.0f;

const std::string kFontPath = getResourcesPathString() + "fonts/Roboto-Regular.ttf";

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

GLuint compileShader(GLenum type, const char* src)
{
    GLuint id = glCreateShader(type);
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    GLint success = 0;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << "\n";
    }
    return id;
}

}  // namespace

// Static member definitions.
label_text_store* TextRenderer::s_store_     = nullptr;
unsigned int      TextRenderer::s_shader_id_ = 0;
bool              TextRenderer::s_initialized_ = false;

// ---- TextRenderer ----

TextRenderer::TextRenderer() {}

TextRenderer::TextRenderer(const TextRenderer& other)
    : current_color_(other.current_color_)
{
    // pending_labels_ starts empty — don't copy in-flight labels.
}

TextRenderer& TextRenderer::operator=(const TextRenderer& other)
{
    if (this != &other) {
        current_color_ = other.current_color_;
        pending_labels_.clear();
    }
    return *this;
}

void TextRenderer::setColor(float r, float g, float b)
{
    current_color_ = glm::vec3(r, g, b);
}

void TextRenderer::renderTextFromCenter(const std::string_view& text,
                                        float x, float y,
                                        float scale,
                                        float axes_width, float axes_height)
{
    const Vec2f sz = calculateStringSize(text, scale, axes_width, axes_height);
    renderTextFromLeftCenter(text, x - sz.x / 2.0f, y, scale, axes_width, axes_height);
}

void TextRenderer::renderTextFromRightCenter(const std::string_view& text,
                                             float x, float y,
                                             float scale,
                                             float axes_width, float axes_height)
{
    const Vec2f sz = calculateStringSize(text, scale, axes_width, axes_height);
    renderTextFromLeftCenter(text, x - sz.x, y, scale, axes_width, axes_height);
}

void TextRenderer::renderTextFromLeftCenter(const std::string_view& text,
                                            float x, float y,
                                            float scale,
                                            float axes_width, float axes_height)
{
    if (!s_initialized_ || text.empty()) return;

    const float x_scale = scale * kTextScaleParameter / axes_width;
    const float y_scale = scale * kTextScaleParameter / axes_height;

    // Shift y down by half the text height for vertical centering.
    const Vec2f sz = calculateStringSize(text, scale, axes_width, axes_height);
    const float loc_y = y - sz.y / 2.0f;

    pending_labels_.push_back({std::string(text),
                               glm::vec2(x, loc_y),
                               current_color_,
                               x_scale,
                               y_scale});
}

void TextRenderer::flush()
{
    if (!s_initialized_ || pending_labels_.empty()) return;

    s_store_->labels.clear();
    for (const auto& lbl : pending_labels_) {
        s_store_->add_text(lbl.text, lbl.loc, lbl.color, 0.0f, lbl.x_scale, lbl.y_scale);
    }
    s_store_->set_buffers();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);

    glUseProgram(s_shader_id_);
    glUniform1i(glGetUniformLocation(s_shader_id_, "u_Texture"), 0);
    glUniform1f(glGetUniformLocation(s_shader_id_, "zoomscale"), 1.0f);

    s_store_->paint_text();

    glUseProgram(0);
    glDisable(GL_BLEND);

    pending_labels_.clear();
}

// ---- Free functions ----

bool initFreetype()
{
    if (TextRenderer::s_initialized_) return true;

    TextRenderer::s_store_ = new label_text_store();
    if (!TextRenderer::s_store_->main_font.init(kFontPath, 68)) {
        std::cerr << "initFreetype: failed to load font: " << kFontPath << "\n";
        delete TextRenderer::s_store_;
        TextRenderer::s_store_ = nullptr;
        return false;
    }

    // Compile and link the text shader.
    GLuint vs = compileShader(GL_VERTEX_SHADER,   kVertSrc);
    GLuint fs = compileShader(GL_FRAGMENT_SHADER, kFragSrc);
    TextRenderer::s_shader_id_ = glCreateProgram();
    glAttachShader(TextRenderer::s_shader_id_, vs);
    glAttachShader(TextRenderer::s_shader_id_, fs);
    glLinkProgram(TextRenderer::s_shader_id_);

    GLint success = 0;
    glGetProgramiv(TextRenderer::s_shader_id_, GL_LINK_STATUS, &success);
    if (!success) {
        char log[512];
        glGetProgramInfoLog(TextRenderer::s_shader_id_, 512, nullptr, log);
        std::cerr << "Text shader link error: " << log << "\n";
        TextRenderer::s_shader_id_ = 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    TextRenderer::s_initialized_ = (TextRenderer::s_shader_id_ != 0);
    return TextRenderer::s_initialized_;
}

Vec2f calculateStringSize(const std::string_view& text,
                          const float scale,
                          const float axes_width,
                          const float axes_height)
{
    if (!TextRenderer::s_initialized_ || text.empty()) return Vec2f(0.0f, 0.0f);

    const std::string str(text);
    hb_buffer_t* hb_buf = hb_buffer_create();
    hb_buffer_add_utf8(hb_buf, str.c_str(), -1, 0, -1);
    hb_buffer_guess_segment_properties(hb_buf);
    hb_shape(TextRenderer::s_store_->main_font.hb_font, hb_buf, nullptr, 0);

    unsigned int         glyph_count = 0;
    hb_glyph_position_t* glyph_pos   = hb_buffer_get_glyph_positions(hb_buf, &glyph_count);

    float total_advance = 0.0f;
    for (unsigned int i = 0; i < glyph_count; i++) {
        total_advance += glyph_pos[i].x_advance / 64.0f;
    }
    hb_buffer_destroy(hb_buf);

    const float x_scale = scale * kTextScaleParameter / axes_width;
    const float y_scale = scale * kTextScaleParameter / axes_height;

    // The vertex shader (zoomscale=1) doubles offsets from the text origin.
    // Account for that here so centering math is consistent.
    const float width  = total_advance * x_scale * 2.0f;

    // Approximate height from the font's ascender value.
    const float ascender = TextRenderer::s_store_->main_font.ft_face
                               ? (TextRenderer::s_store_->main_font.ft_face->ascender / 64.0f)
                               : 0.0f;
    const float height = ascender * y_scale * 2.0f;

    return Vec2f(width, height);
}
