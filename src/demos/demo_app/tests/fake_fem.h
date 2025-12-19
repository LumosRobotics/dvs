#ifndef DEMOS_TESTS_FAKE_FEM_H_
#define DEMOS_TESTS_FAKE_FEM_H_

#include "lumos/plotting/duoplot.h"

using namespace lumos;

namespace fake_fem
{

class FakeFem
{
private:
    size_t num_iterations_;
    size_t size_;
    float* x_pos_;
    float* y_pos_;
    Vec2f min_bnd_;
    Vec2f max_bnd_;

public:
    FakeFem() = delete;
    FakeFem(const size_t num_iterations, const Vec2f p0, const Vec2f min_bnd, const Vec2f max_bnd)
        : num_iterations_{num_iterations},
          size_{num_iterations + 2},
          x_pos_{new float[size_]},
          y_pos_{new float[size_]},
          min_bnd_{min_bnd},
          max_bnd_{max_bnd}
    {
        const auto f_rand = []() -> float { return (rand() % 1001) / 1000.0f; };
        const auto f_rand_zero = []() -> float { return 2.0f * (rand() % 1001) / 1000.0f - 1.0f; };

        for (size_t k = 0; k < size_; k++)
        {
            x_pos_[k] = 0.0f;
            y_pos_[k] = 0.0f;
        }

        x_pos_[0] = p0.x;
        y_pos_[0] = p0.y;

        const float r = 0.1f;
        const float r_var = 0.01f;

        const float angle = f_rand() * 2.0f * M_PI;

        const float r0 = r + f_rand_zero() * r_var;

        x_pos_[1] = x_pos_[0] + r0 * std::cos(angle);
        y_pos_[1] = y_pos_[0] + r0 * std::sin(angle);

        VectorConstView<float> x{x_pos_, size_};
        VectorConstView<float> y{y_pos_, size_};

        // const Vec2f delta_vec = max_bnd - min_bnd;

        float m = 1.0f;

        for (size_t k = 0; k < num_iterations_; k++)
        {
            const Vec2f pk{x_pos_[k], y_pos_[k]};
            const Vec2f pk_1{x_pos_[k + 1], y_pos_[k + 1]};

            if ((k % 2) == 0)
            {
                m = 1.0f;
            }
            else
            {
                m = -1.0f;
            }

            const Vec2f pd = pk_1 - pk;
            const Vec2f pdr{-m * pd.y, m * pd.x};
            const Vec2f pdr_normalized = pdr.normalized();

            const Vec2f mid_point = (pk + pk_1) / 2.0f;

            const Vec2f new_point = mid_point + r * pdr_normalized;

            x_pos_[k + 2] = new_point.x;
            y_pos_[k + 2] = new_point.y;

            std::cout << new_point << std::endl;

            scatter(x, y, properties::Color::RED, properties::ScatterStyle::DISC, properties::PointSize(30));
            usleep(500 * 1000);
            clearViewOnUpdate();
        }
    }

    ~FakeFem()
    {
        delete[] x_pos_;
        delete[] y_pos_;
    }

    void update() {}

    size_t size() const
    {
        return size_;
    }

    float* getXPos() const
    {
        return x_pos_;
    }

    float* getYPos() const
    {
        return y_pos_;
    }
};

void testBasic()
{
    const size_t num_steps = 1;
    const size_t num_particles = 500;
    const Vec2f min_bnd{0.0f, 0.0f};
    const Vec2f max_bnd{1.0f, 1.0f};
    const Vec2f p0{0.5f, 0.5f};

    // VectorConstView<float> x{ff.getXPos(), ff.size()};
    // VectorConstView<float> y{ff.getYPos(), ff.size()};

    setActiveView("p_view_0");
    clearView();
    axis(min_bnd, max_bnd);
    FakeFem ff{num_particles, p0, min_bnd, max_bnd};

    // scatter(x, y, properties::Color::RED, properties::ScatterStyle::DISC, properties::PointSize(30));
    // usleep(50 * 1000);
    // clearViewOnUpdate();
}

}  // namespace fake_fem

#endif  // DEMOS_TESTS_FAKE_FEM_H_
