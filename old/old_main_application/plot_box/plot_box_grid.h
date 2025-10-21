#ifndef MAIN_APPLICATION_OLD_PLOT_BOX_PLOT_BOX_GRID_H_
#define MAIN_APPLICATION_OLD_PLOT_BOX_PLOT_BOX_GRID_H_

#include "axes/structures/axes_limits.h"
#include "axes/structures/axes_settings.h"
#include "axes/structures/grid_vectors.h"
#include "axes/structures/view_angles.h"
#include "duoplot/math/math.h"

void drawGrid(const GridVectors& gv,
              const AxesSettings& axes_settings,
              const AxesLimits& axes_limits,
              const ViewAngles& view_angles);

#endif  // MAIN_APPLICATION_OLD_PLOT_BOX_PLOT_BOX_GRID_H_
