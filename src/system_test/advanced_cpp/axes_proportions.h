#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_AXES_PROPORTIONS_H
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_AXES_PROPORTIONS_H

#include <unistd.h>

#include "lumos/lumos.h"
#include "utils.h"

using namespace lumos;

namespace axes_proportions
{

void testBasic()
{
    openProjectFile("../../project_files/exp0.duoplot");
    const double xp = 1.0, yp = 2.0;
    Vector<double> x{VectorInitializer<double>{-xp, xp, xp, -xp, -xp, xp, xp, -xp}};
    Vector<double> y{VectorInitializer<double>{-yp, -yp, yp, yp, -yp, -yp, yp, yp}};
    Vector<double> z{VectorInitializer<double>{-1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0}};

    /*

      {3} -1,1  |    {2}  1,1
                |
                |
    ------------|----------------
                |
                |
    {0} -1,-1   |    {1}  1,-1
                |



      {7} -1,1  |    {6}  1,1
                |
                |
    ------------|----------------
                |
                |
    {4} -1,-1   |    {5}  1,-1
                |
    */

    // clang-format off
    Vector<IndexTriplet> indices{VectorInitializer<IndexTriplet>{
        {0, 1, 2},
        {2, 3, 0},

        {4, 5, 6},
        {6, 7, 4},

        {1, 2, 6},
        {1, 6, 5},
        }};

    // clang-format on

    setActiveView("p_view_0");
    clearView();
    globalIllumination({2, 2, 2});
    disableScaleOnRotation();
    axesSquare();
    drawMesh(x, y, z, indices);

    setActiveView("p_view_1");
    clearView();
    globalIllumination({2, 2, 2});
    disableScaleOnRotation();
    drawMesh(x, y, z, indices);

    setActiveView("p1");
    clearView();
    globalIllumination({2, 2, 2});
    axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});
    drawMesh(x, y, z, indices);
    axesSquare();
}

void addTests()
{
    addTest("cpp", "axes_proportions", "basic", axes_proportions::testBasic);
}

}  // namespace axes_proportions

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_APPEND_PROPERTIES_H_
