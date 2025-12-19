#ifndef DEMOS_TESTS_IMU_H_
#define DEMOS_TESTS_IMU_H_

#include <Box2D/Box2D.h>
#include <Box2D/Particle/b2Particle.h>

#include <fstream>
#include <nlohmann/json.hpp>
#include <tuple>

#include "lumos/plotting/duoplot.h"

using namespace lumos;

namespace imu
{

properties::Transform operator*(const properties::Transform& t0, const properties::Transform& t1)
{
    properties::Transform res;

    res.rotation = t1.rotation * t1.scale * t0.rotation * t0.scale;
    res.translation = t1.rotation * t1.scale * t0.translation + t1.translation;
    res.scale = diagFixedSizeMatrix<double>({1.0, 1.0, 1.0});

    // Pt0 = R0 * S0 * p + t0
    // Pt1 = (R1 * S1 * R0 * S0 * p) + (R1 * S1 * t0) + (t1)

    return res;
}

struct PointsAndIndices
{
    Vector<Point3f> points;
    Vector<IndexTriplet> indices;
};

PointsAndIndices generateCylinder(const size_t num_circle_points)
{
    const size_t num_vertices = 2U * num_circle_points;
    const size_t num_triangles = 2U * num_circle_points;
    const float delta_angle = M_PI * 2.0f / static_cast<float>(num_circle_points);

    PointsAndIndices pts_indices{};

    pts_indices.points = Vector<Point3f>{num_vertices};
    pts_indices.indices = Vector<IndexTriplet>{num_triangles};

    pts_indices.indices.fill(IndexTriplet(0, 0, 0));

    size_t idx = 0U;
    size_t indices_idx = 0U;

    float theta = 0.0f;

    for (size_t k = 0; k < num_circle_points; k++)
    {
        pts_indices.points(idx) = {std::cos(theta), std::sin(theta), 1.0f};
        pts_indices.points(idx + num_circle_points) = {std::cos(theta), std::sin(theta), -1.0f};

        const size_t rounded_idx = (idx + 1U) % num_circle_points;

        pts_indices.indices(indices_idx) = IndexTriplet(idx, rounded_idx, idx + num_circle_points);
        pts_indices.indices(indices_idx + 1) =
            IndexTriplet(rounded_idx, idx + num_circle_points, rounded_idx + num_circle_points);

        idx++;
        indices_idx += 2;

        theta += delta_angle;
    }

    return pts_indices;
}

PointsAndIndices generateCone(const size_t num_circle_points)
{
    const size_t num_vertices = num_circle_points + 1;
    const size_t num_triangles = num_circle_points;
    const float delta_angle = M_PI * 2.0f / static_cast<float>(num_circle_points);

    PointsAndIndices pts_indices{};

    pts_indices.points = Vector<Point3f>{num_vertices};
    pts_indices.indices = Vector<IndexTriplet>{num_triangles};

    pts_indices.points(num_vertices - 1U) = {0.0f, 0.0f, 1.0f};

    size_t idx = 0U;
    float theta = 0.0f;

    for (size_t k = 0; k < num_circle_points; k++)
    {
        pts_indices.points(idx) = {std::cos(theta), std::sin(theta), 0.0f};

        const size_t rounded_idx = (idx + 1U) % num_circle_points;

        pts_indices.indices(idx) = IndexTriplet(idx, rounded_idx, num_vertices - 1U);

        theta += delta_angle;

        idx++;
    }

    return pts_indices;
}

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

std::tuple<Vector<float>, Vector<float>, Vector<float>> unzipVectors(const Vector<Point3f>& pts)
{
    Vector<float> x{pts.size()}, y{pts.size()}, z{pts.size()};

    for (size_t k = 0; k < pts.size(); k++)
    {
        x(k) = pts(k).x;
        y(k) = pts(k).y;
        z(k) = pts(k).z;
    }

    return std::make_tuple(x, y, z);
}

class ImuVisualizer
{
private:
    properties::Transform t_cyl_x;
    properties::Transform t_cone_x;

    properties::Transform t_cyl_y;
    properties::Transform t_cone_y;

    properties::Transform t_cyl_z;
    properties::Transform t_cone_z;

    properties::Transform t_cube;

public:
    ImuVisualizer() = delete;
    ImuVisualizer(const size_t num_circle_points, const properties::Transform& transform)
    {
        const auto cube_pts_indices = generateCube();
        const auto cylinder_pts_indices = generateCylinder(num_circle_points);
        const auto cone_pts_indices = generateCone(num_circle_points);

        const auto unit_mat = unitMatrix<double>(3, 3);

        t_cyl_x = properties::Transform{
            diagMatrix<double>({0.1, 0.1, 1.0}), rotationMatrixY<double>(M_PI / 2.0), {2.0, 0.0, 0.0}};
        t_cone_x = properties::Transform{
            diagMatrix<double>({0.15, 0.15, 1.0}), rotationMatrixY<double>(M_PI / 2.0), {3.0, 0.0, 0.0}};

        t_cyl_y = properties::Transform{
            diagMatrix<double>({0.1, 0.1, 1.0}), rotationMatrixX<double>(M_PI / 2.0), {0.0, 1.0, 0.0}};
        t_cone_y = properties::Transform{
            diagMatrix<double>({0.15, 0.15, 1.0}), rotationMatrixX<double>(-M_PI / 2.0), {0.0, 2.0, 0}};

        t_cyl_z =
            properties::Transform{diagMatrix<double>({0.1, 0.1, 1.0}), rotationMatrixX<double>(0.0), {0.0, 0.0, 1.0}};
        t_cone_z =
            properties::Transform{diagMatrix<double>({0.15, 0.15, 1.0}), rotationMatrixX<double>(0.0), {0.0, 0.0, 2.0}};

        t_cube = properties::Transform{diagMatrix<double>({2.0, 1.0, 0.5}), unit_mat, {0, 0, 0}};

        const properties::FaceColor x_color{214, 28, 95};
        const properties::FaceColor y_color{0, 168, 136};
        const properties::FaceColor z_color{0, 152, 205};

        const properties::FaceColor cube_color{242, 152, 124};

        drawMesh(cube_pts_indices.points,
                 cube_pts_indices.indices,
                 cube_color,
                 properties::EdgeColor::NONE,
                 properties::ID0,
                 t_cube);

        // X arrow
        drawMesh(cylinder_pts_indices.points,
                 cylinder_pts_indices.indices,
                 x_color,
                 properties::EdgeColor::NONE,
                 properties::ID1,
                 t_cyl_x);
        drawMesh(cone_pts_indices.points,
                 cone_pts_indices.indices,
                 x_color,
                 properties::EdgeColor::NONE,
                 properties::ID2,
                 t_cone_x);

        // Y arrow
        drawMesh(cylinder_pts_indices.points,
                 cylinder_pts_indices.indices,
                 y_color,
                 properties::EdgeColor::NONE,
                 properties::ID3,
                 t_cyl_y);
        drawMesh(cone_pts_indices.points,
                 cone_pts_indices.indices,
                 y_color,
                 properties::EdgeColor::NONE,
                 properties::ID4,
                 t_cone_y);

        // Z arrow
        drawMesh(cylinder_pts_indices.points,
                 cylinder_pts_indices.indices,
                 z_color,
                 properties::EdgeColor::NONE,
                 properties::ID5,
                 t_cyl_z);
        drawMesh(cone_pts_indices.points,
                 cone_pts_indices.indices,
                 z_color,
                 properties::EdgeColor::NONE,
                 properties::ID6,
                 t_cone_z);
    }

    void visualize(const properties::Transform& new_transform)
    {
        const auto cube_transformed = t_cube * new_transform;

        const auto t_cyl_x_transformed = t_cyl_x * new_transform;
        const auto t_cone_x_transformed = t_cone_x * new_transform;

        const auto t_cyl_y_transformed = t_cyl_y * new_transform;
        const auto t_cone_y_transformed = t_cone_y * new_transform;

        const auto t_cyl_z_transformed = t_cyl_z * new_transform;
        const auto t_cone_z_transformed = t_cone_z * new_transform;
        setTransform(properties::ID0, cube_transformed.scale, cube_transformed.rotation, cube_transformed.translation);

        setTransform(
            properties::ID1, t_cyl_x_transformed.scale, t_cyl_x_transformed.rotation, t_cyl_x_transformed.translation);
        setTransform(properties::ID2,
                     t_cone_x_transformed.scale,
                     t_cone_x_transformed.rotation,
                     t_cone_x_transformed.translation);

        setTransform(
            properties::ID3, t_cyl_y_transformed.scale, t_cyl_y_transformed.rotation, t_cyl_y_transformed.translation);
        setTransform(properties::ID4,
                     t_cone_y_transformed.scale,
                     t_cone_y_transformed.rotation,
                     t_cone_y_transformed.translation);

        setTransform(
            properties::ID5, t_cyl_z_transformed.scale, t_cyl_z_transformed.rotation, t_cyl_z_transformed.translation);
        setTransform(properties::ID6,
                     t_cone_z_transformed.scale,
                     t_cone_z_transformed.rotation,
                     t_cone_z_transformed.translation);
    }
};

void testBasic()
{
    const auto cube_pts_indices = generateCube();
    const auto cylinder_pts_indices = generateCylinder(20U);
    const auto cone_pts_indices = generateCone(20U);

    const auto [xcube, ycube, zcube] = unzipVectors(cube_pts_indices.points);
    const auto [xcyl, ycyl, zcyl] = unzipVectors(cylinder_pts_indices.points);
    const auto [xcone, ycone, zcone] = unzipVectors(cone_pts_indices.points);

    const std::string project_file_path = "../../project_files/imu.duoplot";

    openProjectFile(project_file_path);

    setActiveView("raw");
    clearView();
    axesSquare();
    waitForFlush();
    axis({-4.0, -4.0, -4.0}, {4.0, 4.0, 4.0});
    viewAngles(-38.0, 32.0);
    globalIllumination({2.0, 2.0, 2.0});

    ImuVisualizer visualizer_raw{
        20U, properties::Transform{diagMatrix<double>({1.0, 1.0, 1.0}), rotationMatrixZ<double>(0), {0, 0, 0}}};

    setActiveView("filtered");
    clearView();
    axesSquare();
    waitForFlush();
    axis({-4.0, -4.0, -4.0}, {4.0, 4.0, 4.0});
    viewAngles(-38.0, 32.0);
    globalIllumination({2.0, 2.0, 2.0});

    ImuVisualizer visualizer{
        20U, properties::Transform{diagMatrix<double>({1.0, 1.0, 1.0}), rotationMatrixZ<double>(0), {0, 0, 0}}};

    setActiveView("rt");
    clearView();
    waitForFlush();
    setProperties(properties::ID0, properties::Color::RED);
    setProperties(properties::ID1, properties::Color(127, 0, 136));
    axis({0.0, -1.1}, {1.0, 1.1});
    viewAngles(-180.0, -90.0);

    double t = 0.0;

    const auto f_noise = []() -> double { return 2.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5); };

    for (size_t k = 0; k < 1000; k++)
    {
        const double theta_x = std::sin(t * 20.0);
        const double theta_y = std::sin(t * 10.0);
        const double theta_z = std::sin(t);

        const double theta_raw_x = theta_x + f_noise() * 0.15;
        const double theta_raw_y = theta_y + f_noise() * 0.15;
        const double theta_raw_z = theta_z + f_noise() * 0.15;

        const double t_x = f_noise() * 0.05;
        const double t_y = f_noise() * 0.05;
        const double t_z = f_noise() * 0.05;

        const auto r_mat =
            rotationMatrixZ<double>(theta_z) * rotationMatrixY<double>(theta_y) * rotationMatrixX<double>(theta_x);
        const auto r_mat_noise = rotationMatrixZ<double>(theta_raw_z) * rotationMatrixY<double>(theta_raw_y) *
                                 rotationMatrixX<double>(theta_raw_x);
        const properties::Transform tr{diagMatrix<double>({1.0, 1.0, 1.0}), r_mat, {0, 0, 0}};
        const properties::Transform tr_noise{diagMatrix<double>({1.0, 1.0, 1.0}), r_mat_noise, {t_x, t_y, t_z}};

        setActiveView("filtered");
        visualizer.visualize(tr);

        setActiveView("raw");
        visualizer_raw.visualize(tr_noise);

        setActiveView("rt");
        realTimePlot(0.01, theta_x, properties::ID0);
        realTimePlot(0.01, theta_raw_x, properties::ID1);

        flushMultipleElements("filtered", "raw", "rt");
        t += 0.005;
    }
}

}  // namespace imu

#endif  // DEMOS_TESTS_IMU_H_
