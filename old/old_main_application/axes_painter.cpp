#include "axes/axes_painter.h"

#include "axes/plot_box/plot_box_coordinate_arrows.h"
#include "axes/plot_box/plot_box_grid.h"
#include "axes/plot_box/plot_box_grid_numbers.h"
#include "axes/plot_box/plot_box_outline.h"
#include "axes/plot_box/plot_box_walls.h"
#include "duoplot/math/math.h"
#include "misc/misc.h"

using namespace duoplot;

AxesPainter::AxesPainter(const AxesSettings& axes_settings) : axes_settings_(axes_settings) {}

void drawSelectedSilhouette()
{
    setColor(0.0f, 0.0f, 0.0f);
    glLineWidth(5.0f);
    const float f = 1.0f;
    const float ymin = -1.0f;
    drawLine2D(-f, ymin, -f, f);
    drawLine2D(-f, ymin, f, ymin);

    drawLine2D(-f, f, f, f);
    drawLine2D(f, ymin, f, f);
    glLineWidth(1.0f);
}

void drawDebugSilhouette()
{
    setColor(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    const float f = 0.9999f;
    const float ymin = -1.0f;
    drawLine2D(-f, ymin, -f, f);
    drawLine2D(-f, ymin, f, ymin);

    drawLine2D(-f, f, f, f);
    drawLine2D(f, ymin, f, f);
    glLineWidth(1.0f);
}

Vec3d findScale(const Matrixd& R)
{
    // Currently unknown exactly how 'q' affects the results...
    const double q = 0.3;
    // clang-format off
    const Vector<Point3d> points = {{q, q, q},
                                     {-q, q, q},
                                     {q, -q, q},
                                     {-q, -q, q},
                                     {q, q, -q},
                                     {-q, q, -q},
                                     {q, -q, -q},
                                     {-q, -q, -q}};
    const Point3d pr0 = R * points(0);
    // clang-format on
    std::pair<Point3d, Point3d> pmiw = {points(0), pr0}, pmaw = {points(0), pr0}, pmih = {points(0), pr0},
                                pmah = {points(0), pr0};
    for (const Point3d p : points)
    {
        const auto pqr = R * p;

        if (pqr.x < pmiw.second.x)
        {
            pmiw.first = p;
            pmiw.second = pqr;
        }

        if (pqr.x > pmaw.second.x)
        {
            pmaw.first = p;
            pmaw.second = pqr;
        }

        if (pqr.y < pmih.second.y)
        {
            pmih.first = p;
            pmih.second = pqr;
        }

        if (pqr.y > pmah.second.y)
        {
            pmah.first = p;
            pmah.second = pqr;
        }
    }

    const double r00 = R(0, 0), r01 = R(0, 1), r02 = R(0, 2);
    const double r10 = R(1, 0), r11 = R(1, 1), r12 = R(1, 2);
    const double w = 0.5, h = 0.5;
    const double pmiw_x = pmiw.first.x, pmiw_y = pmiw.first.y, pmiw_z = pmiw.first.z;
    const double pmih_x = pmih.first.x, pmih_y = pmih.first.y, pmih_z = pmih.first.z;

    const double sx = -w / (pmiw_x * r00 + pmiw_y * r01 + pmiw_z * r02);
    const double sy = -h / (pmih_x * r10 + pmih_y * r11 + pmih_z * r12);
    const double sz = 1.0;

    return Vec3d(sx, sy, sz);
}

void AxesPainter::updateStates(const AxesLimits& axes_limits,
                               const ViewAngles& view_angles,
                               const GridVectors& gv,
                               const CoordinateConverter& coord_converter)
{
    axes_limits_ = axes_limits;
    view_angles_ = view_angles;
    gv_ = gv;
    coord_converter_ = coord_converter;
    const Vec3d new_scale = findScale(view_angles_.getSnappedRotationMatrix());
    axes_settings_.setAxesScale(new_scale);
}

void AxesPainter::paint(const AxesLimits& axes_limits,
                        const ViewAngles& view_angles,
                        const GridVectors& gv,
                        const CoordinateConverter& coord_converter,
                        const bool draw_selected_bb,
                        const bool left_mouse_pressed)
{
    axes_limits_ = axes_limits;
    view_angles_ = view_angles;
    gv_ = gv;
    coord_converter_ = coord_converter;
    const Vec3d new_scale = findScale(view_angles_.getSnappedRotationMatrix());
    axes_settings_.setAxesScale(new_scale);

    glEnable(GL_DEPTH_CLAMP);
    glPointSize(1);

    // Plot box
    setOpenGLStateForPlotBox();

    drawPlotBoxWalls(axes_settings_, view_angles_);
    drawDottedBox(axes_settings_);
    drawOutline(axes_settings_, view_angles_);

    glPopMatrix();

    // drawAxesArrows();

    setOpenGLStateForAxesGrid();

    drawGrid(gv_, axes_settings_, axes_limits_, view_angles_);

    glPopMatrix();

    drawAxisNumbers(gv_, axes_limits_, view_angles_, axes_settings_, coord_converter_, width_, height_);

    if (left_mouse_pressed)
    {
        printViewAnglesInPlotWindow();
    }
    if (draw_selected_bb)
    {
        drawSelectedSilhouette();
    }
    drawDebugSilhouette();
}

void setClipPlane(const GLenum gl_plane, const Point3d& p0, const Point3d& p1, const Point3d& p2, const bool invert)
{
    // Fit plane
    const Planed fp = planeFromThreePoints(p0, p1, p2);

    // Invert
    const Planed plane = invert ? Planed(-fp.a, -fp.b, -fp.c, fp.d) : fp;

    // Members in "Plane" stored in same way as "double ptr[4]", which is what "glClipPlane" expects
    glClipPlane(gl_plane, reinterpret_cast<const double*>(&plane));
    glEnable(gl_plane);
}

void AxesPainter::enableClipPlanes() const
{
    const double f = 0.5;
    const Vec3d s = axes_settings_.getAxesScale();

    const ViewAngles view_ang(-view_angles_.getAzimuth(), -view_angles_.getElevation(), view_angles_.getAngleLimit());

    const Matrixd rot_mat = view_ang.getSnappedRotationMatrix();

    // clang-format off
    const Vector<Point3d> points_x = {(rot_mat * Point3d(f, f, f)).elementWiseMultiply(s),
                                       (rot_mat * Point3d(f, -f, f)).elementWiseMultiply(s),
                                       (rot_mat * Point3d(f, f, -f)).elementWiseMultiply(s)};
    const Vector<Point3d> points_y = {(rot_mat * Point3d(-f, f, f)).elementWiseMultiply(s),
                                       (rot_mat * Point3d(f, f, f)).elementWiseMultiply(s),
                                       (rot_mat * Point3d(f, f, -f)).elementWiseMultiply(s)};
    const Vector<Point3d> points_z = {(rot_mat * Point3d(-f, f, -f)).elementWiseMultiply(s),
                                       (rot_mat * Point3d(f, -f, -f)).elementWiseMultiply(s),
                                       (rot_mat * Point3d(-f, -f, -f)).elementWiseMultiply(s)};
    // clang-format on

    setClipPlane(GL_CLIP_PLANE0, points_x(0), points_x(1), points_x(2), true);
    setClipPlane(GL_CLIP_PLANE1, points_x(0), points_x(1), points_x(2), false);
    setClipPlane(GL_CLIP_PLANE2, points_y(0), points_y(1), points_y(2), true);
    setClipPlane(GL_CLIP_PLANE3, points_y(0), points_y(1), points_y(2), false);
    setClipPlane(GL_CLIP_PLANE4, points_z(0), points_z(1), points_z(2), true);
    setClipPlane(GL_CLIP_PLANE5, points_z(0), points_z(1), points_z(2), false);
}

void AxesPainter::disableClipPlanes() const
{
    glDisable(GL_CLIP_PLANE0);
    glDisable(GL_CLIP_PLANE1);
    glDisable(GL_CLIP_PLANE2);
    glDisable(GL_CLIP_PLANE3);
    glDisable(GL_CLIP_PLANE4);
    glDisable(GL_CLIP_PLANE5);
}

void AxesPainter::plotBegin()
{
    // TODO: Why do angles need to be negated?
    const ViewAngles view_ang(-view_angles_.getAzimuth(), -view_angles_.getElevation(), view_angles_.getAngleLimit());
    const AxisAngled ax_ang = view_ang.getSnappedAngleAxis();

    const Vec3d s = axes_limits_.getAxesScale();
    const Vec3d axes_center = axes_limits_.getAxesCenter();

    enableClipPlanes();
    const Vec3d sq = axes_settings_.getAxesScale();

    glPushMatrix();
    glScaled(sq.x, sq.y, sq.z);

    glRotated(ax_ang.phi * 180.0f / M_PI, ax_ang.x, ax_ang.y, ax_ang.z);
    // TODO: Not sure why y axis should be negated... But it works like this.
    glScaled(1.0 / s.x, -1.0 / s.y, 1.0 / s.z);
    glTranslated(-axes_center.x, -axes_center.y, -axes_center.z);
}

void AxesPainter::plotEnd()
{
    disableClipPlanes();
    glPopMatrix();
}

void AxesPainter::setOpenGLStateForPlotBox() const
{
    // Must be closed with glPopMatrix()
    const AxisAngled ax_ang = view_angles_.getSnappedAngleAxis();
    const Vec3d s = axes_settings_.getAxesScale();

    glPushMatrix();
    glScalef(s.x, s.y, s.z);
    glRotatef(ax_ang.phi * 180.0f / M_PI, ax_ang.x, ax_ang.y, ax_ang.z);
}

void AxesPainter::setOpenGLStateForAxesGrid() const
{
    // Must be closed with glPopMatrix()
    const AxisAngled ax_ang = view_angles_.getSnappedAngleAxis();

    const Vec3d scale = axes_limits_.getAxesScale();
    const Vec3d s = axes_settings_.getAxesScale();

    glPushMatrix();
    glScalef(s.x, s.y, s.z);
    glRotatef(ax_ang.phi * 180.0f / M_PI, ax_ang.x, ax_ang.y, ax_ang.z);
    glScaled(1.0 / scale.x, 1.0 / scale.y, 1.0 / scale.z);
}

void AxesPainter::drawAxesArrows() const
{
    setLinewidth(1.0f);
    setOpenGLStateForPlotBox();
    drawCoordinateAxesLines(axes_settings_);
    glPopMatrix();

    drawCoordinateAxesLetters(axes_settings_, coord_converter_);
    drawCoordinateArrowHead(axes_settings_, coord_converter_);
}

void AxesPainter::setWindowSize(const float width, const float height)
{
    width_ = width;
    height_ = height;
}

void AxesPainter::printViewAnglesInPlotWindow() const
{
    setColor(0.0f, 0.0f, 0.0f);
    putTextAt("(" + toStringWithNumDecimalPlaces(view_angles_.getSnappedAzimuth() * 180.0 / M_PI, 1) + ", " +
                  toStringWithNumDecimalPlaces(view_angles_.getSnappedElevation() * 180.0 / M_PI, 1) + ")",
              -0.97,
              -0.95,
              width_,
              height_,
              100.0f);
}
