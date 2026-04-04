
#include "text_rendering.h"

#include <ft2build.h>

#include <iostream>
#include <map>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

using namespace lumos;

namespace tr
{
struct Character
{
    unsigned int texture_id;
    glm::ivec2 size;
    glm::ivec2 bearing;
    unsigned int bitmap_width;
    unsigned int bitmap_rows;
    signed int bitmap_left;
    signed int bitmap_top;
    signed int diff;
    unsigned int increment;
};

}  // namespace


std::map<GLchar, tr::Character> characters;

bool is_initialized = false;

const std::string kFontPath = "/Users/danielpi/work/dvs/src/resources/fonts/Roboto-Regular.ttf";

constexpr float kTextScaleParameter = 1000.0f;

TextRenderer::TextRenderer()
{
    /*glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vbo_);
    glBindVertexArray(vao_);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);*/

    label_store_.init();

    label_store_.add_text("Length = 98.76532101", glm::vec2(-0.8, -0.3), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f, 45, 0.0004f);
    label_store_.add_text("Big text!", glm::vec2(-1.5, -0.3), glm::vec3(1.0f, 1.0f, 0.0f), 1.0f, 0, 0.002f);
    label_store_.add_text("Pulled Pork Sandwich!!", glm::vec2(-0.4, 0), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, -15, 0.0003f);
    label_store_.add_text("Hello", glm::vec2(-0.8, 0.3), glm::vec3(1.0f, 1.0f, 1.0f), 1.0f, 5.0f, 0.0003f);

	label_store_.set_buffers();

}

/*void TextRenderer::renderTextFromCenter(
    const std::string_view& text, float x, float y, float scale, const float axes_width, const float axes_height) const
{
    const Vec2f text_size = calculateStringSize(text, scale, axes_width, axes_height);
    this->renderTextFromLeftCenter(text, x - text_size.x / 2.0f, y, scale, axes_width, axes_height);
}

void TextRenderer::renderTextFromRightCenter(
    const std::string_view& text, float x, float y, float scale, const float axes_width, const float axes_height) const
{
    const Vec2f text_size = calculateStringSize(text, scale, axes_width, axes_height);
    this->renderTextFromLeftCenter(text, x - text_size.x, y, scale, axes_width, axes_height);
}

void TextRenderer::renderTextFromLeftCenter(
    const std::string_view& text, float x, float y, float scale, const float axes_width, const float axes_height) const
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    const Vec2f text_size = calculateStringSize(text, scale, axes_width, axes_height);
    y -= text_size.y / 2.0f;

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao_);

    const float sx = kTextScaleParameter / axes_width;
    const float sy = kTextScaleParameter / axes_height;

    for (size_t k = 0; k < text.length(); k++)
    {
        const tr::Character& ch = characters[text[k]];

        float xpos = x + ch.bitmap_left * scale;
        // float ypos = y + (ch.bitmap_top - static_cast<signed int>(ch.bitmap_rows)) * scale;
        float ypos = y + ch.diff * 0.0005f;

        const float w = ch.bitmap_width * scale * sx;
        const float h = ch.bitmap_rows * scale * sy;

        const float vertices[6][4] = {{xpos, ypos + h, 0.0f, 0.0f},
                                      {xpos, ypos, 0.0f, 1.0f},
                                      {xpos + w, ypos, 1.0f, 1.0f},

                                      {xpos, ypos + h, 0.0f, 0.0f},
                                      {xpos + w, ypos, 1.0f, 1.0f},
                                      {xpos + w, ypos + h, 1.0f, 0.0f}};

        glBindTexture(GL_TEXTURE_2D, ch.texture_id);

        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        x += (ch.increment >> 6) * scale * sx;
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

void TextRenderer::renderTextTest(
    const char c_val, float x, float y, float scale, const float axes_width, const float axes_height) const
{
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA

    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(vao_);

    const float sx = kTextScaleParameter / axes_width;
    const float sy = kTextScaleParameter / axes_height;

    const tr::Character& ch = characters[c_val];

    float xpos = x;
    float ypos = y;

    const float w = ch.bitmap_width * scale * sx;
    const float h = ch.bitmap_rows * scale * sy;

    const float xy_size = 1.0f;
    const float uv_size = 1.0f;

    const float vertices[6][4] = {{-xy_size, xy_size, 0.0f, 0.0f},
                                    {-xy_size, -xy_size, 0.0f, uv_size},
                                    {xy_size, -xy_size, uv_size, uv_size},

                                    {-xy_size, xy_size, 0.0f, 0.0f},
                                    {xy_size, -xy_size, uv_size, uv_size},
                                    {xy_size, xy_size, uv_size, 0.0f}};

    glBindTexture(GL_TEXTURE_2D, ch.texture_id);

    glBindBuffer(GL_ARRAY_BUFFER, vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}*/

void TextRenderer::renderTextNew(const std::string_view& text,
                                  float x,
                                  float y,
                                  float scale,
                                  const float axes_width,
                                  const float axes_height)
{
    // glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBlendEquation(GL_FUNC_ADD);
    label_store_.paint_text();

    // glDisable(GL_CULL_FACE);
    // glDisable(GL_BLEND);
}

bool initFreetype()
{
    FT_Library ft;

    if (FT_Init_FreeType(&ft))
    {
        std::cout << "Could not initialize font library." << std::endl;
        return false;
    }

    FT_Face face;
    if (FT_New_Face(ft, kFontPath.c_str(), 0, &face))
    {
        std::cout << "Failed to load font" << std::endl;
        return false;
    }

    FT_Set_Pixel_Sizes(face, 0, 168);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    for (unsigned char c = 0; c < 128; c++)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "Could not load glyph '" << c << "'" << std::endl;
            continue;
        }

        // std::cout <<  c << ": " << (static_cast<float>(face->glyph->bitmap_top) - static_cast<float>(face->glyph->bitmap.rows)) << std::endl;

        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RED,
                     face->glyph->bitmap.width,
                     face->glyph->bitmap.rows,
                     0,
                     GL_RED,
                     GL_UNSIGNED_BYTE,
                     face->glyph->bitmap.buffer);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // GL_CLAMP_TO_BORDER or GL_CLAMP_TO_EDGE?
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const tr::Character character = {texture,
                               glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                               glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                               face->glyph->bitmap.width,
                               face->glyph->bitmap.rows,
                               face->glyph->bitmap_left,
                               face->glyph->bitmap_top,
                               static_cast<signed int>(static_cast<float>(face->glyph->bitmap_top) - static_cast<float>(face->glyph->bitmap.rows)),
                               static_cast<unsigned int>(face->glyph->advance.x)};
        characters.insert(std::pair<char, tr::Character>(c, character));
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    is_initialized = true;

    return true;
}

// Calculates relative string size in the pane it's in, which is in the interval [-1.0, 1.0]
// So a width of 2.0 covers the full pane from left to right
Vec2f calculateStringSize(const std::string_view& text,
                          const float scale,
                          const float axes_width,
                          const float axes_height)
{
    if (text.length() == 0)
    {
        return Vec2f(0.0f, 0.0f);
    }

    float x = 0.0f;

    const float sx = kTextScaleParameter / axes_width;
    const float sy = kTextScaleParameter / axes_height;

    float x_min, x_max, y_min, y_max;

    tr::Character ch = characters[text[0]];

    x_min = ch.bearing.x * scale;
    x_max = x_min;
    y_min = -(ch.size.y - ch.bearing.y) * scale;
    y_max = y_min;

    for (size_t k = 0; k < text.length(); k++)
    {
        const tr::Character& ch = characters[text[k]];

        const float xpos = x + ch.bearing.x * scale;
        const float ypos = -(ch.size.y - ch.bearing.y) * scale;

        const float w = ch.size.x * scale * sx;
        const float h = ch.size.y * scale * sy;

        x_min = std::min(x_min, xpos);
        x_min = std::min(x_min, xpos + w);

        x_max = std::max(x_min, xpos);
        x_max = std::max(x_min, xpos + w);

        y_min = std::min(y_min, ypos);
        y_min = std::min(y_min, ypos + h);

        y_max = std::max(y_min, ypos);
        y_max = std::max(y_min, ypos + h);

        x += (ch.increment >> 6) * scale * sx;
    }

    return Vec2f(x_max - x_min, y_max - y_min);
}
