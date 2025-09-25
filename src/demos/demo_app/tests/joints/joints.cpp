#include "tests/joints/joints.h"

namespace joints
{

void drawGrid()
{
    const float grid_step = 2.5;
    const float grid_size = 200.0;

    std::vector<float> x_vec, y_vec;

    float x_pos = -grid_size / 2.0f, y_pos = -grid_size / 2.0f;

    const size_t n_its = static_cast<float>(grid_size / grid_step);

    for (size_t k = 0; k < n_its; k++)
    {
        // Left point
        x_vec.push_back(x_pos);
        y_vec.push_back(y_pos);

        // Right point
        if ((k % 2) == 0)
        {
            x_pos += grid_size;
        }
        else
        {
            x_pos -= grid_size;
        }

        x_vec.push_back(x_pos);
        y_vec.push_back(y_pos);

        y_pos += grid_step;
    }

    x_pos = -grid_size / 2.0f;
    y_pos = -grid_size / 2.0f;

    for (size_t k = 0; k < n_its; k++)
    {
        // Left point
        x_vec.push_back(x_pos);
        y_vec.push_back(y_pos);

        // Right point
        if ((k % 2) == 0)
        {
            y_pos += grid_size;
        }
        else
        {
            y_pos -= grid_size;
        }

        x_vec.push_back(x_pos);
        y_vec.push_back(y_pos);

        x_pos += grid_step;
    }

    Vector<float> x{x_vec}, y{y_vec};

    plot(x, y, properties::Color(180, 180, 180), properties::ZOffset(-1.0f), properties::LineWidth(1.0f));
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/joints.duoplot";

    openProjectFile(project_file_path);

    setActiveView("_");
    clearView();
    waitForFlush();
    axesSquare();
    disableScaleOnRotation();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    drawGrid();

    Joints joints{};

    for (size_t k = 0; k < 2000; k++)
    {
        joints.step(k);
        flushCurrentElement();
        usleep(1000 * 10);
        // std::cout << k << std::endl;
        // deletePlotObject(properties::ID250);
        // deletePlotObject(properties::ID249);
    }
}

}  // namespace joints
