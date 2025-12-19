#ifndef DEMOS_TESTS_JOINTS_JOINTS_H_
#define DEMOS_TESTS_JOINTS_JOINTS_H_

#include <Box2D/Box2D.h>

#include <array>

#include "debug_value_reader.h"
#include "lumos/plotting/duoplot.h"
// #include <box2d/b2_prismatic_joint.h>
// src/externals/box2d/include/box2d/b2_prismatic_joint.h

using namespace lumos;

namespace joints
{

inline Vec3d calculateColormapRainbow(double value)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value >= 1.0)
    {
        value = 0.99999;
    }

    double full_range_value = value * 5.0;
    double integer_part = floor(full_range_value);
    double fraction_part = full_range_value - integer_part;

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

    return Vec3d(r, g, b);
}

inline Vec3d calculateColormapPastel(double value)
{
    if (value < 0.0)
    {
        value = 0.0;
    }
    else if (value >= 1.0)
    {
        value = 0.99999;
    }

    double full_range_value = value * 5.0;
    double integer_part = floor(full_range_value);
    double fraction_part = full_range_value - integer_part;

    double r = 0.0, g = 0.0, b = 0.0;

    switch (static_cast<int>(integer_part))
    {
        case 0:
            r = 1.0;
            g = 0.6039215686274509 + fraction_part * 0.11372549019607847;
            b = 0.6352941176470588 + fraction_part * 0.06274509803921569;
            break;
        case 1:
            r = 1.0;
            g = 0.7176470588235294 + fraction_part * 0.13725490196078427;
            b = 0.6980392156862745 + fraction_part * 0.06274509803921569;
            break;
        case 2:
            r = 1.0 - fraction_part * 0.11372549019607847;
            g = 0.8549019607843137 + fraction_part * 0.0862745098039216;
            b = 0.7607843137254902 + fraction_part * 0.04313725490196085;
            break;
        case 3:
            r = 0.8862745098039215 - fraction_part * 0.180392156862745;
            g = 0.9411764705882353 - fraction_part * 0.02352941176470591;
            b = 0.803921568627451 + fraction_part * 0.04313725490196074;
            break;
        case 4:
            r = 0.7058823529411765 + fraction_part * 0.07450980392156858;
            g = 0.9176470588235294 - fraction_part * 0.1098039215686274;
            b = 0.8470588235294118 + fraction_part * 0.06666666666666665;
            break;
    }

    return Vec3d(r, g, b);
}

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

inline ImageRGBA<std::uint8_t> createRectangle(
    const size_t width, const size_t height, const std::uint8_t red, const std::uint8_t green, const std::uint8_t blue)
{
    ImageRGBA<std::uint8_t> rectangle{height, width};

    rectangle.fill(red, green, blue, 255);
    const size_t black_border_width = 2;

    for (size_t r = 0; r < height; r++)
    {
        for (size_t c = 0; c < width; c++)
        {
            if ((r < black_border_width) || (r > (height - black_border_width)) || (c < black_border_width) ||
                (c > (width - black_border_width)))
            {
                rectangle(r, c, 0) = 0;
                rectangle(r, c, 1) = 0;
                rectangle(r, c, 2) = 0;
            }
        }
    }

    return rectangle;
}

inline ImageRGBA<std::uint8_t> createCircle(const size_t radius,
                                            const std::uint8_t red,
                                            const std::uint8_t green,
                                            const std::uint8_t blue)
{
    assert((radius % 2 == 0));
    ImageRGBA<std::uint8_t> circle{radius * 2, radius * 2};

    const size_t num_rows = circle.height();
    const size_t num_cols = circle.width();

    circle.fill(0, 0, 0, 255);

    const double edge_threshold_pixels = 3.0;
    const double blend_threshold_pixels = 1.0;

    const double input_radius = static_cast<double>(radius);
    const double inner_edge_radius = input_radius - edge_threshold_pixels;

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            const double x = static_cast<double>(c) - static_cast<double>(num_cols) / 2.0;
            const double y = static_cast<double>(r) - static_cast<double>(num_rows) / 2.0;

            const double current_radius = std::sqrt(x * x + y * y);

            if (current_radius < input_radius)
            {
                if (current_radius > (input_radius - edge_threshold_pixels))
                {
                    // In black silhouette
                    circle(r, c, 0) = 0;
                    circle(r, c, 1) = 0;
                    circle(r, c, 2) = 0;
                    const double dist_from_edge = input_radius - current_radius;

                    if (dist_from_edge < blend_threshold_pixels)
                    {
                        const double alpha = dist_from_edge / blend_threshold_pixels;
                        circle(r, c, 3) = static_cast<std::uint8_t>(255.0 * alpha);
                    }
                    else
                    {
                        circle(r, c, 3) = 255;
                    }
                }
                else
                {
                    circle(r, c, 0) = red;
                    circle(r, c, 1) = green;
                    circle(r, c, 2) = blue;
                    circle(r, c, 3) = 255;
                }
            }
            else
            {
                circle(r, c, 3) = 0;
            }

            // Make transition between color and silhouette
            const double dist_from_edge = inner_edge_radius - current_radius;
            if (std::abs(dist_from_edge) < blend_threshold_pixels)
            {
                const double blend_factor = (dist_from_edge + blend_threshold_pixels) / (2.0 * blend_threshold_pixels);

                circle(r, c, 0) = static_cast<std::uint8_t>(blend_factor * static_cast<double>(red));
                circle(r, c, 1) = static_cast<std::uint8_t>(blend_factor * static_cast<double>(green));
                circle(r, c, 2) = static_cast<std::uint8_t>(blend_factor * static_cast<double>(blue));
            }
            // if(input_radius - current_radius)
        }
    }

    const size_t offset_r = 40U;
    const size_t offset_c = 40U;

    for (size_t r = 0; r < 10; r++)
    {
        for (size_t c = 0; c < 10; c++)
        {
            const double x = static_cast<double>(c) - 5.0;
            const double y = static_cast<double>(r) - 5.0;
            const double current_radius = std::sqrt(x * x + y * y);
            if (current_radius < 5.0)
            {
                circle(r + offset_r, c + offset_c, 0) = 0;
                circle(r + offset_r, c + offset_c, 1) = 0;
                circle(r + offset_r, c + offset_c, 2) = 0;
            }
        }
    }

    return circle;
}

inline properties::Transform getTransformExt(const ImageRGBAConstView<uint8_t>& img,
                                             const Vec3d& size,
                                             const double phi,
                                             const double x_offset,
                                             const double y_offset)
{
    const double image_width = img.width();
    const double image_height = img.height();

    const double max_dim_size = std::max(size.x, size.y);
    const double max_dim_img = std::max(img.width(), img.height());
    const Vec3d scale_vec{max_dim_size / max_dim_img, max_dim_size / max_dim_img, 1.0};

    const auto scale_mat = diagMatrix<double>(scale_vec);
    const Vec3d center_of_rotation = Vec3d{image_width / 2.0, image_height / 2.0, 0.0}.elementWiseMultiply(scale_vec);

    const Matrix<double> rot_mat = rotationMatrixZ<double>(phi);
    const Vec3<double> translation_vec =
        -rotationMatrixZ<double>(-phi) * center_of_rotation + Vec3d{x_offset, y_offset, 0.0};

    return properties::Transform{scale_mat, rot_mat, translation_vec};
}

struct Shape
{
    float x_pos;
    float y_pos;
    float angle;
    float width;
    float height;

    ImageRGBA<std::uint8_t> shape_img;

    b2Body* body_handle = nullptr;
    ItemId id;

    float scale_factor = 100.0f;

    Shape() = default;
    Shape(b2Body* body_handle_,
          const float width_,
          const float height_,
          const ItemId& id_,
          const std::uint8_t red,
          const std::uint8_t green,
          const std::uint8_t blue)
        : width{width_}, height{height_}, body_handle{body_handle_}, id{id_}
    {
        b2Vec2 pos = body_handle_->GetPosition();
        x_pos = pos.x;
        y_pos = pos.y;

        angle = body_handle_->GetAngle();
        shape_img = createRectangle(width * scale_factor, height * scale_factor, red, green, blue);
        imShow(shape_img, id);
    }

    float getXScale() const
    {
        return width / 2.0;
    }

    float getYScale() const
    {
        return height / 2.0;
    }

    properties::Transform getTransform() const
    {
        const properties::Transform transform = getTransformExt(shape_img.constView(),
                                                                {width, height, 1.0},
                                                                -body_handle->GetAngle(),
                                                                body_handle->GetPosition().x,
                                                                body_handle->GetPosition().y);
        return transform;
    }
};

class Joints
{
public:
    static constexpr std::size_t kNumSquares{10U};

private:
    b2Vec2 gravity_{};
    b2World world_;
    b2PolygonShape ground_box_{};

    float time_step = 1.0f / 60.0f;
    int32 velocity_iterations = 6;
    int32 position_iterations = 2;

    ItemId id_counter_;

    b2Body* createStaticCollisionlessBody(const Vec2d& position, const Vec2d& size)
    {
        b2BodyDef body_def;
        body_def.type = b2_staticBody;
        body_def.position.Set(position.x, position.y);

        b2Body* body = world_.CreateBody(&body_def);

        return body;
    }

    b2Body* createStaticBody(const Vec2d& position, const Vec2d& size, const double angle)
    {
        b2BodyDef body_def;
        body_def.type = b2_staticBody;
        body_def.position.Set(position.x, position.y);
        body_def.angle = angle;

        b2PolygonShape body_shape{};
        body_shape.SetAsBox(size.x, size.y);

        b2Body* body = world_.CreateBody(&body_def);

        body->CreateFixture(&body_shape, 0.0f);

        return body;
    }

    Shape createStaticBoxShape(const Vec2d& position, const Vec2d& size, const double angle, const Vec3d& color)
    {
        b2BodyDef body_def;
        body_def.type = b2_staticBody;
        body_def.position.Set(position.x, position.y);
        body_def.angle = angle;

        b2PolygonShape body_shape{};
        body_shape.SetAsBox(size.x / 2.0, size.y / 2.0);

        b2Body* body = world_.CreateBody(&body_def);

        const std::uint8_t red = static_cast<std::uint8_t>(color.x * 255.0);
        const std::uint8_t green = static_cast<std::uint8_t>(color.y * 255.0);
        const std::uint8_t blue = static_cast<std::uint8_t>(color.z * 255.0);

        body->CreateFixture(&body_shape, 0.0f);

        const Shape shp(body, size.x, size.y, id_counter_, red, green, blue);
        id_counter_ = static_cast<ItemId>(static_cast<int>(id_counter_) + 1);
        return shp;
    }

    Shape createDynamicBoxShape(const Vec2d& position, const Vec2d& size, const double angle, const Vec3d& color)
    {
        // Creating the dynamic body
        b2BodyDef body_def;
        body_def.type = b2_dynamicBody;
        body_def.position.Set(position.x, position.y);
        body_def.angle = angle;

        b2Body* body = world_.CreateBody(&body_def);

        b2PolygonShape dynamic_box;
        dynamic_box.SetAsBox(size.x / 2.0, size.y / 2.0);

        b2FixtureDef fixture_def;
        fixture_def.shape = &dynamic_box;
        fixture_def.density = debug_value_reader::getValue<float>("density");
        fixture_def.friction = debug_value_reader::getValue<float>("friction");

        body->CreateFixture(&fixture_def);

        const std::uint8_t red = static_cast<std::uint8_t>(color.x * 255.0);
        const std::uint8_t green = static_cast<std::uint8_t>(color.y * 255.0);
        const std::uint8_t blue = static_cast<std::uint8_t>(color.z * 255.0);

        const Shape shp(body, size.x, size.y, id_counter_, red, green, blue);
        id_counter_ = static_cast<ItemId>(static_cast<int>(id_counter_) + 1);
        return shp;
    }

    static constexpr lumos::ItemId kLaunchSquareId = properties::ID0;
    static constexpr lumos::ItemId kSpinningSquareId = properties::ID1;

    float t = 0.0f;
    std::vector<Shape> shapes_;
    Shape funnel_left_;
    Shape funnel_right_;
    Shape ground_body_;
    Shape pusher_;
    Shape flipper_;

    // Pendulum
    Shape pend_base_;
    Shape pend_second_;
    Shape pend_horizontal_;
    Shape pend_right_;
    Shape pend_left_;

    // Pusher
    float pusher_p;
    float pusher_d;

    float pusher_frequency;
    float pusher_amplitude;
    float pusher_x;
    float pusher_y;

    // Flipper
    float flipper_p;
    float flipper_d;

    float flipper_frequency;
    float flipper_amplitude;
    float flipper_offset;

    static constexpr size_t kNumShapes{80U};

public:
    Joints() : gravity_{0.0f, -10.0f}, world_{gravity_}, ground_box_{}
    {
        id_counter_ = static_cast<ItemId>(0);

        b2Body* wall_body = createStaticCollisionlessBody({0.0, 0.0}, {50.0, 50.0});

        // createStaticBody({0.0, -10.0}, {6.0, 1.0}, 0.0);

        ground_body_ = createStaticBoxShape(Vec2d{0.0, -10.0}, Vec2d{12.0, 2.0}, 0.0, calculateColormapPastel2(0.0));

        funnel_left_ = createStaticBoxShape(
            Vec2d{-4.0, -1.0}, Vec2d{12.0, 1.0}, (120.0) * M_PI / 180.0, calculateColormapPastel2(0.7));
        funnel_right_ = createStaticBoxShape(
            Vec2d{4.0, -1.0}, Vec2d{12.0, 1.0}, (-120.0) * M_PI / 180.0, calculateColormapPastel2(0.8));

        pusher_x = debug_value_reader::getValue<float>("pusher_x");
        pusher_y = debug_value_reader::getValue<float>("pusher_y");
        const double pusher_width = debug_value_reader::getValue<double>("pusher_width");
        const double pusher_height = debug_value_reader::getValue<double>("pusher_height");

        pusher_frequency = debug_value_reader::getValue<float>("pusher_frequency");
        pusher_amplitude = debug_value_reader::getValue<float>("pusher_amp");
        pusher_p = debug_value_reader::getValue<float>("pusher_p");
        pusher_d = debug_value_reader::getValue<float>("pusher_d");
        // float pusher_p;
        // float pusher_d;

        pusher_ = createDynamicBoxShape(
            Vec2d{pusher_x, pusher_y}, Vec2d{pusher_width, pusher_height}, 0.0, calculateColormapPastel2(0.2));

        flipper_p = debug_value_reader::getValue<float>("flipper_p");
        flipper_d = debug_value_reader::getValue<float>("flipper_d");

        float x_pos = -6.0f;
        float y_pos = 6.0f;

        for (size_t k = 0; k < kNumShapes; k++)
        {
            // Between -6 and 6
            x_pos += 1.2;
            if (x_pos > 6.0)
            {
                x_pos = -6.0;
                y_pos += 1.2;
            }

            const Vec3d col = calculateColormapPastel2(static_cast<double>(k) / static_cast<double>(kNumShapes - 1U));

            const float rot = static_cast<float>(rand() & 1001) / 1000.0f - 0.5f;
            shapes_.emplace_back(createDynamicBoxShape(Vec2d{x_pos, y_pos}, Vec2d{1.0, 1.0}, rot, col));
        }

        flipper_offset = debug_value_reader::getValue<double>("flipper_offset");
        flipper_ = createDynamicBoxShape(
            Vec2d{debug_value_reader::getValue<double>("flipper_x"), debug_value_reader::getValue<double>("flipper_y")},
            Vec2d{debug_value_reader::getValue<double>("flipper_width"),
                  debug_value_reader::getValue<double>("flipper_height")},
            flipper_offset,
            calculateColormapPastel2(0.1));

        flipper_frequency = debug_value_reader::getValue<double>("flipper_frequency");
        flipper_amplitude = debug_value_reader::getValue<double>("flipper_amplitude");

        // Pendulum

        float pendulum_width = 1.0f;
        float pendulum_height = 8.0f;

        pend_base_ = createDynamicBoxShape(Vec2d{debug_value_reader::getValue<double>("pend_base_x"),
                                                 debug_value_reader::getValue<double>("pend_base_y")},
                                           Vec2d{pendulum_width, pendulum_height},
                                           0.0,
                                           calculateColormapPastel2(0.2));

        pend_second_ =
            createDynamicBoxShape(Vec2d{debug_value_reader::getValue<double>("pend_base_x"),
                                        debug_value_reader::getValue<double>("pend_base_y") + pendulum_height},
                                  Vec2d{pendulum_width, pendulum_height},
                                  0.0,
                                  calculateColormapPastel2(0.5));

        pend_horizontal_ =
            createDynamicBoxShape(Vec2d{debug_value_reader::getValue<double>("pend_base_x"),
                                        debug_value_reader::getValue<double>("pend_base_y") + pendulum_height * 2.0f},
                                  Vec2d{pendulum_width, pendulum_height * 0.8},
                                  0.0,
                                  calculateColormapPastel2(0.7));

        b2RevoluteJointDef joint_def0;
        joint_def0.bodyA = wall_body;
        joint_def0.bodyB = pend_base_.body_handle;
        joint_def0.localAnchorA = b2Vec2{debug_value_reader::getValue<float>("pend_base_x"),
                                         debug_value_reader::getValue<float>("pend_base_y") - pendulum_height / 2.0f};
        joint_def0.localAnchorB = b2Vec2{0.0, -pendulum_height / 2.0f};
        joint_def0.enableLimit = false;

        b2Joint* joint0 = world_.CreateJoint(&joint_def0);

        b2RevoluteJointDef joint_def1;
        joint_def1.bodyA = pend_base_.body_handle;
        joint_def1.bodyB = pend_second_.body_handle;
        joint_def1.collideConnected = false;
        joint_def1.localAnchorA = b2Vec2{0.0f, pendulum_height / 2.0f};
        joint_def1.localAnchorB = b2Vec2{0.0, -pendulum_height / 2.0f};
        joint_def1.enableLimit = false;

        b2Joint* joint1 = world_.CreateJoint(&joint_def1);

        b2RevoluteJointDef joint_def2;
        joint_def2.bodyA = pend_second_.body_handle;
        joint_def2.bodyB = pend_horizontal_.body_handle;
        joint_def2.collideConnected = false;
        joint_def2.localAnchorA = b2Vec2{0.0f, pendulum_height / 2.0f};
        joint_def2.localAnchorB = b2Vec2{0.0f, -0.8f * pendulum_height / 2.0f};
        joint_def2.enableLimit = false;

        b2Joint* joint2 = world_.CreateJoint(&joint_def2);

        b2RevoluteJointDef joint_def_flipper;
        joint_def_flipper.bodyA = wall_body;
        joint_def_flipper.bodyB = flipper_.body_handle;
        joint_def_flipper.localAnchorA =
            b2Vec2{debug_value_reader::getValue<float>("flipper_x"), debug_value_reader::getValue<float>("flipper_y")};
        joint_def_flipper.localAnchorB = b2Vec2{0.0, 0.0};
        joint_def_flipper.enableLimit = false;

        b2Joint* joint_flipper = world_.CreateJoint(&joint_def_flipper);
    }

    ~Joints() {}

    Vec2d base_zoom_half_range{20.0, 20.0};
    Vec2d base_zoom_center{12.0, -8.0};
    double z_min = -1.0;
    double z_max = 1.0;
    double current_zoom_factor = 1.0;
    Vec2d current_pos;
    Vec2d current_range;

    bool angle_reached = false;

    float angle_offset = 0.0f;
    float current_angle;

    void setAxes(const size_t iteration)
    {
        const float elevation = 90.0f;
        if (iteration == 0)
        {
            viewAngles(0, elevation);
            current_pos.x = base_zoom_center.x;
            current_pos.y = base_zoom_center.y;
            current_range.x = base_zoom_half_range.x * current_zoom_factor;
            current_range.y = base_zoom_half_range.y * current_zoom_factor;
        }
        else if (angle_reached)
        {
            const Vec2d current_reference = base_zoom_center;
            const double zoom_factor_reference = 1.0;
            const double reference_angle = 0.0;

            const double h = 0.01;
            const double h1 = (1.0 - h);

            current_pos.x = current_pos.x * h1 + current_reference.x * h;
            current_pos.y = current_pos.y * h1 + current_reference.y * h;
            current_zoom_factor = current_zoom_factor * h1 + zoom_factor_reference * h;

            current_angle = current_angle * h1 + reference_angle * h;

            current_range.x = base_zoom_half_range.x * current_zoom_factor;
            current_range.y = base_zoom_half_range.y * current_zoom_factor;
            viewAngles(current_angle * 180.0 / M_PI, 90.0);
        }
        else if (iteration < 150) {}
        else if (iteration < 600)
        {
            const auto p = shapes_[16].body_handle->GetPosition();
            const Vec2d current_reference(p.x, p.y);
            const double zoom_factor_reference = 0.5;

            const double h = 0.05;
            const double h1 = (1.0 - h);

            current_pos.x = current_pos.x * h1 + current_reference.x * h;
            current_pos.y = current_pos.y * h1 + current_reference.y * h;
            current_zoom_factor = current_zoom_factor * h1 + zoom_factor_reference * h;

            current_range.x = base_zoom_half_range.x * current_zoom_factor;
            current_range.y = base_zoom_half_range.y * current_zoom_factor;
        }
        else
        {
            const auto p = pend_horizontal_.body_handle->GetPosition();
            const auto ang = pend_horizontal_.body_handle->GetAngle();

            if (ang < (-M_PI))
            {
                angle_reached = true;
                current_angle = ang;
            }

            viewAngles(((angle_offset + -ang) * 180.0 / M_PI), elevation);

            const Vec2d current_reference(p.x, p.y);
            const double zoom_factor_reference = 0.8;

            const double h = 0.2;
            const double h1 = (1.0 - h);

            current_pos.x = current_pos.x * h1 + current_reference.x * h;
            current_pos.y = current_pos.y * h1 + current_reference.y * h;
            current_zoom_factor = current_zoom_factor * h1 + zoom_factor_reference * h;

            current_range.x = base_zoom_half_range.x * current_zoom_factor;
            current_range.y = base_zoom_half_range.y * current_zoom_factor;
        }

        axis({current_pos.x - current_range.x, current_pos.y - current_range.y, z_min},
             {current_pos.x + current_range.x, current_pos.y + current_range.y, z_max});
    }

    void step(const size_t iteration)
    {
        setAxes(iteration);

        world_.Step(time_step, velocity_iterations, position_iterations);

        std::vector<PropertySet> props;

        for (size_t k = 0; k < kNumShapes; k++)
        {
            const auto p = shapes_[k].body_handle->GetPosition();

            const properties::Transform transform = shapes_[k].getTransform();
            props.emplace_back(shapes_[k].id, transform);
        }

        props.emplace_back(funnel_left_.id, funnel_left_.getTransform());

        props.emplace_back(funnel_right_.id, funnel_right_.getTransform());

        props.emplace_back(pusher_.id, pusher_.getTransform());

        props.emplace_back(flipper_.id, flipper_.getTransform());

        props.emplace_back(pend_base_.id, pend_base_.getTransform());

        props.emplace_back(pend_second_.id, pend_second_.getTransform());
        props.emplace_back(pend_horizontal_.id, pend_horizontal_.getTransform());

        props.emplace_back(ground_body_.id, ground_body_.getTransform());

        setProperties(props);

        const double phi_left = std::cos(t * 2.0) * 0.08f;
        const double phi_right = std::sin(t * 2.0) * 0.08f;
        funnel_left_.body_handle->SetTransform(funnel_left_.body_handle->GetPosition(),
                                               (120.0) * M_PI / 180.0 + phi_left);
        funnel_right_.body_handle->SetTransform(funnel_right_.body_handle->GetPosition(),
                                                (-120.0) * M_PI / 180.0 + phi_right);

        // Pusher
        const float pusher_reference_pos = std::sin(t * pusher_frequency) * pusher_amplitude + pusher_x;
        const float pusher_pos_error = pusher_reference_pos - pusher_.body_handle->GetPosition().x;
        const float pusher_vel = pusher_.body_handle->GetLinearVelocity().x;

        const float u_pusher = pusher_pos_error * pusher_p + pusher_vel * pusher_d;

        pusher_.body_handle->ApplyForce(b2Vec2(u_pusher, 0.0f), pusher_.body_handle->GetPosition(), true);

        // Flipper
        const double flipper_reference_angle = std::cos(t * flipper_frequency) * flipper_amplitude + flipper_offset;

        const float flipper_ang_error = flipper_reference_angle - flipper_.body_handle->GetAngle();
        const float flipper_ang_vel = flipper_.body_handle->GetAngularVelocity();
        const float u = flipper_ang_error * flipper_p + flipper_ang_vel * flipper_d;

        flipper_.body_handle->ApplyTorque(u, true);

        // Pendulum

        t += 0.05f;
    }
};

void testBasic();

}  // namespace joints

#endif  // DEMOS_TESTS_JOINTS_JOINTS_H_