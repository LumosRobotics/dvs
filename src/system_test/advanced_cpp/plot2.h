#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_PLOT2_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_PLOT2_H_

#include "lumos/lumos.h"
#include "utils.h"

using namespace lumos;

namespace plot2
{

void testWithVectorView()
{
    const size_t num_elements = 10;
    const Vector<double> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<double> y = lumos::sin(x);

    const VectorConstView<double> xv = x.constView();
    const VectorConstView<double> yv = y.constView();

    setActiveView("view_00");
    clearView();
    scatter(x, y, properties::Color::RED);
    plot(xv, yv, properties::Color::BLUE);
}

void addTests()
{
    addTest("cpp", "plot", "view", plot2::testWithVectorView);
}

}  // namespace plot2

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_PLOT2_H_
