#ifndef MAIN_APPLICATION_OLD_OPENGL_TEXT_H_
#define MAIN_APPLICATION_OLD_OPENGL_TEXT_H_

#include <string>

#include "duoplot/math/math.h"

using namespace duoplot;

/*void drawTextFromBottomRight(const std::string s, float x, float y);
void drawTextFromBottomLeft(const std::string s, float x, float y);
void drawTextFromTopRight(const std::string s, float x, float y);
void drawTextFromTopLeft(const std::string s, float x, float y);

double calculateStringWidth(const std::string& s);
double calculateStringHeight();*/

void putTextAt(
    const std::string& text, const double x, const double y, const float size_x, const float size_y, const float scale);
void putTextAtNew(const std::string& text);
void putTextAt_Old(const std::string& s, const Vec2d& v);
void putTextAt3D(const std::string& s, const Vec3d& v);
void putTextAt_Old(const std::string& s, const double x, const double y);
void putTextAt3D(const std::string& s, const double x, const double y, const double z);

#endif  // MAIN_APPLICATION_OLD_OPENGL_TEXT_H_
