#include "tests/boxes/boxes.h"

using namespace reactphysics3d;

namespace boxes
{

struct PointsAndIndices
{
    Vector<Point3f> points;
    Vector<IndexTriplet> indices;
};

namespace
{
inline Vec3d calculateColormapPastel2(double value)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value >= 1.0)
    {
        value = 0.99999;
    }

    double full_range_value = value * 20.0;
    double integer_part = floor(full_range_value);
    double fraction_part = full_range_value - integer_part;

    double r = 0.0, g = 0.0, b = 0.0;

    switch (static_cast<int>(integer_part))
    {
        case 0:
            r = 0.6471 - fraction_part * 0.10980000000000001;
            g = 0.7804 - fraction_part * 0.050999999999999934;
            b = 0.9137 - fraction_part * 0.04699999999999993;
            break;
        case 1:
            r = 0.5373 - fraction_part * 0.13340000000000002;
            g = 0.7294 - fraction_part * 0.12550000000000006;
            b = 0.8667 - fraction_part * 0.20000000000000007;
            break;
        case 2:
            r = 0.4039 + fraction_part * 0.25490000000000007;
            g = 0.6039 + fraction_part * 0.251;
            b = 0.6667 + fraction_part * 0.15290000000000004;
            break;
        case 3:
            r = 0.6588 - fraction_part * 0.09800000000000009;
            g = 0.8549 - fraction_part * 0.04310000000000003;
            b = 0.8196 - fraction_part * 0.1059;
            break;
        case 4:
            r = 0.5608 - fraction_part * 0.031399999999999983;
            g = 0.8118 - fraction_part * 0.07850000000000001;
            b = 0.7137 - fraction_part * 0.04700000000000004;
            break;
        case 5:
            r = 0.5294 + fraction_part * 0.2118;
            g = 0.7333 + fraction_part * 0.13340000000000007;
            b = 0.6667 + fraction_part * 0.023500000000000076;
            break;
        case 6:
            r = 0.7412 - fraction_part * 0.0039000000000000146;
            g = 0.8667 - fraction_part * 0.0706;
            b = 0.6902 + fraction_part * 0.015699999999999936;
            break;
        case 7:
            r = 0.7373 + fraction_part * 0.23920000000000008;
            g = 0.7961 + fraction_part * 0.14900000000000002;
            b = 0.7059 - fraction_part * 0.1412;
            break;
        case 8:
            r = 0.9765 - fraction_part * 0.41180000000000005;
            g = 0.9451 - fraction_part * 0.031400000000000095;
            b = 0.5647 + fraction_part * 0.007800000000000029;
            break;
        case 9:
            r = 0.5647 + fraction_part * 0.38040000000000007;
            g = 0.9137 + fraction_part * 0.015700000000000047;
            b = 0.5725 - fraction_part * 0.0039000000000000146;
            break;
        case 10:
            r = 0.9451 + fraction_part * 0.011799999999999922;
            g = 0.9294 + fraction_part * 0.015700000000000047;
            b = 0.5686 + fraction_part * 0.13339999999999996;
            break;
        case 11:
            r = 0.9569 - fraction_part * 0.15689999999999993;
            g = 0.9451 - fraction_part * 0.27060000000000006;
            b = 0.702 - fraction_part * 0.13339999999999996;
            break;
        case 12:
            r = 0.8 + fraction_part * 0.1842999999999999;
            g = 0.6745 + fraction_part * 0.19610000000000005;
            b = 0.5686 + fraction_part * 0.30200000000000005;
            break;
        case 13:
            r = 0.9843 - fraction_part * 0.007799999999999918;
            g = 0.8706 - fraction_part * 0.1726000000000001;
            b = 0.8706 - fraction_part * 0.13330000000000009;
            break;
        case 14:
            r = 0.9765 - fraction_part * 0.011800000000000033;
            g = 0.698 - fraction_part * 0.11369999999999991;
            b = 0.7373 - fraction_part * 0.1412;
            break;
        case 15:
            r = 0.9647 + fraction_part * 0.015700000000000047;
            g = 0.5843 + fraction_part * 0.06279999999999997;
            b = 0.5961 - fraction_part * 0.06669999999999998;
            break;
        case 16:
            r = 0.9804 - fraction_part * 0.04310000000000003;
            g = 0.6471 + fraction_part * 0.007800000000000029;
            b = 0.5294 + fraction_part * 0.06279999999999997;
            break;
        case 17:
            r = 0.9373 + fraction_part * 0.06269999999999998;
            g = 0.6549 + fraction_part * 0.08629999999999993;
            b = 0.5922 - fraction_part * 0.09019999999999995;
            break;
        case 18:
            r = 1.0 - fraction_part * 0.19610000000000005;
            g = 0.7412 - fraction_part * 0.03920000000000001;
            b = 0.502 + fraction_part * 0.3294;
            break;
        case 19:
            r = 0.8039 - fraction_part * 0.10189999999999999;
            g = 0.702 + fraction_part * 0.0039000000000000146;
            b = 0.8314 + fraction_part * 0.01959999999999995;
            break;
    }

    return Vec3d(r, g, b);
}
}  // namespace

void drawGrid()
{
    const float grid_step = 0.5;
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

    plot(x, y, properties::Color(140, 140, 140), properties::ZOffset(-2.0f), properties::LineWidth(1.0f));
    // TODO: CHange line width for this demo:
    // shader_collection_.plot_2d_shader.uniform_handles.half_line_width.setFloat(line_width_ / 20.0f);
}

PointsAndIndices generateCube(const float size = 1.0f)
{
    PointsAndIndices pts_indices{};

    // clang-format off
    pts_indices.points = Vector<Point3f>{VectorInitializer<Point3f>{
        {size, size, size},
        {size, -size, size},
        {-size, -size, size},
        {-size, size, size},
        {size, size, -size},
        {size, -size, -size},
        {-size, -size, -size},
        {-size, size, -size},
    }};

    pts_indices.indices = Vector<IndexTriplet>{VectorInitializer<IndexTriplet>{
        {0, 2, 1}, // Upper xy
        {0, 3, 2},
        {4, 5, 6}, // Lower xy
        {4, 6, 7},
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

const Vector3 kCubeExtents(0.1, 0.1, 0.1);

Matrix<float> r3dMatToMat(const Matrix3x3& mat)
{
    Matrix<float> m(3, 3);

    for (size_t r = 0; r < 3; r++)
    {
        for (size_t c = 0; c < 3; c++)
        {
            m(r, c) = mat[r][c];
        }
    }

    return m;
}

Matrix3x3 matToR3dMat(const Matrix<float>& mat)
{
    Matrix3x3 m;

    for (size_t r = 0; r < 3; r++)
    {
        for (size_t c = 0; c < 3; c++)
        {
            m[r][c] = mat(r, c);
        }
    }

    return m;
}

RigidBody* placeCubeAt(PhysicsWorld* world,
                       PhysicsCommon& physicsCommon,
                       const Quaternion& orientation,
                       const Vector3& position,
                       const Vector3& size = kCubeExtents)
{
    // Create a rigid body in the world
    BoxShape* boxShape = physicsCommon.createBoxShape(size);

    const Transform transform(position, orientation);
    RigidBody* body = world->createRigidBody(transform);

    Collider* collider_body = body->addCollider(boxShape, Transform{Vector3(0, 0, 0), Quaternion::identity()});
    Material& material = collider_body->getMaterial();

    // Change the bounciness of the collider
    material.setBounciness(0.1);

    // Change the friction coefficient of the collider
    material.setFrictionCoefficient(0.2);

    return body;
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/boxes.duoplot";
    openProjectFile(project_file_path);

    PhysicsCommon physicsCommon;

    PhysicsWorld::WorldSettings settings;
    settings.defaultVelocitySolverNbIterations = 20;
    settings.isSleepingEnabled = false;
    settings.gravity = Vector3(0, 0.0, 0);

    // Create a physics world
    PhysicsWorld* world = physicsCommon.createPhysicsWorld(settings);

    const Vector3 halfExtents(10.0, 0.1, 10.0);
    BoxShape* boxShapeGround = physicsCommon.createBoxShape(halfExtents);

    /*Vector3 ground_position(0, 0, 0);
    Transform ground_transform(ground_position, Quaternion::identity());
    RigidBody* ground_body = world->createRigidBody(ground_transform);
    ground_body->setType(BodyType::STATIC);

    Collider* collider;
    collider = ground_body->addCollider(boxShapeGround, Transform{Vector3(0, 0, 0), Quaternion::identity()});*/

    float timeStep = 1.0f / 600.0f;

    setActiveView("p_view_0");
    clearView();
    globalIllumination({2.0, 2.0, 2.0});
    disableScaleOnRotation();
    axesSquare();
    waitForFlush();

    const float bullet_size = 0.8f;

    PointsAndIndices cube_pts_indices = generateCube();
    PointsAndIndices cube_pts_indices_10 = generateCube(bullet_size);
    const properties::FaceColor cube_color{242, 152, 124};
    properties::Transform cube_transform{
        diagMatrix<double>({0.15, 0.15, 1.0}), rotationMatrixX<double>(0.0), {0.0, 0.0, 2.0}};

    std::vector<RigidBody*> bodies;
    const size_t num_rows = 5;
    const size_t num_cols = 5;
    const size_t num_cubes_z = 7;

    const auto rand_func = []() -> std::uint8_t {
        const std::size_t v = 150U;
        return (rand() % v) + (256U - v);
    };

    const auto col_func = []() -> Vec3<std::uint8_t> {
        const double d = static_cast<double>(rand() % 1001) / 1000.0;

        const Vec3d v = calculateColormapPastel2(d);
        return Vec3<std::uint8_t>{static_cast<std::uint8_t>(v.x * 255.0),
                                  static_cast<std::uint8_t>(v.y * 255.0),
                                  static_cast<std::uint8_t>(v.z * 255.0)};
    };
    size_t idx = 0;
    const float o_offset = 1.0f;
    for (size_t r = 0; r < num_rows; r++)
    {
        const float rf = r;

        for (size_t c = 0; c < num_cols; c++)
        {
            const float cf = c;

            for (size_t z = 0; z < num_cubes_z; z++)
            {
                const float zf = z;
                RigidBody* body = placeCubeAt(world,
                                              physicsCommon,
                                              Quaternion::identity(),
                                              Vector3{1.001f * rf * 2.0f * kCubeExtents.x,
                                                      1.001f * kCubeExtents.y * 2.0f + zf * 2.0f * kCubeExtents.y,
                                                      1.001f * cf * 2.0f * kCubeExtents.z});
                bodies.push_back(body);

                const auto cube_col = col_func();

                drawMesh(cube_pts_indices.points,
                         cube_pts_indices.indices,
                         cube_color,
                         properties::EdgeColor::NONE,
                         properties::FaceColor(cube_col.x, cube_col.y, cube_col.z),
                         static_cast<ItemId>(idx),
                         cube_transform);
                assert(idx < 254);
                idx++;
            }
        }
    }

    const Quaternion bullet_rotation{Quaternion::fromEulerAngles(M_PI / debug_value_reader::getValue<float>("div_x"),
                                                                 M_PI / debug_value_reader::getValue<float>("div_y"),
                                                                 M_PI / debug_value_reader::getValue<float>("div_z"))};

    RigidBody* bullet_body = placeCubeAt(world,
                                         physicsCommon,
                                         bullet_rotation,
                                         Vector3{0.375f, 0.75f, 8.0f},
                                         Vector3(bullet_size, bullet_size, bullet_size));
    bullet_body->setMass(bullet_body->getMass() * 100.0f);

    drawMesh(cube_pts_indices_10.points,
             cube_pts_indices_10.indices,
             cube_color,
             properties::EdgeColor::NONE,
             properties::ID254,
             cube_transform);
    float axis_zoom_scale_ref = 2.0f;
    float axis_zoom_scale = 2.0f;
    axis({-axis_zoom_scale, -axis_zoom_scale, -axis_zoom_scale}, {axis_zoom_scale, axis_zoom_scale, axis_zoom_scale});

    float azimuth = -52.0f;
    float azimuth_ref = -52.0f;
    // axis({-3.0, 0.0, -4.0}, {5.0, 8.0, 4.0});
    // viewAngles(-48, 17);
    viewAngles(azimuth, 14);
    // viewAngles(-73, 43);
    bullet_body->setLinearVelocity(Vector3{0.0, 0.0, -10.0});
    // bullet_body->setAngularVelocity(Vector3{50.0, 50.0, 50.0});

    bool run_simulation = true;
    const auto ps = bullet_body->getTransform().getPosition();
    Vec3f axes_center{ps.x, ps.y, ps.z};
    Vec3f axes_center_filtered = axes_center;
    bool center_on_bullet = true;

    drawGrid();

    // Step the simulation a few steps
    for (int i = 0; i < 1500; i++)
    {
        if (i < 350)
        {
            // axes_center = Vec3f(0.0, 0.0, 0.0);
        }
        else if (i < 500)
        {
            axis_zoom_scale_ref = std::max(1.5f, axis_zoom_scale_ref - 0.01f);
            azimuth_ref += 0.1f;
            const float h = 0.99f;
            azimuth = azimuth_ref * (1.0f - h) + azimuth * h;
            viewAngles(azimuth, 14);
        }
        else if ((i < 700) && ((timeStep - 1.0 / 60000.0) > 0.0f))
        {
            timeStep -= 1.0 / 70000.0;
            axis_zoom_scale_ref = std::max(1.5f, axis_zoom_scale_ref - 0.01f);
            azimuth_ref += 0.1f;
            const float h = 0.99f;
            azimuth = azimuth_ref * (1.0f - h) + azimuth * h;
            viewAngles(azimuth, 14);
        }
        else if (i < 1000)
        {
            // azimuth += 0.1f;
            azimuth_ref += 0.5f;
            const float h = 0.99f;
            azimuth = azimuth_ref * (1.0f - h) + azimuth * h;
            viewAngles(azimuth, 14);
            run_simulation = false;
        }
        else if (i < 1500)
        {
            // azimuth_ref += 0.5f;
            const float h = 0.99f;
            azimuth = azimuth_ref * (1.0f - h) + azimuth * h;
            timeStep = std::min(timeStep + 1.0f / 7000.0f, 1.0f / 60.0f);
            axis_zoom_scale_ref = std::min(2.0f, axis_zoom_scale_ref + 0.01f);
            viewAngles(azimuth, 14);
            center_on_bullet = false;

            run_simulation = true;
        }

        if (run_simulation)
        {
            world->update(timeStep);
        }

        std::vector<PropertySet> props;

        for (size_t k = 0; k < bodies.size(); k++)
        {
            RigidBody* body = bodies[k];

            // Get the updated position of the body
            const Transform& transform = body->getTransform();
            const Vector3& position = transform.getPosition();
            const Matrix<float> rot_mat = r3dMatToMat(transform.getOrientation().getMatrix());

            props.emplace_back(static_cast<ItemId>(k),
                               properties::Transform{
                                   diagMatrix<double>({0.1, 0.1, 0.1}), rot_mat, {position.x, position.z, position.y}});
        }

        const Transform& transform = bullet_body->getTransform();
        const Vector3& position = transform.getPosition();
        if (center_on_bullet)
        {
            axes_center = Vec3f(position.x, position.y, position.z);
        }

        const float h = 0.95f;
        const float h1 = 1.0f - h;
        azimuth = azimuth_ref * (1.0f - h) + azimuth * h;
        axis_zoom_scale = axis_zoom_scale_ref * (1.0f - h) + axis_zoom_scale * h;

        axes_center_filtered.x = axes_center.x * h1 + axes_center_filtered.x * h;
        axes_center_filtered.z = axes_center.z * h1 + axes_center_filtered.z * h;
        axes_center_filtered.y = axes_center.y * h1 + axes_center_filtered.y * h;

        axis({axes_center_filtered.x - axis_zoom_scale,
              axes_center_filtered.z - axis_zoom_scale,
              axes_center_filtered.y - axis_zoom_scale},
             {axes_center_filtered.x + axis_zoom_scale,
              axes_center_filtered.z + axis_zoom_scale,
              axes_center_filtered.y + axis_zoom_scale});
        const Matrix<float> rot_mat_bullet = r3dMatToMat(transform.getOrientation().getMatrix());

        props.emplace_back(
            properties::ID254,
            properties::Transform{
                diagMatrix<double>({1.0, 1.0, 1.0}), rot_mat_bullet, {position.x, position.z, position.y}});

        setProperties(props);
        flushCurrentElement();
        // std::cin.ignore();
        // usleep(1000 * 10);
        // softClearView();
    }

    // waitForFlush();
    // axesSquare();
    // disableScaleOnRotation();
    // setAxesBoxScaleFactor({1.0, 1.0, 1.0});
}

}  // namespace boxes
