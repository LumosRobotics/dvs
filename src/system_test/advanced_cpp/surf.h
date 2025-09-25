#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SURF_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SURF_H_

#include "duoplot/duoplot.h"
#include "utils.h"

using namespace duoplot;

namespace surf_ns
{

void testWithMatrixView()
{
    const int num_rows = 200, num_cols = 250;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);

    const double inc = 0.08;
    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double rd = (static_cast<double>(r) - 128.32) * inc;
            const double cd = (static_cast<double>(c) - 128.32) * inc;
            x(r, c) = c;
            y(r, c) = r;
            z(r, c) = 50.0 * std::sin(std::sqrt(rd * rd + cd * cd));
        }
    }

    setActiveView("view_00");
    clearView();
    surf(x.constView(), y.constView(), z.constView(), properties::ColorMap::JET);
}

void addTests()
{
    addTest("cpp", "surf", "view", surf_ns::testWithMatrixView);
}

}  // namespace surf_ns

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SURF_H_
