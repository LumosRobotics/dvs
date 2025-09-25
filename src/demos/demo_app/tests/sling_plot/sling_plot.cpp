#include "tests/sling_plot/sling_plot.h"

#include "duoplot/duoplot.h"
#include "tests/slam/color_maps.h"

namespace sling_plot
{

using namespace duoplot;

void updateSling(Vector<float>& x, Vector<float>& y, Vector<float>& vx, Vector<float>& vy)
{
    const float g = 9.81f;
    const float Kf = 100.1f;
    const float m = 1.0f;
    const float dt = 0.001f;
    const float L = 1.0f;
    const float Cf = 0.5f;

    const size_t num_particles = x.size();

    for (size_t k = 1; k < num_particles; ++k)
    {
        const float x_k = x(k);
        const float y_k = y(k);

        const float x_k_1 = x(k - 1U);
        const float y_k_1 = y(k - 1U);

        float ax_k = 0.0f;
        float ay_k = 0.0f;

        {
            const Vec2d diff_vec{x_k - x_k_1, y_k - y_k_1};
            const Vec2d diff_vec_norm = -diff_vec.normalized();
            // const float force_factor = std::abs(diff_vec.norm() - L) * Kf;
            float force_factor = (diff_vec.norm() - L) * Kf;
            force_factor = force_factor < 0.0f ? 0.0f : force_factor;

            ax_k += diff_vec_norm.x * force_factor / m;
            ay_k += diff_vec_norm.y * force_factor / m - g;
        }

        if (k != (num_particles - 1U))
        {
            const float x_k_p1 = x(k + 1U);
            const float y_k_p1 = y(k + 1U);

            const Vec2d diff_vec{x_k - x_k_p1, y_k - y_k_p1};
            const Vec2d diff_vec_norm = -diff_vec.normalized();
            // const float force_factor = std::abs(diff_vec.norm() - L) * Kf;
            float force_factor = (diff_vec.norm() - L) * Kf;
            force_factor = force_factor < 0.0f ? 0.0f : force_factor;

            ax_k += diff_vec_norm.x * force_factor / m;
            ay_k += diff_vec_norm.y * force_factor / m - g;
        }

        ax_k += -vx(k) * Cf / m;
        ay_k += -vy(k) * Cf / m;

        x(k) = x_k + vx(k) * dt;
        y(k) = y_k + vy(k) * dt;

        vx(k) = vx(k) + ax_k * dt;
        vy(k) = vy(k) + ay_k * dt;
    }
}

void testSlingPlot()
{
    const std::string project_file_path = "../../project_files/sling_plot.duoplot";
    openProjectFile(project_file_path);

    const size_t num_particles = 10U;
    const size_t num_its = 1000U;

    Vector<float> x{num_particles};
    Vector<float> y{num_particles};

    Vector<float> vx{num_particles};
    Vector<float> vy{num_particles};

    Vector<properties::Color> colors{num_particles};

    for (size_t k = 0; k < num_particles; ++k)
    {
        if (k == 0)
        {
            colors(k) = properties::Color{0U, 0U, 0U};
        }
        else
        {
            const float value = static_cast<float>(k - 1U) / static_cast<float>(num_particles - 2U);
            const RGBTripletf color_val = calculateColormapJetBright(value);
            colors(k) = properties::Color(color_val.red * 255.0f, color_val.green * 255.0f, color_val.blue * 255.0f);
        }
    }

    for (size_t k = 0; k < num_particles; ++k)
    {
        x(k) = k;
    }

    y.fill(0.0f);
    vx.fill(0.0f);
    vy.fill(0.0f);

    setActiveView("velocity");
    clearView();
    axis({0.0, 0.0}, {10.0, 25.0});
    waitForFlush();

    setActiveView("view");
    clearView();
    axis({-10.0, -19.0}, {10.0, 1.0});
    waitForFlush();

    std::vector<std::vector<float>> v_vec;
    std::vector<float> t_vec;

    const float dt = 0.01f;
    float t = 0.0f;

    v_vec.resize(num_particles);

    t_vec.reserve(num_its);

    for (size_t k = 0; k < num_particles; ++k)
    {
        v_vec[k].reserve(num_its);
    }

    for (size_t k = 0; k < num_its; k++)
    {
        for (size_t i = 0; i < 10; i++)
        {
            updateSling(x, y, vx, vy);
        }

        t_vec.push_back(t);
        const Vector<float> t_copy = t_vec;
        t += dt;

        setActiveView("velocity");
        softClearView();

        for (size_t q = 0; q < num_particles; ++q)
        {
            const float v_q = std::sqrt(vx(q) * vx(q) + vy(q) * vy(q));

            v_vec[q].push_back(v_q);

            const Vector<float> v_copy = v_vec[q];

            if ((k > 1) && (q > 0))
            {
                const properties::Color col{colors(q).red, colors(q).green, colors(q).blue};
                plot(t_copy, v_copy, properties::LineWidth(5.0f), col);
            }
        }
        flushCurrentElement();
        std::cout << v_vec[0].size() << std::endl;

        setActiveView("view");
        softClearView();

        scatter(x, y, colors, properties::PointSize(15.0f), properties::ScatterStyle::DISC);
        plot(x, y, properties::LineWidth(5.0f));
        flushCurrentElement();

        usleep(1000U * 10U);
    }
}

}  // namespace sling_plot
