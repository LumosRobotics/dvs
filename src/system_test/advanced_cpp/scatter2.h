#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SCATTER2_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SCATTER2_H_

#include "lumos/lumos.h"
#include "utils.h"

using namespace lumos;

namespace scatter2
{

void testWithVectorView()
{
    const size_t num_elements = 100;
    const Vector<double> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<double> y = lumos::sin(x);

    const VectorConstView<double> xv = x.constView();
    const VectorConstView<double> yv = y.constView();

    setActiveView("p_view_0");
    clearView();
    plot(x, y, properties::Color::BLUE);
    scatter(xv, yv, properties::Color::RED);
}

void testWithColorVec()
{
    const size_t num_elements = 100;
    const Vector<double> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<double> y = lumos::sin(x);
    Vector<properties::Color> color(num_elements);

    for (size_t k = 0; k < num_elements; k++)
    {
        const uint8_t k8 = k;
        color(k) = properties::Color{k8, static_cast<uint8_t>(255U - k8), static_cast<uint8_t>(127U + k8 * 2U)};
    }

    setActiveView("p_view_0");
    clearView();
    scatter(x, y, color, properties::PointSize(20));
}

void addTests()
{
    addTest("cpp", "scatter", "view", scatter2::testWithVectorView);
    addTest("cpp", "scatter", "color_vec", scatter2::testWithColorVec);
}

}  // namespace scatter2

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SCATTER2_H_
