#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_DYNAMIC_PLOTTING_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_DYNAMIC_PLOTTING_H_

#include <unistd.h>

#include "lumos/lumos.h"
#include "utils.h"

using namespace lumos;

namespace dynamic_plotting
{

void test2DFunctionExpandingPlotObject()
{
    const size_t num_elements = 100;

    setActiveView("p_view_0");
    clearView();

    double t = 0.0;

    for (size_t k = 0; k < 100; k++)
    {
        const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0 + t, 5.0 + t, num_elements);
        const Vector<double> y = lumos::sin(x) + t;

        t += 0.1;

        plot(x, y, properties::Color::BLUE);
        usleep(1000);
    }
}

void test3DFunctionExpandingPlotObject()
{
    const size_t num_elements = 100;

    setActiveView("p_view_0");
    clearView();

    double t = 0.0;

    for (size_t k = 0; k < 100; k++)
    {
        const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0 + t, 5.0 + t, num_elements);
        const Vector<double> y = lumos::sin(x) + t;
        const Vector<double> z = lumos::cos(x) + t * 2.0;

        t += 0.1;

        plot3(x, y, z, properties::Color::BLUE);
        usleep(1000);
    }
}

void test2DFunctionRotatingView()
{
    const size_t num_elements = 100;

    setActiveView("p_view_0");
    clearView();

    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 5.0, num_elements);
    const Vector<double> y = lumos::sin(x);
    plot(x, y, properties::Color::BLUE, properties::LineWidth(14));

    for (size_t k = 0; k < 100; k++)
    {
        viewAngles(30 + k, 180);
        usleep(10000);
    }
}

void test2DFunctionChangingAxes()
{
    const size_t num_elements = 100;

    setActiveView("p_view_0");
    clearView();

    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 5.0, num_elements);
    const Vector<double> y = lumos::sin(x);
    plot(x, y, properties::Color::BLUE, properties::LineWidth(14));

    const Vec2d min_vec(lumos::min(x), lumos::min(y));
    const Vec2d max_vec(lumos::max(x), lumos::max(y));

    for (size_t k = 0; k < 100; k++)
    {
        axis({min_vec.x, min_vec.y}, {max_vec.x + static_cast<double>(k) / 10.0, max_vec.y});
        usleep(10000);
    }
}

void test2DFunctionNewDataAndClear()
{
    const size_t num_elements = 100;

    setActiveView("p_view_0");
    clearView();

    double t = 0.0;
    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 5.0, num_elements);
    const double min_x = lumos::min(x);
    const double max_x = lumos::max(x);

    for (size_t k = 0; k < 100; k++)
    {
        const Vector<double> y = lumos::sin(x + t);

        t += 0.1;
        const Vec2d min_vec(min_x, lumos::min(y));
        const Vec2d max_vec(max_x, lumos::max(y));

        plot(x, y, properties::Color::BLUE, properties::LineWidth(14));

        axis({min_vec.x, -1.0}, {max_vec.x, 1.0});
        clearViewOnUpdate();

        usleep(10000);
    }
}

void test3DFunctionNewDataAndClear()
{
    const size_t num_elements = 100;

    const auto mat_xy = lumos::meshGrid<double>(-1.0, 1.0, -1.0, 1.0, 100, 100);
    const Matrix<double>& x_mat = mat_xy.first;
    const Matrix<double>& y_mat = mat_xy.second;
    Matrix<double> z_mat{100, 100};

    setActiveView("p_view_0");
    clearView();

    double t = 0.0;
    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 5.0, num_elements);
    viewAngles(10, 10);
    float azimuth = 10.0;

    for (size_t k = 0; k < 1000; k++)
    {
        const Vector<double> y = lumos::sin(x + t);
        const Vector<double> z = lumos::cos(x + t);

        for (size_t r = 0; r < x_mat.numRows(); r++)
        {
            for (size_t c = 0; c < x_mat.numCols(); c++)
            {
                const double x_val = x_mat(r, c);
                const double y_val = y_mat(r, c);
                const double r_val = std::sqrt(x_val * x_val + y_val * y_val);

                const double phi = std::sin(t * 0.2) * 50.0;
                z_mat(r, c) = std::sin(r_val * phi) / (r_val * phi);
            }
        }

        surf(x_mat, y_mat, z_mat, properties::ColorMap::JET);
        t += 0.1;

        azimuth = azimuth > 180.0f ? -180.0f : azimuth + 1.0f;
        axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});
        viewAngles(azimuth, 10);

        usleep(10000);
        clearViewOnUpdate();
    }
}

void test3DFunctionManualInteraction()
{
    const size_t num_elements = 100;

    const auto mat_xy = lumos::meshGrid<double>(-1.0, 1.0, -1.0, 1.0, 100, 100);
    const Matrix<double>& x_mat = mat_xy.first;
    const Matrix<double>& y_mat = mat_xy.second;
    Matrix<double> z_mat{100, 100};

    setActiveView("p_view_0");
    clearView();
    disableAutomaticAxesAdjustment();

    double t = 0.0;
    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 5.0, num_elements);
    viewAngles(10, 10);
    float azimuth = 10.0;

    for (size_t k = 0; k < 1000; k++)
    {
        const Vector<double> y = lumos::sin(x + t);
        const Vector<double> z = lumos::cos(x + t);

        for (size_t r = 0; r < x_mat.numRows(); r++)
        {
            for (size_t c = 0; c < x_mat.numCols(); c++)
            {
                const double x_val = x_mat(r, c);
                const double y_val = y_mat(r, c);
                const double r_val = std::sqrt(x_val * x_val + y_val * y_val);

                const double phi = std::sin(t * 0.2) * 50.0;
                z_mat(r, c) = std::sin(r_val * phi) / (r_val * phi);
            }
        }

        surf(x_mat, y_mat, z_mat, properties::ColorMap::JET);
        t += 0.1;

        azimuth = azimuth > 180.0f ? -180.0f : azimuth + 1.0f;

        usleep(10000);
        clearViewOnUpdate();
    }
}

void addTests()
{
    addTest("cpp", "dynamic_plotting", "2d_expanding_plot", dynamic_plotting::test2DFunctionExpandingPlotObject);
    addTest("cpp", "dynamic_plotting", "3d_expanding_plot", dynamic_plotting::test3DFunctionExpandingPlotObject);
    addTest("cpp", "dynamic_plotting", "2d_changing_rotation", dynamic_plotting::test2DFunctionRotatingView);
    addTest("cpp", "dynamic_plotting", "2d_changing_axes", dynamic_plotting::test2DFunctionChangingAxes);
    addTest("cpp", "dynamic_plotting", "2d_new_data_clear", dynamic_plotting::test2DFunctionNewDataAndClear);
    addTest("cpp", "dynamic_plotting", "3d_new_data_clear", dynamic_plotting::test3DFunctionNewDataAndClear);
    addTest("cpp", "dynamic_plotting", "3d_manual_interaction", dynamic_plotting::test3DFunctionManualInteraction);
}

}  // namespace dynamic_plotting

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_DYNAMIC_PLOTTING_H_
