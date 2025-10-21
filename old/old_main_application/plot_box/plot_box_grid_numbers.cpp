#include "axes/plot_box/plot_box_grid_numbers.h"

#include <cassert>

#include "misc/misc.h"
#include "opengl_low_level/opengl_header.h"

Vec3d gs;
const float kTextScale = 1.0f / 8.0f;

void drawAxisLetter(const double x_offset,
                    const double y_offset,
                    const double z_offset,
                    const std::string& axis_label,
                    const AxesLimits& axes_limits,
                    const CoordinateConverter& coord_converter,
                    const float width,
                    const float height)
{
    const Vec3d s = axes_limits.getAxesScale();

    const float sx = 2.0 * kTextScale / width;
    const float sy = 2.0 * kTextScale / height;

    glPushMatrix();
    glScalef(gs.x, gs.y, gs.x);

    const Vec3d v(x_offset / s.x, y_offset / s.y, z_offset / s.z);
    const Vec2d vv = coord_converter.modelToViewCoordinate(v);
    glPushMatrix();
    glTranslated(vv.x, vv.y, 0);
    glScalef(sx / gs.x, sy / gs.y, 1.0);

    putTextAtNew(axis_label);

    glPopMatrix();

    glPopMatrix();
}

void drawNumbersForYAxis(const Vectord& y_values,
                         const double x_offset,
                         const double z_offset,
                         const AxesLimits& axes_limits,
                         const CoordinateConverter& coord_converter,
                         const float width,
                         const float height)
{
    const Vec3d axes_center = axes_limits.getAxesCenter();
    const Vec3d s = axes_limits.getAxesScale();

    const float sx = kTextScale / width;
    const float sy = kTextScale / height;

    glPushMatrix();
    glScalef(gs.x, gs.y, gs.x);

    for (size_t k = 0; k < y_values.size(); k++)
    {
        const Vec3d v(x_offset / s.x, y_values(k) / s.y, z_offset / s.z);
        const Vec2d vv = coord_converter.modelToViewCoordinate(v);
        glPushMatrix();
        glTranslated(vv.x, vv.y, 0);
        glScalef(sx / gs.x, sy / gs.y, 1.0);

        putTextAtNew(formatNumber(y_values(k) + axes_center.y, 3));

        glPopMatrix();
    }
    glPopMatrix();
}

void drawNumbersForZAxis(const Vectord& z_values,
                         const double x_offset,
                         const double y_offset,
                         const AxesLimits& axes_limits,
                         const CoordinateConverter& coord_converter,
                         const float width,
                         const float height)
{
    const Vec3d axes_center = axes_limits.getAxesCenter();
    const Vec3d s = axes_limits.getAxesScale();

    const float sx = kTextScale / width;
    const float sy = kTextScale / height;

    glPushMatrix();
    glScalef(gs.x, gs.y, gs.x);

    for (size_t k = 0; k < z_values.size(); k++)
    {
        const Vec3d v(x_offset / s.x, y_offset / s.y, z_values(k) / s.z);
        const Vec2d vv = coord_converter.modelToViewCoordinate(v);
        glPushMatrix();
        glTranslated(vv.x, vv.y, 0);
        glScalef(sx / gs.x, sy / gs.y, 1.0);

        putTextAtNew(formatNumber(z_values(k) + axes_center.z, 3));
        glPopMatrix();
    }
    glPopMatrix();
}

void drawNumbersForXAxis(const Vectord& x_values,
                         const double y_offset,
                         const double z_offset,
                         const AxesLimits& axes_limits,
                         const CoordinateConverter& coord_converter,
                         const float width,
                         const float height)
{
    const Vec3d axes_center = axes_limits.getAxesCenter();
    const Vec3d s = axes_limits.getAxesScale();

    const float sx = kTextScale / width;
    const float sy = kTextScale / height;

    glPushMatrix();
    glScalef(gs.x, gs.y, gs.x);

    for (size_t k = 0; k < x_values.size(); k++)
    {
        const Vec3d v(x_values(k) / s.x, y_offset / s.y, z_offset / s.z);
        const Vec2d vv = coord_converter.modelToViewCoordinate(v);
        glPushMatrix();
        glTranslated(vv.x, vv.y, 0);
        glScalef(sx / gs.x, sy / gs.y, 1.0);

        putTextAtNew(formatNumber(x_values(k) + axes_center.x, 3));
        glPopMatrix();
    }
    glPopMatrix();
}

void drawAxisNumbers(const GridVectors& gv,
                     const AxesLimits& axes_limits,
                     const ViewAngles& view_angles,
                     const AxesSettings& axes_settings,
                     const CoordinateConverter& coord_converter,
                     const float width,
                     const float height)
{
    const Vec3d s = axes_limits.getAxesScale();

    gs = axes_settings.getAxesScale();

    const double box_x = s.x;
    const double box_y = s.y;
    const double box_z = s.z;
    setColor(0.0, 0.0, 0.0);

    bool draw_x_numbers = !view_angles.isSnappedAlongX();
    bool draw_y_numbers = !view_angles.isSnappedAlongY();
    bool draw_z_numbers = !view_angles.isSnappedAlongZ();

    double x_value_to_use_vert, z_value_to_use_vert;
    double x_value_to_use, y_value_to_use, z_value_to_use, y_value_to_use_2;
    if (view_angles.getSnappedElevation() < 0.0)
    {
        y_value_to_use = box_y / 2.0;
    }
    else
    {
        y_value_to_use = -box_y / 2.0;
    }

    // Magic numbers are for moving text away from the plot box
    if ((view_angles.getSnappedAzimuth() > -M_PI_2) && (view_angles.getSnappedAzimuth() < M_PI_2))
    {
        z_value_to_use = -1.1 * box_z / 2.0;
        y_value_to_use_2 = box_y / 2.0;
    }
    else
    {
        z_value_to_use = box_z / 2.0;
        y_value_to_use_2 = -box_y / 2.0;
    }

    if ((view_angles.getSnappedAzimuth() > 0.0) && (view_angles.getSnappedAzimuth() < M_PI))
    {
        x_value_to_use = -1.15 * box_x / 2.0;
    }
    else
    {
        x_value_to_use = 1.05 * box_x / 2.0;
    }

    if (view_angles.getSnappedElevation() > 0)
    {
        z_value_to_use_vert = -box_z / 2.0;
    }
    else
    {
        z_value_to_use_vert = box_z / 2.0;
    }

    if ((view_angles.getSnappedAzimuth() > 0.0f) && (view_angles.getSnappedAzimuth() <= M_PI))
    {
        x_value_to_use_vert = -box_x / 2.0;
    }
    else
    {
        x_value_to_use_vert = box_x / 2.0;
    }

    if (draw_x_numbers)
    {
        drawNumbersForXAxis(gv.x, y_value_to_use, z_value_to_use, axes_limits, coord_converter, width, height);
        drawAxisLetter(
            0.0, y_value_to_use * 1.1, z_value_to_use * 1.1, "X", axes_limits, coord_converter, width, height);
    }
    if (draw_y_numbers)
    {
        drawNumbersForYAxis(
            gv.y, x_value_to_use_vert, z_value_to_use_vert, axes_limits, coord_converter, width, height);
        drawAxisLetter(x_value_to_use_vert * 1.1,
                       0.0,
                       z_value_to_use_vert * 1.1,
                       "Y",
                       axes_limits,
                       coord_converter,
                       width,
                       height);
    }
    if (draw_z_numbers)
    {
        drawAxisLetter(
            x_value_to_use_vert * 1.1, y_value_to_use_2 * 1.1, 0.0, "Z", axes_limits, coord_converter, width, height);
        drawNumbersForZAxis(gv.z, x_value_to_use, y_value_to_use_2, axes_limits, coord_converter, width, height);
    }
}
