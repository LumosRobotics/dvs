#include "axes/plot_box/plot_box_walls.h"

#include "opengl_low_level/opengl_header.h"

void drawPlotBoxWalls(const AxesSettings& axes_settings, const ViewAngles& view_angles)
{
    setLinewidth(1.0f);
    setColor(axes_settings.getPlotBoxWallColor());
    const Vec3d s = Vec3d(1.0, 1.0, 1.0) * axes_settings.getPlotBoxSizeFactor();

    if (view_angles.getSnappedElevation() > 0.0f)
    {
        glBegin(GL_QUADS);
        glVertex3f(s.x, s.y, -s.z);
        glVertex3f(s.x, -s.y, -s.z);
        glVertex3f(-s.x, -s.y, -s.z);
        glVertex3f(-s.x, s.y, -s.z);
        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);
        glVertex3f(s.x, s.y, s.z);
        glVertex3f(s.x, -s.y, s.z);
        glVertex3f(-s.x, -s.y, s.z);
        glVertex3f(-s.x, s.y, s.z);
        glEnd();
    }

    if ((view_angles.getSnappedAzimuth() > 0.0) && (view_angles.getSnappedAzimuth() < M_PI))
    {
        glBegin(GL_QUADS);
        glVertex3f(s.x, s.y, s.z);
        glVertex3f(s.x, s.y, -s.z);
        glVertex3f(s.x, -s.y, -s.z);
        glVertex3f(s.x, -s.y, s.z);
        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);
        glVertex3f(-s.x, s.y, s.z);
        glVertex3f(-s.x, s.y, -s.z);
        glVertex3f(-s.x, -s.y, -s.z);
        glVertex3f(-s.x, -s.y, s.z);
        glEnd();
    }

    if ((view_angles.getSnappedAzimuth() > -M_PI_2) && (view_angles.getSnappedAzimuth() < M_PI_2))
    {
        glBegin(GL_QUADS);
        glVertex3f(s.x, s.y, s.z);
        glVertex3f(s.x, s.y, -s.z);
        glVertex3f(-s.x, s.y, -s.z);
        glVertex3f(-s.x, s.y, s.z);
        glEnd();
    }
    else
    {
        glBegin(GL_QUADS);
        glVertex3f(s.x, -s.y, s.z);
        glVertex3f(s.x, -s.y, -s.z);
        glVertex3f(-s.x, -s.y, -s.z);
        glVertex3f(-s.x, -s.y, s.z);
        glEnd();
    }
}
