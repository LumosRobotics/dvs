#ifndef MAIN_APPLICATION_OLD_PLOT_BOX_PLOT_BOX_GRID_NUMBERS_H_
#define MAIN_APPLICATION_OLD_PLOT_BOX_PLOT_BOX_GRID_NUMBERS_H_

#include "axes/structures/axes_limits.h"
#include "axes/structures/axes_settings.h"
#include "axes/structures/coordinate_converter.h"
#include "axes/structures/grid_vectors.h"
#include "axes/structures/view_angles.h"
#include "duoplot/math/math.h"

void drawAxisNumbers(const GridVectors& gv,
                     const AxesLimits& axes_limits,
                     const ViewAngles& view_angles,
                     const AxesSettings& axes_settings,
                     const CoordinateConverter& coord_converter,
                     const float width,
                     const float height);

#endif  // MAIN_APPLICATION_OLD_PLOT_BOX_PLOT_BOX_GRID_NUMBERS_H_
