#ifndef DEMOS_SMALL_H
#define DEMOS_SMALL_H

#include <complex>
#include <random>

#include "debug_value_args.h"
#include "particles/particles.h"

namespace small
{

namespace
{

properties::Color calculateColormapViridis(double value)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value >= 1.0)
    {
        value = 0.99999;
    }

    double full_range_value = value * 8.0;
    double integer_part = floor(full_range_value);
    double fraction_part = full_range_value - integer_part;

    double r = 0.0, g = 0.0, b = 0.0;

    switch (static_cast<int>(integer_part))
    {
        case 0:
            r = 0.2627450980392157 + fraction_part * 0.01568627450980392;
            g = fraction_part * 0.16470588235294117;
            b = 0.32941176470588235 + fraction_part * 0.1568627450980392;
            break;
        case 1:
            r = 0.2784313725490196 - fraction_part * 0.03529411764705884;
            g = 0.16470588235294117 + fraction_part * 0.16470588235294117;
            b = 0.48627450980392156 + fraction_part * 0.05882352941176466;
            break;
        case 2:
            r = 0.24313725490196078 - fraction_part * 0.03137254901960784;
            g = 0.32941176470588235 + fraction_part * 0.1215686274509804;
            b = 0.5450980392156862 + fraction_part * 0.007843137254902044;
            break;
        case 3:
            r = 0.21176470588235294 - fraction_part * 0.01568627450980392;
            g = 0.45098039215686275 + fraction_part * 0.10980392156862745;
            b = 0.5529411764705883 - fraction_part * 0.007843137254902044;
            break;
        case 4:
            r = 0.19607843137254902 + fraction_part * 0.023529411764705882;
            g = 0.5607843137254902 + fraction_part * 0.11372549019607847;
            b = 0.5450980392156862 - fraction_part * 0.039215686274509776;
            break;
        case 5:
            r = 0.2196078431372549 + fraction_part * 0.1215686274509804;
            g = 0.6745098039215687 + fraction_part * 0.09411764705882353;
            b = 0.5058823529411764 - fraction_part * 0.0784313725490196;
            break;
        case 6:
            r = 0.3411764705882353 + fraction_part * 0.2823529411764706;
            g = 0.7686274509803922 + fraction_part * 0.09411764705882353;
            b = 0.42745098039215684 - fraction_part * 0.17254901960784313;
            break;
        case 7:
            r = 0.6235294117647059 + fraction_part * 0.3411764705882353;
            g = 0.8627450980392157 + fraction_part * 0.050980392156862675;
            b = 0.2549019607843137 - fraction_part * 0.0549019607843137;
            break;
    }

    return properties::Color{static_cast<std::uint8_t>(r * 255.0),
                             static_cast<std::uint8_t>(g * 255.0),
                             static_cast<std::uint8_t>(b * 255.0)};
}

inline properties::Color calculateColormapJet(double value)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value >= 1.0)
    {
        value = 0.99999;
    }

    double full_range_value = value * 6.0;
    double integer_part = floor(full_range_value);
    double fraction_part = full_range_value - integer_part;

    double r = 0.0, g = 0.0, b = 0.0;

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

    return properties::Color{static_cast<std::uint8_t>(r * 255.0),
                             static_cast<std::uint8_t>(g * 255.0),
                             static_cast<std::uint8_t>(b * 255.0)};
}

properties::Color calculateColormapJetSoft(double value)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value >= 1.0)
    {
        value = 0.99999;
    }

    double full_range_value = value * 10.0;
    double integer_part = floor(full_range_value);
    double fraction_part = full_range_value - integer_part;

    double r = 0.0, g = 0.0, b = 0.0;

    switch (int(integer_part))
    {
        case 0:
            r = 0.3686274509803922 - fraction_part * 0.17254901960784316;
            g = 0.30980392156862746 + fraction_part * 0.22352941176470587;
            b = 0.6352941176470588 + fraction_part * 0.10588235294117654;
            break;
        case 1:
            r = 0.19607843137254902 + fraction_part * 0.203921568627451;
            g = 0.5333333333333333 + fraction_part * 0.22745098039215683;
            b = 0.7411764705882353 - fraction_part * 0.09411764705882353;
            break;
        case 2:
            r = 0.4 + fraction_part * 0.2705882352941176;
            g = 0.7607843137254902 + fraction_part * 0.10588235294117654;
            b = 0.6470588235294118 - fraction_part * 0.0039215686274509665;
            break;
        case 3:
            r = 0.6705882352941176 + fraction_part * 0.2313725490196079;
            g = 0.8666666666666667 + fraction_part * 0.09411764705882353;
            b = 0.6431372549019608 - fraction_part * 0.04705882352941182;
            break;
        case 4:
            r = 0.9019607843137255 + fraction_part * 0.0980392156862745;
            g = 0.9607843137254902 + fraction_part * 0.039215686274509776;
            b = 0.596078431372549 + fraction_part * 0.15294117647058825;
            break;
        case 5:
            r = 1.0 - fraction_part * 0.0039215686274509665;
            g = 1.0 - fraction_part * 0.1215686274509804;
            b = 0.7490196078431373 - fraction_part * 0.20392156862745103;
            break;
        case 6:
            r = 0.996078431372549 - fraction_part * 0.0039215686274509665;
            g = 0.8784313725490196 - fraction_part * 0.196078431372549;
            b = 0.5450980392156862 - fraction_part * 0.16470588235294115;
            break;
        case 7:
            r = 0.9921568627450981 - fraction_part * 0.03529411764705881;
            g = 0.6823529411764706 - fraction_part * 0.25490196078431376;
            b = 0.3803921568627451 - fraction_part * 0.11764705882352938;
            break;
        case 8:
            r = 0.9568627450980393 - fraction_part * 0.1215686274509804;
            g = 0.42745098039215684 - fraction_part * 0.18431372549019606;
            b = 0.2627450980392157 + fraction_part * 0.047058823529411764;
            break;
        case 9:
            r = 0.8352941176470589 - fraction_part * 0.21568627450980393;
            g = 0.24313725490196078 - fraction_part * 0.2392156862745098;
            b = 0.30980392156862746 - fraction_part * 0.05098039215686273;
            break;
    }

    return properties::Color{static_cast<std::uint8_t>(r * 255.0),
                             static_cast<std::uint8_t>(g * 255.0),
                             static_cast<std::uint8_t>(b * 255.0)};
}

}  // namespace

void testLorenz()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const Vec3f p0{8.5f, 8.5f, 27.0f};
    const Vec3f p1{-8.5f, -8.5f, 27.0f};

    const size_t num_its = 50000;

    Vector<float> x(num_its), y(num_its), z(num_its);
    Vector<properties::Color> colorp(num_its);

    float x0 = 1.0f;
    float y0 = 1.0f;
    float z0 = 1.0f;

    const float dt = 0.005f;
    const float sigma = 10.0f;
    const float rho = 28.0f;
    const float beta = 8.0f / 3.0f;

    const float max_dist = 32.0f;

    for (size_t k = 0; k < num_its; k++)
    {
        const float dx = sigma * (y0 - x0);
        const float dy = x0 * (rho - z0) - y0;
        const float dz = x0 * y0 - beta * z0;

        const float x1 = x0 + dx * dt;
        const float y1 = y0 + dy * dt;
        const float z1 = z0 + dz * dt;

        x(k) = x0;
        y(k) = y0;
        z(k) = z0;

        const float dist0 =
            std::sqrt((x1 - p0.x) * (x1 - p0.x) + (y1 - p0.y) * (y1 - p0.y) + (z1 - p0.z) * (z1 - p0.z));
        const float dist1 =
            std::sqrt((x1 - p1.x) * (x1 - p1.x) + (y1 - p1.y) * (y1 - p1.y) + (z1 - p1.z) * (z1 - p1.z));

        if (dist0 < dist1)
        {
            colorp(k) = calculateColormapViridis(dist0 / max_dist);
        }
        else
        {
            colorp(k) = calculateColormapViridis(dist1 / max_dist);
        }

        x0 = x1;
        y0 = y1;
        z0 = z1;
    }

    setActiveView("p_view_0");
    clearView();
    axis({-28.0f, -28.0f, -10.0f}, {28.0f, 28.0f, 55.0f});

    plot(x, y, colorp);

    plot3(x, y, z, colorp);
}

void testBump()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const int num_rows = 70, num_cols = 70;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);

    const double inc = 0.4;

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double rd = static_cast<double>(r - num_cols / 2) * inc;
            const double cd = static_cast<double>(c - num_rows / 2) * inc;
            x(r, c) = c;
            y(r, c) = r;
            const float r_val = std::sqrt(rd * rd + cd * cd);
            // const float r_val = std::sqrt(rd * rd + cd * cd);
            const float rand_val = static_cast<float>(rand() % 1001) / 1000.0f - 0.5f;
            z(r, c) = std::exp(-r_val * r_val * 0.1) + rand_val * 0.05f;
        }
    }

    setActiveView("p_view_0");
    clearView();
    surf(x, y, z, properties::EdgeColor::BLACK, properties::ColorMap::MAGMA);
}

void testScatterColorMap()
{
    // TOOD: To use white plot box
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const int num_rows = 70, num_cols = 70;
    Matrix<float> x(num_rows, num_cols), y(num_rows, num_cols);

    const float inc = 0.4;
    const float nr = static_cast<float>(num_rows - 1);
    const float nc = static_cast<float>(num_cols - 1);

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const float rand_val_x = static_cast<float>(rand() % 1001) / 1000.0f - 0.5f;
            const float rand_val_y = static_cast<float>(rand() % 1001) / 1000.0f - 0.5f;

            x(r, c) = static_cast<float>(c) / nc + rand_val_x * 0.01f;
            y(r, c) = static_cast<float>(r) / nr + rand_val_y * 0.01f;
        }
    }

    const VectorConstView<float> x_vec{x.data(), x.size()}, y_vec{y.data(), y.size()};

    setActiveView("p_view_0");
    clearView();
    scatter(x_vec,
            y_vec,
            properties::ScatterStyle::DISC,
            properties::DistanceFrom::xy({0.5, 0.5}, 0.0, 1.0),
            properties::ColorMap::VIRIDIS);
}

void testScatterSamples()
{
    std::vector<float> x, y;

    float mean_val = 0.0f;
    for (std::int32_t k = 0; k < 20; k++)
    {
        std::int32_t n_samples = static_cast<std::int32_t>(rand() % 6) - 3 + 10;
        const float y_val = static_cast<float>(rand() % 1001) / 1000.0f + 1.2f;

        for (std::int32_t i = 0; i < n_samples; i++)
        {
            x.push_back(static_cast<float>(k) / 2.0f);
            const float y_rand = static_cast<float>(rand() % 1001) / 1000.0f - 0.5f;
            mean_val += y_val + y_rand * 0.1f;
            y.push_back(y_val + y_rand * 0.1f);
        }
    }
    Vector<float> x_vec = x;
    Vector<float> y_vec = y;

    mean_val /= static_cast<float>(x.size());

    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);
    disableScaleOnRotation();

    setActiveView("p_view_0");
    clearView();
    scatter(x_vec,
            y_vec,
            properties::ScatterStyle::DISC,
            properties::DistanceFrom::y(mean_val, 0.0, 0.5),
            properties::ColorMap::VIRIDIS,
            properties::PointSize(20.0f));
}

void testFakeContour()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const std::int32_t num_rows = 100, num_cols = 100;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);

    const double inc = 1.0 / static_cast<double>(num_rows);

    const auto r_func = [](const double x, const double y) -> double { return std::sqrt(x * x + y * y); };

    for (std::int32_t r = 0; r < num_rows; r++)
    {
        for (std::int32_t c = 0; c < num_cols; c++)
        {
            x(r, c) = c;
            y(r, c) = r;

            const double yd = (static_cast<double>(r + 1) + 0.5) * inc;
            const double xd = (static_cast<double>(c + 1) + 0.5) * inc;

            const float r_val0 = r_func(xd - 0.1, yd - 0.4);
            z(r, c) = std::sin(r_val0 * 15.0) / (r_val0 * 15.0);

            const float r_val1 = r_func(xd - 0.3, yd - 1.0);
            z(r, c) += std::sin(r_val1 * 15.0) / (r_val1 * 15.0);

            const float r_val2 = r_func(xd - 0.5, yd - 0.7);
            z(r, c) += std::sin(r_val2 * 7.0) / (r_val2 * 7.0);
        }
    }

    double min_val = duoplot::min(z);
    double max_val = duoplot::max(z);

    z = z + (std::abs(min_val) * 2.0);
    min_val = duoplot::min(z);
    max_val = duoplot::max(z);

    ImageRGB<std::uint8_t> img(num_rows, num_cols);

    const double num_values = 3.0;

    for (std::int32_t r = 0; r < num_rows; r++)
    {
        for (std::int32_t c = 0; c < num_cols; c++)
        {
            const double mapped_value = std::floor(num_values * (z(r, c) - min_val) / (max_val - min_val)) / num_values;
            const properties::Color col = calculateColormapViridis(mapped_value);
            img(r, c, 0) = col.red;
            img(r, c, 1) = col.green;
            img(r, c, 2) = col.blue;
        }
    }

    ImageRGB<std::uint8_t> img2 = img;

    properties::Color segment_color = properties::Color{img(0, 0, 0), img(0, 0, 1), img(0, 0, 2)};

    std::int64_t idx = 0;
    std::int64_t current_row = 0, current_col = 0;

    // For vertical, 1 means down, -1 means up
    std::int64_t current_horizontal_dir = 1, current_vertical_dir = 1;

    Vec2<std::int64_t> start_index{0, 0};
    Vec2<std::int64_t> next_index{0, 0};

    for (std::int32_t r = 1; r < (num_rows - 1U); r++)
    {
        for (std::int32_t c = 1; c < (num_cols - 1U); c++)
        {
            const properties::Color col{img(r, c, 0), img(r, c, 1), img(r, c, 2)};
            const properties::Color col_left{img(r, c - 1, 0), img(r, c - 1, 1), img(r, c - 1, 2)};
            const properties::Color col_right{img(r, c + 1, 0), img(r, c + 1, 1), img(r, c + 1, 2)};
            const properties::Color col_up{img(r - 1, c, 0), img(r - 1, c, 1), img(r - 1, c, 2)};
            const properties::Color col_down{img(r + 1, c, 0), img(r + 1, c, 1), img(r + 1, c, 2)};
            if (((col == col_left) && (col == col_right) && (col == col_up) && (col == col_down)))
            {
                img2(r, c, 0) = 0;
                img2(r, c, 1) = 0;
                img2(r, c, 2) = 0;
            }
        }
    }

    while (false)
    {
        // Paint the edge pixel black
        img(current_row, current_col, 0) = 255;
        img(current_row, current_col, 1) = 255;
        img(current_row, current_col, 2) = 255;

        // Get the color of the current pixel
        const properties::Color current_color = properties::Color{
            img(current_row, current_col, 0), img(current_row, current_col, 1), img(current_row, current_col, 2)};

        // Get the next pixel
        if (current_row == 0)
        {
            // Going right
            next_index = {current_row, current_col + 1};
        }

        if (current_color != segment_color)
        {
            segment_color = current_color;
            // img2(r, c, 0) = 255;
            // img2(r, c, 1) = 255;
            // img2(r, c, 2) = 255;
        }
    }

    setActiveView("p_view_0");
    clearView();
    // surf(x, y, z, properties::EdgeColor::BLACK, properties::ColorMap::VIRIDIS);

    imShow(img, properties::ZOffset(-0.05f));
    imShow(img2);
}

void testStocks()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 1000;
    Vector<double> x(num_elements), y0(num_elements), yp0(num_elements), y1(num_elements), y2(num_elements);

    const auto r_func = []() -> double { return static_cast<double>(rand() % 1001) / 1000.0; };

    x(0) = 0.0;

    y0(0) = r_func();
    yp0(0) = 0.0;
    // y1(0) = r_func();
    // y2(0) = r_func();

    const double dt = 0.1;
    const double h = 0.5;
    const double h1 = 1.0 - h;

    for (size_t k = 1; k < num_elements; k++)
    {
        x(k) = static_cast<double>(k) * 0.1;

        yp0(k - 1) = yp0(k) + h1 * ((r_func() - 0.5) + 0.0 * std::sin(x(k) / 10.0));
        y0(k - 1) = y0(k) + yp0(k - 1) * dt;
        // y1(k - 1) = y1(k) + r_func();
        // y2(k - 1) = y2(k) + r_func();
    }

    setActiveView("p_view_0");
    clearView();
    plot(x, y0, properties::LineWidth(3.0f));
    // plot(x, y1);
    // plot(x, y2);
}

void testScatterCluster()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 1000, num_cluester = 6;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);

    const auto r_func = []() -> double { return static_cast<double>(rand() % 1001) / 1000.0; };

    setActiveView("p_view_0");
    clearView();

    std::vector<properties::Color> colors{properties::Color{218, 83, 29},
                                          properties::Color{0, 114, 187},
                                          properties::Color{238, 177, 49},
                                          properties::Color{118, 172, 59},
                                          properties::Color{29, 218, 186},
                                          properties::Color{218, 29, 155}};

    // random device class instance, source of 'true' randomness for initializing random seed
    std::random_device rd;

    // Mersenne twister PRNG, initialized with seed from previous random device instance
    std::mt19937 gen(rd());

    for (size_t i = 0; i < num_cluester; i++)
    {
        const double x_center = r_func() * 10.0;
        const double y_center = r_func() * 10.0;
        const double z_center = r_func() * 10.0;

        const double x_std = (r_func() + 1.0) * 1.0;
        const double y_std = (r_func() + 1.0) * 1.0;
        const double z_std = (r_func() + 1.0) * 1.0;

        std::normal_distribution<double> dx{x_center, x_std};
        std::normal_distribution<double> dy{y_center, y_std};
        std::normal_distribution<double> dz{z_center, z_std};

        for (size_t k = 1; k < num_elements; k++)
        {
            x(k) = dx(gen);
            y(k) = dy(gen);
            z(k) = dz(gen);
        }

        scatter3(x, y, z, colors[i], properties::ScatterStyle::DISC);
    }
}

void testScatterVaryingSize()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 200;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);
    Vector<double> point_sizes(num_elements);
    Vector<properties::Color> colorp(num_elements);

    double t = 0.0;
    double r = 1.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        x(k) = r * std::cos(t);
        y(k) = r * std::sin(t);
        z(k) = r;
        point_sizes(k) = static_cast<double>(k) * 0.1 + 5.0;
        colorp(k) = calculateColormapJet(std::sin(t) * 0.5 + 0.5);
        t += 0.1;
        r += 0.01;
    }

    setActiveView("p_view_0");
    clearView();
    axis({-2.0, -2.0, -2.0}, {2.0, 2.0, 2.0});

    scatter(x, y, colorp, point_sizes, properties::ScatterStyle::DISC);
    scatter3(x, y, z, colorp, point_sizes, properties::ScatterStyle::DISC);
    // scatter(x, y, point_sizes, properties::ScatterStyle::DISC);
    plot(x, y, properties::ScatterStyle::DISC);
}

void testScatterSmallPoints()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 50000;
    Vector<double> x(num_elements), y(num_elements);

    double t = 0.0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dx{0.0, 0.1};

    for (size_t k = 0; k < num_elements; k++)
    {
        x(k) = std::cos(t) + dx(gen);
        y(k) = std::sin(t) + dx(gen);
        t += 0.1;
    }

    setActiveView("p_view_0");
    clearView();
    axis({-2.0, -2.0, -2.0}, {2.0, 2.0, 2.0});

    scatter(x,
            y,
            properties::PointSize(1),
            properties::ScatterStyle::SQUARE,
            properties::DistanceFrom::xy({0.0, 0.0}, 0.625, 1.25),
            properties::ColorMap::MAGMA);

    scatter(x + 0.3,
            y - 0.5,
            properties::PointSize(1),
            properties::ScatterStyle::SQUARE,
            properties::DistanceFrom::xy({0.3, -0.5}, 0.625, 1.25),
            properties::ColorMap::HSV);
}

void testTransparentFillBelowPlot()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 1000;
    Vector<double> t(num_elements);

    const auto create_mesh = [](const Vector<double>& t,
                                const Vector<double>& x,
                                const double z_offset) -> std::tuple<Vector<Point3<double>>, Vector<IndexTriplet>> {
        const size_t num_elements = t.size();

        const size_t num_gaps = num_elements - 1U;
        const size_t num_triangles = num_gaps * 2U;
        const size_t num_vertices = num_elements * 2U;

        Vector<Point3<double>> vertices(num_vertices);
        Vector<IndexTriplet> indices(num_triangles);

        for (size_t k = 0; k < num_elements; k++)
        {
            vertices(k) = {t(k), x(k), z_offset};
            vertices(k + num_elements) = {t(k), 0.0, z_offset};
        }

        const std::uint32_t num_elements_uint32 = num_elements;

        for (size_t k = 0; k < num_gaps; k++)
        {
            const std::uint32_t ku = k;
            indices(k * 2U) = {ku, ku + 1U, ku + num_elements_uint32};
            indices(k * 2U + 1U) = {ku + 1U, ku + num_elements_uint32 + 1U, ku + num_elements_uint32};
        }

        return {vertices, indices};
    };

    t(0) = 0.0;
    double dt = 0.01;

    struct FunctionParameters
    {
        double Kf;
        double Cf;
        double m;
        double L;
        double dt;
        double x0;
        double dx0;
        double mu;
        double sigma;
        size_t num_elements;
    };

    for (size_t k = 0U; k < (num_elements - 1U); k++)
    {
        t(k + 1U) = t(k) + dt;
    }

    const auto create_function = [](const Vector<double>& t,
                                    const FunctionParameters& function_parameters) -> Vector<double> {
        const size_t num_elements = t.size();

        Vector<double> x{num_elements}, dx{num_elements};

        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> rand_gen{function_parameters.mu, function_parameters.sigma};

        x(0) = function_parameters.x0;
        dx(0) = function_parameters.dx0;

        for (size_t k = 0U; k < (function_parameters.num_elements - 1U); k++)
        {
            const double diff_x = x(k) - function_parameters.L;

            const double u = std::sin(t(k)) + rand_gen(gen);
            dx(k + 1) = dx(k) + function_parameters.dt *
                                    (-function_parameters.Kf * diff_x - function_parameters.Cf * dx(k) + u) /
                                    function_parameters.m;
            x(k + 1) = x(k) + function_parameters.dt * dx(k + 1);
        }

        return x;
    };

    const FunctionParameters fp0{.Kf = 5.0,
                                 .Cf = 0.1,
                                 .m = 1.0,
                                 .L = 0.25,
                                 .dt = dt,
                                 .x0 = 0.0,
                                 .dx0 = 0.0,
                                 .mu = 0.0,
                                 .sigma = 1.0,
                                 .num_elements = num_elements};

    const FunctionParameters fp1{.Kf = 4.0,
                                 .Cf = 0.3,
                                 .m = 1.3,
                                 .L = 0.25,
                                 .dt = dt,
                                 .x0 = 0.1,
                                 .dx0 = -1.0,
                                 .mu = 0.0,
                                 .sigma = 1.0,
                                 .num_elements = num_elements};
    const FunctionParameters fp2{.Kf = 3.0,
                                 .Cf = 0.2,
                                 .m = 1.0,
                                 .L = 0.25,
                                 .dt = dt,
                                 .x0 = 0.0,
                                 .dx0 = 0.0,
                                 .mu = 0.0,
                                 .sigma = 1.0,
                                 .num_elements = num_elements};

    const Vector<double> x0 = create_function(t, fp0);
    const Vector<double> x1 = create_function(t, fp1);
    const Vector<double> x2 = create_function(t, fp2);

    Vector<Point3<double>> vertices0, vertices1, vertices2;
    Vector<IndexTriplet> indices0, indices1, indices2;
    std::tie(vertices0, indices0) = create_mesh(t, x0, 0.0);
    std::tie(vertices1, indices1) = create_mesh(t, x1, -0.2);
    std::tie(vertices2, indices2) = create_mesh(t, x2, -0.4);

    setActiveView("p_view_0");
    clearView();
    // axis({-2.0, -2.0, -2.0}, {2.0, 2.0, 2.0});

    viewAngles(0, 90);

    plot(t, x0, properties::LineWidth(5), properties::Color::BLACK);
    plot(t, x1, properties::LineWidth(5), properties::Color::BLACK);
    plot(t, x2, properties::LineWidth(5), properties::Color::BLACK);
    drawMesh(vertices2, indices2, properties::Color::GREEN, properties::EdgeColor::NONE, properties::Alpha(0.5f));
    drawMesh(vertices1, indices1, properties::Color::RED, properties::EdgeColor::NONE, properties::Alpha(0.5f));
    drawMesh(vertices0, indices0, properties::Color::BLUE, properties::EdgeColor::NONE, properties::Alpha(0.5f));
}

void testTransitioningSurfs()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t num_rows = 100, num_cols = 100;
    Matrix<double> x{num_rows, num_cols}, y{num_rows, num_cols}, z{num_rows, num_cols};

    const double x_min = 0.0, x_max = 1.0, y_min = 0.0, y_max = 1.0;

    const double inc = (x_max - x_min) / static_cast<double>(num_rows - 1U);

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            x(r, c) = c * inc;
            y(r, c) = r * inc;

            const double rd = x(r, c) - 5.5 * inc;
            const double cd = y(r, c) - 5.5 * inc;
            const double r_val = std::sqrt(rd * rd + cd * cd) * 10.0;
            z(r, c) = std::sin(r_val) / r_val;
        }
    }

    const size_t num_surfaces{5U};

    Vector<Matrix<double>> surfaces{num_surfaces};

    /////////////////////////////////////////////////////////////////////////
    //////////////////////////// Create surfaces ////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    const Matrix<double> rd = x - 0.5;
    const Matrix<double> cd = y - 0.5;
    const Matrix<double> r_val = duoplot::sqrt(elementWiseMultiply(rd, rd) + elementWiseMultiply(cd, cd)) * 10.0;

    surfaces(0) = duoplot::sin(r_val) / r_val;

    const Matrix<double> r_val1 = duoplot::sqrt(elementWiseMultiply(rd, rd) + elementWiseMultiply(cd, cd)) * 20.0;

    surfaces(1) = duoplot::sin(r_val1) / r_val1;

    const Matrix<double> r_val2 = duoplot::sqrt(elementWiseMultiply(rd, rd) + elementWiseMultiply(cd, cd)) * 30.0;

    surfaces(2) = elementWiseMultiply(rd, rd) - elementWiseMultiply(cd, cd);

    const Matrix<double> r_val3 = duoplot::sqrt(elementWiseMultiply(rd, rd) + elementWiseMultiply(cd, cd)) * 40.0;

    surfaces(3) = elementWiseMultiply(rd, rd) + elementWiseMultiply(cd, cd);

    const Matrix<double> r_val4 = duoplot::sqrt(elementWiseMultiply(rd, rd) + elementWiseMultiply(cd, cd)) * 50.0;

    surfaces(4) = duoplot::sin(r_val4) / r_val4;

    /////////////////////////////////////////////////////////////////////////
    //////////////////////////// Create surfaces ////////////////////////////
    /////////////////////////////////////////////////////////////////////////

    setActiveView("p_view_0");
    clearView();
    disableScaleOnRotation();
    viewAngles(-60, 40);
    axis({0.0, 0.0, -1.0}, {1.0, 1.0, 1.0});

    const size_t num_its = 100U;

    Matrix<double> z_mix{surfaces(0)};

    for (size_t k = 0U; k < (num_surfaces - 1U); k++)
    {
        Matrix<double>& z_k = surfaces(k);
        Matrix<double>& z_kp1 = surfaces(k + 1U);

        for (size_t i = 0; i < num_its; i++)
        {
            const double id = i;
            const double h = id / static_cast<double>(num_its - 1U);
            // 0, 1, 2, ..., i - 1
            // const Matrix<double> z_mix = z_k * (1.0 - h) + z_kp1 * h;
            const double dt = 0.5;
            const double dt1 = 1.0 - dt;
            z_mix = dt1 * z_mix + dt * (z_k * (1.0 - h) + z_kp1 * h);
            surf(x, y, z_mix, properties::EdgeColor(0, 0, 0), properties::ColorMap::JET_BRIGHT);
            usleep(1000 * 10);
            softClearView();
        }
    }
    // surf(x, y, z, properties::EdgeColor(0, 0, 0), properties::ColorMap::JET_BRIGHT);
}

void testIsoSurfaces()
{
    const size_t n_elems{100U};

    // const std::pair<Matrix<double>, Matrix<double>> uv_mats =
    //     meshGrid<double>(0.0, M_PI, 0.0, 2.0 * M_PI, n_elems, n_elems);
    const std::pair<Matrix<double>, Matrix<double>> uv_mats = meshGrid<double>(1.0, 5.0, 1.0, 10.0, n_elems, n_elems);
    Matrix<double> u{uv_mats.first}, v{uv_mats.second};

    const Matrix<double> r = 2.0 + sin(7.0 * u + 5.0 * v);
    // const Matrix<double> x = elementWiseMultiply(r, elementWiseMultiply(duoplot::cos(u), duoplot::sin(v)));
    // const Matrix<double> y = elementWiseMultiply(r, elementWiseMultiply(duoplot::sin(u), duoplot::sin(v)));
    // const Matrix<double> z = elementWiseMultiply(r, duoplot::cos(v));

    const Matrix<double> x = elementWiseMultiply(u, sin(v));
    const Matrix<double> y = elementWiseMultiply(-u, cos(v));
    const Matrix<double> z = v;

    // funx = @(u,v) u.*sin(v);
    // funy = @(u,v) -u.*cos(v);
    // funz = @(u,v) v;

    setActiveView("p_view_0");
    clearView();
    surf(x, y, z, properties::EdgeColor(0, 0, 0), properties::ColorMap::JET_BRIGHT);
}

void testHyperboloid()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);
    const size_t n_elems{100U};

    struct XYZMats
    {
        Matrix<double> x;
        Matrix<double> y;
        Matrix<double> z;
    };

    const auto hyberboloid = [](const double v0, const double v1, const double f, const size_t n_elems) -> XYZMats {
        const std::pair<Matrix<double>, Matrix<double>> theta_v_mats =
            meshGrid<double>(0, 2.0 * M_PI, v0, v1, n_elems, n_elems);

        Matrix<double> theta{theta_v_mats.first}, v{theta_v_mats.second};
        const double a = 1.0;
        const double b = 1.0;
        const double c = 1.5;

        const Matrix<double> v2 = elementWiseMultiply(cosh(v), cosh(v));

        // if(f < )

        // const Matrix<double> x = a * elementWiseMultiply(duoplot::sqrt(f + v2), cos(theta));
        // const Matrix<double> y = b * elementWiseMultiply(duoplot::sqrt(f + v2), sin(theta));
        // const Matrix<double> z = c * sinh(v);

        const Matrix<double> x = a * elementWiseMultiply(duoplot::sqrt(f + v2), duoplot::cos(theta));
        const Matrix<double> y = b * elementWiseMultiply(duoplot::sqrt(f + v2), duoplot::sin(theta));
        const Matrix<double> z = c * sinh(v);

        return {x, y, z};

        // const Matrix<double> x = elementWiseMultiply(a * cosh(v), cos(theta));
        // const Matrix<double> y = elementWiseMultiply(b * cosh(v), sin(theta));
        // const Matrix<double> z = c * sinh(v);

        // const Matrix<double> x = elementWiseMultiply(a * sinh(v), cos(theta));
        // const Matrix<double> y = elementWiseMultiply(b * sinh(v), sin(theta));
        // const Matrix<double> z = c * cosh(v);

        // return {x, y, z};
    };

    setActiveView("p_view_0");
    clearView();
    disableScaleOnRotation();
    axesSquare();

    axis({-10.0, -10.0, -10.0}, {10.0, 10.0, 10.0});
    viewAngles(-65, 12);

    XYZMats xyz_mats;

    const size_t n_iters = 400U;

    for (size_t k = 0; k < n_iters; k++)
    {
        const double kf = static_cast<double>(k) / static_cast<double>(n_iters - 1U);

        const double f = -(4.0 - 8.0 * kf);
        xyz_mats = hyberboloid(-3.0, 3.0, f, n_elems);
        auto [x, y, z] = xyz_mats;

        softClearView();
        if (k < n_iters / 2U)
        {
            surf(x,
                 y,
                 z,
                 properties::ID0,
                 properties::EdgeColor::NONE,
                 properties::ColorMap::JET_SOFT,
                 properties::INTERPOLATE_COLORMAP);
        }
        else
        {
            surf(x, y, z, properties::ID0, properties::EdgeColor::NONE, properties::ColorMap::JET_SOFT);
        }

        usleep(1000 * 1);
    }

    usleep(1000U * 1000U * 6U);
    openProjectFile("../../project_files/small_bright.duoplot");

    setActiveView("p_view_0");
    clearView();
    disableScaleOnRotation();
    axesSquare();
    axis({-10.0, -10.0, -10.0}, {10.0, 10.0, 10.0});
    viewAngles(-65, 12);

    softClearView();
    xyz_mats = hyberboloid(-3.0, 3.0, 1.0, 30U);
    auto [x, y, z] = xyz_mats;
    surf(x, y, z, properties::ID0, properties::EdgeColor(0, 0, 0), properties::FaceColor::NONE);

    double t = 0.0;

    for (size_t k = 0; k < 500; k++)
    {
        const double theta_x = t * 10.0;
        const double theta_y = t * 10.0;
        const double theta_z = t * 10.0;

        const auto r_mat =
            rotationMatrixZ<double>(theta_z) * rotationMatrixY<double>(theta_y) * rotationMatrixX<double>(theta_x);

        const properties::Transform tr{diagMatrix<double>({1.0, 1.0, 1.0}), r_mat, {0, 0, 0}};

        // setProperties(properties::ID0, properties::FaceColor::NONE);
        setTransform(properties::ID0, tr.scale, tr.rotation, tr.translation);

        usleep(1000 * 10);
        t += 0.001;
    }
}

void testSphere()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);
    const size_t n_elems{30U};

    const std::pair<Matrix<double>, Matrix<double>> theta_phi_mats =
        meshGrid<double>(0.0, M_PI, 0.0, 2.0 * M_PI, n_elems, n_elems);

    Matrix<double> theta{theta_phi_mats.first}, phi{theta_phi_mats.second};

    Matrix<double> r{n_elems, n_elems};
    r.fill(1.0);

    const Matrix<double> x = elementWiseMultiply(elementWiseMultiply(r, duoplot::sin(theta)), duoplot::cos(phi));
    const Matrix<double> y = elementWiseMultiply(elementWiseMultiply(r, duoplot::sin(theta)), duoplot::sin(phi));
    const Matrix<double> z = elementWiseMultiply(r, cos(theta));

    setActiveView("p_view_0");
    clearView();
    surf(x, y, z, properties::EdgeColor(0, 0, 0), properties::ColorMap::JET_SOFT);
}

void testLinesAndDots()
{
    // IMPORTANT: For this demo, set minimum_y_pos_ = 0; in "plot_pane.cpp"
    // and remove the showing of handle. Also set the snapping angle to 0.0
    const std::string project_file_path = "../../project_files/lines_and_dots.duoplot";
    openProjectFile(project_file_path);

    const double size_bias = debug_value_args::getValue<double>("size-bias", 3.0);
    const double size_multiplier = debug_value_args::getValue<double>("size-multiplier", 10.0);

    const size_t num_elements = 200U, num_its = 500U;
    Vector<double> x(num_elements), y(num_elements), z(num_elements), vx(num_elements), vy(num_elements),
        vz(num_elements), point_sizes(num_elements), bx(num_elements), by(num_elements), bz(num_elements);

    for (size_t k = 0; k < num_elements; k++)
    {
        x(k) = (static_cast<double>(rand() % 1001) / 1000.0 - 0.5) * 3.5;
        y(k) = (static_cast<double>(rand() % 1001) / 1000.0 - 0.5) * 3.5;
        z(k) = (static_cast<double>(rand() % 1001) / 1000.0 - 0.5) * 3.5;

        bx(k) = (static_cast<double>(rand() % 1001) / 1000.0 - 0.5) * 2.5;
        by(k) = (static_cast<double>(rand() % 1001) / 1000.0 - 0.5) * 2.5;
        bz(k) = (static_cast<double>(rand() % 1001) / 1000.0 - 0.5) * 2.5;

        vx(k) = 0.0;
        vy(k) = 0.0;
        vz(k) = 0.0;

        point_sizes(k) = (static_cast<double>(rand() % 1001) / 1000.0) * size_multiplier + size_bias;
    }

    const double thresh = debug_value_args::getValue<double>("dist-thresh", 0.05);

    setActiveView("p_view_0");
    clearView();
    viewAngles(180.0, 90.0);
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    axesSquare();
    disableAutomaticAxesAdjustment();
    disableScaleOnRotation();

    axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});

    double azimuth = 180.0;
    double elevation = 90.0;

    for (size_t k = 0; k < num_its; k++)
    {
        std::vector<double> x_lines_std, y_lines_std, z_lines_std;
        x_lines_std.reserve(num_elements * num_elements);
        y_lines_std.reserve(num_elements * num_elements);
        z_lines_std.reserve(num_elements * num_elements);

        for (size_t i = 0; i < num_elements; i++)
        {
            const double outer_x = x(i);
            const double outer_y = y(i);
            const double outer_z = z(i);

            for (size_t j = 0; j < num_elements; j++)
            {
                const double inner_x = x(j);
                const double inner_y = y(j);
                const double inner_z = z(j);

                const double dx = outer_x - inner_x;
                const double dy = outer_y - inner_y;
                const double dz = outer_z - inner_z;
                const double dx2 = dx * dx;
                const double dy2 = dy * dy;
                const double dz2 = dz * dz;

                if (dx2 + dy2 + dz2 < thresh)
                {
                    x_lines_std.push_back(inner_x);
                    x_lines_std.push_back(outer_x);
                    y_lines_std.push_back(inner_y);
                    y_lines_std.push_back(outer_y);
                    z_lines_std.push_back(inner_z);
                    z_lines_std.push_back(outer_z);
                }
            }
        }

        Vector<double> x_lines = x_lines_std;
        Vector<double> y_lines = y_lines_std;
        Vector<double> z_lines = z_lines_std;

        softClearView();
        // if (k > num_its / 2U)
        azimuth -= 0.5;
        if (azimuth < -180.0)
        {
            azimuth = 180.0;
        }
        elevation -= 0.1;
        viewAngles(azimuth, elevation);

        scatter3(x,
                 y,
                 z,
                 point_sizes,
                 properties::Color(0.1686 * 255.0, 0.1686 * 255.0, 0.2510 * 255.0),
                 properties::ScatterStyle::DISC);
        lineCollection3(x_lines, y_lines, z_lines, properties::Color(0.1686 * 255.0, 0.1686 * 255.0, 0.2510 * 255.0));

        const double h = 0.01;

        for (size_t i = 0; i < num_elements; i++)
        {
            vx(i) = (static_cast<double>(rand() % 1001) / 1000.0 - bx(i)) * 0.1;
            vy(i) = (static_cast<double>(rand() % 1001) / 1000.0 - by(i)) * 0.1;

            x(i) = x(i) + vx(i) * h;
            y(i) = y(i) + vy(i) * h;
            /*if (y(i) > 1.0)
            {
                y(i) = 1.0 - (y(i) - 1.0);
                vy(i) = -vy(i);
            }*/
        }

        usleep(10 * 1000);
    }
}

void testLissaJous()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t n_elements = 700;
    Vector<double> x(n_elements), y(n_elements);

    const double t_end = 2.0 * M_PI;

    double t = 0.0;

    const double dt = t_end / static_cast<double>(n_elements - 1U);

    const double a = 3.0;
    const double b = 4.0;
    const double d = M_PI / 2.0;

    for (size_t k = 0; k < n_elements; k++)
    {
        x(k) = std::sin(a * t + d);
        y(k) = std::sin(b * t);

        t = t + dt;
    }

    setActiveView("p_view_0");
    clearView();

    const double delta = 0.3;

    for (size_t k = 10; k < n_elements; k += 3)
    {
        Vector<double> xt(k), yt(k);

        for (size_t i = 0; i < k; i++)
        {
            xt(i) = x(i);
            yt(i) = y(i);
        }

        softClearView();
        axis({xt(k - 1U) - delta, yt(k - 1U) - delta}, {xt(k - 1U) + delta, yt(k - 1U) + delta});
        plot(xt, yt, properties::Color(201, 238, 121), properties::LineWidth(10.0f));
    }

    const double x_final = 1.0, y_final = 0.0;

    for (size_t k = 0; k < 100; k++)
    {
        const double d_val = static_cast<double>(k) / 99.0;
        const double d_val_1 = 1.0 - d_val;

        const double x_min = (x_final - delta) * d_val_1 + d_val * -1.0;
        const double x_max = (x_final + delta) * d_val_1 + d_val * 1.0;
        const double y_min = (y_final - delta) * d_val_1 + d_val * -1.0;
        const double y_max = (y_final + delta) * d_val_1 + d_val * 1.0;

        axis({x_min, y_min}, {x_max, y_max});
        usleep(1000 * 10);
    }

    axis({-1.0, -1.0}, {1.0, 1.0});

    plot(x, y, properties::Color(201, 238, 121), properties::LineWidth(10.0f));
}

void testCubeImage()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    ImageRGBA<std::uint8_t> input_img{10, 10};

    for (size_t r = 0; r < input_img.height(); r++)
    {
        for (size_t c = 0; c < input_img.width(); c++)
        {
            input_img(r, c, 0) = r * 25;
            input_img(r, c, 1) = c * 25;
            input_img(r, c, 2) = r * c % 40;
            input_img(r, c, 3) = 255;
        }
    }

    const size_t num_cubes = input_img.width() * input_img.height();

    Vector<double> x(num_cubes), y(num_cubes), z(num_cubes);

    Vector<properties::Color> colors{num_cubes};

    const size_t img_height = input_img.height();
    const size_t img_width = input_img.width();

    const double img_width_1 = static_cast<double>(img_width - 1);
    const double img_height_1 = static_cast<double>(img_height - 1);

    for (size_t r = 0; r < img_height; r++)
    {
        for (size_t c = 0; c < img_width; c++)
        {
            const size_t idx = r * img_width + c;

            x(idx) = static_cast<double>(c) * 1.05 - 0.5;
            y(idx) = static_cast<double>(r) * 1.05 - 0.5;
            z(idx) = 0.0;

            colors(idx) = properties::Color{input_img(r, c, 0), input_img(r, c, 1), input_img(r, c, 2)};
        }
    }

    setActiveView("p_view_0");
    clearView();

    axis({-1.0, -1.0, -5.0}, {10.0, 10.0, 5.0});

    drawCubes(x, y, z, colors, 1.0, properties::EdgeColor::NONE);
}

void testFilteringWithSliders()
{
    const std::string project_file_path = "../../project_files/slider_and_plot.duoplot";
    openProjectFile(project_file_path);

    const size_t num_elements = 1000U;
    const size_t end_idx = num_elements - 1U;

    const Vector<double> x = linspaceFromBoundariesAndCount<double>(0.0, 1.0, num_elements);
    Vector<double> y = sin((2.0 * M_PI) * x);
    Vector<double> y_n = y;
    Vector<double> y_f = y;

    for (size_t k = 0; k < num_elements; k++)
    {
        const double r = static_cast<double>(rand() % 1001) / 1000.0 - 0.5;
        y_n(k) += r * 0.1;
    }

    duoplot::gui::registerGuiCallback("slider0", [&](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
        const double val0 = static_cast<double>(gui_element_handle.getValue()) / 100.0;

        Vector<double> y_f = y_n;
        const double c0 = val0;
        const double c1 = 1.0 - c0;

        for (size_t k = 1U; k < num_elements; k++)
        {
            y_f(k) = y_f(k - 1U) * c0 + c1 * y_n(k);
        }

        plot(x, y_n, properties::LineWidth(3.0f));
        plot(x, y_f, properties::LineWidth(5.0f));
        flushCurrentElement();
        softClearView();
    });

    duoplot::gui::startGuiReceiveThread();

    setActiveView("p_view_0");
    clearView();
    waitForFlush();
    axis({0.125, 0.625}, {0.375, 1.125});
    // scatter(t, c);
    // flushCurrentElement();

    while (true)
    {
        usleep(1000 * 1000 * 10);
    }
}

void testPidTuner()
{
    const std::string project_file_path = "../../project_files/3_sliders_and_plot.duoplot";
    openProjectFile(project_file_path);

    struct SimParams
    {
        double h;
        double x0;
        double vx0;
        double L;
        double Kf;
        double Cf;
        double m;
        double Kp;
        double Ki;
        double Kd;
        double r;
        double error_bound;
    };

    SimParams sim_params{.h = 0.01,
                         .x0 = 0.5,
                         .vx0 = 0.0,
                         .L = 0.25,
                         .Kf = 5.0,
                         .Cf = 0.5,
                         .m = 0.1,
                         .Kp = 0.0,
                         .Ki = 0.0,
                         .Kd = 0.0,
                         .r = 1.0,
                         .error_bound = 0.05};

    const size_t num_elements = 1000U;
    const size_t end_idx = num_elements - 1U;

    const Vector<double> t = linspaceFromStartPointIncAndCount<double>(0.0, sim_params.h, num_elements);
    Vector<double> x(num_elements);
    Vector<double> vx(num_elements);

    const auto run_sim = [](const SimParams& sim_params, Vector<double>& x, Vector<double>& vx) -> void {
        x(0) = sim_params.x0;
        vx(0) = sim_params.vx0;

        const size_t num_elements = x.size();

        float eI = 0.0;

        for (size_t k = 0; k < num_elements - 1U; k++)
        {
            const double x_k = x(k);
            const double vx_k = vx(k);

            const double e = sim_params.r - x_k;

            eI += e * sim_params.h;

            const double F = sim_params.Kp * e - sim_params.Kd * vx_k + sim_params.Ki * eI;

            vx(k + 1) =
                vx_k + sim_params.h * (-sim_params.Kf * (x_k - sim_params.L) + F - sim_params.Cf * vx_k) / sim_params.m;
            x(k + 1) = x_k + sim_params.h * vx(k + 1);
        }
    };

    const auto find_ss = [](SimParams& sim_params, const Vector<double>& x) -> size_t {
        size_t candidate_index = 0U;

        const size_t x_size = x.size();

        for (size_t k = 1; k < x_size; k++)
        {
            const double prev_error = std::abs(x(k - 1U) - sim_params.r);
            const double current_error = std::abs(x(k) - sim_params.r);

            if (prev_error > sim_params.error_bound && current_error < sim_params.error_bound)
            {
                candidate_index = k;
            }
            else if (prev_error < sim_params.error_bound && current_error > sim_params.error_bound)
            {
                candidate_index = x_size - 1U;
            }
        }

        return candidate_index;
    };

    const auto plot_data = [&run_sim, &find_ss](const double val_kp,
                                                const double val_ki,
                                                const double val_kd,
                                                SimParams& sim_params,
                                                const Vector<double>& t,
                                                Vector<double>& x,
                                                Vector<double>& vx) -> void {
        sim_params.Kp = val_kp * 50.0;
        sim_params.Ki = val_ki * 50.0;
        sim_params.Kd = val_kd * 5.0;

        run_sim(sim_params, x, vx);

        const Vector<double> start_stop_x{VectorInitializer<double>{0.0, t(t.size() - 1U)}};
        const Vector<double> ref_vec_y{VectorInitializer<double>{sim_params.r, sim_params.r}};
        const Vector<double> l_vec_y{VectorInitializer<double>{sim_params.L, sim_params.L}};

        const size_t ss_idx = find_ss(sim_params, x);

        using Vd = Vector<double>;
        using Vid = VectorInitializer<double>;

        duoplot::gui::getGuiElementHandle<duoplot::gui::TextLabelHandle>("tl_rt").setLabel("Rise time: " +
                                                                                           std::to_string(t(ss_idx)));

        setActiveView("p_view_0");

        plot(t, x, properties::LineWidth(7.0f), properties::Label("Position"));
        plot(t, vx, properties::LineWidth(7.0f), properties::Label("Velocity"));
        plot(start_stop_x,
             ref_vec_y,
             properties::LineWidth(3.0f),
             properties::Color::BLACK,
             properties::Label("Reference"));
        plot(start_stop_x,
             l_vec_y,
             properties::LineWidth(3.0f),
             properties::Color::BLACK,
             properties::Label("Position equilibrium state"));
        plot(Vd{Vid{t(ss_idx), t(ss_idx)}},
             Vd{Vid{0.0, 2.0}},
             properties::LineWidth(7.0f),
             properties::Color{255, 127, 0});
        // showLegend();
        flushCurrentElement();
        softClearView();

        setActiveView("p_view_1");

        plot(Vd{Vid{t(0), t(end_idx)}},
             Vd{Vid{sim_params.error_bound, sim_params.error_bound}},
             properties::LineWidth(3.0f),
             properties::Color::BLUE);

        plot(Vd{Vid{t(0), t(end_idx)}},
             Vd{Vid{-sim_params.error_bound, -sim_params.error_bound}},
             properties::LineWidth(3.0f),
             properties::Color::BLUE);

        plot(Vd{Vid{t(ss_idx), t(ss_idx)}},
             Vd{Vid{-sim_params.error_bound, sim_params.error_bound}},
             properties::LineWidth(7.0f),
             properties::Color{255, 127, 0});
        plot(t, x - sim_params.r, properties::LineWidth(7.0f), properties::Label("Velocity"));

        flushCurrentElement();
        softClearView();
    };

    duoplot::gui::registerGuiCallback("slider_kp", [&](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
        const double val_kp = gui_element_handle.getNormalizedValue();
        const double val_ki =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ki").getNormalizedValue();
        const double val_kd =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kd").getNormalizedValue();

        plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);
    });

    duoplot::gui::registerGuiCallback("slider_ki", [&](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
        const double val_kp =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kp").getNormalizedValue();
        const double val_ki = gui_element_handle.getNormalizedValue();
        const double val_kd =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kd").getNormalizedValue();

        plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);
    });

    duoplot::gui::registerGuiCallback("slider_kd", [&](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
        const double val_kp =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kp").getNormalizedValue();
        const double val_ki =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ki").getNormalizedValue();
        const double val_kd = gui_element_handle.getNormalizedValue();

        plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);
    });

    run_sim(sim_params, x, vx);

    duoplot::gui::startGuiReceiveThread();

    setActiveView("p_view_0");
    clearView();
    waitForFlush();
    axis({0.0, -0.5}, {t(end_idx), 1.5});

    setActiveView("p_view_1");
    clearView();
    waitForFlush();
    axis({0.0, -0.5}, {t(end_idx), 0.5});

    const double val_kp =
        duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kp").getNormalizedValue();
    const double val_ki =
        duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ki").getNormalizedValue();
    const double val_kd =
        duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kd").getNormalizedValue();

    plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);

    while (true)
    {
        usleep(1000 * 1000 * 10);
    }
}

void testThreeBodyProblem()
{
    const std::string project_file_path = "../../project_files/demo_black.duoplot";
    openProjectFile(project_file_path);

    struct Body
    {
        double x;
        double y;
        double vx;
        double vy;
        double ax;
        double ay;

        double m;

        double min_vel;
        double max_vel;

        Vec3d F;

        Vector<double> vec_x;
        Vector<double> vec_y;

        properties::Color ball_color;
        properties::Color track_color;
    };

    std::vector<Body> bodies;

    const size_t n_bodies = 3U;
    const size_t n_its = 1000U;

    assert(n_bodies == 3U && "Stuff is hardcoded to 3 bodies");

    double theta = 0.0;

    const auto f_rand = [] { return 2.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5); };

    for (size_t k = 0; k < n_bodies; k++)
    {
        Body body;

        body.x = std::cos(theta);
        body.y = std::sin(theta);

        body.vx = 0.05 * std::cos(theta + 3.0 * M_PI / 2.0) + f_rand();
        body.vy = 0.05 * std::sin(theta + 3.0 * M_PI / 2.0) + f_rand();

        theta += 2.0 * M_PI / static_cast<double>(n_bodies);

        body.m = 0.5;

        body.vec_x.resize(n_its);
        body.vec_y.resize(n_its);

        for (size_t i = 0; i < n_its; i++)
        {
            body.vec_x(i) = 0.0;
            body.vec_y(i) = 0.0;
        }

        bodies.push_back(body);
    }

    bodies[0].ball_color = properties::Color{200, 0, 0};
    bodies[1].ball_color = properties::Color{0, 200, 0};
    bodies[2].ball_color = properties::Color{0, 0, 200};

    bodies[0].track_color = properties::Color{255, 100, 100};
    bodies[1].track_color = properties::Color{100, 255, 100};
    bodies[2].track_color = properties::Color{100, 100, 255};

    // bodies[0].m = 1.5;
    // bodies[1].m = 2.0;
    // bodies[2].m = 2.0;

    const auto plot_bodies = [](const std::vector<Body>& bodies, const size_t it) -> void {
        const size_t num_bodies = bodies.size();

        Vector<double> x(num_bodies), y(num_bodies);
        Vector<properties::Color> scatter_colors(num_bodies);

        double y0 = 0.0;

        for (size_t k = 0; k < num_bodies; k++)
        {
            x(k) = bodies[k].vec_x(it);
            y(k) = bodies[k].vec_y(it);
            y0 += y(k);
            scatter_colors(k) = bodies[k].ball_color;
        }

        y0 /= 3.0;

        axis({-2.0, -2.0 + y0, -4.0}, {2.0, 2.0 + y0, 4.0});

        scatter(
            x, y, scatter_colors, properties::ScatterStyle::DISC, properties::PointSize(20), properties::Color::BLACK);

        if (it != 0U)
        {
            for (size_t k = 0; k < num_bodies; k++)
            {
                const VectorConstView x_vec{bodies[k].vec_x.data(), it + 1U};
                const VectorConstView y_vec{bodies[k].vec_y.data(), it + 1U};

                plot(x_vec, y_vec, bodies[k].track_color, properties::LineWidth(2.0f));
            }
        }

        flushCurrentElement();
    };

    setActiveView("p0");
    clearView();
    axesSquare();
    disableScaleOnRotation();
    axis({-2.0, -2.0, -4.0}, {2.0, 2.0, 4.0});
    viewAngles(-20, 47);

    const double h = debug_value_args::getValue<double>("h", 0.01);
    const double damping = 0.0001;
    const double G = debug_value_args::getValue<double>("g", 1.0);

    const auto update_body = [&](Body& body_to_update, const Body& other_body) -> void {
        const double dx = body_to_update.x - other_body.x;
        const double dy = body_to_update.y - other_body.y;

        const double dx2 = dx * dx;
        const double dy2 = dy * dy;

        const double r = std::sqrt(dx2 + dy2);

        body_to_update.ax += -G * other_body.m * dx / (r * r * r);
        body_to_update.ay += -G * other_body.m * dy / (r * r * r);
    };

    for (size_t k = 0; k < n_its; k++)
    {
        for (size_t i = 0; i < n_bodies; i++)
        {
            Body& body_to_update = bodies[i];

            for (size_t j = 0; j < n_bodies; j++)
            {
                if (i == j)
                {
                    continue;
                }

                const Body& other_body = bodies[j];

                body_to_update.ax = 0.0;
                body_to_update.ay = 0.0;

                update_body(body_to_update, other_body);
            }

            if (std::abs(body_to_update.ax) > 10.0)
            {
                body_to_update.ax = 10.0 * body_to_update.ax / std::abs(body_to_update.ax);
            }
            if (std::abs(body_to_update.ay) > 10.0)
            {
                body_to_update.ay = 10.0 * body_to_update.ay / std::abs(body_to_update.ay);
            }

            body_to_update.vx += h * body_to_update.ax;
            body_to_update.vy += h * body_to_update.ay;

            body_to_update.x += h * body_to_update.vx;
            body_to_update.y += h * body_to_update.vy;

            body_to_update.vec_x(k) = body_to_update.x;
            body_to_update.vec_y(k) = body_to_update.y;

            // body_i.vx *= (1.0 - damping);
            // body_i.vy *= (1.0 - damping);
        }
    }

    disableAutomaticAxesAdjustment();
    disableScaleOnRotation();
    waitForFlush();

    viewAngles(0, -90);

    for (size_t k = 0; k < n_its; k++)
    {
        plot_bodies(bodies, k);

        softClearView();
    }
}

void testSolarSystem()
{
    const std::string project_file_path = "../../project_files/demo_white.duoplot";
    openProjectFile(project_file_path);

    // Run with
    // ./demos/demo_app/demos small solar_system -g 5 -h 0.05 --ampl 8 --its 400 --lw 3
    // Show with perspective view

    struct Body
    {
        double x;
        double y;
        double z;
        double vx;
        double vy;
        double vz;
        double ax;
        double ay;
        double az;
        double m;

        double min_vel;
        double max_vel;

        Vec3d F;

        Vector<double> vec_x;
        Vector<double> vec_y;
        Vector<double> vec_z;
        Vector<double> vec_vx;
        Vector<double> vec_vy;
        Vector<double> vec_vz;

        Vector<properties::Color> colors;
    };

    std::vector<Body> bodies;

    const size_t n_bodies = 3;
    const size_t n_its = debug_value_args::getValue<uint64_t>("its", 500U);

    const auto f_rand = [] { return 2.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5); };

    const double ampl = debug_value_args::getValue<double>("ampl", 5.0);
    const double body_mass = debug_value_args::getValue<double>("body_mass", 5.0);

    for (size_t k = 0; k < n_bodies; k++)
    {
        Body body;

        body.x = 5.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);
        body.y = 5.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);
        body.z = 5.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);

        body.vx = ampl * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);
        body.vy = ampl * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);
        body.vz = ampl * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);

        // body.m = (static_cast<double>(rand() % 1001) / 1000.0 + 0.5) * 0.1;
        body.m = body_mass;

        body.vec_x.resize(n_its);
        body.vec_y.resize(n_its);
        body.vec_z.resize(n_its);
        body.vec_vx.resize(n_its);
        body.vec_vy.resize(n_its);
        body.vec_vz.resize(n_its);

        body.colors.resize(n_its);

        for (size_t i = 0; i < n_its; i++)
        {
            body.vec_x(i) = 0.0;
            body.vec_y(i) = 0.0;
            body.vec_z(i) = 0.0;
            body.vec_vx(i) = 0.0;
            body.vec_vy(i) = 0.0;
            body.vec_vz(i) = 0.0;

            body.colors(i) = properties::Color{255, 255, 255};
        }

        bodies.push_back(body);
    }

    const double lw = debug_value_args::getValue<double>("lw", 5.0);

    const auto plot_bodies = [&lw](const std::vector<Body>& bodies, const size_t it) -> void {
        const size_t num_bodies = bodies.size();

        Vector<double> x(num_bodies), y(num_bodies), z(num_bodies);
        Vector<double> xc(1U), yc(1U), zc(1U);

        for (size_t k = 0; k < num_bodies; k++)
        {
            x(k) = bodies[k].vec_x(it);
            y(k) = bodies[k].vec_y(it);
            z(k) = bodies[k].vec_z(it);
        }

        xc(0) = 0.0;
        yc(0) = 0.0;
        zc(0) = 0.0;

        scatter3(x, y, z, properties::ScatterStyle::DISC, properties::PointSize(20), properties::Color(125, 196, 207));
        scatter3(xc, yc, zc, properties::ScatterStyle::DISC, properties::PointSize(50), properties::Color::BLACK);

        if (it != 0U)
        {
            for (size_t k = 0; k < num_bodies; k++)
            {
                const VectorConstView x_vec{bodies[k].vec_x.data(), it + 1U};
                const VectorConstView y_vec{bodies[k].vec_y.data(), it + 1U};
                const VectorConstView z_vec{bodies[k].vec_z.data(), it + 1U};

                const VectorConstView cols{bodies[k].colors.data(), it + 1U};

                plot3(x_vec, y_vec, z_vec, cols, properties::LineWidth(lw));
            }
        }

        flushCurrentElement();
    };

    setActiveView("p_view_0");
    clearView();
    axesSquare();
    disableScaleOnRotation();
    axis({-2.0, -2.0, -4.0}, {2.0, 2.0, 4.0});
    viewAngles(-20, 47);

    const double h = debug_value_args::getValue<double>("h", 0.05);
    const double damping = 0.0001;
    const double G = debug_value_args::getValue<double>("g", 5.0);

    Body center_body;
    center_body.x = 0.0;
    center_body.y = 0.0;
    center_body.z = 0.0;
    center_body.m = 10.0;

    const auto update_body = [&](Body& body_to_update, const Body& other_body) -> void {
        const double dx = body_to_update.x - other_body.x;
        const double dy = body_to_update.y - other_body.y;
        const double dz = body_to_update.z - other_body.z;

        const double dx2 = dx * dx;
        const double dy2 = dy * dy;
        const double dz2 = dz * dz;

        const double r = std::sqrt(dx2 + dy2 + dz2);

        body_to_update.ax += -G * other_body.m * dx / (r * r * r);
        body_to_update.ay += -G * other_body.m * dy / (r * r * r);
        body_to_update.az += -G * other_body.m * dz / (r * r * r);
    };

    for (size_t k = 0; k < n_its; k++)
    {
        for (size_t i = 0; i < n_bodies; i++)
        {
            Body& body_to_update = bodies[i];

            for (size_t j = 0; j < n_bodies; j++)
            {
                if (i == j)
                {
                    continue;
                }

                const Body& other_body = bodies[j];

                body_to_update.ax = 0.0;
                body_to_update.ay = 0.0;
                body_to_update.az = 0.0;

                update_body(body_to_update, other_body);
                update_body(body_to_update, center_body);
            }

            if (std::abs(body_to_update.ax) > 10.0)
            {
                body_to_update.ax = 10.0 * body_to_update.ax / std::abs(body_to_update.ax);
            }
            if (std::abs(body_to_update.ay) > 10.0)
            {
                body_to_update.ay = 10.0 * body_to_update.ay / std::abs(body_to_update.ay);
            }
            if (std::abs(body_to_update.az) > 10.0)
            {
                body_to_update.az = 10.0 * body_to_update.az / std::abs(body_to_update.az);
            }

            body_to_update.vx += h * body_to_update.ax;
            body_to_update.vy += h * body_to_update.ay;
            body_to_update.vz += h * body_to_update.az;

            body_to_update.x += h * body_to_update.vx;
            body_to_update.y += h * body_to_update.vy;
            body_to_update.z += h * body_to_update.vz;

            body_to_update.vec_x(k) = body_to_update.x;
            body_to_update.vec_y(k) = body_to_update.y;
            body_to_update.vec_z(k) = body_to_update.z;
        }
    }

    for (size_t i = 0; i < n_bodies; i++)
    {
        Body& body_i = bodies[i];

        Vector<double> v{body_i.vec_vx.size()};

        for (size_t k = 0; k < n_its; k++)
        {
            const double vx = body_i.vec_x(k);
            const double vy = body_i.vec_y(k);
            const double vz = body_i.vec_z(k);
            v(k) = sqrt(vx * vx + vy * vy + vz * vz);
        }

        body_i.min_vel = v.min();
        body_i.max_vel = v.max();

        const double delta = body_i.max_vel - body_i.min_vel;

        for (size_t k = 0; k < n_its; k++)
        {
            const double v_k = v(k);
            const double v_n = (v_k - body_i.min_vel) / delta;

            body_i.colors(k) = calculateColormapJetSoft(v_n);
        }
    }

    disableAutomaticAxesAdjustment();
    disableScaleOnRotation();
    waitForFlush();

    float azimuth = -180.0f;

    for (size_t k = 0; k < n_its; k++)
    {
        const double t = static_cast<double>(k) * 0.01;
        const double elevation = std::sin(t) * 20.0;
        plot_bodies(bodies, k);

        viewAngles(azimuth, elevation);
        azimuth += 0.5f;
        if (azimuth > 180.0f)
        {
            azimuth = -180.0f;
        }

        softClearView();
    }
}

void testSwirls()
{
    const std::string project_file_path = "../../project_files/demo_white.duoplot";
    openProjectFile(project_file_path);

    const size_t n_its = 500U;
    const size_t n_elems = 100U;

    const double t_end = 2.0 * M_PI;

    const Vector<double> t = linspaceFromBoundariesAndCount<double>(0.0, t_end, n_elems);
    const Vectord xc = duoplot::sin(t);
    const Vectord yc = duoplot::cos(t);

    const Vectord xs = duoplot::sin(t + xc / 4.0);

    const auto col = properties::Color::BLACK;
    const auto lw = properties::LineWidth(1.0f);

    const auto plot_circle = [&](const double xs, const double ys, const double xo, const double yo) -> void {
        plot((xc * xs) + xo, (yc * ys) + yo, col, lw);
    };

    const auto plot_sin = [&]() -> void { plot(t, xc, col, lw); };

    const auto plot_point = [&](const double x, const double y, const double s) -> void {
        Vectord x_vec{1U};
        Vectord y_vec{1U};
        x_vec(0) = x;
        y_vec(0) = y;
        scatter(x_vec, y_vec, properties::ScatterStyle::DISC, properties::PointSize(s), col);
    };

    setActiveView("p_view_0");
    clearView();
    viewAngles(0, 90);
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    axesSquare();
    axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});
    waitForFlush();

    const size_t n_circles = 5;

    struct SwirlSettings
    {
        double r;
        double xo;
        double yo;
        double ps;
        double rs;

        double x_pos;
        double y_pos;
        double t;
    };

    std::vector<SwirlSettings> settings;

    for (size_t k = 0; k < n_circles; k++)
    {
        // Radius, center, point size, rotation speed
        SwirlSettings ss;

        ss.r = static_cast<double>(rand() % 1001) / 1000.0 + 0.1;
        ss.xo = 2.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);
        ss.yo = 2.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);
        ss.ps = debug_value_args::getValue<double>("ps", 100.0) * (static_cast<double>(rand() % 1001) / 1000.0 + 0.1);
        ss.rs = 0.01 * (static_cast<double>(rand() % 1001) / 1000.0 + 0.1);

        ss.t = 0.0;

        settings.push_back(ss);
    }

    // for (size_t k = 0; k < n_its; k++)
    {
        for (size_t i = 0; i < n_circles; i++)
        {
            SwirlSettings& ss = settings[i];

            plot_circle(ss.r, ss.r, ss.xo, ss.yo);
            const double xo = ss.xo + std::sin(ss.t) * ss.r;
            const double yo = ss.yo + std::cos(ss.t) * ss.r;

            ss.t += ss.rs;

            plot_point(xo, yo, ss.ps);
        }

        // plot_point(1.0, 0.0, 50.0);
        plot(t, xs);
        flushCurrentElement();
        // softClearView();
        // usleep(1000U * 10U);
    }
}

void testBouncingBalls()
{
    const std::string project_file_path = "../../project_files/demo_black.duoplot";
    openProjectFile(project_file_path);

    const size_t n_balls = 50U;
    const size_t n_its = 1000U;

    const double radius = 1.0;
    const double radius2 = radius * radius;

    const Vector<double> t = linspaceFromBoundariesAndCount<double>(0.0, 2.0 * M_PI, 100U);
    const Vectord xc = radius * duoplot::sin(t);
    const Vectord yc = radius * duoplot::cos(t);

    struct Ball
    {
        double x;
        double y;
        double vx;
        double vy;
        double h;
        double r;
        double m;
        double e;
        double c;

        properties::Color col;
        properties::Color rgb_val;

        Vector<double> vec_x;
        Vector<double> vec_y;
    };

    const double g = 9.82;

    const auto update_balls = [&](std::vector<Ball>& balls, const size_t k) -> void {
        for (size_t i = 0; i < n_balls; i++)
        {
            Ball& ball = balls[i];

            const double v = std::sqrt(ball.vx * ball.vx + ball.vy * ball.vy);
            const double fc = ball.c * v * v;
            const double vxn = ball.vx / v;
            const double vyn = ball.vy / v;

            const double ax = (-fc * vxn) / ball.m;
            const double ay = (-fc * vyn - g) / ball.m;

            const double vx_inc = ax * ball.h;
            const double vy_inc = ay * ball.h;

            ball.vx += vx_inc;
            ball.vy += vy_inc;

            const double x_inc = ball.vx * ball.h;
            const double y_inc = ball.vy * ball.h;

            ball.x += x_inc;
            ball.y += y_inc;

            // The balls shall reflect at the walls of the circle with centre (0, 0) and radius 1.0
            const double x2 = ball.x * ball.x;
            const double y2 = ball.y * ball.y;

            const double r2 = x2 + y2;

            if (r2 > radius2)
            {
                const double r = std::sqrt(r2);

                const double x_prev = ball.x - x_inc;
                const double y_prev = ball.y - y_inc;

                // radius2 == (x_prev + t * (ball.x - x_prev))^2 + (y_prev + t * (ball.y - y_prev))^2
                // radius2 == x_prev^2 + t * 2 * x_prev * (ball.x - x_prev) + t^2 * (ball.x - x_prev)^2
                //          + y_prev^2 + t * 2 * y_prev * (ball.y - y_prev) + t^2 * (ball.y - y_prev)^2

                const double a = (ball.x - x_prev) * (ball.x - x_prev) + (ball.y - y_prev) * (ball.y - y_prev);
                const double b = 2.0 * x_prev * (ball.x - x_prev) + 2.0 * y_prev * (ball.y - y_prev);
                const double c = x_prev * x_prev + y_prev * y_prev - radius2;

                double t_sol = (-b + std::sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
                if (t_sol < 0.0)
                {
                    t_sol = (-b - std::sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
                }

                const double x_sol = x_prev + t_sol * (ball.x - x_prev);
                const double y_sol = y_prev + t_sol * (ball.y - y_prev);

                const Vec2d full_vec{ball.x - x_prev, ball.y - y_prev};
                const Vec2d first_vec{x_sol - x_prev, y_sol - y_prev};

                const double remainder = full_vec.norm() - first_vec.norm();

                const Vec2d circle_normal_vec = Vec2d{x_sol, y_sol}.normalized();

                const Vec2d reflected_vec =
                    (first_vec -
                     2.0 * (first_vec.x * circle_normal_vec.x + first_vec.y * circle_normal_vec.y) * circle_normal_vec)
                        .normalized();

                const double v = std::sqrt(ball.vx * ball.vx + ball.vy * ball.vy);

                const Vec2d v_final{x_sol + reflected_vec.x * remainder, y_sol + reflected_vec.y * remainder};
                const Vec2d vel_final{reflected_vec.x * v, reflected_vec.y * v};

                ball.x = v_final.x;
                ball.y = v_final.y;

                ball.vx = vel_final.x;
                ball.vy = vel_final.y;
            }

            ball.vec_x(k) = ball.x;
            ball.vec_y(k) = ball.y;
        }
    };

    Vector<double> xb{n_balls}, yb{n_balls};
    Vector<properties::Color> cols{n_balls};

    const float lw = 3.0f;

    const auto plot_balls = [&](std::vector<Ball>& balls, const size_t k) -> void {
        for (size_t i = 0; i < n_balls; i++)
        {
            Ball& ball = balls[i];

            xb(i) = ball.vec_x(k);
            yb(i) = ball.vec_y(k);
            cols(i) = ball.rgb_val;
        }

        if (k > 1U)
        {
            for (size_t i = 0; i < n_balls; i++)
            {
                Ball& ball = balls[i];

                const VectorConstView xv{ball.vec_x.data(), k + 1U};
                const VectorConstView yv{ball.vec_y.data(), k + 1U};
                plot(xv, yv, ball.col, properties::LineWidth(lw));
            }
        }

        scatter(xb, yb, cols, properties::ScatterStyle::DISC, properties::PointSize(10.0f), properties::Color::WHITE);
    };

    std::vector<Ball> balls;
    const Vec2d init_pos{std::cos(-M_PI / 4.0), std::sin(-M_PI / 4.0)};

    const double start_dir = M_PI / 4.0;
    double tv = start_dir;

    for (size_t k = 0; k < n_balls; k++)
    {
        Ball ball;

        const double d = static_cast<double>(k) / static_cast<double>(n_balls - 1U);

        ball.x = init_pos.x;
        ball.y = init_pos.y;
        ball.c = 0.05;
        ball.h = 0.001;
        ball.m = 0.1;

        ball.vx = -100.0 * std::cos(tv);
        ball.vy = 100.0 * std::sin(tv);

        ball.rgb_val = calculateColormapViridis(d);
        ball.col = properties::Color{ball.rgb_val.red, ball.rgb_val.green, ball.rgb_val.blue};

        ball.vec_x.resize(n_its);
        ball.vec_y.resize(n_its);

        for (size_t i = 0; i < n_its; i++)
        {
            ball.vec_x(i) = 0.0;
            ball.vec_y(i) = 0.0;
        }

        tv += 0.01;

        balls.push_back(ball);
    }

    setActiveView("p0");
    clearView();
    viewAngles(0, 90);
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    axesSquare();
    axis({-1.0, -1.0, -1.0}, {1.0, 1.0, 1.0});
    waitForFlush();

    plot(xc, yc, properties::Color::WHITE, properties::LineWidth(lw), properties::PERSISTENT);

    for (size_t k = 0; k < n_its; k++)
    {
        update_balls(balls, k);
    }

    for (size_t k = 0; k < n_its; k++)
    {
        plot_balls(balls, k);
        softClearView();
        usleep(1000U * 10U);
        flushCurrentElement();
    }
}

std::function<double(double, double)> f_spiral = [](const double x, const double y) -> double {
    return std::cos(x + y);
};

std::function<double(double, double)> f_sin_cos = [](const double x, const double y) -> double {
    return std::sin(x) * std::cos(y);
};

std::function<double(double, double)> f_egg_crate = [](const double x, const double y) -> double {
    return std::cos(x) * std::cos(y);
};

std::function<double(double, double)> f_saddle = [](const double x, const double y) -> double { return x * x - y * y; };

std::function<double(double, double)> f_mobius = [](const double x, const double y) -> double {
    const double pi = 3.14159265358979323846;
    return std::sin(y) * (1.0 + x / (2.0 * pi));
};

std::function<double(double, double)> f_enneper = [](const double x, const double y) -> double {
    const float r_val = std::sqrt(x * x + y * y);
    return std::sin(r_val * 3.0) / r_val;
};

std::function<double(double, double)> f_sinc = [](const double x, const double y) -> double {
    const float r_val = std::sqrt(x * x + y * y);
    return std::sin(r_val * 3.0) / r_val;
};

std::function<double(double, double)> f_klein = [](const double x, const double y) -> double {
    return (8.0 * (1.0 - y) * std::cos(x) - 2.0 * std::sin(2.0 * x) * std::sin(2.0 * y)) / 8.0;
};

void testSeries()
{
    const std::string project_file_path = "../../project_files/series.duoplot";
    openProjectFile(project_file_path);

    const size_t n_size = 100;
    Matrix<double> z(n_size, n_size);

    const auto [x, y] = meshGrid(1.0, 3.0, 1.0, 3.0, n_size, n_size);

    const auto apply_function = [&](const std::function<double(double, double)>& f) -> void {
        for (int r = 0; r < n_size; r++)
        {
            for (int c = 0; c < n_size; c++)
            {
                const double x_val = x(r, c) - 2.01;
                const double y_val = y(r, c) - 2.01;
                z(r, c) = f(x_val, y_val);
            }
        }
    };

    const size_t n_elements = 15U;

    for (size_t k = 0; k < n_elements; k++)
    {
        const std::string element_name = "p" + std::to_string(k);
        setActiveView(element_name);
        clearView();
        viewAngles(0, 90);
        disableScaleOnRotation();
        // setAxesBoxScaleFactor({1.0, 1.0, 1.0});
        axesSquare();
        axis({1.0, 1.0, -1.0}, {3.0, 3.0, 1.0});
    }

    std::vector<std::function<double(double, double)>> functions = {f_sinc,
                                                                    f_klein,
                                                                    f_saddle,
                                                                    f_egg_crate,
                                                                    f_sin_cos,
                                                                    f_spiral,
                                                                    f_sinc,
                                                                    f_klein,
                                                                    f_saddle,
                                                                    f_egg_crate,
                                                                    f_sin_cos,
                                                                    f_spiral,
                                                                    f_sinc,
                                                                    f_klein,
                                                                    f_saddle};
    std::vector<properties::ColorMap> colormaps = {properties::ColorMap::JET_SOFT,
                                                   properties::ColorMap::VIRIDIS,
                                                   properties::ColorMap::MAGMA,
                                                   properties::ColorMap::JET_BRIGHT,
                                                   properties::ColorMap::JET,
                                                   properties::ColorMap::JET_SOFT,
                                                   properties::ColorMap::VIRIDIS,
                                                   properties::ColorMap::MAGMA,
                                                   properties::ColorMap::JET_BRIGHT,
                                                   properties::ColorMap::JET,
                                                   properties::ColorMap::JET_SOFT,
                                                   properties::ColorMap::VIRIDIS,
                                                   properties::ColorMap::MAGMA,
                                                   properties::ColorMap::JET_BRIGHT,
                                                   properties::ColorMap::JET};

    std::vector<float> elevations = {
        70.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 70.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f, 30.0f};

    const size_t n_its = 1000U;
    size_t current_element = 0U;

    float azimuth = 30.0f;

    const auto f_rotate_all = [&]() -> void {
        for (size_t k = 0; k < current_element; k++)
        {
            const std::string element_name = "p" + std::to_string(k);
            setActiveView(element_name);
            viewAngles(azimuth, elevations[k]);
        }
        azimuth += 0.7f;
        if (azimuth > 180.0f)
        {
            azimuth = -180.0f;
        }
    };

    size_t stride = 50U;

    for (size_t i = 0; i < n_its; i++)
    {
        if ((i % stride == 0) && current_element < n_elements)
        {
            apply_function(functions[current_element]);
            const std::string element_name = "p" + std::to_string(current_element);
            setActiveView(element_name);
            surf(x, y, z, colormaps[current_element], properties::EdgeColor::NONE);
            current_element++;

            stride = ((stride - 5U) > 10U) ? stride - 5U : 10U;
        }
        f_rotate_all();
        usleep(1000U * 10U);
    }
}

void testTopology()
{
    const std::string project_file_path = "../../project_files/small.duoplot";
    openProjectFile(project_file_path);

    const size_t n_size = 100;
    const size_t n_triangles = (n_size - 1U) * (n_size - 1U) * 2U;

    Vector<Point3<double>> vertices{n_size * n_size};
    Vector<IndexTriplet> indices(n_triangles);

    size_t idx = 0U;
    for (size_t r = 0; r < (n_size - 1U); r++)
    {
        for (size_t c = 0; c < (n_size - 1U); c++)
        {
            indices(idx) = IndexTriplet(r * n_size + c, r * n_size + c + 1U, (r + 1U) * n_size + c);
            idx++;

            indices(idx) = IndexTriplet((r + 1U) * n_size + c + 1U, r * n_size + c + 1U, (r + 1U) * n_size + c);
            idx++;
        }
    }

    const auto rf = []() -> double { return 2.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5); };

    // Setup domain
    for (size_t r = 0; r < n_size; r++)
    {
        for (size_t c = 0; c < n_size; c++)
        {
            const size_t idx = r * n_size + c;
            const double x = static_cast<double>(c) + (rf() - 0.5) * 0.1;
            const double y = static_cast<double>(r) + (rf() - 0.5) * 0.1;
            vertices(idx) = Point3<double>{y, x, 0.0};
        }
    }

    const auto apply_noise = [&](Vector<Point3<double>>& vertices) -> void {
        for (size_t r = 0; r < n_size; r++)
        {
            for (size_t c = 0; c < n_size; c++)
            {
                const size_t idx = r * n_size + c;
                vertices(idx).z = vertices(idx).z + rf();
            }
        }
    };

    const auto apply_average = [](Vector<Point3<double>>& vertices) -> void {
        const Vector<Point3<double>> old_vertices = vertices;

        for (int64_t r = 0; r < static_cast<int64_t>(n_size); r++)
        {
            for (int64_t c = 0; c < static_cast<int64_t>(n_size); c++)
            {
                double s = 0.0;
                for (int64_t ri = -1; ri < 2; ri++)
                {
                    for (int64_t ci = -1; ci < 2; ci++)
                    {
                        int64_t new_r = r + ri;
                        int64_t new_c = c + ci;
                        if (new_r < 0)
                        {
                            new_r = 1;
                        }
                        else if (new_r >= static_cast<int64_t>(n_size))
                        {
                            new_r = static_cast<int64_t>(n_size) - 2;
                        }

                        if (new_c < 0)
                        {
                            new_c = 1;
                        }
                        else if (new_c >= static_cast<int64_t>(n_size))
                        {
                            new_c = static_cast<int64_t>(n_size) - 2;
                        }

                        const size_t new_idx = static_cast<size_t>(new_r) * n_size + static_cast<size_t>(new_c);
                        s += old_vertices(new_idx).z;
                    }
                }
                const size_t idx = r * n_size + c;

                vertices(idx).z = s / 9.0;
            }
        }
    };

    const auto apply_circle = [](Vector<Point3<double>>& vertices,
                                 const int64_t radius,
                                 const int64_t x,
                                 const int64_t y,
                                 const double amplitude) -> void {
        for (int64_t r = (y - radius); r < (y + radius); r++)
        {
            for (int64_t c = (x - radius); c < (x + radius); c++)
            {
                const int64_t r_diff = r - y;
                const int64_t c_diff = c - x;
                if ((r_diff * r_diff + c_diff * c_diff) > (radius * radius))
                {
                    continue;
                }

                int64_t new_r = r;
                int64_t new_c = c;
                if (new_r < 0)
                {
                    new_r = 0;
                }
                else if (new_r >= static_cast<int64_t>(n_size))
                {
                    new_r = static_cast<int64_t>(n_size) - 1;
                }

                if (new_c < 0)
                {
                    new_c = 0;
                }
                else if (new_c >= static_cast<int64_t>(n_size))
                {
                    new_c = static_cast<int64_t>(n_size) - 1;
                }

                const size_t new_idx = static_cast<size_t>(new_r) * n_size + static_cast<size_t>(new_c);

                vertices(new_idx).z = amplitude;
            }
        }
    };

    const auto apply_over_domain = [](Vector<Point3<double>>& vertices,
                                      const double x0,
                                      const double x1,
                                      const double y0,
                                      const double y1,
                                      const double z_g,
                                      const std::function<double(double, double)> apply_fun) -> void {
        const double dx = x1 - x0;
        const double dy = y1 - y0;

        const double d = static_cast<double>(n_size - 1U);

        for (size_t r = 0; r < n_size; r++)
        {
            for (size_t c = 0; c < n_size; c++)
            {
                const double x = dx * static_cast<double>(c) / d + x0;
                const double y = dy * static_cast<double>(r) / d + y0;

                const size_t idx = r * n_size + c;

                vertices(idx).z = apply_fun(x, y) * z_g;
            }
        }
    };

    // apply_over_domain(vertices, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 0.1, f_spiral);
    // apply_over_domain(vertices, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 0.1, f_sin_cos);
    // apply_over_domain(vertices, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 0.1, f_egg_crate);
    apply_over_domain(vertices, 0.0, 2.0 * M_PI, 0.0, 2.0 * M_PI, 0.02, f_saddle);
    // apply_over_domain(vertices, 1.0, 2.0 * M_PI, 1.0, 2.0 * M_PI, 0.1, f_sinc);

    apply_noise(vertices);
    apply_average(vertices);
    apply_average(vertices);
    apply_average(vertices);
    // apply_average(vertices);

    setActiveView("p_view_0");
    clearView();
    globalIllumination({2.0, 2.0, 2.0});
    // viewAngles(0, 90);
    // setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    // axesSquare();
    axis({0.0, 0.0, -1.0}, {static_cast<double>(n_size - 1U), static_cast<double>(n_size - 1U), 1.0});
    drawMesh(vertices, indices, properties::ColorMap::JET_SOFT, properties::EdgeColor::BLACK);
}

void testChaos()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t n_its = 100U;
    const size_t n_elements = 100U;

    const auto rnd = []() -> double { return static_cast<double>(rand() % 1001) / 1000.0 - 0.5; };

    double theta = M_PI / 2.0;
    double r = 0;
    const double h = 0.01;

    setActiveView("p_view_0");
    clearView();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});

    Vector<double> x_v{n_elements}, y_v{n_elements};

    for (size_t n = 0; n < n_its; n++)
    {
        double x = 0.0, y = 0.0;

        for (size_t k = 0; k < n_elements; k++)
        {
            x_v(k) = x;
            y_v(k) = y;

            double d = theta - M_PI / 2.0;
            d = d * d * (d < 0.0 ? -1.0 : 1.0);

            theta = theta + h * rnd() * 3.0 - d * 2.5;
            r = rnd() * 0.5 + 1.0;
            x = x + r * std::cos(theta);
            y = y + r * std::sin(theta);
        }

        plot(x_v, y_v, properties::Color::BLACK);
    }
}

void testColorfulScatter()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    // TODO: When running this demo, uncomment the "black edge" in scatter_shader.fs for "DISC"

    const size_t num_elements = 12020;
    Vector<double> x(num_elements), y(num_elements);
    Vector<double> point_sizes(num_elements);
    Vector<properties::Color> colorp(num_elements);

    const auto rnd = []() -> double { return static_cast<double>(rand() % 1001) / 1000.0; };

    const double g = 5.0;

    std::mt19937 gen{std::random_device{}()};
    std::normal_distribution<> dist;

    Vec2d mn{0.0, 28.0};
    Matrixd sigma{2U, 2U};
    sigma(0, 0) = 1.0;
    sigma(0, 1) = 0.1;
    sigma(1, 0) = 0.1;
    sigma(1, 1) = 1.0;

    double max_dist = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        const Vec2d rnd_val = sigma * Vec2d{dist(gen), dist(gen)} + mn;

        const double xk = rnd_val.x;
        const double yk = rnd_val.y - rnd_val.x * rnd_val.x;
        x(k) = rnd_val.x;
        y(k) = yk;

        const double dx = xk - mn.x;
        const double dy = yk - mn.y;
        const double d = dx * dx + dy * dy;
        if (d > max_dist)
        {
            max_dist = d;
        }
    }

    max_dist = 40.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        const double xk = x(k);
        const double yk = y(k);

        const double dx = xk - mn.x;
        const double dy = yk - mn.y;
        const double d = dx * dx + dy * dy;

        point_sizes(k) = 4.0 * d / max_dist;
        colorp(k) = calculateColormapJetSoft(d / max_dist);
    }

    setActiveView("p_view_0");
    clearView();
    // setAxesBoxScaleFactor({1.0, 1.0, 1.0});

    scatter(x, y, colorp, properties::ScatterStyle::DISC);
}

}  // namespace small

#endif  // DEMOS_SMALL_H
