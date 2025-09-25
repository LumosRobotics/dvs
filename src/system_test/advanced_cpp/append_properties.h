#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_APPEND_PROPERTIES_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_APPEND_PROPERTIES_H_

#include <unistd.h>

#include "duoplot/duoplot.h"
#include "utils.h"

using namespace duoplot;

namespace append_properties
{

void testBasic()
{
    const size_t num_elements = 100;

    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 5.0, num_elements);
    const Vector<double> y = duoplot::sin(x);

    setActiveView("p_view_0");
    clearView();

    setProperties(properties::ID0, properties::Color::BLUE, properties::LineWidth(10));
    setProperties(properties::ID0, properties::LineWidth(20));
    plot(x, y, properties::ID0, properties::Alpha(0.5f), properties::INTERPOLATE_COLORMAP);

    usleep(1000 * 1000);
    setProperties(properties::ID0, properties::Color::RED);
}

void addTests()
{
    addTest("cpp", "append_properties", "basic", append_properties::testBasic);
}

}  // namespace append_properties

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_APPEND_PROPERTIES_H_
