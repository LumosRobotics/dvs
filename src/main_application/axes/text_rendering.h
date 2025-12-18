#ifndef MAIN_APPLICATION_AXES_TEXT_RENDERING_H_
#define MAIN_APPLICATION_AXES_TEXT_RENDERING_H_

#include <glm/glm.hpp>
#include <string>
#include <string_view>

#include "duoplot/math/math.h"
#include "opengl_low_level/opengl_header.h"

class TextRenderer
{
private:
    unsigned int vao, vbo;

public:
    TextRenderer();
    void renderTextFromCenter(const std::string_view& text,
                              float x,
                              float y,
                              float scale,
                              const float axes_width,
                              const float axes_height) const;
    void renderTextFromRightCenter(const std::string_view& text,
                                   float x,
                                   float y,
                                   float scale,
                                   const float axes_width,
                                   const float axes_height) const;
    void renderTextFromLeftCenter(const std::string_view& text,
                                  float x,
                                  float y,
                                  float scale,
                                  const float axes_width,
                                  const float axes_height) const;
};

bool initFreetype();
duoplot::Vec2f calculateStringSize(const std::string_view& text,
                               const float scale,
                               const float axes_width,
                               const float axes_height);

#endif  // MAIN_APPLICATION_AXES_TEXT_RENDERING_H_
