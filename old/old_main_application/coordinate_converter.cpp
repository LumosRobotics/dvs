#include "axes/structures/coordinate_converter.h"

#include <cassert>

CoordinateConverter::CoordinateConverter(const Vec2d& window_size,
                                         const ViewAngles& view_angles,
                                         const AxesLimits& axes_limits)
    : window_size_(window_size), view_angles_(view_angles), axes_limits_(axes_limits)
{
}

void CoordinateConverter::updateInternalState(const Vec2d& window_size,
                                              const ViewAngles& view_angles,
                                              const AxesLimits& axes_limits)
{
    window_size_ = window_size;
    view_angles_ = view_angles;
    axes_limits_ = axes_limits;
}

Vec2d CoordinateConverter::orthogonalViewToModelCoordinate(const Vec2d& view_coord) const
{
    // Only works when azimuth = elevation = 0
    assert((view_angles_.getSnappedElevation() == 0.0) && (view_angles_.getSnappedAzimuth() == 0.0));

    const Vec3d scale = axes_limits_.getAxesScale();
    const Vec3d offset = axes_limits_.getAxesCenter();
    const Vec3d res = Vec3d(view_coord.x, view_coord.y, 0.0).elementWiseMultiply(scale) + offset;

    return Vec2d(res.x, res.y);
}

Vec2d CoordinateConverter::modelToViewCoordinate(const Vec3d& model_coord) const
{
    const Matrixd rotation_mat = view_angles_.getSnappedRotationMatrix();
    const Vec3d vr = rotation_mat * model_coord;

    return Vec2d(vr.x, vr.y);
}

Vec2d CoordinateConverter::screenToViewCoordinate(const Vec2d& screen_coord) const
{
    const double w_x = window_size_.x;
    const double w_y = window_size_.y;

    Vec2d vret;

    vret.x = 2.0 * (screen_coord.x / w_x - 0.5);
    vret.y = 2.0 * ((w_y - screen_coord.y + 1.0) / (w_y - 1.0) - 0.5);

    return vret;
}

Vec2d CoordinateConverter::viewToScreenCoordinate(const Vec2d& view_coord) const
{
    const double w_x = window_size_.x;
    const double w_y = window_size_.y;

    Vec2d vret;
    vret.x = (view_coord.x / 2.0 + 0.5) * w_x;
    vret.y = 2.0 * w_y - ((view_coord.y / 2.0 + 0.5) * (w_y - 1.0) - 1.0 + w_y);

    return vret;
}
