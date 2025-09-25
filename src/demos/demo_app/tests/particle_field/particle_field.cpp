#include "tests/boxes/boxes.h"

namespace particle_field
{

void initializeMatrices(Matrix<float>& x, Matrix<float>& y, const float d)
{
    const size_t num_rows = x.numRows(), num_cols = x.numCols();

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            x(r, c) = static_cast<float>(c) * d;
            y(r, c) = static_cast<float>(r) * d;
        }
    }
}

properties::Color calculateColormapRainbow(double value)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value >= 1.0)
    {
        value = 0.99999;
    }

    const double full_range_value = value * 5.0;
    const double integer_part = floor(full_range_value);
    const double fraction_part = full_range_value - integer_part;

    double r = 0.0, g = 0.0, b = 0.0;

    switch (static_cast<int>(integer_part))
    {
        case 0:
            r = 1.0 - fraction_part;
            g = 1.0;
            b = 0.0;
            break;
        case 1:
            r = 0.0;
            g = 1.0;
            b = fraction_part;
            break;
        case 2:
            r = 0.0;
            g = 1.0 - fraction_part;
            b = 1.0;
            break;
        case 3:
            r = fraction_part;
            g = 0.0;
            b = 1.0;
            break;
        case 4:
            r = 1.0;
            g = 0.0;
            b = 1.0 - fraction_part;
            break;
    }

    return properties::Color{static_cast<std::uint8_t>(r * 255.0),
                             static_cast<std::uint8_t>(g * 255.0),
                             static_cast<std::uint8_t>(b * 255.0)};
}

void stepSimulation(Matrix<float>& x,
                    Matrix<float>& y,
                    Matrix<float>& vx,
                    Matrix<float>& vy,
                    Matrix<properties::Color>& color,
                    Matrix<float>& x0,
                    Matrix<float>& y0,
                    const float d)
{
    const float h = 0.01f;
    const float K = 0.5f;
    const float Kn = 0.1f;
    const float C = 0.3f;
    const float m = 1.0f;

    const Matrix<float> dx = x - x0;
    const Matrix<float> dy = y - y0;

    std::int32_t num_rows = x.numRows(), num_cols = x.numCols();

    const float d_sqrt_2 = d * std::sqrt(2.0f);

    for (std::int32_t r = 0; r < num_rows; r++)
    {
        for (std::int32_t c = 0; c < num_cols; c++)
        {
            float dnx = 0.0f, dny = 0.0f;

            for (std::int32_t i = -1; i < 1; i++)
            {
                for (std::int32_t j = -1; j < 1; j++)
                {
                    if (((r + i) >= 0) && ((r + i) < num_rows) && c + j >= 0 && c + j < num_cols)
                    {
                        if (i == 0 && j == 0)
                        {
                            continue;
                        }
                        else if ((i == 0) || (j == 0))
                        {
                            const float ldx = x(r + i, c + j) - x(r, c);
                            const float ldy = y(r + i, c + j) - y(r, c);
                            const float ld = std::sqrt(ldx * ldx + ldy * ldy);
                            const float F = ld - d;
                            dnx += F * ldx / ld;
                            dny += F * ldy / ld;
                        }
                        else
                        {
                            const float ldx = x(r + i, c + j) - x(r, c);
                            const float ldy = y(r + i, c + j) - y(r, c);
                            const float ld = std::sqrt(ldx * ldx + ldy * ldy);
                            const float F = ld - d_sqrt_2;
                            dnx += F * ldx / ld;
                            dny += F * ldy / ld;
                        }
                    }
                }
            }
            const float lvx = vx(r, c);
            const float lvy = vy(r, c);

            const float v = std::sqrt(lvx * lvx + lvy * lvy);

            vx(r, c) = lvx + h * (-K * dx(r, c) + Kn * dnx - C * lvx) / m;
            vy(r, c) = lvy + h * (-K * dy(r, c) + Kn * dny - C * lvy) / m;
            color(r, c) = calculateColormapRainbow(v / 0.1f);

            x(r, c) = x(r, c) + h * vx(r, c);
            y(r, c) = y(r, c) + h * vy(r, c);
        }
    }
}

void placeBumpAt(Matrix<float>& vx,
                 Matrix<float>& vy,
                 const std::int32_t xc,
                 const std::int32_t yc,
                 const std::int32_t num_rows,
                 const std::int32_t num_cols,
                 const std::int32_t rad,
                 const float mul)
{
    const std::int32_t r_min = std::max(0, yc - rad);
    const std::int32_t r_max = std::min(num_rows, yc + rad);
    const std::int32_t c_min = std::max(0, xc - rad);
    const std::int32_t c_max = std::min(num_cols, xc + rad);
    const float rf = rad;

    for (std::int32_t r = r_min; r < r_max; r++)
    {
        for (std::int32_t c = c_min; c < c_max; c++)
        {
            const float dx = static_cast<float>(r) - static_cast<float>(yc);
            const float dy = static_cast<float>(c) - static_cast<float>(xc);

            vx(r, c) += std::exp((-dx * dx - dy * dy) * mul);
            vy(r, c) += std::exp((-dx * dx - dy * dy) * mul);
        }
    }
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/particle_field.duoplot";
    openProjectFile(project_file_path);

    setActiveView("p_view_0");
    clearView();

    const size_t num_rows = 400, num_cols = 400;
    const size_t num_particles = num_rows * num_cols;

    Matrix<float> x(num_rows, num_cols), y(num_rows, num_cols);
    Matrix<properties::Color> color(num_rows, num_cols);
    const float d = 0.05f;

    initializeMatrices(x, y, d);

    Matrix<float> x0 = x, y0 = y, vx = x, vy = y;

    vx.fill(0.0f);
    vy.fill(0.0f);

    std::int32_t xc = 50, yc = 60, rad = 20;

    placeBumpAt(vx, vy, xc, yc, num_rows, num_cols, rad, 0.01f);

    placeBumpAt(vx, vy, 200, 100, num_rows, num_cols, 200, 0.01f);

    placeBumpAt(vx, vy, 200, 100, num_rows, num_cols, 200, 0.01f);
    placeBumpAt(vx, vy, 0, 100, num_rows, num_cols, 200, 0.01f);
    placeBumpAt(vx, vy, 140, 10, num_rows, num_cols, 200, 0.01f);
    placeBumpAt(vx, vy, 100, 200, num_rows, num_cols, 200, 0.01f);

    const VectorConstView<float> x_vec(x.data(), x.numElements()), y_vec(y.data(), y.numElements());
    const VectorConstView<properties::Color> color_view(color.data(), color.numElements());

    axis({0.0, 0.0}, {20.0, 20.0});

    for (size_t k = 0; k < 2000; k++)
    {
        stepSimulation(x, y, vx, vy, color, x0, y0, d);
        scatter(x_vec, y_vec, color_view, properties::ScatterStyle::SQUARE, properties::PointSize(2.0f));

        clearViewOnUpdate();
    }
}

}  // namespace particle_field
