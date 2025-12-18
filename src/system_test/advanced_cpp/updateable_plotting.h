#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_UPDATEABLE_PLOT_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_UPDATEABLE_PLOT_H_

#include "lumos/lumos.h"
#include "utils.h"

using namespace lumos;

namespace updateable_plotting
{

void testRealTimePlotBasic()
{
    const size_t num_its = 1500;

    float t = 0.0f;
    float dt = 0.01f;

    setActiveView("p_view_0");
    clearView();
    viewAngles(180, -90);
    axis({-1.0, -1.5f, -1.0}, {20.0, 1.5f, 1.0f});

    setProperties(properties::ID0, properties::Color::BLUE);
    // #define IGNORE std::cin.ignore();
#define IGNORE
    IGNORE

    for (size_t k = 0; k < num_its; k++)
    {
        const float y = std::sin(t) + 0.1f * std::sin(t * 10.0f);
        t += dt;

        if (k == 600)
        {
            setProperties(properties::ID0, properties::Color::RED);
            IGNORE
            setProperties(properties::ID0, properties::BufferSize(1000U));
        }
        /*if (k < 120)
        {
            realTimePlot(dt, y, properties::ID0, properties::Color::BLUE);
        }
        else if (k == 120)
        {
            setProperties(properties::ID0, properties::Color::RED);
            realTimePlot(dt, y, properties::ID0);
        }
        else
        {
            realTimePlot(dt, y, properties::ID0);
        }*/
        realTimePlot(dt, y, properties::ID0);
        IGNORE
        usleep(10000);
    }
}

void testPlot2Basic()
{
    const size_t num_its = 100;
    const size_t num_elements = 100;

    float t = 0.0f;
    float dt = 0.01f;

    setActiveView("p_view_0");
    clearView();
    axis({-1.0, -1.5f, -1.0}, {6.0, 1.5f, 1.0f});

    setProperties(properties::ID0, properties::not_ready::UPDATABLE);
    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 5.0, num_elements);

    for (size_t k = 0; k < num_its; k++)
    {
        const Vector<double> xt = linspaceFromBoundariesAndCount<double>(0.0 + t, 5.0 + t, num_elements);
        const Vector<double> y = lumos::sin(xt);

        plot(x, y, properties::ID0);

        usleep(1000 * 50);

        t += dt;
    }
}

void testSurfBasic()
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

        surf(x_mat, y_mat, z_mat, properties::ColorMap::JET, properties::not_ready::UPDATABLE, properties::ID0);
        t += 0.1;

        azimuth = azimuth > 180.0f ? -180.0f : azimuth + 1.0f;
        axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});
        viewAngles(azimuth, 10);

        usleep(10000);
    }
}

void testScatter2DAppendable()
{
    const size_t num_elements = 100;

    setActiveView("p_view_0");
    clearView();
    axis({-4.0, -4.0}, {4.0, 4.0});

    double t = 0.0;
    const size_t num_its = 17;

    const size_t num_new_points = 20;

    Vector<double> x{num_new_points}, y{num_new_points};
    Vector<properties::Color> colorp(num_new_points);
    double y_offset = 0.0;

    setProperties(properties::ID0,
                  properties::BufferSize(59),
                  properties::Color::BLUE,
                  properties::ScatterStyle::CIRCLE,
                  properties::PointSize(20));

    for (size_t k = 0; k < num_its; k++)
    {
        t = 0.0;
        for (size_t i = 0; i < num_new_points; i++)
        {
            x(i) = t;
            y(i) = std::sin(t) + y_offset;

            colorp((i + k * 3) % num_new_points).red = i * 25;
            colorp((i + k * 3) % num_new_points).green = 255 - i * 25;
            colorp((i + k * 3) % num_new_points).blue = 255 + i * 25;

            t += 0.1;
        }
        y_offset += 0.1;

        scatter(x, y, colorp, properties::ID0, properties::not_ready::APPENDABLE);

        scatter(x, y, properties::Color::RED, properties::ScatterStyle::CROSS, properties::PointSize(10));

        usleep(10000);
    }
}

void testScatter3DAppendable()
{
    const size_t num_elements = 100;

    setActiveView("p_view_0");
    clearView();
    axis({-4.0, -4.0}, {4.0, 4.0});

    double t = 0.0;
    const size_t num_its = 3;

    const size_t num_new_points = 20;

    Vector<double> x{num_new_points}, y{num_new_points}, z{num_new_points};
    Vector<properties::Color> colorp(num_new_points);
    double y_offset = 0.0;

    setProperties(properties::ID0,
                  properties::BufferSize(60),
                  properties::Color::BLUE,
                  properties::ScatterStyle::CIRCLE,
                  properties::PointSize(20));

    for (size_t k = 0; k < num_its; k++)
    {
        t = 0.0;
        for (size_t i = 0; i < num_new_points; i++)
        {
            x(i) = t;
            y(i) = std::sin(t) + y_offset;
            z(i) = std::cos(t);

            colorp((i + k * 3) % num_new_points).red = i * 25;
            colorp((i + k * 3) % num_new_points).green = 255 - i * 25;
            colorp((i + k * 3) % num_new_points).blue = 255 + i * 25;

            t += 0.1;
        }
        y_offset += 0.1;

        scatter3(x, y, z, colorp, properties::ID0, properties::not_ready::APPENDABLE);

        scatter3(x, y, z, properties::Color::RED, properties::ScatterStyle::CROSS, properties::PointSize(10));

        usleep(10000);
    }
}

void testFastPlot2Appendable()
{
    setActiveView("p_view_0");
    clearView();
    axis({-4.0, -4.0}, {4.0, 4.0});

    double t = 0.0;
    const size_t num_its = 3;

    const size_t num_new_points = 20;

    Vector<double> x{num_new_points}, y{num_new_points};
    Vector<properties::Color> colorp(num_new_points);
    double y_offset = 0.0;

    setProperties(properties::ID0, properties::BufferSize(60), properties::Color::BLUE);

    setProperties(properties::ID1,
                  properties::BufferSize(60),
                  properties::Color::BLUE,
                  properties::ScatterStyle::CIRCLE,
                  properties::PointSize(20));

    for (size_t k = 0; k < num_its; k++)
    {
        for (size_t i = 0; i < num_new_points; i++)
        {
            x(i) = t;
            y(i) = std::sin(t) + y_offset;

            colorp((i + k * 3) % num_new_points).red = i * 25;
            colorp((i + k * 3) % num_new_points).green = 255 - i * 25;
            colorp((i + k * 3) % num_new_points).blue = 255 + i * 25;

            t += 0.1;
        }
        y_offset += 0.1;

        plot(x, y, properties::ID0, properties::not_ready::APPENDABLE);
        scatter(x, y, colorp, properties::ID1, properties::not_ready::APPENDABLE);

        scatter(x, y, properties::Color::RED, properties::ScatterStyle::CROSS, properties::PointSize(10));
    }
}

void testAppendableOnePointOnly()
{
    setActiveView("p_view_0");
    clearView();
    axis({-4.0, -4.0}, {4.0, 4.0});

    double t = 0.0;

    const size_t num_new_points = 20;

    Vector<double> x{1}, y{1};

    setProperties(properties::ID0, properties::BufferSize(60), properties::Color::BLUE);

    setProperties(properties::ID1,
                  properties::BufferSize(60),
                  properties::Color::BLUE,
                  properties::ScatterStyle::CIRCLE,
                  properties::PointSize(20));

    for (size_t i = 0; i < num_new_points; i++)
    {
        x(0) = t;
        y(0) = std::sin(t);

        plot(x, y, properties::ID0, properties::not_ready::APPENDABLE);
        scatter(x, y, properties::ID1, properties::not_ready::APPENDABLE);
        usleep(100 * 1000);

        t += 0.1;
    }
}

void addTests()
{
    addTest("cpp", "updateable_plotting", "basic_real_time", updateable_plotting::testRealTimePlotBasic);
    addTest("cpp", "updateable_plotting", "basic_plot2", updateable_plotting::testPlot2Basic);
    addTest("cpp", "updateable_plotting", "basic_surf", updateable_plotting::testSurfBasic);
    addTest("cpp", "updateable_plotting", "scatter2d_appendable", updateable_plotting::testScatter2DAppendable);
    addTest("cpp", "updateable_plotting", "scatter3d_appendable", updateable_plotting::testScatter3DAppendable);
    addTest("cpp", "updateable_plotting", "fast_plot2d_appendable", updateable_plotting::testFastPlot2Appendable);
    addTest("cpp", "updateable_plotting", "appendable_one_point_only", updateable_plotting::testAppendableOnePointOnly);
}

}  // namespace updateable_plotting

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_SCROLLING_PLOT_H_
