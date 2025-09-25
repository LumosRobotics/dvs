#ifndef DEMOS_TESTS_WAVE_EQUATION_H_
#define DEMOS_TESTS_WAVE_EQUATION_H_

#include "duoplot/duoplot.h"

using namespace duoplot;

namespace wave_equation
{

struct WaveEquationParams
{
    float a;
    float b;
    float c;

    float dx;
    float dy;
    float dt;

    WaveEquationParams() = default;
    WaveEquationParams(const float c_, const float dx_, const float dy_, const float dt_)
    {
        dx = dx_;
        dy = dy_;
        dt = dt_;

        c = c_;

        a = c_ * c_ * dt_ / (dx_ * dx_);
        b = 2.0f * 2.0f * dt_ / (dy_ * dy_);

        if ((a > 0.5f) || (b > 1.0f))
        {
            std::cout << "Stability not ensured!" << std::endl;
            std::cout << "a" << a << std::endl;
            std::cout << "b" << b << std::endl;
        }
    }
};

class WaveEquationStepper
{
private:
    Matrix<float> m0_;
    Matrix<float> m1_;
    Matrix<float> m2_;
    const WaveEquationParams params_;
    size_t num_rows_;
    size_t num_cols_;
    int it_;

public:
    WaveEquationStepper() = delete;
    WaveEquationStepper(const size_t num_rows, const size_t num_cols, const WaveEquationParams& params)
        : m0_{num_rows, num_cols},
          m1_{num_rows, num_cols},
          m2_{num_rows, num_cols},
          params_{params},
          num_rows_{num_rows},
          num_cols_{num_cols},
          it_{0}
    {
        m0_.fill(0.0f);
        m1_.fill(0.0f);
        m2_.fill(0.0f);

        const size_t r_c = num_rows_ / 2;
        const size_t c_c = num_cols_ / 2;

        const size_t cc = 5;
        assert(r_c > cc);
        assert(c_c > cc);

        for (size_t r = (r_c - cc); r < (r_c + cc); r++)
        {
            for (size_t c = (c_c - cc); c < (c_c + cc); c++)
            {
                m0_(r, c) = 0.1f;
            }
        }
        // m0_(2, 2) = 0.1f;
        // m0_(3, 3) = 0.1f;
    }

    void run()
    {
        const float a = params_.a;
        const float b = params_.b;

        const float damping = 0.01f;

        MatrixView<float> prev;
        MatrixView<float> curr;
        MatrixView<float> next;

        if (it_ == 0)
        {
            prev = m0_.view();
            curr = m1_.view();
            next = m2_.view();
        }
        else if (it_ == 1)
        {
            prev = m1_.view();
            curr = m2_.view();
            next = m0_.view();
        }
        else
        {
            prev = m2_.view();
            curr = m0_.view();
            next = m1_.view();
        }

        for (size_t r = 1; r < (num_rows_ - 1); r++)
        {
            for (size_t c = 1; c < (num_cols_ - 1); c++)
            {
                next(r, c) = -damping * (curr(r, c) - prev(r, c)) + 2.0f * curr(r, c) - prev(r, c) +
                             a * (curr(r + 1U, c) - 2.0f * curr(r, c) + curr(r - 1U, c)) +
                             b * (curr(r, c + 1U) - 2.0f * curr(r, c) + curr(r, c - 1U));
            }
        }

        it_++;
        if (it_ == 3)
        {
            it_ = 0;
        }
    }

    MatrixConstView<float> getCurr()
    {
        MatrixConstView<float> curr;

        if (it_ == 0)
        {
            curr = m1_.constView();
        }
        else if (it_ == 1)
        {
            curr = m2_.constView();
        }
        else
        {
            curr = m0_.constView();
        }

        return curr;
    }
};

float getMax(const MatrixConstView<float>& m)
{
    float min_val = std::fabs(m(0, 0));

    for (size_t r = 0; r < m.numRows(); r++)
    {
        for (size_t c = 0; c < m.numCols(); c++)
        {
            if (std::fabs(m(r, c)) > min_val)
            {
                min_val = std::fabs(m(r, c));
            }
        }
    }

    return min_val;
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    const size_t num_rows = 200, num_cols = 200, num_its = 2000;
    const float c = 2.0f, dx = 0.005f, dy = 0.005f, dt = 0.000001f;

    const Vector<float> xv = linspaceFromStartPointIncAndCount<float>(0.0f, dx, num_cols);
    const Vector<float> yv = linspaceFromStartPointIncAndCount<float>(0.0f, dy, num_rows);

    Matrix<float> x, y;
    std::tie(x, y) = meshGrid(xv, yv);

    const WaveEquationParams params{c, dx, dy, dt};

    WaveEquationStepper stepper{num_rows, num_cols, params};

    setActiveView("p_view_0");
    clearView();
    // globalIllumination({1.0, 1.0, 1.0});

    const double z_max = 0.05;
    axis({0.0, 0.0, -z_max}, {1.0, 1.0, z_max});
    viewAngles(-14, 40);

    const VectorConstView<float> x_vec(x.data(), x.numElements()), y_vec(y.data(), y.numElements());

    for (size_t k = 0; k < num_its; k++)
    {
        stepper.run();
        const MatrixConstView<float> z = stepper.getCurr();
        const VectorConstView<float> z_vec(z.data(), z.numElements());

        const float z_max = getMax(z);

        if (k < 500)
        {
            surf(x.constView(),
                 y.constView(),
                 z,
                 properties::ColorMap::JET_BRIGHT,
                 properties::EdgeColor::NONE,
                 properties::INTERPOLATE_COLORMAP);
        }
        else if (k < 700)
        {
            surf(x.constView(), y.constView(), z, properties::FaceColor::NONE, properties::EdgeColor::BLACK);
        }
        else
        {
            scatter3(x_vec,
                     y_vec,
                     z_vec,
                     properties::ScatterStyle::DISC,
                     properties::PointSize(5.0f),
                     properties::ColorMap::VIRIDIS,
                     properties::DistanceFrom::z(0.0f, 0.0f, z_max));
        }

        // std::cin.ignore();
        // usleep(50 * 1000);
        clearViewOnUpdate();
    }
}

}  // namespace wave_equation

#endif  // DEMOS_TESTS_WAVE_EQUATION_H_
