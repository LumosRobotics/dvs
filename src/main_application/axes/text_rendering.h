#ifndef MAIN_APPLICATION_AXES_TEXT_RENDERING_H_
#define MAIN_APPLICATION_AXES_TEXT_RENDERING_H_

#include <string>
#include <string_view>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "lumos/math.h"

// Forward declarations to avoid pulling GL headers into this public header.
class label_text_store;

class TextRenderer
{
public:
    TextRenderer();
    TextRenderer(const TextRenderer& other);
    TextRenderer& operator=(const TextRenderer& other);
    ~TextRenderer() = default;

    void setColor(float r, float g, float b);

    void renderTextFromCenter(const std::string_view& text,
                              float x,
                              float y,
                              float scale,
                              float axes_width,
                              float axes_height);
    void renderTextFromRightCenter(const std::string_view& text,
                                   float x,
                                   float y,
                                   float scale,
                                   float axes_width,
                                   float axes_height);
    void renderTextFromLeftCenter(const std::string_view& text,
                                  float x,
                                  float y,
                                  float scale,
                                  float axes_width,
                                  float axes_height);

    // Render all accumulated labels, then clear the batch.
    void flush();

    // Shared across all TextRenderer instances. Public so initFreetype() and
    // calculateStringSize() (free functions in text_rendering.cpp) can access them.
    static label_text_store* s_store_;
    static unsigned int      s_shader_id_;
    static bool              s_initialized_;

private:
    struct Label {
        std::string text;
        glm::vec2   loc;
        glm::vec3   color;
        float       x_scale;
        float       y_scale;
    };

    std::vector<Label> pending_labels_;
    glm::vec3          current_color_{0.0f, 0.0f, 0.0f};

};

bool initFreetype();
lumos::Vec2f calculateStringSize(const std::string_view& text,
                                 float scale,
                                 float axes_width,
                                 float axes_height);

#endif  // MAIN_APPLICATION_AXES_TEXT_RENDERING_H_
