#ifndef DEMOS_TESTS_DUOPLOT_TEXT_H_
#define DEMOS_TESTS_DUOPLOT_TEXT_H_

#include "duoplot/duoplot.h"

using namespace duoplot;

namespace duoplot_text
{

void readShapeImage(const std::string bin_path, ImageRGBA<std::uint8_t>& output_img)
{
    std::ifstream input(bin_path, std::ios::binary);

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    std::uint16_t num_rows, num_cols;

    std::memcpy(&num_cols, buffer.data(), sizeof(std::uint16_t));
    std::memcpy(&num_rows, buffer.data() + sizeof(std::uint16_t), sizeof(std::uint16_t));

    const std::uint8_t* const img_raw_ptr = buffer.data() + 2 * sizeof(std::uint16_t);

    output_img.resize(num_rows, num_cols);

    const size_t num_element_per_channel = num_rows * num_cols;

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            const std::uint8_t pixel_val_r = img_raw_ptr[r * num_cols + c];
            const std::uint8_t pixel_val_g = img_raw_ptr[num_element_per_channel + r * num_cols + c];
            const std::uint8_t pixel_val_b = img_raw_ptr[2U * num_element_per_channel + r * num_cols + c];

            if (pixel_val_r == 0 && pixel_val_g == 0 && pixel_val_b == 0)
            {
                // Text
                output_img(r, c, 0) = 0;
                output_img(r, c, 1) = 0;
                output_img(r, c, 2) = 0;
                continue;
            }
            else
            {
                // Background
                output_img(r, c, 0) = 0;
                output_img(r, c, 1) = 127;
                output_img(r, c, 2) = 127;
            }
        }
    }

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            output_img(r, c, 3) = 255;
        }
    }
}

properties::Color calculateColormapJetSoft_duoplot_text(double value)
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

void testBasic()
{
    const std::string project_file_path = "../../project_files/small_bright.duoplot";
    openProjectFile(project_file_path);

    ImageRGBA<std::uint8_t> img;

    readShapeImage("../demos/demo_app/tests/duoplot_text/img.bin", img);

    const size_t num_cubes = img.width() * img.height();

    Vector<double> x(num_cubes), y(num_cubes), z(num_cubes);

    Vector<properties::Color> colors{num_cubes}, colors_sparkle{num_cubes};

    const size_t img_height = img.height();
    const size_t img_width = img.width();

    const double img_width_1 = static_cast<double>(img_width - 1);
    const double img_height_1 = static_cast<double>(img_height - 1);

    Matrix<double> phases(img_height, img_width);

    for (size_t r = 0; r < img_height; r++)
    {
        for (size_t c = 0; c < img_width; c++)
        {
            phases(r, c) = static_cast<double>(r + c) * 0.1;
        }
    }

    const double s = 1.1;

    const double ox = s * static_cast<double>(img_width) / 2.0;
    const double oy = s * static_cast<double>(img_height) / 2.0;

    for (size_t r = 0; r < img_height; r++)
    {
        for (size_t c = 0; c < img_width; c++)
        {
            const size_t idx = r * img_width + c;

            const size_t new_r = img_height - r - 1;
            const size_t new_c = c;

            x(idx) = static_cast<double>(c) * s + 0.5 - ox;
            y(idx) = static_cast<double>(r) * s + 0.5 - oy;
            z(idx) = 0.0;

            colors(idx) = properties::Color{img(new_r, new_c, 0), img(new_r, new_c, 1), img(new_r, new_c, 2)};
        }
    }

    setActiveView("p_view_0");
    clearView();
    axesSquare();
    disableScaleOnRotation();
    disableAutomaticAxesAdjustment();
    const double axes_scale = 16.0;
    globalIllumination({2.0, 2.0, 2.0});
    axis({-axes_scale, -axes_scale, -axes_scale}, {axes_scale, axes_scale, axes_scale});
    // Use perspective projection for this test

    float azimuth = 0.0f;
    viewAngles(-90.0f, 90.0f);
    // viewAngles(azimuth, 32.0);
    const size_t n_its = 500U;
    double t = 0.0;

    const auto safe_pixel = [&colors_sparkle, &img_height, &img_width](
                                const std::int32_t r, const std::int32_t c, const properties::Color& col) -> void {
        if ((r >= 0) && (r < static_cast<std::int32_t>(img_height)) && (c >= 0) &&
            (c < static_cast<std::int32_t>(img_width)))
        {
            const size_t idx = static_cast<size_t>(r) * img_width + static_cast<size_t>(c);
            colors_sparkle(idx) = col;
        }
    };

    for (size_t k = 0; k < n_its; k++)
    {
        for (size_t r = 0; r < img_height; r++)
        {
            for (size_t c = 0; c < img_width; c++)
            {
                const size_t idx = r * img_width + c;

                const double zk = 3.0 * std::sin(t + phases(r, c));

                z(idx) = zk;

                const properties::Color& col_res{colors(idx)};

                if (!(col_res.red == 0 && col_res.green == 0 && col_res.blue == 0))
                {
                    colors_sparkle(idx) = calculateColormapJetSoft_duoplot_text((zk + 3.0) / 6.0);
                }
            }
        }

        t += 0.1;
        drawCubes(x, y, z, colors_sparkle, 1.0, properties::EdgeColor::NONE);
        viewAngles(azimuth, 32.0);
        azimuth += 0.5f;

        if (azimuth > 180.0f)
        {
            azimuth = -180.0f;
        }
        clearViewOnUpdate();
    }

    // imShow(img);
}

}  // namespace duoplot_text

#endif  // DEMOS_TESTS_IMU_H_
