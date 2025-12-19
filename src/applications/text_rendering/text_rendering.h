#ifndef MAIN_APPLICATION_AXES_TEXT_RENDERING_H_
#define MAIN_APPLICATION_AXES_TEXT_RENDERING_H_

#include <glm/glm.hpp>
#include <string>
#include <string_view>

#include "lumos/math/math.h"
#include "opengl_textrendering/font_atlas.h"
#include "opengl_textrendering/label_text_store.h"

#include <OpenGL/gl3.h>

class TextRenderer
{
private:
    unsigned int vao_, vbo_;
    label_text_store label_store_;

public:
    TextRenderer();
    /*void renderTextFromCenter(const std::string_view& text,
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
    void renderTextTest(
        const char c_val, float x, float y, float scale, const float axes_width, const float axes_height) const;*/
    
    void renderTextNew(const std::string_view& text,
                                  float x,
                                  float y,
                                  float scale,
                                  const float axes_width,
                                  const float axes_height);
};

bool initFreetype();
lumos::Vec2f calculateStringSize(const std::string_view& text,
                               const float scale,
                               const float axes_width,
                               const float axes_height);

#endif  // MAIN_APPLICATION_AXES_TEXT_RENDERING_H_
