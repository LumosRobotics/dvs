#ifndef MAIN_APPLICATION_OLD_AXES_PAINTER_H_
#define MAIN_APPLICATION_OLD_AXES_PAINTER_H_

#include <cmath>
#include <utility>
#include <vector>

#include "axes/structures/axes_limits.h"
#include "axes/structures/axes_settings.h"
#include "axes/structures/coordinate_converter.h"
#include "axes/structures/grid_vectors.h"
#include "axes/structures/view_angles.h"
#include "opengl_low_level/opengl_header.h"

class AxesPainter
{
    // Variables
    ViewAngles view_angles_;
    AxesLimits axes_limits_;

    AxesSettings axes_settings_;
    CoordinateConverter coord_converter_;

    float width_, height_;

    GridVectors gv_;

    // Functions
    void printViewAnglesInPlotWindow() const;

    void setOpenGLStateForPlotBox() const;
    void setOpenGLStateForAxesGrid() const;

    void drawAxesArrows() const;
    void drawZoomButtons() const;

    void enableClipPlanes() const;
    void disableClipPlanes() const;

public:
    void plotBegin();
    void plotEnd();

    void setWindowSize(const float width, const float height);

    AxesPainter() = default;
    AxesPainter(const AxesSettings& axes_settings);

    void paint(const AxesLimits& axes_limits,
               const ViewAngles& view_angles,
               const GridVectors& gv,
               const CoordinateConverter& coord_converter,
               const bool draw_selected_bb,
               const bool left_mouse_pressed);
    void updateStates(const AxesLimits& axes_limits,
                      const ViewAngles& view_angles,
                      const GridVectors& gv,
                      const CoordinateConverter& coord_converter);
};

#endif  // MAIN_APPLICATION_OLD_AXES_PAINTER_H_
