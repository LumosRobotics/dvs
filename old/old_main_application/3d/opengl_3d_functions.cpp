
#include "opengl_low_level/3d/opengl_3d_functions.h"

#include <assert.h>

#include <vector>

#include "opengl_low_level/opengl_header.h"

void drawLines3D(const Vectord& x_values, const Vectord& y_values, const Vectord& z_values)
{
    assert(x_values.size() > 1);
    assert(x_values.size() == y_values.size());
    assert(x_values.size() == z_values.size());

    glBegin(GL_LINES);
    for (size_t k = 0; k < x_values.size() - 1; k++)
    {
        glVertex3f(x_values(k), y_values(k), z_values(k));
        glVertex3f(x_values(k + 1), y_values(k + 1), z_values(k + 1));
    }

    glEnd();
}

void drawPoints3D(const Vectord& x_values, const Vectord& y_values, const Vectord& z_values)
{
    assert(x_values.size() > 1);
    assert(x_values.size() == y_values.size());
    assert(x_values.size() == z_values.size());

    glBegin(GL_POINTS);
    for (size_t k = 0; k < x_values.size(); k++)
    {
        glVertex3f(x_values(k), y_values(k), z_values(k));
    }

    glEnd();
}

void drawRectangle3D() {}

void drawTriangle3D(const Vec3d& v0, const Vec3d& v1, const Vec3d& v2)
{
    glBegin(GL_TRIANGLES);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glEnd();
}

void drawLine3D(const double x0, const double y0, const double z0, const double x1, const double y1, const double z1)
{
    glBegin(GL_LINES);
    glVertex3f(x0, y0, z0);
    glVertex3f(x1, y1, z1);
    glEnd();
}

void drawLine3D(const Vec3d& v0, const Vec3d& v1)
{
    glBegin(GL_LINES);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glEnd();
}

void drawPolygon3D(const Vec3d& c0, const Vec3d& c1, const Vec3d& c2, const Vec3d& c3)
{
    glBegin(GL_POLYGON);
    glVertex3f(c0.x, c0.y, c0.z);
    glVertex3f(c1.x, c1.y, c1.z);
    glVertex3f(c2.x, c2.y, c2.z);
    glVertex3f(c3.x, c3.y, c3.z);
    glEnd();
}

void drawTriangle3D(const Triangle3D<double>& triangle)
{
    glBegin(GL_POLYGON);
    glVertex3f(triangle.p0.x, triangle.p0.y, triangle.p0.z);
    glVertex3f(triangle.p1.x, triangle.p1.y, triangle.p1.z);
    glVertex3f(triangle.p2.x, triangle.p2.y, triangle.p2.z);
    glEnd();
}

void drawTriangleEdge3D(const Triangle3D<double>& triangle)
{
    glBegin(GL_LINES);
    drawLine3D(triangle.p0, triangle.p1);
    drawLine3D(triangle.p1, triangle.p2);
    drawLine3D(triangle.p2, triangle.p0);
    glEnd();
}
