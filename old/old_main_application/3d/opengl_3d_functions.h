#ifndef MAIN_APPLICATION_OLD_3D_OPENGL_3D_FUNCTIONS_H_
#define MAIN_APPLICATION_OLD_3D_OPENGL_3D_FUNCTIONS_H_

#include <cstddef>
#include <vector>

#include "duoplot/math/math.h"
#include "misc/color_map.h"

using namespace duoplot;

void drawLines3D(const Vectord& x_values, const Vectord& y_values, const Vectord& z_values);
void drawTriangle3D(const Vec3d& v0, const Vec3d& v1, const Vec3d& v2);
void drawLine3D(const double x0, const double y0, const double z0, const double x1, const double y1, const double z1);
void drawLine3D(const Vec3d& v0, const Vec3d& v1);
void drawPolygon3D(const Vec3d& c0, const Vec3d& c1, const Vec3d& c2, const Vec3d& c3);
void drawPoints3D(const Vectord& x_values, const Vectord& y_values, const Vectord& z_values);
void drawTriangle3D(const Triangle3D<double>& triangle);
void drawTriangleEdge3D(const Triangle3D<double>& triangle);

#endif  // MAIN_APPLICATION_OLD_3D_OPENGL_3D_FUNCTIONS_H_
