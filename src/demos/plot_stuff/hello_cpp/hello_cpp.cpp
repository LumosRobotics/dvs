#include "duoplot/duoplot.h"

using namespace duoplot;

int main()
{
    const int num_rows = 200, num_cols = 200;
    Matrix<double> x(num_rows, num_cols), y(num_rows, num_cols), z(num_rows, num_cols);

    const double inc = 0.1;

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double rd = static_cast<double>(r - 100) * inc;
            const double cd = static_cast<double>(c - 100) * inc;
            x(r, c) = c * inc;
            y(r, c) = r * inc;
            const float r_val = std::sqrt(rd * rd + cd * cd);
            z(r, c) = std::sin(r_val * 1.5);
        }
    }

    setActiveView("hello_cpp");
    clearView();
    viewAngles(-135, 17);
    setTitle("Hello from C++!");
    surf(x, y, z, properties::EdgeColor::NONE, properties::ColorMap::JET_SOFT);
}