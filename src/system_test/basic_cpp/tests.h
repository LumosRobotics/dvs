#ifndef SYSTEM_TEST_BASIC_CPP_TESTS_H
#define SYSTEM_TEST_BASIC_CPP_TESTS_H

#include <algorithm>

#include "duoplot/duoplot.h"
#include "utils.h"

using namespace duoplot;

namespace basic_cpp
{

properties::Color colorMapJet(const float value)
{
    float _val = value;
    if (_val < 0.0)
    {
        _val = 0.0;
    }
    else if (_val >= 1.0)
    {
        _val = 0.99999;
    }

    const float full_range_value = _val * 6.0;
    const float integer_part = std::floor(full_range_value);
    const float fraction_part = full_range_value - integer_part;

    float r = 0.0, g = 0.0, b = 0.0;

    switch (static_cast<int>(integer_part))
    {
        case 0:
            r = 0.07450980392156863 + fraction_part * 0.11372549019607843;
            g = fraction_part * 0.047058823529411764;
            b = 0.51 + fraction_part * 0.48235294117647065;
            break;
        case 1:
            r = 0.18823529411764706 + fraction_part * 0.0784313725490196;
            g = 0.047058823529411764 + fraction_part * 0.8509803921568628;
            b = 0.9923529411764707 - fraction_part * 0.03137254901960784;
            break;
        case 2:
            r = 0.26666666666666666 + fraction_part * 0.3137254901960785;
            g = 0.8980392156862745 + fraction_part * 0.10196078431372546;
            b = 0.9609803921568628 - fraction_part * 0.5372549019607844;
            break;
        case 3:
            r = 0.5803921568627451 + fraction_part * 0.3999999999999999;
            g = 1.0 - fraction_part * 0.196078431372549;
            b = 0.4237254901960784 - fraction_part * 0.2901960784313725;
            break;
        case 4:
            r = 0.9803921568627451 - fraction_part * 0.0117647058823529;
            g = 0.803921568627451 - fraction_part * 0.611764705882353;
            b = 0.13352941176470587 - fraction_part * 0.05098039215686273;
            break;
        case 5:
            r = 0.9686274509803922 - fraction_part * 0.47058823529411764;
            g = 0.19215686274509805 - fraction_part * 0.19215686274509805;
            b = 0.08254901960784314 - fraction_part * 0.06274509803921569;
            break;
    }

    return properties::Color(
        static_cast<uint8_t>(r * 255.0f), static_cast<uint8_t>(g * 255.0f), static_cast<uint8_t>(b * 255.0f));
}

void testOpenProjectFile()
{
    const std::string base_path = "/Users/danielpi/work/dvs/project_files/";
    const std::vector<std::string> project_files = {"ad_dataset.duoplot",
                                                    "car.duoplot",
                                                    "exp0.duoplot"
                                                    "imu.duoplot",
                                                    "legend.duoplot",
                                                    "particles.duoplot"};

    for (const auto& pf : project_files)
    {
        openProjectFile(base_path + pf);
        usleep(1000 * 100);
    }

    openProjectFile(base_path + "exp0.duoplot");
}

void testSurf()
{
    const int num_rows = 200, num_cols = 250;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);
    Matrix<properties::Color> color(num_rows, num_cols);

    const double inc = 0.4;
    const float nr = (num_rows - 100U);
    const float nc = (num_cols - 100U);
    const float mul = 1.0f / (std::sqrt(nr * nr + nc * nc));

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double rd = static_cast<double>(r - 5) * inc;
            const double cd = static_cast<double>(c - 5) * inc * 2;
            x(r, c) = c;
            y(r, c) = r;
            const float r_val = std::sqrt(rd * rd + cd * cd);
            z(r, c) = 50.0 * std::sin(r_val * 0.05);

            color(r, c) = colorMapJet(r_val * mul);
        }
    }

    setActiveView("p_view_0");
    clearView();
    surf(x, y, z, properties::EdgeColor(0, 0, 0), properties::FaceColor(255, 0, 0));

    setActiveView("p_view_1");
    clearView();
    surf(x, y, z, properties::EdgeColor::NONE, properties::ColorMap::JET_BRIGHT);

    setActiveView("p_view_2");
    clearView();
    surf(x, y, z, properties::EdgeColor::NONE, properties::ColorMap::JET_SOFT, properties::INTERPOLATE_COLORMAP);

    setActiveView("w1_p_view_0");
    clearView();
    surf(x, y, z, color, properties::EdgeColor::NONE, properties::ColorMap::JET);

    setActiveView("p1");
    clearView();
    surf(x, y, z);
    surf(x, y, z + 1.0);
    surf(x, y, z + 2.0);
    surf(x, y, z + 3.0);
    surf(x, y, z + 4.0);
}

void testScatter()
{
    Vector<double> x(20), y(20);
    Vector<float> xf(20), yf(20);

    double t = 0.0;

    for (size_t k = 0; k < 20; k++)
    {
        xf(k) = k;
        yf(k) = 5.0 * std::sin(3.0 * t);

        x(k) = xf(k);
        y(k) = yf(k);

        t = t + 0.1;
    }

    setActiveView("p_view_0");
    clearView();
    axis({0.0, -8.0, -3.3}, {32.0, 32.0, 6.6});

    plot(xf, yf, properties::Color(212, 14, 55));
    scatter(x, y, properties::Color(12, 14, 55));
    scatter(x, y + 1.0, properties::Color::BLACK, properties::PointSize(11));
    scatter(x, y + 2.0, properties::Color::CYAN, properties::PointSize(12), properties::ScatterStyle::SQUARE);
    scatter(x, y + 3.0, properties::Color::MAGENTA, properties::PointSize(13), properties::ScatterStyle::DISC);
    scatter(x, y + 4.0, properties::Color::BLUE, properties::PointSize(14), properties::ScatterStyle::PLUS);
    scatter(x, y + 5.0, properties::Color::RED, properties::PointSize(14), properties::ScatterStyle::CROSS);

    scatter(x,
            y + 8.0,
            properties::Color::BLUE,
            properties::PointSize(50),
            properties::Silhouette{0, 0, 0, 0.1f},
            properties::ScatterStyle::DISC);

    scatter(x,
            y + 9.0,
            properties::Color::BLUE,
            properties::PointSize(50),
            properties::Silhouette{127, 127, 127, 0.5f},
            properties::ScatterStyle::DISC);

    scatter(x,
            y + 10.0,
            properties::Color::BLUE,
            properties::PointSize(50),
            properties::Silhouette{127, 127, 127, 0.9f},
            properties::ScatterStyle::DISC);
    scatter(x,
            y + 11.0,
            properties::Color::BLUE,
            properties::PointSize(50),
            properties::Silhouette{255, 0, 0},
            properties::ScatterStyle::DISC);

    const size_t new_num_elements = 500;

    x.resize(new_num_elements);
    y.resize(new_num_elements);

    for (size_t k = 0; k < new_num_elements; k++)
    {
        const float r = static_cast<float>(rand() % 1001) / 1000.0f;
        const float phi = 2.0f * M_PI * static_cast<float>(rand() % 1001) / 1000.0f;

        const float xf = r * std::cos(phi);
        const float yf = r * std::sin(phi);

        x(k) = xf;
        y(k) = yf;
    }

    setActiveView("p_view_1");
    clearView();

    axis({-1.0, -1.0}, {1.0, 1.0});

    scatter(x,
            y,
            properties::DistanceFrom::xy({0.0, 0.0}, 0.0, 1.0),
            properties::ColorMap::HSV,
            properties::PointSize(13),
            properties::ScatterStyle::CIRCLE);

    setActiveView("p1");
    clearView();
    axis({-4.0, -8.0, -3.3}, {8.0, 8.0, 6.6});
    scatter(x, y);
    scatter(x, y + 1.0, properties::PointSize(11));
    scatter(x, y + 2.0, properties::PointSize(12), properties::ScatterStyle::SQUARE);
    scatter(x, y + 3.0, properties::PointSize(13), properties::ScatterStyle::DISC);
    scatter(x, y + 4.0, properties::PointSize(14), properties::ScatterStyle::PLUS);
    scatter(x, y + 5.0, properties::PointSize(14), properties::ScatterStyle::CROSS);
}

void testScatter3()
{
    const size_t num_elements = 14600;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);

    double t0 = 0.0f, t1 = 0.0f, t2 = 0.0f, y0 = 0.0f, z0 = 0.0f;

    for (size_t k = 0; k < num_elements; k++)
    {
        const float xf = 7.0 * cos(t0) + 20.0f;
        const float yf = 7.0 * sin(t0) - 110.0f;
        const float zf = 0.0f;

        const Vec3f v0 = rotationMatrixY<float>(t1) * Vec3f(xf, yf, zf);
        const Vec3f v1 = rotationMatrixZ<float>(t2) * (v0 + Vec3f(50.0f, y0, 0.0f));

        x(k) = v1.x;
        y(k) = v1.y;
        z(k) = v1.z + z0 - 70;
        t0 = t0 + 0.3;
        t1 = t1 - 0.01;
        t2 = t2 + 0.002;
        y0 = y0 + 0.05;
        z0 = z0 + 0.02;
    }

    setActiveView("p_view_0");
    clearView();

    plot3(x, y, z, properties::Color(255, 14, 255), properties::LineWidth(1));
    scatter3(x, y, z, properties::Color(12, 14, 55));
    scatter3(x, y, z + 1.0, properties::Color::BLACK, properties::PointSize(11));
    scatter3(x, y, z + 2.0, properties::Color::CYAN, properties::PointSize(12), properties::ScatterStyle::SQUARE);
    scatter3(x, y, z + 3.0, properties::Color::MAGENTA, properties::PointSize(13), properties::ScatterStyle::DISC);
    scatter3(x, y, z + 4.0, properties::Color::BLUE, properties::PointSize(14), properties::ScatterStyle::PLUS);
    scatter3(x, y, z + 5.0, properties::Color::RED, properties::PointSize(14), properties::ScatterStyle::CROSS);

    const size_t new_num_elements = 500;

    x.resize(new_num_elements);
    y.resize(new_num_elements);
    z.resize(new_num_elements);

    for (size_t k = 0; k < new_num_elements; k++)
    {
        const float r = static_cast<float>(rand() % 1001) / 1000.0f;
        const float theta = M_PI * static_cast<float>(rand() % 1001) / 1000.0f;
        const float phi = 2.0f * M_PI * static_cast<float>(rand() % 1001) / 1000.0f;

        const float xf = r * std::sin(theta) * std::cos(phi);
        const float yf = r * std::sin(theta) * std::sin(phi);
        const float zf = r * std::cos(theta);

        // const float xf = 2.0f * static_cast<float>(rand() % 1001) / 1000.0f - 1.0f;
        // const float yf = 2.0f * static_cast<float>(rand() % 1001) / 1000.0f - 1.0f;
        // const float zf = 2.0f * static_cast<float>(rand() % 1001) / 1000.0f - 1.0f;

        x(k) = (xf < 0) ? -xf : xf;
        y(k) = yf;
        z(k) = zf;
    }

    setActiveView("p_view_1");
    clearView();

    axis({-2.0, -2.0, -2.0}, {2.0, 2.0, 2.0});

    scatter3(x,
             y,
             z,
             properties::DistanceFrom::x(0.0, 0.0, 1.0),
             properties::ColorMap::JET,
             properties::PointSize(13),
             properties::ScatterStyle::CIRCLE);

    setActiveView("w1_p_view_0");
    clearView();

    const size_t new_new_num_elements = 100;

    x.resize(new_new_num_elements);
    y.resize(new_new_num_elements);
    z.resize(new_new_num_elements);

    float t = 0.0;

    for (size_t k = 0; k < new_new_num_elements; k++)
    {
        x(k) = t;
        y(k) = std::sin(2.0 * t);
        z(k) = std::cos(t);

        t = t + 0.1;
    }

    plot3(x, y, z, properties::Color(212, 14, 55));
    scatter3(x, y, z, properties::Color(12, 14, 55));
    scatter3(x, y, z + 2.0, properties::Color::BLACK, properties::PointSize(11));
    scatter3(x, y, z + 4.0, properties::Color::CYAN, properties::PointSize(12), properties::ScatterStyle::SQUARE);
    scatter3(x, y, z + 6.0, properties::Color::MAGENTA, properties::PointSize(13), properties::ScatterStyle::DISC);
    scatter3(x, y, z + 8.0, properties::Color::BLUE, properties::PointSize(14), properties::ScatterStyle::PLUS);
    scatter3(x, y, z + 10.0, properties::Color::RED, properties::PointSize(14), properties::ScatterStyle::CROSS);

    scatter3(x,
             y,
             z + 12.0,
             properties::Color::BLUE,
             properties::PointSize(50),
             properties::Silhouette{0, 0, 0, 0.1f},
             properties::ScatterStyle::DISC);

    scatter3(x,
             y,
             z + 14.0,
             properties::Color::BLUE,
             properties::PointSize(50),
             properties::Silhouette{127, 127, 127, 0.5f},
             properties::ScatterStyle::DISC);

    scatter3(x,
             y,
             z + 16.0,
             properties::Color::BLUE,
             properties::PointSize(50),
             properties::Silhouette{127, 127, 127, 0.9f},
             properties::ScatterStyle::DISC);
    scatter3(x,
             y,
             z + 18.0,
             properties::Color::BLUE,
             properties::PointSize(50),
             properties::Silhouette{255, 0, 0},
             properties::ScatterStyle::DISC);
}

void testPlotCollection()
{
    const size_t num_elements = 40;
    Vector<double> x0(num_elements), y0(num_elements), x1(num_elements), y1(num_elements), x2(num_elements),
        y2(num_elements);

    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        const double kd = k;
        x0(k) = 10.0 * cos(t);
        y0(k) = kd * 0.05 * 10.0 * sin(t) + 0.2;

        x1(k) = 14.0 * cos(t);
        y1(k) = kd * 0.07 * 11.0 * sin(t) + 0.3;

        x2(k) = 9.0 * cos(t);
        y2(k) = kd * 0.08 * 15.0 * sin(t) + 0.4;
        t = t + 0.1;
    }

    std::vector<VectorConstView<double>> pc_x = {x0.constView(), x1.constView(), x2.constView()};
    std::vector<VectorConstView<double>> pc_y = {y0.constView(), y1.constView(), y2.constView()};

    setActiveView("p_view_0");
    clearView();

    plotCollection(pc_x, pc_y, properties::Color(0, 0, 0));
    scatter(x0, y0, properties::Color(212, 14, 55), properties::PointSize(10));
    scatter(x1, y1, properties::Color(0, 14, 55), properties::PointSize(10));
    scatter(x2, y2, properties::Color(0, 255, 55), properties::PointSize(10));

    std::vector<Vector<double>> pcm_x;
    std::vector<Vector<double>> pcm_y;

    auto rand_num = [](void) -> double { return static_cast<double>(rand() % 1001) / 1000.0; };

    std::vector<size_t> np = {50, 50, 50, 50, 56};

    for (size_t k = 0; k < 30; k++)
    {
        const size_t num_elems = (rand() % 20) + 50;
        Vector<double> x(num_elems), y(num_elems);

        t = 0.0;

        const double t_inc = rand_num() / 100.0 + 0.05;
        const double x_a = rand_num() * 3.0;
        const double y_a = rand_num() * 3.0;
        const double x_o = rand_num() * 10.0;
        const double y_o = rand_num() * 10.0;

        for (size_t i = 0; i < num_elems; i++)
        {
            x(i) = x_a * cos(t) + x_o;
            y(i) = y_a * t * sin(t) + y_o;
            t += t_inc;
        }
        pcm_x.push_back(x);
        pcm_y.push_back(y);
    }

    setActiveView("p_view_1");
    clearView();

    for (size_t k = 0; k < pcm_x.size(); k++)
    {
        scatter(pcm_x[k], pcm_y[k], properties::Color(255, 0, 0), properties::PointSize(10));
    }

    plotCollection(pcm_x, pcm_y, properties::Color(0, 0, 0));
}

void testPlotCollection3()
{
    const size_t num_elements = 40;
    Vector<double> x0(num_elements), y0(num_elements), z0(num_elements), x1(num_elements), y1(num_elements),
        z1(num_elements), x2(num_elements), y2(num_elements), z2(num_elements);

    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        const double kd = k;
        x0(k) = 10.0 * cos(t);
        y0(k) = kd * 0.05 * 10.0 * sin(t) + 0.2;
        z0(k) = kd * 0.05 * 10.0 * cos(t) * sin(t) + 0.3;

        x1(k) = 14.0 * cos(t);
        y1(k) = kd * 0.07 * 11.0 * sin(t) + 0.3;
        z1(k) = kd * 0.07 * 11.0 * sin(t) * cos(t) + 0.3;

        x2(k) = 9.0 * cos(t);
        y2(k) = kd * 0.08 * 15.0 * sin(t) + 0.4;
        z2(k) = kd * 0.08 * 15.0 * sin(t) * cos(t) + 0.4;
        t = t + 0.1;
    }

    std::vector<VectorConstView<double>> pc_x = {x0.constView(), x1.constView(), x2.constView()};
    std::vector<VectorConstView<double>> pc_y = {y0.constView(), y1.constView(), y2.constView()};
    std::vector<VectorConstView<double>> pc_z = {z0.constView(), z1.constView(), z2.constView()};

    setActiveView("p_view_0");
    clearView();

    plotCollection3(pc_x, pc_y, pc_z, properties::Color(0, 0, 0));
    scatter3(x0, y0, z0, properties::Color(212, 14, 55), properties::PointSize(10));
    scatter3(x1, y1, z1, properties::Color(0, 14, 55), properties::PointSize(10));
    scatter3(x2, y2, z2, properties::Color(0, 255, 55), properties::PointSize(10));

    std::vector<Vector<double>> pcm_x;
    std::vector<Vector<double>> pcm_y;
    std::vector<Vector<double>> pcm_z;

    auto rand_num = [](void) -> double { return static_cast<double>(rand() % 1001) / 1000.0; };

    std::vector<size_t> np = {50, 50, 50, 50, 56};

    for (size_t k = 0; k < 30; k++)
    {
        const size_t num_elems = (rand() % 20) + 50;
        Vector<double> x(num_elems), y(num_elems), z(num_elems);

        t = 0.0;

        const double t_inc = rand_num() / 100.0 + 0.05;
        const double x_a = rand_num() * 3.0;
        const double y_a = rand_num() * 3.0;
        const double z_a = rand_num() * 3.0;
        const double x_o = rand_num() * 10.0;
        const double y_o = rand_num() * 10.0;
        const double z_o = rand_num() * 10.0;

        for (size_t i = 0; i < num_elems; i++)
        {
            x(i) = x_a * cos(t) + x_o;
            y(i) = y_a * t * sin(t) + y_o;
            z(i) = z_a * t * sin(t) * cos(t) + z_o;
            t += t_inc;
        }
        pcm_x.push_back(std::move(x));
        pcm_y.push_back(std::move(y));
        pcm_z.push_back(std::move(z));
    }

    setActiveView("p_view_1");
    clearView();

    for (size_t k = 0; k < pcm_x.size(); k++)
    {
        scatter3(pcm_x[k], pcm_y[k], pcm_z[k], properties::Color(255, 0, 0), properties::PointSize(10));
    }

    plotCollection3(pcm_x, pcm_y, pcm_z, properties::Color(0, 0, 0));
}

void testPlot()
{
    const size_t num_elements = 30;
    Vector<float> x(num_elements), y(num_elements), z(num_elements);

    const size_t num_points = 7;
    Vector<float> xp(num_points), yp(num_points), zp(num_points);
    Vector<properties::Color> colorp(num_points);

    Vector<float> x_some_equal(8U), y_some_equal(8U);
    Vector<properties::Color> color_some_equal(8U);
    Vector<float> x_all_equal(3U), y_all_equal(3U);

    xp(0) = 0.0;
    xp(1) = 1.0;
    xp(2) = 2.2;
    xp(3) = 3.0;
    xp(4) = 3.5;
    xp(5) = 3.5;
    xp(6) = 4.5;

    yp(0) = 0.0;
    yp(1) = 3.5;
    yp(2) = 3.5;
    yp(3) = 2.5;
    yp(4) = 4.0;
    yp(5) = 2.0;
    yp(6) = 2.0;

    colorp(0) = properties::Color{255, 0, 0};
    colorp(1) = properties::Color{255, 255, 0};
    colorp(2) = properties::Color{255, 0, 255};
    colorp(3) = properties::Color{0, 255, 0};
    colorp(4) = properties::Color{0, 255, 255};
    colorp(5) = properties::Color{0, 0, 0};
    colorp(6) = properties::Color{0, 0, 255};

    zp.fill(0.01f);

    setActiveView("p_view_0");
    clearView();

    axis({-1.0, -1.0, -1.0}, {5.0, 5.0, 1.0});
    plot(xp, yp, colorp, properties::LineWidth(20), properties::Color(200, 200, 200));
    scatter3(xp, yp, zp, properties::PointSize(10), properties::Color(255, 0, 0));

    zp.fill(-0.01f);
    plot3(xp, yp, zp, properties::LineWidth(60), properties::Color(0, 255, 0));

    viewAngles(0, 90);

    setActiveView("p1");
    clearView();

    xp(0) = 0.0;
    xp(1) = 0.0;
    xp(2) = 1.0;
    xp(3) = 1.0;
    xp(4) = 3.5;
    xp(5) = 3.5;

    yp(0) = 0.0;
    yp(1) = 3.0;
    yp(2) = 3.0;
    yp(3) = 0.0;
    yp(4) = 4.0;
    yp(5) = 2.0;

    x_some_equal(0) = 0.0f;
    x_some_equal(1) = 0.0f;
    x_some_equal(2) = 1.0f;
    x_some_equal(3) = 1.0f;
    x_some_equal(4) = 2.0f;
    x_some_equal(5) = 2.0f;
    x_some_equal(6) = 3.0f;
    x_some_equal(7) = 3.0f;

    y_some_equal(0) = -1.0f;
    y_some_equal(1) = -1.0f;
    y_some_equal(2) = -1.5f;
    y_some_equal(3) = -1.5f;
    y_some_equal(4) = -1.7f;
    y_some_equal(5) = -1.7f;
    y_some_equal(6) = -1.8f;
    y_some_equal(7) = -1.8f;

    color_some_equal(0) = properties::Color(255, 0, 0);      // [0.0f, -1.0f], Red,      Point will be added
    color_some_equal(1) = properties::Color(0, 255, 0);      // [0.0f, -1.0f], Green
    color_some_equal(2) = properties::Color(0, 0, 255);      // [1.0f, -1.5f], Blue,     Point will be added
    color_some_equal(3) = properties::Color(255, 255, 0);    // [1.0f, -1.5f], Yellow
    color_some_equal(4) = properties::Color(255, 0, 255);    // [2.0f, -1.7f], Magenta,  Point will be added
    color_some_equal(5) = properties::Color(255, 255, 255);  // [2.0f, -1.7f], White
    color_some_equal(6) = properties::Color(0, 255, 255);    // [3.0f, -1.8f], Cyan,     Point will be added
    color_some_equal(7) = properties::Color(0, 0, 0);        // [3.0f, -1.8f], Black

    x_all_equal(0) = 0.0f;
    x_all_equal(1) = 0.0f;
    x_all_equal(2) = 0.0f;

    y_all_equal(0) = -1.0f;
    y_all_equal(1) = -1.0f;
    y_all_equal(2) = -1.0f;

    axis({-2.0, -2.0, -1.0}, {6.0, 6.0, 1.0});
    // plot(xp, yp, properties::LineWidth(50), properties::Color(200, 200, 200));
    plot(x_some_equal, y_some_equal, properties::LineWidth(50), properties::Color::RED);
    plot(x_some_equal, y_some_equal + 2.0f, color_some_equal, properties::LineWidth(50));
    plot(x_all_equal, y_all_equal, properties::LineWidth(50), properties::Color::BLUE);
    viewAngles(0, 90);

    setActiveView("p_view_1");
    float t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        x(k) = 10.0 * cos(t) + 20.0;
        y(k) = 10.0 * sin(t) + 20.0 + k;
        z(k) = 0.01f;

        t = t + 0.3;
    }

    axis({0.0, 16.0, -1.0}, {50.0, 64.0, 1.0});
    plot(x + 3.0f, y, properties::Color(0, 255, 255), properties::LineWidth(1));
    plot(x + 4.0f, y, properties::Color(212, 14, 55), properties::LineWidth(1));
    plot(x + 5.0f, y, properties::Color(212, 255, 55), properties::LineWidth(4));
    plot(x + 6.0f, y, properties::Color(212, 14, 255), properties::LineWidth(7));
    plot(x + 6.0f, y, properties::Color(212, 14, 255), properties::LineWidth(7), properties::PERSISTENT);
    scatter3(x + 3.0f, y, z, properties::Color::BLACK, properties::PointSize(14));

    setActiveView("p_view_2");
    clearView();
    t = 0.0;
    x.resize(num_elements * 10);
    y.resize(num_elements * 10);
    z.resize(num_elements * 10);
    Vector<properties::Color> color(num_elements * 10);

    for (size_t k = 0; k < (num_elements * 10); k++)
    {
        x(k) = 10.0f * cos(t) + 20.0f;
        y(k) = 10.0f * sin(t) + 20.0f + static_cast<float>(k) / 10.0f;
        z(k) = 0.01f;

        color(k) = colorMapJet(static_cast<float>(k) / static_cast<float>(num_elements * 10U - 1U));

        t = t + 0.03;
    }

    axis({0.0, 16.0, -1.0}, {50.0, 64.0, 1.0});
    plot(x + 3.0f, y, properties::LineWidth(3));
    plot(x + 4.0f, y, properties::LineWidth(3));
    plot(x + 5.0f, y, properties::LineWidth(3));
    plot(x + 6.0f, y, color, properties::Color(212, 14, 255), properties::LineWidth(7));

    setActiveView("w1_p_view_0");
    clearView();

    axis({0.0, 16.0, -1.0}, {50.0, 64.0, 1.0});
    plot(x, y, properties::LineWidth(7));
    plot(x + 1.0f, y, properties::LineWidth(7));
    plot(x + 2.0f, y, properties::LineWidth(7));
    plot(x + 3.0f, y, properties::LineWidth(7));
    plot(x + 4.0f, y, properties::LineWidth(7));
    plot(x + 5.0f, y, properties::LineWidth(7));
    plot(x + 6.0f, y, properties::LineWidth(7));
}

void testFastPlot()
{
    const size_t num_elements = 30;
    Vector<float> x(num_elements), y(num_elements);

    setActiveView("p_view_0");
    clearView();

    float t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        x(k) = 10.0 * cos(t) + 20.0;
        y(k) = 10.0 * sin(t) + 20.0 + k;

        t = t + 0.3;
    }

    axis({0.0, 16.0, -1.0}, {50.0, 64.0, 1.0});
    plot(x + 3.0f, y, properties::Color(0, 255, 255), properties::not_ready::FAST_PLOT);
    plot(x + 4.0f, y, properties::Color(212, 14, 55), properties::not_ready::FAST_PLOT);
    plot(x + 5.0f, y, properties::Color(212, 255, 55), properties::not_ready::FAST_PLOT);
    plot(x + 6.0f, y, properties::Color(212, 14, 255), properties::not_ready::FAST_PLOT);

    setActiveView("p_view_2");
    clearView();
    t = 0.0;
    x.resize(num_elements * 10);
    y.resize(num_elements * 10);

    for (size_t k = 0; k < (num_elements * 10); k++)
    {
        x(k) = 10.0f * cos(t) + 20.0f;
        y(k) = 10.0f * sin(t) + 20.0f + static_cast<float>(k) / 10.0f;

        t = t + 0.03;
    }

    axis({0.0, 16.0, -1.0}, {50.0, 64.0, 1.0});
    plot(x + 3.0f, y, properties::Color(0, 255, 255), properties::not_ready::FAST_PLOT);
    plot(x + 4.0f, y, properties::Color(212, 14, 55), properties::not_ready::FAST_PLOT);
    plot(x + 5.0f, y, properties::Color(212, 255, 55), properties::not_ready::FAST_PLOT);
    plot(x + 6.0f, y, properties::Color(212, 14, 255), properties::not_ready::FAST_PLOT);
}

void testLineCollection()
{
    const size_t num_lines = 5;
    Vector<float> x(num_lines * 2), y(num_lines * 2);

    setActiveView("p_view_0");
    clearView();

    float t = 0.0f;

    size_t idx = 0;

    for (size_t k = 0; k < num_lines; k++)
    {
        x(idx) = t * 2.0f;
        y(idx) = t * 2.0f;

        x(idx + 1) = t * 2.0f + cos(t);
        y(idx + 1) = t * 2.0f + sin(t);

        idx += 2;

        t = t + 0.3;
    }

    axis({0.0, 0.0, -1.0}, {10.0, 10.0, 1.0});
    lineCollection(x, y, properties::Color(255, 0, 0));
}

void testLineCollection3()
{
    const size_t num_lines = 5;
    Vector<float> x(num_lines * 2), y(num_lines * 2), z(num_lines * 2);

    setActiveView("p_view_0");
    clearView();

    float t = 0.0f;

    size_t idx = 0;

    for (size_t k = 0; k < num_lines; k++)
    {
        x(idx) = t * 2.0f;
        y(idx) = t * 2.0f;
        z(idx) = t * 2.0f;

        x(idx + 1) = t * 2.0f + cos(t);
        y(idx + 1) = t * 2.0f + sin(t);
        z(idx + 1) = t * 2.0f + sin(t) + cos(t);

        idx += 2;

        t = t + 0.3;
    }

    axis({0.0, 0.0, -1.0}, {10.0, 10.0, 10.0});
    lineCollection3(x, y, z, properties::Color(255, 0, 0));
}

void testStem()
{
    const size_t num_elements = 30;

    const Vector<float> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<float> y = duoplot::sin(x);

    setActiveView("p_view_0");
    clearView();

    axis({0.0, -1.0, -1.0}, {5.0, 5.0, 1.0});
    stem(x, y, properties::Color::RED);
}

void testStairs()
{
    const size_t num_elements = 30;
    Vector<float> xf(num_elements), yf(num_elements);

    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        xf(k) = t;
        yf(k) = 10.0 * sin(t) + 20.0 + k;

        t = t + 0.3;
    }

    setActiveView("p_view_0");
    clearView();

    stairs(xf, yf, properties::Color(21, 14, 255), properties::LineWidth(2));
    plot(xf, yf, properties::Color(21, 14, 255), properties::LineWidth(2));
    scatter(xf, yf, properties::Color(255, 0, 0), properties::PointSize(10));
}

void testPlot3()
{
    openProjectFile("../../project_files/exp0.duoplot");
    setActiveView("p_view_0");
    clearView();

    const size_t num_points = 6;
    Vector<float> xp(num_points), yp(num_points), zp(num_points);
    Vector<float> x_some_equal(8U), y_some_equal(8U), z_some_equal(8U);
    Vector<properties::Color> color_some_equal(8U);
    Vector<float> x_all_equal(3U), y_all_equal(3U), z_all_equal(3U);

    xp(0) = 0.0;
    xp(1) = 1.0;
    xp(2) = 2.2;
    xp(3) = 3.0;
    xp(4) = 3.5;
    xp(5) = 3.5;

    yp(0) = 0.0;
    yp(1) = 3.5;
    yp(2) = 3.5;
    yp(3) = 2.5;
    yp(4) = 4.0;
    yp(5) = 2.0;

    zp(0) = 0.0;
    zp(1) = 0.1;
    zp(2) = 0.2;
    zp(3) = 0.3;
    zp(4) = 0.4;
    zp(5) = 0.5;

    zp.fill(0.01f);

    axis({-1.0, -1.0, -1.0}, {5.0, 5.0, 1.0});
    plot3(xp, yp, zp, properties::LineWidth(60), properties::Color(200, 200, 200));
    scatter3(xp, yp, zp, properties::PointSize(10), properties::Color(255, 0, 0));

    const size_t num_elements = 30;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);
    Vector<float> xf(num_elements), yf(num_elements), zf(num_elements);

    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        xf(k) = 1.0 * cos(t) + 0.0;
        yf(k) = 1.0 * sin(t) + 0.0;
        zf(k) = t;

        x(k) = xf(k);
        y(k) = yf(k);
        z(k) = t;
        t = t + 0.3;
    }

    setActiveView("p_view_1");
    clearView();

    plot3(x, y, z, properties::Color(212, 14, 55), properties::LineWidth(1));
    plot3(xf + 0.1f, yf, zf, properties::Color(21, 14, 55), properties::LineWidth(7));

    setActiveView("p_view_2");
    clearView();

    x_some_equal(0) = 0.0f;
    x_some_equal(1) = 0.0f;
    x_some_equal(2) = 1.0f;
    x_some_equal(3) = 1.0f;
    x_some_equal(4) = 2.0f;
    x_some_equal(5) = 2.0f;
    x_some_equal(6) = 3.0f;
    x_some_equal(7) = 3.0f;

    y_some_equal(0) = -1.0f;
    y_some_equal(1) = -1.0f;
    y_some_equal(2) = -1.5f;
    y_some_equal(3) = -1.5f;
    y_some_equal(4) = -1.7f;
    y_some_equal(5) = -1.7f;
    y_some_equal(6) = -1.8f;
    y_some_equal(7) = -1.8f;

    z_some_equal.fill(0.0f);

    color_some_equal(0) = properties::Color(255, 0, 0);      // [0.0f, -1.0f], Red,      Point will be added
    color_some_equal(1) = properties::Color(0, 255, 0);      // [0.0f, -1.0f], Green
    color_some_equal(2) = properties::Color(0, 0, 255);      // [1.0f, -1.5f], Blue,     Point will be added
    color_some_equal(3) = properties::Color(255, 255, 0);    // [1.0f, -1.5f], Yellow
    color_some_equal(4) = properties::Color(255, 0, 255);    // [2.0f, -1.7f], Magenta,  Point will be added
    color_some_equal(5) = properties::Color(255, 255, 255);  // [2.0f, -1.7f], White
    color_some_equal(6) = properties::Color(0, 255, 255);    // [3.0f, -1.8f], Cyan,     Point will be added
    color_some_equal(7) = properties::Color(0, 0, 0);        // [3.0f, -1.8f], Black

    x_all_equal(0) = 0.0f;
    x_all_equal(1) = 0.0f;
    x_all_equal(2) = 0.0f;

    y_all_equal(0) = -1.0f;
    y_all_equal(1) = -1.0f;
    y_all_equal(2) = -1.0f;
    z_all_equal.fill(0.0f);

    axis({-2.0, -2.0, -1.0}, {6.0, 6.0, 1.0});
    plot3(x_some_equal, y_some_equal, z_some_equal, properties::LineWidth(50), properties::Color::RED);
    plot3(x_some_equal, y_some_equal + 2.0f, z_some_equal, color_some_equal, properties::LineWidth(50));
    plot3(x_all_equal, y_all_equal, z_all_equal, properties::LineWidth(50), properties::Color::BLUE);
}

void testFastPlot3()
{
    const size_t num_elements = 30;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);
    Vector<float> xf(num_elements), yf(num_elements), zf(num_elements);

    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        xf(k) = 1.0 * cos(t) + 0.0;
        yf(k) = 1.0 * sin(t) + 0.0;
        zf(k) = t;

        x(k) = xf(k);
        y(k) = yf(k);
        z(k) = t;
        t = t + 0.3;
    }

    setActiveView("p_view_0");
    clearView();

    plot3(x, y, z, properties::Color(212, 14, 55), properties::not_ready::FAST_PLOT);
    plot3(xf + 0.1f, yf, zf, properties::Color(21, 14, 55), properties::not_ready::FAST_PLOT);
}

void testImShow4()
{
    const uint32_t num_rows = 800, num_cols = 800;
    ImageRGBA<float> img4(num_rows, num_cols);
    ImageRGB<float> img3(num_rows, num_cols);

    for (uint32_t r = 0; r < num_rows; r++)
    {
        for (uint32_t c = 0; c < num_cols; c++)
        {
            const float xr = 3.0f * (static_cast<float>(c) - 300.5f) / 800.0f;
            const float yr = 3.0f * (static_cast<float>(r) - 400.5f) / 800.0f;
            const float rr = std::sqrt(xr * xr + yr * yr);

            const float xg = 2.0f * (static_cast<float>(c) - 500.5f) / 800.0f;
            const float yg = 2.0f * (static_cast<float>(r) - 350.5f) / 800.0f;
            const float rg = std::sqrt(xg * xg + yg * yg);

            const float xb = 4.0f * (static_cast<float>(c) - 200.5f) / 800.0f;
            const float yb = 4.0f * (static_cast<float>(r) - 600.5f) / 800.0f;
            const float rb = std::sqrt(xb * xb + yb * yb);

            img4(r, c, 0) = (std::sin(rb) / rr + 1.0f) * 0.5f;
            img4(r, c, 1) = (std::sin(rb) / rg + 1.0f) * 0.5f;
            img4(r, c, 2) = (std::sin(rb) / rb + 1.0f) * 0.5f;
            img4(r, c, 3) = static_cast<float>(r + c) / 1600;

            img3(r, c, 0) = (std::sin(rb) / rr + 1.0f) * 0.5f;
            img3(r, c, 1) = (std::sin(rb) / rg + 1.0f) * 0.5f;
            img3(r, c, 2) = (std::sin(rb) / rb + 1.0f) * 0.5f;
        }
    }
    setActiveView("p_view_0");
    clearView();
    imShow(img4);

    setActiveView("p_view_1");
    clearView();
    imShow(img3);
}

void testImShow()
{
    const uint32_t num_rows = 800, num_cols = 800;
    ImageRGB<float> img3(num_rows, num_cols);
    ImageGray<float> img1(num_rows, num_cols);
    ImageRGBA<double> img4(num_rows, num_cols);

    ImageGray<uint8_t> img1_uint8(num_rows, num_cols);
    ImageRGB<uint8_t> img3_uint8(num_rows, num_cols);

    for (uint32_t r = 0; r < num_rows; r++)
    {
        for (uint32_t c = 0; c < num_cols; c++)
        {
            const float xr = 3.0f * (static_cast<float>(c) - 300.5f) / 800.0f;
            const float yr = 3.0f * (static_cast<float>(r) - 400.5f) / 800.0f;
            const float rr = std::sqrt(xr * xr + yr * yr);

            const float xg = 2.0f * (static_cast<float>(c) - 500.5f) / 800.0f;
            const float yg = 2.0f * (static_cast<float>(r) - 350.5f) / 800.0f;
            const float rg = std::sqrt(xg * xg + yg * yg);

            const float xb = 4.0f * (static_cast<float>(c) - 200.5f) / 800.0f;
            const float yb = 4.0f * (static_cast<float>(r) - 600.5f) / 800.0f;
            const float rb = std::sqrt(xb * xb + yb * yb);

            img3(r, c, 0) = (std::sin(rb) / rr + 1.0f) * 0.5f;
            img3(r, c, 1) = (std::sin(rb) / rg + 1.0f) * 0.5f;
            img3(r, c, 2) = (std::sin(rb) / rb + 1.0f) * 0.5f;

            img4(r, c, 0) = (std::sin(rb) / rr + 1.0f) * 0.5f;
            img4(r, c, 1) = (std::sin(rb) / rg + 1.0f) * 0.5f;
            img4(r, c, 2) = (std::sin(rb) / rb + 1.0f) * 0.5f;
            img4(r, c, 3) = static_cast<float>(r + c) / 1600;

            img1(r, c) = (std::sin(rb) / rr + 1.0f) * 0.5f;

            img1_uint8(r, c) = static_cast<uint8_t>(100.0f * (std::sin(rb) + 1.0f) / rr);

            img3_uint8(r, c, 0) = static_cast<uint8_t>(100.0f * (std::sin(rb) + 1.0f) / rr);
            img3_uint8(r, c, 1) = static_cast<uint8_t>(100.0f * (std::sin(rb) + 1.0f) / rg);
            img3_uint8(r, c, 2) = static_cast<uint8_t>(100.0f * (std::sin(rb) + 1.0f) / rb);
        }
    }
    setActiveView("p_view_0");
    clearView();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    setActiveViewToImageView();
    imShow(img3, properties::Alpha(0.5f));

    setActiveView("p_view_1");
    clearView();
    imShow(img1);

    setActiveView("p_view_2");
    clearView();
    imShow(img1_uint8);

    setActiveView("w1_p_view_0");
    clearView();
    imShow(img3_uint8);

    setActiveView("p1");
    clearView();
    imShow(img4);
}

void testAxis()
{
    setActiveView("p_view_0");
    clearView();

    axis({1.0, 2.5}, {7.2, 8.4});

    setActiveView("p_view_1");
    clearView();

    axis({-1.0, 1.5, 3.4}, {5.4, 9.2, 5.5});
}

void testLegend()
{
    const int num_rows = 20, num_cols = 25;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);

    double inc = 0.4;
    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double rd = static_cast<double>(r - 5) * inc;
            const double cd = static_cast<double>(c - 5) * inc * 2;
            x(r, c) = c;
            y(r, c) = r;
            z(r, c) = std::sin(std::sqrt(rd * rd + cd * cd));
        }
    }

    const size_t num_elements = 30;
    Vector<float> x0(num_elements), y0(num_elements);
    Vector<float> x1(num_elements), y1(num_elements);
    Vector<float> x2(num_elements), y2(num_elements);

    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        x0(k) = 10.0 * cos(t);
        y0(k) = 10.0 * sin(t) + k;

        t = t + 0.3;
    }

    x1 = x0 * 2.0f;
    y1 = y0 * 2.0f;

    x2 = x0 * 3.0f;
    y2 = y0 * 3.0f;

    setActiveView("p_view_0");
    clearView();

    axis({-32.0, 0.0, -2.0}, {32.0, 110.0, 2.0});
    plot(x0, y0, properties::Color(212, 14, 55), properties::Label("AAAaaaBBBbbb"));
    plot(x1, y1, properties::Color(12, 255, 55), properties::Label("sig0"));
    plot(x2, y2, properties::Color(127, 14, 255), properties::Label("ej0293e2?rq430#€pqigj"));
    surf(x, y, z, properties::EdgeColor(0, 255, 0), properties::FaceColor(255, 0, 0), properties::Label("SURF"));
    surf(x,
         y,
         z + 1.0,
         properties::EdgeColor(255, 255, 255),
         properties::ColorMap::JET,
         properties::Label("SURF_COLOR_MAP"));
    scatter(x0, y0, properties::Color(255, 0, 0), properties::Label("Scatter"));
    scatter(x1,
            y1,
            properties::Color(255, 0, 255),
            properties::Label("Scatter"),
            properties::PointSize(40),
            properties::ScatterStyle::CROSS);
    showLegend();

    setActiveView("p_view_1");
    clearView();

    axis({-32.0, 0.0, -2.0}, {32.0, 110.0, 2.0});
    plot(x0, y0, properties::Color(212, 14, 55), properties::Label("AAAaaaBBBbbb"));
    plot(x1, y1, properties::Color(12, 255, 55), properties::Label("sig0"));
    plot(x2, y2, properties::Color(127, 14, 255), properties::Label("ej0293e2?rq430#€pqigj"));
    surf(x, y, z, properties::EdgeColor(0, 255, 0), properties::FaceColor(255, 0, 0), properties::Label("SURF"));
    surf(x, y, z + 1.0, properties::EdgeColor(0, 0, 0), properties::ColorMap::JET, properties::Label("SURF_COLOR_MAP"));
    showLegend();

    setActiveView("p_view_2");
    clearView();

    surf(x, y, z, properties::EdgeColor(0, 255, 0), properties::ColorMap::JET, properties::Label("Jet"));
    surf(x + 1.0, y, z, properties::EdgeColor(0, 255, 0), properties::ColorMap::HSV, properties::Label("Rainbow"));
    surf(x + 2.0, y, z, properties::EdgeColor(0, 255, 0), properties::ColorMap::MAGMA, properties::Label("Magma"));
    surf(x + 3.0, y, z, properties::EdgeColor(0, 255, 0), properties::ColorMap::VIRIDIS, properties::Label("Viridis"));
    surf(x + 4.0, y, z, properties::EdgeColor(0, 255, 0), properties::ColorMap::PASTEL, properties::Label("Pastel"));

    showLegend();
}

void testDrawMesh()
{
    using tp = double;

    Vector<Point3<tp>> vertices(12);
    vertices(0) = Point3<tp>(0.0, 0.0, 0.0);
    vertices(1) = Point3<tp>(1.0, 0.0, 0.0);
    vertices(2) = Point3<tp>(0.5, -1.0, 1.0);

    vertices(3) = Point3<tp>(0.0, 0.0, 0.0);
    vertices(4) = Point3<tp>(-1.0, 0.0, 0.0);
    vertices(5) = Point3<tp>(0.0, 1.0, 1.0);

    vertices(6) = Point3<tp>(0.0, 0.0, 0.0);
    vertices(7) = Point3<tp>(-1.0, 0.0, 0.0);
    vertices(8) = Point3<tp>(0.0, 1.0, -1.0);

    vertices(9) = Point3<tp>(0.0, 0.0, 0.0);
    vertices(10) = Point3<tp>(1.0, 0.0, 0.0);
    vertices(11) = Point3<tp>(0.5, -1.0, -1.0);

    Vector<IndexTriplet> indices(4);
    indices(0) = IndexTriplet(0, 1, 2);
    indices(1) = IndexTriplet(3, 4, 5);
    indices(2) = IndexTriplet(6, 7, 8);
    indices(3) = IndexTriplet(9, 10, 11);

    Vector<tp> x(12), y(12), z(12);

    for (size_t k = 0; k < x.size(); k++)
    {
        x(k) = vertices(k).x;
        y(k) = vertices(k).y;
        z(k) = vertices(k).z + 2.0;
    }

    setActiveView("p_view_0");
    clearView();

    axis({-2.0, -2.0, -2.0}, {2.0, 2.0, 2.0});

    drawMesh(vertices, indices, properties::EdgeColor(0, 0, 0), properties::FaceColor(12, 244, 244));
    drawMesh(x, y, z, indices, properties::EdgeColor(0, 0, 0), properties::FaceColor(255, 0, 244));

    drawMesh(x + 2.0, y, z, indices, properties::EdgeColor::NONE, properties::FaceColor(255, 0, 244));
    drawMesh(x + 2.0, y, z - 2.0, indices, properties::EdgeColor::RED, properties::FaceColor::NONE);
}

void testSetProperties()
{
    const size_t num_elements = 30;

    const Vector<float> x = linspaceFromBoundariesAndCount<float>(0.0f, 5.0f, num_elements);
    const Vector<float> y = duoplot::sin(x);

    setActiveView("p_view_0");
    clearView();
    axis({0.0, -1.0, -1.0}, {5.0, 5.0, 1.0});

    setProperties(properties::ID0, properties::Color::CYAN, properties::LineWidth(20));
    plot(x, y, properties::ID0);

    scatter(x, y + 2.0f, properties::ID1, properties::Color::MAGENTA, properties::ScatterStyle::PLUS);

    std::cout << "Initializing plot to be CYAN and LineWidth(10), press enter to continue..." << std::endl;

    std::cin.ignore();

    setProperties(properties::ID0, properties::Color::YELLOW, properties::LineWidth(3));
    setProperties(properties::ID1, properties::Color::RED, properties::ScatterStyle::CIRCLE);
    std::cout << "Setting properties of plot to be YELLOW and LineWidth(3)" << std::endl;
}

void testDeleteObject()
{
    const int num_rows = 200, num_cols = 250;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);

    const double inc = 0.4;

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double rd = static_cast<double>(r - 5) * inc;
            const double cd = static_cast<double>(c - 5) * inc * 2;
            x(r, c) = c;
            y(r, c) = r;
            const float r_val = std::sqrt(rd * rd + cd * cd);
            z(r, c) = 50.0 * std::sin(r_val * 0.05);
        }
    }

    setActiveView("p_view_0");
    clearView();
    surf(x, y, z, properties::FaceColor::BLUE);
    surf(x, y, z + 1.0, properties::FaceColor::YELLOW, properties::ID7);

    std::cout << "Press enter to continue delete yellow surf object..." << std::endl;

    std::cin.ignore();

    deletePlotObject(properties::ID7);
}

struct Rect
{
    std::int32_t top_left_x;
    std::int32_t top_left_y;
    std::int32_t width;
    std::int32_t height;
};

void renderRects(ImageRGB<std::uint8_t>& img, const std::vector<Rect>& rects)
{
    for (std::size_t r = 0; r < img.numRows(); r++)
    {
        for (std::size_t c = 0; c < img.numCols(); c++)
        {
            img(r, c, 0) = (255.0f * 0.543f);
            img(r, c, 1) = (255.0f * 0.751f);
            img(r, c, 2) = (255.0f * 0.684f);
        }
    }

    const std::int32_t margin = 20;
    const std::int32_t radius = 5;

    const float margin_f = margin;
    const float radius_f = radius;
    const float margin_and_radius = margin_f + radius_f;

    for (const auto& rect : rects)
    {
        const std::int32_t min_row = std::max(0, rect.top_left_y - margin);
        const std::int32_t max_row =
            std::min(static_cast<std::int32_t>(img.numRows()) - 1, rect.top_left_y + rect.height + margin);

        const std::int32_t min_col = std::max(0, rect.top_left_x - margin);
        const std::int32_t max_col =
            std::min(static_cast<std::int32_t>(img.numCols()) - 1, rect.top_left_x + rect.width + margin);

        const float rect_width_f = rect.width;
        const float rect_height_f = rect.height;
        const float rect_top_left_x_f = rect.top_left_x;
        const float rect_top_left_y_f = rect.top_left_y;

        const auto shadow_mapping = [](const float d) -> float { return d * 2.0f; };

        const Vec2<std::int32_t> top_left_corner(rect.top_left_x, rect.top_left_y);
        const Vec2<std::int32_t> top_right_corner(rect.top_left_x + rect.width, rect.top_left_y);
        const Vec2<std::int32_t> bottom_left_corner(rect.top_left_x, rect.top_left_y + rect.height);
        const Vec2<std::int32_t> bottom_right_corner(rect.top_left_x + rect.width, rect.top_left_y + rect.height);

        const Vec2<float> top_left_corner_f(rect.top_left_x, rect.top_left_y);
        const Vec2<float> top_right_corner_f(rect.top_left_x + rect.width, rect.top_left_y);
        const Vec2<float> bottom_left_corner_f(rect.top_left_x, rect.top_left_y + rect.height);
        const Vec2<float> bottom_right_corner_f(rect.top_left_x + rect.width, rect.top_left_y + rect.height);

        const Vec2<std::int32_t> top_left_inner_corner(rect.top_left_x + radius, rect.top_left_y + radius);
        const Vec2<std::int32_t> top_right_inner_corner(rect.top_left_x + rect.width - radius,
                                                        rect.top_left_y + radius);
        const Vec2<std::int32_t> bottom_left_inner_corner(rect.top_left_x + radius,
                                                          rect.top_left_y + rect.height - radius);
        const Vec2<std::int32_t> bottom_right_inner_corner(rect.top_left_x + rect.width - radius,
                                                           rect.top_left_y + rect.height - radius);

        const Vec2<float> top_left_inner_corner_f(rect.top_left_x + radius, rect.top_left_y + radius);
        const Vec2<float> top_right_inner_corner_f(rect.top_left_x + rect.width - radius, rect.top_left_y + radius);
        const Vec2<float> bottom_left_inner_corner_f(rect.top_left_x + radius, rect.top_left_y + rect.height - radius);
        const Vec2<float> bottom_right_inner_corner_f(rect.top_left_x + rect.width - radius,
                                                      rect.top_left_y + rect.height - radius);

        for (std::int32_t r = min_row; r <= max_row; r++)
        {
            const float rf = r;

            for (std::int32_t c = min_col; c < max_col; c++)
            {
                const float cf = c;
                if (r < rect.top_left_y)
                {
                    if (c < rect.top_left_x)
                    {
                        // Top left corner
                        const float x_diff = cf - top_left_inner_corner_f.x;
                        const float y_diff = rf - top_left_inner_corner_f.y;
                        const float d = (std::sqrt(x_diff * x_diff + y_diff * y_diff) - radius_f) / margin_f;
                        const float sm = shadow_mapping(d);

                        if (sm <= 1.0f)
                        {
                            img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                            img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                            img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                        }
                    }
                    else if (c <= (rect.top_left_x + rect.width))
                    {
                        // Top center side
                        const float d = static_cast<float>(std::abs(r - rect.top_left_y)) / margin_f;
                        const float sm = shadow_mapping(d);
                        img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                        img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                        img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                    }
                    else
                    {
                        // Top right corner
                        const float x_diff = cf - (rect_top_left_x_f + rect_width_f);
                        const float y_diff = rf - rect_top_left_y_f;
                        const float d = std::sqrt(x_diff * x_diff + y_diff * y_diff) / margin_f;
                        const float sm = shadow_mapping(d);

                        if (sm <= 1.0f)
                        {
                            img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                            img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                            img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                        }
                    }
                }
                else if (r <= (rect.top_left_y + rect.height))
                {
                    if (c < rect.top_left_x)
                    {
                        // Left side

                        if (r < top_left_inner_corner.y)
                        {
                            const float x_diff = cf - top_left_inner_corner_f.x;
                            const float y_diff = rf - top_left_inner_corner_f.y;
                            const float d = (std::sqrt(x_diff * x_diff + y_diff * y_diff) - radius_f) / margin_f;
                            const float sm = shadow_mapping(d);

                            if (sm <= 1.0f)
                            {
                                img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                                img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                                img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                            }
                        }
                        else
                        {
                            const float d = static_cast<float>(std::abs(c - top_left_corner.x)) / margin;
                            const float sm = shadow_mapping(d);
                            img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                            img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                            img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                        }
                    }
                    else if (c <= (rect.top_left_x + rect.width))
                    {
                        // Inside

                        if ((c <= top_left_inner_corner.x) && (r <= top_left_inner_corner.y))
                        {
                            const float x_diff = cf - top_left_inner_corner_f.x;
                            const float y_diff = rf - top_left_inner_corner_f.y;
                            // const float d = (std::sqrt(x_diff * x_diff + y_diff * y_diff) - radius_f) / margin_f;
                            const float d = std::sqrt(x_diff * x_diff + y_diff * y_diff);
                            const float sm = shadow_mapping((d - radius_f) / margin_f);

                            if (d >= radius_f)
                            {
                                img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                                img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                                img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                            }
                        }

                        /*if ((c < (rect.top_left_x + radius)) && (r < (rect.top_left_y + radius)))
                        {
                            // Top left corner
                            const float x_diff = cf - (rect_top_left_x_f + radius_f);
                            const float y_diff = rf - (rect_top_left_y_f + radius_f);
                            const float d = std::sqrt(x_diff * x_diff + y_diff * y_diff) / radius_f;

                            if (d > 1.0f)
                            {
                                const float sm = shadow_mapping(d);

                                img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                                img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                                img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                            }
                        }
                        else
                        {
                            img(r, c, 0) = 127;
                            img(r, c, 1) = 0;
                            img(r, c, 2) = 0;
                        }*/
                    }
                    else
                    {
                        // Right side
                        const float d = static_cast<float>(std::abs(c - (rect.top_left_x + rect.width))) / margin_f;
                        const float sm = shadow_mapping(d);
                        img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                        img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                        img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                    }
                }
                else
                {
                    if (c < rect.top_left_x)
                    {
                        // Bottom left corner
                        const float x_diff = cf - rect_top_left_x_f;
                        const float y_diff = rf - (rect_top_left_y_f + rect_height_f);
                        const float d = std::sqrt(x_diff * x_diff + y_diff * y_diff) / margin_f;
                        const float sm = shadow_mapping(d);

                        if (sm <= 1.0f)
                        {
                            img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                            img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                            img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                        }
                    }
                    else if (c <= (rect.top_left_x + rect.width))
                    {
                        // Bottom center side
                        const float d = static_cast<float>(std::abs(r - (rect.top_left_y + rect.height))) / margin_f;
                        const float sm = shadow_mapping(d);
                        img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                        img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                        img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                    }
                    else
                    {
                        // Bottom right corner
                        const float x_diff = cf - (rect_top_left_x_f + rect_width_f);
                        const float y_diff = rf - (rect_top_left_y_f + rect_height_f);
                        const float d = std::sqrt(x_diff * x_diff + y_diff * y_diff) / margin_f;
                        const float sm = shadow_mapping(d);

                        if (sm <= 1.0f)
                        {
                            img(r, c, 0) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 0)) * sm);
                            img(r, c, 1) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 1)) * sm);
                            img(r, c, 2) = static_cast<std::uint8_t>(static_cast<float>(img(r, c, 2)) * sm);
                        }
                    }
                }

                /*if ((c == (rect.top_left_x)) || (c == (rect.top_left_x + rect.width)))
                {
                    img(r, c, 0) = 255U;
                    img(r, c, 1) = 255U;
                    img(r, c, 2) = 255U;
                }*/
            }
        }
    }
}

void testBackground()
{
    std::vector<Rect> rects;

    rects.push_back({50, 60, 30, 40});

    rects.push_back({110, 70, 20, 30});

    const int num_rows = 200, num_cols = 250;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);

    const double inc = 0.4;

    ImageRGB<std::uint8_t> img(num_rows, num_cols);

    renderRects(img, rects);

    setActiveView("p_view_0");
    clearView();
    imShow(img);
    // setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    // axesSquare();
}

void testPointSelector()
{
    const std::string project_file_path = "../../project_files/small.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 500;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);
    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        const double r = t * 2.0;
        const double theta = t;

        x(k) = r * std::cos(theta);
        y(k) = r * std::sin(theta);
        z(k) = t;

        t += 0.1;
    }

    setActiveView("p_view_0");
    clearView();

    axis({-96.0, -96.0, 0.0}, {96.0, 96.0, 52.0});
    viewAngles(0, 90);

    scatter3(x, y, z, properties::PointSize(13));
    plot3(x, y, z);
}

void testPointSelectorWithExcludeSelection()
{
    const std::string project_file_path = "../../project_files/small.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 500;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);
    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        const double r = t * 2.0;
        const double theta = t;

        x(k) = r * std::cos(theta);
        y(k) = r * std::sin(theta);
        z(k) = 0.0;

        t += 0.1;
    }

    setActiveView("p_view_0");
    clearView();

    axis({-24.0, -24.0}, {24.0, 24.0});
    viewAngles(-15, 72);

    scatter3(x, y, z, properties::PointSize(13), properties::EXCLUDE_FROM_SELECTION);
    scatter3(x, y + 5.0, z, properties::PointSize(13));
}

void testPlot2DashedLine()
{
    const std::string project_file_path = "../../project_files/dashed_lines.duoplot";
    openProjectFile(project_file_path);

    const auto prepare_element = [](const std::string& element_name) -> void {
        setActiveView(element_name);
        clearView();
    };

    const auto assign_vec = [](const float x, const float y) -> std::pair<float, float> {
        return std::make_pair(x, y);
    };

    {
        prepare_element("p0");

        const size_t num_elements = 5;
        Vector<float> x0(num_elements), y0(num_elements);
        Vector<float> x1(3), y1(3);
        Vector<float> x2(3), y2(3);
        Vector<float> x3(4), y3(4);

        std::tie(x0(0), y0(0)) = assign_vec(1.0f, 2.0f);
        std::tie(x0(1), y0(1)) = assign_vec(1.0f, 7.0f);
        std::tie(x0(2), y0(2)) = assign_vec(6.0f, 9.0f);
        std::tie(x0(3), y0(3)) = assign_vec(8.0f, 7.0f);
        std::tie(x0(4), y0(4)) = assign_vec(7.0f, 1.0f);

        std::tie(x1(0), y1(0)) = assign_vec(3.0f, 2.0f);
        std::tie(x1(1), y1(1)) = assign_vec(3.0f, 5.0f);
        std::tie(x1(2), y1(2)) = assign_vec(5.0f, 7.0f);

        std::tie(x2(0), y2(0)) = assign_vec(1.0f, 1.0f);
        std::tie(x2(1), y2(1)) = assign_vec(5.0f, 1.0f);
        std::tie(x2(2), y2(2)) = assign_vec(8.0f, 1.0f);

        std::tie(x3(0), y3(0)) = assign_vec(5.0f, 2.0f);
        std::tie(x3(1), y3(1)) = assign_vec(5.0f, 4.0f);
        std::tie(x3(2), y3(2)) = assign_vec(6.0f, 4.0f);
        std::tie(x3(3), y3(3)) = assign_vec(6.0f, 2.0f);

        axis({0.0, 0.0}, {10.0, 10.0});
        plot(x0, y0, properties::LineWidth(60), properties::LineStyle::DASHED);
        plot(x1, y1, properties::LineWidth(100), properties::LineStyle::DASHED);
        plot(x2, y2, properties::LineWidth(100), properties::LineStyle::DASHED);
        plot(x3, y3, properties::LineWidth(100), properties::LineStyle::DASHED);
    }
    {
        prepare_element("p1");

        const size_t num_elements = 3;
        Vector<float> x(num_elements), y(num_elements);
        Vector<float> x0(9), y0(9);

        std::tie(x(0), y(0)) = assign_vec(3.0f, 1.0f);
        std::tie(x(1), y(1)) = assign_vec(6.0f, 4.0f);
        std::tie(x(2), y(2)) = assign_vec(9.0f, 1.0f);

        std::tie(x0(0), y0(0)) = assign_vec(0.5f, 0.5f);
        std::tie(x0(1), y0(1)) = assign_vec(1.0f, 1.0f);
        std::tie(x0(2), y0(2)) = assign_vec(2.0f, 2.0f);
        std::tie(x0(3), y0(3)) = assign_vec(3.0f, 3.0f);
        std::tie(x0(4), y0(4)) = assign_vec(4.5f, 4.5f);
        std::tie(x0(5), y0(5)) = assign_vec(6.0f, 6.0f);
        std::tie(x0(6), y0(6)) = assign_vec(8.0f, 8.0f);
        std::tie(x0(7), y0(7)) = assign_vec(9.0f, 9.0f);
        std::tie(x0(8), y0(8)) = assign_vec(10.0f, 10.0f);

        axis({0.0, 0.0}, {10.0, 10.0});
        plot(x, y, properties::LineWidth(60), properties::LineStyle::DASHED);
        plot(x0, y0, properties::LineWidth(100), properties::LineStyle::DASHED);
    }
    {
        prepare_element("p2");

        const size_t num_elements = 3;
        Vector<float> x(num_elements), y(num_elements);

        std::tie(x(0), y(0)) = assign_vec(2.0f, 2.0f);
        std::tie(x(1), y(1)) = assign_vec(5.0f, 8.0f);
        std::tie(x(2), y(2)) = assign_vec(8.0f, 2.0f);
        axis({0.0, 0.0}, {10.0, 10.0});
        plot(x, y, properties::LineWidth(60), properties::LineStyle::DASHED);
    }
    {
        prepare_element("p2");

        const std::vector<size_t> sizes = {10, 100, 1000, 10000, 100000};
        const std::vector<properties::LineStyle> line_styles = {properties::LineStyle::DASHED,
                                                                properties::LineStyle::DASHED,
                                                                properties::LineStyle::LONG_DASHED,
                                                                properties::LineStyle::SHORT_DASHED,
                                                                properties::LineStyle::DASHED};
        float offset = 0.0f;

        for (size_t i = 0; i < 5; i++)
        {
            const float inc = 10.0f / static_cast<float>(sizes[i]);

            const size_t num_elements = sizes[i];
            Vector<float> x(num_elements), y0(num_elements), y1(num_elements);

            for (size_t k = 0; k < num_elements; k++)
            {
                x(k) = static_cast<float>(k) * inc;
                y0(k) = std::sin(x(k));
                y1(k) = x(k) * 0.1f - 0.5f;
            }

            axis({-0.5, -1.2}, {10.5, 1.2});
            plot(x, y0 + offset, properties::LineWidth(2), line_styles[i]);
            plot(x, y1 + offset, properties::LineWidth(2), line_styles[i]);
            offset += 0.1f;
        }
    }
    {
        prepare_element("p3");

        const size_t num_elements = 1000;
        Vector<float> x(num_elements), y(num_elements);
        Vector<float> x0(2), y0(2);

        std::tie(x0(0), y0(0)) = assign_vec(0.0f, -1.0f);
        std::tie(x0(1), y0(1)) = assign_vec(10.0f, 0.0f);

        for (size_t k = 0; k < num_elements; k++)
        {
            x(k) = static_cast<float>(k) * 0.01f;
            y(k) = x(k) * 0.1f - 0.5f;
        }

        axis({-0.5, -1.2}, {10.5, 1.2});
        plot(x, y, properties::LineWidth(20), properties::LineStyle::DASHED);
        plot(x0, y0, properties::LineWidth(20), properties::LineStyle::DASHED);
    }
    {
        prepare_element("p4");

        const size_t num_elements = 1000;
        Vector<float> x(num_elements), y0(num_elements), y1(num_elements);

        for (size_t k = 0; k < num_elements; k++)
        {
            x(k) = static_cast<float>(k) * 0.01f;
            y0(k) = std::sin(x(k)) * 1000.0f;
            y1(k) = (x(k) * 0.1f - 0.5f) * 1000.0f;
        }

        axis({-0.5, -1000.0}, {10.5, 1000.0});
        plot(x, y0, properties::LineWidth(2), properties::LineStyle::DASHED);
        plot(x, y1, properties::LineWidth(2), properties::LineStyle::DASHED);
    }
    {
        prepare_element("p5");

        const size_t num_elements = 1000;
        Vector<float> x(num_elements), y0(num_elements), y1(num_elements), y2(num_elements);

        for (size_t k = 0; k < num_elements; k++)
        {
            x(k) = static_cast<float>(k) * 0.01f;
            y0(k) = (static_cast<float>(rand() % 1001) / 1000.0f - 0.5f) * 1000.0f;
            y1(k) = (static_cast<float>(rand() % 1001) / 1000.0f - 0.5f) * 1000.0f;
            y2(k) = (static_cast<float>(rand() % 1001) / 1000.0f - 0.5f) * 1000.0f;
        }

        axis({-0.5, -1000.0}, {10.5, 1000.0});
        plot(x, y0 - 500.0f, properties::LineWidth(2), properties::LineStyle::DASHED);
        plot(x, y1, properties::LineWidth(2), properties::LineStyle::DASHED);
        plot(x, y2 + 500.0f, properties::LineWidth(2), properties::LineStyle::DASHED);
    }
    {
        prepare_element("p6");

        const size_t num_elements = 1000;
        Vector<float> x(num_elements), y(num_elements);

        for (size_t k = 0; k < num_elements; k++)
        {
            x(k) = static_cast<float>(k) * 0.01f;
            y(k) = std::sin(x(k));
        }

        axis({-0.5, -1.2}, {10.5, 1.2});
        plot(x, y, properties::LineWidth(20), properties::LineStyle::DASHED);
        plot(x, y, properties::LineWidth(20));

        plot(x, y + 0.1f, properties::LineWidth(20), properties::LineStyle::SHORT_DASHED);
        plot(x, y + 0.1f, properties::LineWidth(20));

        plot(x, y - 0.1f, properties::LineWidth(20), properties::LineStyle::LONG_DASHED);
        plot(x, y - 0.1f, properties::LineWidth(20));
    }
}

void testSetTitle()
{
    const std::string project_file_path = "../../project_files/exp0.duoplot";
    openProjectFile(project_file_path);

    setActiveView("p_view_0");
    clearView();
    setTitle("This is p_view_0");

    setActiveView("p1");
    clearView();
    setTitle("This is p1");

    setActiveView("p_view_1");
    clearView();
    setTitle("This is p_view_1");

    setActiveView("p_view_2");
    clearView();
    setTitle("This is p_view_2");

    setActiveView("s_view_0");
    clearView();
    setTitle("This is s_view_0");

    setActiveView("s_view_1");
    clearView();
    setTitle("This is s_view_1");

    setActiveView("s_view_2");
    clearView();
    setTitle("This is s_view_2");

    setActiveView("w1_p_view_0");
    clearView();
    setTitle("This is w1_p_view_0");

    setActiveView("w1_p_view_1");
    clearView();
    setTitle("This is w1_p_view_1");
}

void testScreenshot()
{
    const int num_rows = 200, num_cols = 250;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);
    Matrix<properties::Color> color(num_rows, num_cols);

    const double inc = 0.4;
    const float nr = (num_rows - 100U);
    const float nc = (num_cols - 100U);
    const float mul = 1.0f / (std::sqrt(nr * nr + nc * nc));

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double rd = static_cast<double>(r - 5) * inc;
            const double cd = static_cast<double>(c - 5) * inc * 2;
            x(r, c) = c;
            y(r, c) = r;
            const float r_val = std::sqrt(rd * rd + cd * cd);
            z(r, c) = 50.0 * std::sin(r_val * 0.05);

            color(r, c) = colorMapJet(r_val * mul);
        }
    }

    setActiveView("p_view_0");
    clearView();
    surf(x, y, z, properties::EdgeColor(0, 0, 0), properties::FaceColor(255, 0, 0));

    setActiveView("p_view_1");
    clearView();
    surf(x, y, z, properties::EdgeColor::NONE, properties::ColorMap::JET_BRIGHT);

    setActiveView("p_view_2");
    clearView();
    surf(x, y, z, properties::EdgeColor::NONE, properties::ColorMap::JET_SOFT, properties::INTERPOLATE_COLORMAP);

    setActiveView("w1_p_view_0");
    clearView();
    surf(x, y, z, color, properties::EdgeColor::NONE, properties::ColorMap::JET);

    setActiveView("p1");
    clearView();
    surf(x, y, z);
    surf(x, y, z + 1.0);
    surf(x, y, z + 2.0);
    surf(x, y, z + 3.0);
    surf(x, y, z + 4.0);

    screenshot("/Users/danielpi/work/dvs/screenshots");
}

void testDrawCubes()
{
    const size_t num_elements = 100;
    Vector<double> x(num_elements), y(num_elements), z(num_elements), sizes(num_elements);

    double t0 = 0.0f, t1 = 0.0f, t2 = 0.0f, y0 = 0.0f, z0 = 0.0f;

    for (size_t k = 0; k < num_elements; k++)
    {
        const float xf = 7.0 * cos(t0) + 20.0f;
        const float yf = 7.0 * sin(t0) - 110.0f;
        const float zf = 0.0f;

        const Vec3f v0 = rotationMatrixY<float>(t1) * Vec3f(xf, yf, zf);
        const Vec3f v1 = rotationMatrixZ<float>(t2) * (v0 + Vec3f(50.0f, y0, 0.0f));

        x(k) = v1.x;
        y(k) = v1.y;
        z(k) = v1.z + z0 - 70;
        sizes(k) = static_cast<double>(rand() % 1001) / 1000.0f + 0.3;
        t0 = t0 + 0.3;
        t1 = t1 - 0.01;
        t2 = t2 + 0.002;
        y0 = y0 + 0.05;
        z0 = z0 + 0.02;
    }

    setActiveView("p_view_0");
    clearView();
    globalIllumination({2.0, 2.0, 2.0});

    drawCubes(x, y, z, 1.0, properties::ColorMap::MAGMA, properties::EdgeColor::NONE);
    drawCubes(x - 20.0, y, z, sizes, properties::ColorMap::JET_BRIGHT, properties::EdgeColor::NONE);
}

void testCreateNewElement()
{
    const std::string project_file_path = "../../project_files/exp0.duoplot";
    openProjectFile(project_file_path);

    const Vector<double> t = linspaceFromBoundariesAndCount(0.0, 2.0 * M_PI, 1000);
    const Vector<double> x = duoplot::cos(t);

    setActiveView("p_view_0");
    clearView();
    plot(t, x);

    setActiveView("a_non_existing_element");
    clearView();
    plot(t, x);
}

void testAxesSquare()
{
    const size_t num_elements = 100;
    Vector<double> x(num_elements), y(num_elements), z(num_elements), t(num_elements);
    Vector<double> xc(10U), yc(10U), zc(10U);

    xc = VectorInitializer<double>({-1.0, 1.0, 1.0, -1.0, -1.0, -1.0, 1.0, 1.0, -1.0, -1.0});
    yc = VectorInitializer<double>({-1.0, -1.0, 1.0, 1.0, -1.0, -1.0, -1.0, 1.0, 1.0, -1.0});
    zc = VectorInitializer<double>({-1.0, -1.0, -1.0, -1.0, -1.0, 1.0, 1.0, 1.0, 1.0, 1.0});

    t = linspaceFromBoundariesAndCount(0.0, 2.0 * M_PI, num_elements);

    x = duoplot::cos(t);
    y = duoplot::sin(t);
    z = 2.0 * t / (2.0 * M_PI) - 1.0;

    setActiveView("p_view_0");
    clearView();

    axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});
    plot3(x, y, z, properties::LineWidth(20));
    viewAngles(32, 45);
    axesSquare();

    setActiveView("p_view_1");
    clearView();

    axis({-1.0, -1.0, -1.0}, {1.0, 2.0, 1.0});
    plot3(x, y, z, properties::LineWidth(20));
    viewAngles(32, 45);
    axesSquare();

    setActiveView("p_view_2");
    clearView();

    axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});
    plot3(x, y, z, properties::LineWidth(20));
    viewAngles(32, 45);

    setActiveView("p1");
    clearView();

    axis({-100.0, -100.0, -1.0}, {100.0, 100.0, 1.0});
    plot3(x * 100.0, y * 100.0, z, properties::LineWidth(20));
    plot3(xc * 100.0, yc * 100.0, zc, properties::LineWidth(20));
    viewAngles(32, 45);
    axesSquare();
}

void testNan()
{
    openProjectFile("../../project_files/exp0.duoplot");

    const size_t num_elements = 100;
    Vector<double> x(num_elements), y(num_elements);

    x = linspaceFromBoundariesAndCount(0.0, 1.0, num_elements);
    y = duoplot::sin(x);

    setActiveView("p1");
    clearView();

    x(4U) = NAN;

    for (size_t k = 0; k < num_elements; k++)
    {
        x(k) = NAN;
    }
    plot(x, y, properties::LineWidth(20));
}

void testDifferentAxesScaling()
{
    openProjectFile("../../project_files/exp0.duoplot");

    const size_t num_elements = 100;
    Vector<double> x(num_elements), y(num_elements);

    const std::vector<std::string> element_names{
        "p_view_0", "p1", "p_view_1", "p_view_2", "s_view_0", "s_view_1", "s_view_2", "w1_p_view_0", "w1_p_view_1"};

    // const std::vector<std::string> element_names{"p_view_0"};

    const std::vector<double> multipliers{1e-3, 1e-6, 1e-9, 1e12, 1e15, 1e38, 1e-21, 1e-24, 1e-27};

    x = linspaceFromBoundariesAndCount(-1.0 * M_PI, 1.0 * M_PI, num_elements);
    y = duoplot::sin(x);

    for (size_t k = 0; k < element_names.size(); k++)
    {
        const std::string en = element_names[k];
        Vector<double> x_scaled = x * multipliers[k];
        Vector<double> y_scaled = y * multipliers[k];
        setActiveView(en);
        clearView();
        setTitle(std::to_string(std::log10(multipliers[k])));

        plot(x_scaled, y_scaled, properties::LineWidth(20));
    }
}

void testDifferentScreenSpacePrimitive()
{
    openProjectFile("../../project_files/exp0.duoplot");

    setActiveView("p_view_0");
    clearView();

    drawScreenSpaceRect(0.0, 0.0, 0.5, 0.5, properties::Color(255, 0, 0));

    setActiveView("p1");
    clearView();

    drawScreenSpaceRect(0.0, 0.0, 1.0, 1.0, properties::Color::BLACK);
    drawScreenSpaceRect(-1.0, -1.0, 1.0, 1.0, properties::Color::GREEN);

    setActiveView("p_view_1");
    clearView();

    drawScreenSpaceRect(0.0, 0.0, 0.99, 0.99, properties::Color::MAGENTA);
}

void addTests()
{
    addTest("cpp", "basic", "scatter", testScatter);
    addTest("cpp", "basic", "scatter3", testScatter3);
    addTest("cpp", "basic", "plot", testPlot);
    addTest("cpp", "basic", "plot3", testPlot3);
    addTest("cpp", "basic", "fastPlot", testFastPlot);
    addTest("cpp", "basic", "fastPlot3", testFastPlot3);
    addTest("cpp", "basic", "lineCollection", testLineCollection);
    addTest("cpp", "basic", "lineCollection3", testLineCollection3);
    addTest("cpp", "basic", "stem", testStem);
    addTest("cpp", "basic", "plotCollection", testPlotCollection);
    addTest("cpp", "basic", "plotCollection3", testPlotCollection3);
    addTest("cpp", "basic", "surf", testSurf);
    addTest("cpp", "basic", "imShow", testImShow);
    addTest("cpp", "basic", "axis", testAxis);
    addTest("cpp", "basic", "drawMesh", testDrawMesh);
    addTest("cpp", "basic", "stairs", testStairs);
    addTest("cpp", "basic", "legend", testLegend);
    addTest("cpp", "basic", "openProjectFile", testOpenProjectFile);
    addTest("cpp", "basic", "setProperties", testSetProperties);
    addTest("cpp", "basic", "deletePlotObject", testDeleteObject);
    addTest("cpp", "basic", "background", testBackground);
    addTest("cpp", "basic", "point_selector", testPointSelector);
    addTest("cpp", "basic", "point_selector_with_exclude", testPointSelectorWithExcludeSelection);
    addTest("cpp", "basic", "plot2_dashed_line", testPlot2DashedLine);
    addTest("cpp", "basic", "set_title", testSetTitle);
    addTest("cpp", "basic", "screenshot", testScreenshot);
    addTest("cpp", "basic", "draw_cubes", testDrawCubes);
    addTest("cpp", "basic", "create_new_element", testCreateNewElement);
    addTest("cpp", "basic", "axes_square", testAxesSquare);
    addTest("cpp", "basic", "nan", testNan);
    addTest("cpp", "basic", "axes_numbers", testDifferentAxesScaling);
    addTest("cpp", "basic", "screen_space_primitive", testDifferentScreenSpacePrimitive);
}

}  // namespace basic_cpp

#endif  // TEST_APPLICATIONS_BASIC_TESTS_CPP_TESTS_H_
