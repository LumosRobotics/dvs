#ifndef TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_OBJECT_TRANSFORM_H_
#define TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_OBJECT_TRANSFORM_H_

#include <unistd.h>

#include "lumos/lumos.h"
#include "utils.h"

using namespace lumos;

namespace object_transform
{

struct PointsAndIndices
{
    Vector<Point3f> points;
    Vector<IndexTriplet> indices;
};

PointsAndIndices generateCube()
{
    PointsAndIndices pts_indices{};

    // clang-format off
    pts_indices.points = Vector<Point3f>{VectorInitializer<Point3f>{
        {1.0, 1.0, 1.0},
        {1.0, -1.0, 1.0},
        {-1.0, -1.0, 1.0},
        {-1.0, 1.0, 1.0},
        {1.0, 1.0, -1.0},
        {1.0, -1.0, -1.0},
        {-1.0, -1.0, -1.0},
        {-1.0, 1.0, -1.0},
    }};

    pts_indices.indices = Vector<IndexTriplet>{VectorInitializer<IndexTriplet>{
        {0, 1, 2}, // Upper xy
        {0, 2, 3},
        {4, 6, 5}, // Lower xy
        {4, 7, 6},
        {0, 1, 4}, // Upper yz
        {1, 5, 4},
        {2, 6, 3}, // Lower yz
        {6, 7, 3},
        {0, 7, 3}, // Upper xz
        {0, 4, 7},
        {6, 2, 1}, // Lower xz
        {6, 1, 5},
    }};
    // clang-format on

    return pts_indices;
}

void testBasic2D()
{
    const size_t num_elements = 5;

    Vector<float> x{num_elements}, y{num_elements};

    x(0) = -1.0;
    x(1) = 1.0;
    x(2) = 1.0;
    x(3) = -1.0;
    x(4) = 0.0;

    y(0) = -1.0;
    y(1) = -1.0;
    y(2) = 1.0;
    y(3) = 1.0;
    y(4) = 0.0;

    setActiveView("p_view_0");
    clearView();
    axis({-2.0, -2.0}, {2.0, 2.0});
    plot(x, y, properties::ID0, properties::LineWidth(10.0f));

    usleep(100 * 1000);

    Vec3d translation{0.1, 0.1, 0.0};
    Vec3d scale{1.0, 1.0, 1.0};

    Matrix<double> r_mat = rotationMatrixZ(0.0);

    for (size_t k = 0; k < 50; k++)
    {
        scale.x += 0.01;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(50 * 1000);
    }

    for (size_t k = 0; k < 50; k++)
    {
        translation.x += 0.01;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(50 * 1000);
    }

    float angle = 0.0;

    for (size_t k = 0; k < 50; k++)
    {
        r_mat = rotationMatrixZ(angle);
        angle -= 0.01;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(50 * 1000);
    }

    for (size_t k = 0; k < 50; k++)
    {
        scale.x += 0.01;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(50 * 1000);
    }

    for (size_t k = 0; k < 50; k++)
    {
        translation.x += 0.01;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(50 * 1000);
    }

    for (size_t k = 0; k < 50; k++)
    {
        r_mat = rotationMatrixZ(angle);
        angle -= 0.01;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(50 * 1000);
    }
}

constexpr int delay_ms = 20;

void changeScale(
    Vec3d& scale, const Matrix<double>& r_mat, const Vec3d& translation, const Vec3d& inc, const size_t n_times)
{
    for (size_t k = 0; k < n_times; k++)
    {
        scale = scale + inc;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(delay_ms * 1000);
    }
}

void changeRotation(
    const Vec3d& scale, Matrix<double>& r_mat, const Vec3d& translation, const Vec3d& inc, const size_t n_times)
{
    const Matrix<double> r_inc = rotationMatrixZ(inc.z) * rotationMatrixY(inc.y) * rotationMatrixX(inc.x);

    for (size_t k = 0; k < n_times; k++)
    {
        r_mat = r_mat * r_inc;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(delay_ms * 1000);
    }
}

void changeTranslation(
    const Vec3d& scale, const Matrix<double>& r_mat, Vec3d& translation, const Vec3d& inc, const size_t n_times)
{
    const Matrix<double> r_inc = rotationMatrixZ(inc.z) * rotationMatrixY(inc.y) * rotationMatrixX(inc.x);

    for (size_t k = 0; k < n_times; k++)
    {
        translation = translation + inc;

        setTransform(properties::ID0, diagMatrix<double>(scale), r_mat, translation);
        usleep(delay_ms * 1000);
    }
}

void testBasic3D()
{
    const size_t num_elements = 5;

    const auto cube_pts_indices = generateCube();

    constexpr double axis_limit = 10.0;

    setActiveView("p_view_0");
    clearView();
    axis({-axis_limit, -axis_limit, -axis_limit}, {axis_limit, axis_limit, axis_limit});
    drawMesh(cube_pts_indices.points, cube_pts_indices.indices, properties::FaceColor{242, 152, 124}, properties::ID0);

    Vec3d translation{0.0, 0.0, 0.0};
    Vec3d scale{1.0, 1.0, 1.0};
    Matrix<double> r_mat = rotationMatrixZ(0.0);

    changeScale(scale, r_mat, translation, Vec3d{0.1, 0.0, 0.0}, 50);
    changeScale(scale, r_mat, translation, Vec3d{0.0, 0.1, 0.0}, 50);
    changeScale(scale, r_mat, translation, Vec3d{0.0, 0.0, 0.1}, 50);

    changeRotation(scale, r_mat, translation, Vec3d{0.01, 0.0, 0.0}, 50);
    changeRotation(scale, r_mat, translation, Vec3d{0.0, 0.01, 0.0}, 50);
    changeRotation(scale, r_mat, translation, Vec3d{0.0, 0.0, 0.01}, 50);

    changeTranslation(scale, r_mat, translation, Vec3d{0.05, 0.0, 0.0}, 50);
    changeTranslation(scale, r_mat, translation, Vec3d{0.0, 0.05, 0.0}, 50);
    changeTranslation(scale, r_mat, translation, Vec3d{0.0, 0.0, 0.05}, 50);

    changeRotation(scale, r_mat, translation, Vec3d{0.0, 0.0, -0.01}, 50);
    changeRotation(scale, r_mat, translation, Vec3d{0.0, -0.01, 0.0}, 50);
    changeRotation(scale, r_mat, translation, Vec3d{-0.01, 0.0, 0.0}, 50);
}

void addTests()
{
    addTest("cpp", "object_transform", "basic_2d", object_transform::testBasic2D);
    addTest("cpp", "object_transform", "basic_3d", object_transform::testBasic3D);
}

}  // namespace object_transform

#endif  // TEST_APPLICATIONS_ADVANCED_TESTS_CPP_TESTS_OBJECT_TRANSFORM_H_
