#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_PLOT3_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_PLOT3_H_

#include "duoplot/duoplot.h"
#include "utils.h"

using namespace duoplot;

namespace plot3_ns
{

void testWithVectorView()
{
    const size_t num_elements = 100;
    const Vector<double> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<double> y = duoplot::sin(x);
    const Vector<double> z = duoplot::cos(x);

    const VectorConstView<double> xv = x.constView();
    const VectorConstView<double> yv = y.constView();
    const VectorConstView<double> zv = z.constView();

    setActiveView("view_00");
    clearView();
    scatter3(x, y, z, properties::Color::RED);
    plot3(xv, yv, zv, properties::Color::BLUE);
}

void addTests()
{
    addTest("cpp", "plot3", "view", plot3_ns::testWithVectorView);
}

}  // namespace plot3_ns

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_PLOT3_H_
