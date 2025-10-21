#ifndef MAIN_APPLICATION_OLD_COORDINATE_CONVERTER_H_
#define MAIN_APPLICATION_OLD_COORDINATE_CONVERTER_H_

#include "axes/structures/axes_limits.h"
#include "axes/structures/view_angles.h"
#include "duoplot/math/math.h"

// Model - view - screen
// Model: 3D coordinates, world, defined with axes_limits_
// View: 2D gl world coordinates, defined as [-1, -1] in bottom left corner
//       and [1, 1] in top right corner
// Screen: Window pixels, defined as [0, 0] in top left corner

using namespace duoplot;

struct CoordinateConverter
{
private:
    Vec2d window_size_;
    ViewAngles view_angles_;
    AxesLimits axes_limits_;

public:
    CoordinateConverter() = default;
    CoordinateConverter(const Vec2d& window_size, const ViewAngles& view_angles, const AxesLimits& axes_limits);
    void updateInternalState(const Vec2d& window_size, const ViewAngles& view_angles, const AxesLimits& axes_limits);

    Vec2d orthogonalViewToModelCoordinate(const Vec2d& view_coord) const;
    Vec2d screenToViewCoordinate(const Vec2d& screen_coord) const;
    Vec2d viewToScreenCoordinate(const Vec2d& view_coord) const;
    Vec2d modelToViewCoordinate(const Vec3d& model_coord) const;
};

#endif  // MAIN_APPLICATION_OLD_COORDINATE_CONVERTER_H_
