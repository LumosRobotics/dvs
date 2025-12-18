#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SCATTER3_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SCATTER3_H_

#include "lumos/lumos.h"
#include "utils.h"

using namespace lumos;

namespace scatter3_ns
{

void testWithVectorView()
{
    const size_t num_elements = 100;
    const Vector<double> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<double> y = lumos::sin(x);
    const Vector<double> z = lumos::cos(x);

    const VectorConstView<double> xv = x.constView();
    const VectorConstView<double> yv = y.constView();
    const VectorConstView<double> zv = z.constView();

    setActiveView("view_00");
    clearView();
    scatter3(xv, yv, zv, properties::Color::GREEN);
    plot3(x, y, z, properties::Color::MAGENTA);
}

void testWithColorVec()
{
    const size_t num_elements = 50;
    const Vector<double> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<double> y = lumos::sin(x);
    const Vector<double> z = lumos::cos(x);
    Vector<properties::Color> color(num_elements);

    for (size_t k = 0; k < num_elements; k++)
    {
        const uint8_t k8 = k;
        color(k) = properties::Color{k8, static_cast<uint8_t>(255U - k8), static_cast<uint8_t>(127U + k8 * 2U)};
    }

    setActiveView("p_view_0");
    clearView();
    scatter3(x, y, z, color, properties::PointSize(20), properties::ScatterStyle::DISC);
}

void addTests()
{
    addTest("cpp", "scatter3", "view", scatter3_ns::testWithVectorView);
    addTest("cpp", "scatter3", "color_vec", scatter3_ns::testWithColorVec);
}

}  // namespace scatter3_ns

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SCATTER3_H_
