#include "axes/plot_box/plot_box_coordinate_arrows.h"

#include "opengl_low_level/opengl_header.h"

void drawCoordinateAxesLines(const AxesSettings& axes_settings)
{
    glLineWidth(1.0f);
    const double ext_fac = axes_settings.getCoordAxExtFac();
    const double f = axes_settings.getPlotBoxSizeFactor();

    setColor(1.0f, 0.0f, 0.0f);
    drawLine3D({f, -f, -f}, {ext_fac * f, -f, -f});

    setColor(0.0f, 1.0f, 0.0f);
    drawLine3D({-f, f, -f}, {-f, ext_fac * f, -f});

    setColor(0.0f, 0.0f, 1.0f);
    drawLine3D({-f, -f, f}, {-f, -f, ext_fac * f});
}

void drawArrowHead(const Vec3d& v0, const Vec3d& v1, const CoordinateConverter& coord_converter)
{
    const Vec2d v0_2d = coord_converter.modelToViewCoordinate(v0);
    const Vec2d v1_2d = coord_converter.modelToViewCoordinate(v1);

    const Vec2d v01 = v1_2d.normalizedVectorBetweenPoints(v0_2d);

    const double arrow_angle = M_PI / 10.0;

    const Vec2d a0 = rotationMatrix2D(arrow_angle) * v01;
    const Vec2d a1 = rotationMatrix2D(-arrow_angle) * v01;

    const double f = 0.04;

    drawLine2D(v1_2d.x, v1_2d.y, v1_2d.x + a0.x * f, v1_2d.y + a0.y * f);
    drawLine2D(v1_2d.x, v1_2d.y, v1_2d.x + a1.x * f, v1_2d.y + a1.y * f);
}

void drawCoordinateArrowHead(const AxesSettings& axes_settings, const CoordinateConverter& coord_conv)
{
    const double ext_fac = axes_settings.getCoordAxExtFac();

    const double f = axes_settings.getPlotBoxSizeFactor();
    const Vec3d x1(ext_fac * f, -f, -f);
    const Vec3d y1(-f, ext_fac * f, -f);
    const Vec3d z1(-f, -f, ext_fac * f);

    const Vec3d x0 = x1 - Vec3d(1.0, 0.0, 0.0);
    const Vec3d y0 = y1 - Vec3d(0.0, 1.0, 0.0);
    const Vec3d z0 = z1 - Vec3d(0.0, 0.0, 1.0);

    setColor(1.0, 0.0, 0.0);
    drawArrowHead(x0, x1, coord_conv);
    setColor(0.0, 1.0, 0.0);
    drawArrowHead(y0, y1, coord_conv);
    setColor(0.0, 0.0, 1.0);
    drawArrowHead(z0, z1, coord_conv);
}

void drawCoordinateAxesLetters(const AxesSettings& axes_settings, const CoordinateConverter& coord_conv)
{
    // Extend a little bit (1.1) so letters are not "on" the arrow head
    const double ext_fac = axes_settings.getCoordAxExtFac() * 1.1;

    const double f = axes_settings.getPlotBoxSizeFactor();
    Vec3d xpos(ext_fac * f, -f, -f);
    Vec3d ypos(-f, ext_fac * f, -f);
    Vec3d zpos(-f, -f, ext_fac * f);

    const Vec2d x_vec = coord_conv.modelToViewCoordinate(xpos);
    const Vec2d y_vec = coord_conv.modelToViewCoordinate(ypos);
    const Vec2d z_vec = coord_conv.modelToViewCoordinate(zpos);

    setColor(0.0, 0.0, 0.0);
    putTextAt_Old("X", x_vec.x, x_vec.y);
    putTextAt_Old("Y", y_vec.x, y_vec.y);
    putTextAt_Old("Z", z_vec.x, z_vec.y);
}
