#ifndef DEMOS_TESTS_CAR_CAR_H_
#define DEMOS_TESTS_CAR_CAR_H_

#include <fstream>
#include <iterator>
#include <tuple>
#include <vector>

#include "debug_value_args.h"
#include "lumos/plotting/duoplot.h"

using namespace lumos;

namespace car
{

ImageRGBA<uint8_t> readCarImage(const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue)
{
    const std::string bin_path = "../demos/demo_app/tests/car/car.bin";
    std::ifstream input(bin_path, std::ios::binary);

    std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

    const size_t num_rows = 150;
    const size_t num_cols = 300;

    ImageGrayView<uint8_t> img_view{buffer.data(), num_rows, num_cols};

    ImageRGBA<uint8_t> img_ga(num_rows, num_cols);

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            const uint8_t pixel_val = img_view(r, c);

            if (pixel_val == 255)
            {
                img_ga(r, c, 1) = 0;
            }
            else
            {
                if (pixel_val < 60)
                {
                    // Dark area
                    img_ga(r, c, 0) = pixel_val;
                    img_ga(r, c, 1) = pixel_val;
                    img_ga(r, c, 2) = pixel_val;
                }
                else
                {
                    // Bright area
                    img_ga(r, c, 0) = red;
                    img_ga(r, c, 1) = green;
                    img_ga(r, c, 2) = blue;
                }

                img_ga(r, c, 3) = 255;
            }
        }
    }

    return img_ga;
}

std::tuple<Vector<float>, Vector<float>, Vector<float>> generatePath()
{
    const size_t num_its = 1000;
    srand(4);

    Vector<float> xv{num_its}, yv{num_its}, phiv{num_its}, t{num_its};

    const float b = 0.95f;
    t = linspaceFromBoundariesAndCount(-b * M_PI / 2.0f, b * M_PI / 2.0f, num_its);

    const float t_mul = 7.0f;
    const float y_mul = 7.0f;
    const float y_off = 80.0f;
    // xv = t * t_mul;
    // yv = lumos::tan(t) * y_mul + y_off;

    yv = linspaceFromBoundariesAndCount(-8.94344f, 168.943f, num_its);
    xv = t_mul * lumos::atan((yv - y_off) / y_mul);

    for (size_t k = 0; k < (num_its - 1U); k++)
    {
        const float x0 = xv(k);
        const float x1 = xv(k + 1U);

        const float y0 = yv(k);
        const float y1 = yv(k + 1U);

        const float delta = std::atan2(y1 - y0, x1 - x0);
        phiv(k) = -delta + M_PI / 2.0;
    }

    phiv(num_its - 1U) = phiv(num_its - 2U);

    return {yv, xv, phiv};
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/car.duoplot";

    openProjectFile(project_file_path);

    const ImageRGBA<uint8_t> img_obst = readCarImage(245U, 124U, 124U);

    const ImageRGBA<uint8_t> img = readCarImage(0U, 138U, 138U);
    ImageGray<uint8_t> road(1, 1);
    ImageGray<uint8_t> line(1, 1);

    road(0, 0) = 127U;
    line(0, 0) = 200U;

    Vector<float> x, y, phi;
    std::tie(x, y, phi) = generatePath();

    const Vec3d scale{0.1, 0.1, 1.0};
    const Vec3d center_of_rotation = Vec3d{150.0, 75.0, 0.0}.elementWiseMultiply(scale);

    setActiveView("p_view_0");
    clearView();

    const double y_scale = 63.0;

    axis({-32.0, -y_scale, -10.0}, {-32.0 + 223, y_scale, 10.0});
    viewAngles(-90, -90);

    imShow(road,
           properties::ZOffset(0.0f),
           properties::Transform{diagMatrix<double>({204, 40, 0}), rotationMatrixZ<double>(0), {-20, -20, 3}});

    for (size_t k = 0; k < 17; k++)
    {
        const double kf = k;
        imShow(line,
               properties::Transform{
                   diagMatrix<double>({3, 2, 0}), unitMatrix<double>(3, 3), {kf * 12.0 - 18.0, -1.0, 2}});
    }

    for (size_t k = 0; k < 14; k++)
    {
        const double kf = k;
        const double dy = 17.0;
        imShow(line,
               properties::Transform{
                   diagMatrix<double>({5, 2, 0}), unitMatrix<double>(3, 3), {kf * 15.0 - 19.0, -1.0 - dy, 2}});
        imShow(line,
               properties::Transform{
                   diagMatrix<double>({5, 2, 0}), unitMatrix<double>(3, 3), {kf * 15.0 - 19.0, -1.0 + dy, 2}});
    }

    plot(x, y, properties::Color(0, 255, 0), properties::LineWidth(5), properties::ZOffset(0.15));
    imShow(img,
           properties::ID0,
           properties::ZOffset(0.05f),
           properties::Transform{diagMatrix<double>(scale),
                                 rotationMatrixZ<double>(M_PI),
                                 -rotationMatrixZ<double>(M_PI) * center_of_rotation});

    imShow(img_obst,
           properties::ID1,
           properties::ZOffset(0.05f),
           properties::Transform{diagMatrix<double>(scale), -rotationMatrixZ<double>(M_PI), Vec3d{100.0, -16.0, 0.0f}});

    for (size_t k = 0; k < x.size(); k += 10U)
    {
        const double ang = phi(k) - M_PI;

        const Matrix<double> r_mat = rotationMatrixZ<double>(ang);

        const Vec3d center_of_rotation_r = r_mat * center_of_rotation;
        const Vec3d v_res = Vec3d{x(k), y(k), 0.0f} - center_of_rotation_r;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, v_res);
        usleep(50 * 1000);
    }
}

}  // namespace car

#endif  // DEMOS_TESTS_CAR_CAR_H_
