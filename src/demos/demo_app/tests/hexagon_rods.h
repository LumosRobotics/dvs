#ifndef DEMOS_TESTS_HEXAGON_RODS_H
#define DEMOS_TESTS_HEXAGON_RODS_H

#include "duoplot/duoplot.h"

using namespace duoplot;

namespace hexagon_rods
{

const float z_min = 0.0f;
const float z_max = 1.0f;

const float radius = 1.0f;

void setPoints(Vector<float>& x,
               Vector<float>& y,
               Vector<float>& z,
               Vector<IndexTriplet>& indices,
               const size_t num_rows,
               const size_t num_cols)
{
    Vector<float> x_points(7), y_points(7);

    const float circle_center = static_cast<float>(num_rows) / 2.0f;

    for (size_t i = 0; i < 7; i++)
    {
        const float angle = static_cast<float>(i) * 2.0f * M_PI / 6.0f;
        x_points(i) = std::cos(angle) * radius;
        y_points(i) = std::sin(angle) * radius;
    }

    std::vector<IndexTriplet> indices_vec;
    uint32_t idx = 0U;

    for (size_t i = 0; i < num_rows; i++)
    {
        for (size_t j = 0; j < num_cols; j++)
        {
            float x_offset = static_cast<float>(j) * 3.0f * radius;
            if ((i % 2) == 1)
            {
                x_offset += 1.5f * radius;
            }

            const float y_offset = static_cast<float>(i) * 1.0f * radius * std::sin(M_PI / 3.0f);

            const float d =
                std::sqrt(std::pow(x_offset - circle_center, 2.0f) + std::pow(y_offset - circle_center, 2.0f));
            if (d > (circle_center / 2.0f))
            {
                continue;
            }

            // Hexagon face
            for (uint32_t k = 0; k < 6; k++)
            {
                x(idx) = x_offset;
                y(idx) = y_offset;
                z(idx) = z_max;

                x(idx + 1) = x_offset + x_points(k);
                y(idx + 1) = y_offset + y_points(k);
                z(idx + 1) = z_max;

                x(idx + 2) = x_offset + x_points(k + 1);
                y(idx + 2) = y_offset + y_points(k + 1);
                z(idx + 2) = z_max;

                indices_vec.push_back(IndexTriplet{idx, idx + 1, idx + 2});

                idx += 3;
            }

            // Hexagon sides
            for (uint32_t k = 0; k < 6; k++)
            {
                const float x0 = x_offset + x_points(k);
                const float y0 = y_offset + y_points(k);
                const float x1 = x_offset + x_points(k + 1);
                const float y1 = y_offset + y_points(k + 1);

                x(idx) = x0;
                y(idx) = y0;
                z(idx) = z_max;

                x(idx + 1) = x1;
                y(idx + 1) = y1;
                z(idx + 1) = z_max;

                x(idx + 2) = x0;
                y(idx + 2) = y0;
                z(idx + 2) = z_min;

                indices_vec.push_back(IndexTriplet{idx, idx + 1, idx + 2});

                idx += 3;

                x(idx) = x0;
                y(idx) = y0;
                z(idx) = z_min;

                x(idx + 1) = x1;
                y(idx + 1) = y1;
                z(idx + 1) = z_min;

                x(idx + 2) = x1;
                y(idx + 2) = y1;
                z(idx + 2) = z_max;

                indices_vec.push_back(IndexTriplet{idx, idx + 2, idx + 1});

                idx += 3;
            }
        }
    }

    indices = indices_vec;
}

void updateZ(Vector<float>& z, const size_t num_rows, const size_t num_cols, const float t)
{
    const float circle_center = static_cast<float>(num_rows) / 2.0f;

    uint32_t idx = 0U;

    const float local_radius = 0.1f;

    for (size_t i = 0; i < num_rows; i++)
    {
        for (size_t j = 0; j < num_cols; j++)
        {
            float x_offset = static_cast<float>(j) * 3.0f * radius;
            if ((i % 2) == 1)
            {
                x_offset += 1.5f * radius;
            }

            const float y_offset = static_cast<float>(i) * 1.0f * radius * std::sin(M_PI / 3.0f);

            const float d =
                std::sqrt(std::pow(x_offset - circle_center, 2.0f) + std::pow(y_offset - circle_center, 2.0f));
            if (d > (circle_center / 2.0f))
            {
                continue;
            }

            const float r = std::sqrt(std::pow(x_offset - 22.0f, 2.0f) + std::pow(y_offset - 24.0f, 2.0f)) + t;
            const float local_z_max = z_max + 1.0f * std::sin(r) / r;

            // Hexagon face
            for (uint32_t k = 0; k < 6; k++)
            {
                z(idx) = local_z_max;

                z(idx + 1) = local_z_max;

                z(idx + 2) = local_z_max;

                idx += 3;
            }

            // Hexagon sides
            for (uint32_t k = 0; k < 6; k++)
            {
                z(idx) = local_z_max;

                z(idx + 1) = local_z_max;

                z(idx + 2) = z_min;

                idx += 3;

                z(idx) = z_min;

                z(idx + 1) = z_min;

                z(idx + 2) = local_z_max;

                idx += 3;
            }
        }
    }
}

void testBasic()
{
    const size_t num_cols = 10;
    const size_t num_rows = static_cast<double>(num_cols) * 3.5;
    const size_t num_rods = num_rows * num_cols;

    const size_t num_triangles = num_rods * (6 * 2 + 6) * 3;
    const size_t num_its = 500;

    Vector<float> x(num_triangles);
    Vector<float> y(num_triangles);
    Vector<float> z(num_triangles);
    Vector<IndexTriplet> indices;

    setPoints(x, y, z, indices, num_rows, num_cols);

    setActiveView("p_view_0");
    clearView();

    globalIllumination({16, 16, 10});
    axis(Vec3d{8.0, 8.0, -0.7}, Vec3d{28.0, 28.0, 1.5});
    float azimuth = -143.0f;

    float t = 0.0f;

    for (size_t k = 0; k < num_its; k++)
    {
        // properties::FaceColor(0.5294 * 255.0, 0.7333 * 255.0, 0.6667 * 255.0)
        viewAngles(azimuth, 25);
        updateZ(z, num_rows, num_cols, t);
        drawMesh(x, y, z, indices, properties::FaceColor(0, 0.35 * 255.0, 0), properties::EdgeColor::NONE);
        azimuth += 0.5f;

        usleep(10000);

        softClearView();

        t += 0.1f;
    }
}

}  // namespace hexagon_rods

#endif  // DEMOS_TESTS_KLEIN_H_
