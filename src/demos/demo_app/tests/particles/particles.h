#ifndef DEMOS_TESTS_PARTICLES_H_
#define DEMOS_TESTS_PARTICLES_H_

#include <Box2D/Box2D.h>
#include <Box2D/Particle/b2Particle.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "duoplot/duoplot.h"
#include "helper_functions.h"
#include "line2d.h"
#include "misc/rgb_triplet.h"
#include "polygon.h"
#include "rapidxml.hpp"

using namespace duoplot;

namespace particles
{

Vector<Point2f> splitPointsString(
    const std::string& s, const float mul, const float add_x, const float add_y, const float sign_mul)
{
    const std::vector<std::string> coordinates_strings = splitString(s, " ");

    std::vector<Point2f> coords;
    coords.reserve(coordinates_strings.size() - 1);
    coords.push_back(Point2f());

    const std::vector<std::string> q = splitString(coordinates_strings[0], ",");

    coords[0].x = (std::stof(q[0])) * mul + add_x;
    coords[0].y = (-std::stof(q[1])) * mul + add_y;

    size_t idx = 1;
    for (size_t k = 0; k < coordinates_strings.size(); k++)
    {
        if (coordinates_strings[k] == "")
        {
            continue;
        }
        const std::vector<std::string> q = splitString(coordinates_strings[k], ",");

        const float x = (std::stod(q[0])) * mul + add_x;
        const float y = (sign_mul * std::stod(q[1])) * mul + add_y;

        if ((x == coords[idx - 1U].x) && (y == coords[idx - 1U].y))
        {
            continue;
        }

        coords.push_back(Point2f{x, y});

        idx++;
    }

    Vector<Point2f> coordinates{coords};

    return coordinates;
}

Vector<Point2f> splitPointsString(const std::string& s)
{
    const float mul = 1.0 / 95.0;
    const float add_x = -2.8;
    const float add_y = 1.0;

    return splitPointsString(s, mul, add_x, add_y, -1.0);
}

std::string readTextFile(const std::string& file_path)
{
    std::fstream txt_file;
    txt_file.open(file_path, std::ios::in);

    std::string text_data;

    if (txt_file.is_open())
    {
        std::string tp;
        while (getline(txt_file, tp))
        {
            text_data.append(tp);
        }
        txt_file.close();
    }

    return text_data;
}

std::vector<Vector<Point2f>> readSVG()
{
    std::string text_data = readTextFile("../demos/tests/vectorpaint.svg");

    rapidxml::xml_document<> doc;
    doc.parse<0>(text_data.data());

    // float svg_width, svg_height;

    rapidxml::xml_node<>* node = doc.first_node("svg");

    std::vector<Vector<Point2f>> boundaries;

    for (rapidxml::xml_node<>* child = node->first_node(); child; child = child->next_sibling())
    {
        const std::string child_name = child->name();
        if (child_name == "g")
        {
            for (rapidxml::xml_node<>* sub_child = child->first_node(); sub_child;
                 sub_child = sub_child->next_sibling())
            {
                for (rapidxml::xml_attribute<>* attr = sub_child->first_attribute(); attr;
                     attr = attr->next_attribute())
                {
                    const std::string attr_name = attr->name();
                    if (attr_name == "points")
                    {
                        boundaries.push_back(splitPointsString(attr->value()));
                    }
                }
            }
        }
    }

    return boundaries;
}

void plotBoundaries(const Vector<Point2f>& vertices, const properties::Color& c)
{
    Vector<float> x{vertices.size()}, y{vertices.size()};

    for (size_t k = 0; k < vertices.size(); k++)
    {
        x(k) = vertices(k).x;
        y(k) = vertices(k).y;
    }

    plot(x, y, c);
}

void scatterBoundaries(const Vector<Point2f>& vertices, const properties::Color& c)
{
    Vector<float> x{vertices.size()}, y{vertices.size()};

    for (size_t k = 0; k < vertices.size(); k++)
    {
        x(k) = vertices(k).x;
        y(k) = vertices(k).y;
    }

    scatter(x, y, c);
}

class PointAssigner
{
private:
    std::vector<Vector<Point2f>> boundaries_;
    std::vector<Polygon> polygons_;

    Polygon p_outer_letter_;
    Polygon p_inner_letter_;
    Polygon l_letter_;
    Polygon o_outer_letter_;
    Polygon o_inner_letter_;

    Vector<properties::Color> output_color_;

    void assignForLetter(const Vector<Point2f>& points, const Polygon& polygon, const properties::Color& col)
    {
        const Vec2f min_vec = polygon.getMinVec();
        const Vec2f max_vec = polygon.getMaxVec();

        for (size_t k = 0; k < points.size(); k++)
        {
            const Vec2f pt{points(k).x, points(k).y};

            if ((pt.x < min_vec.x) || (pt.x > max_vec.x) || (pt.y < min_vec.y) || (pt.y > max_vec.y))
            {
                continue;
            }
            else if (polygon.pointIsInPolygon(pt))
            {
                output_color_(k) = col;
            }
        }
    }

public:
    PointAssigner()
    {
        boundaries_ = readSVG();

        for (size_t k = 0; k < boundaries_.size(); k++)
        {
            polygons_.emplace_back(boundaries_[k]);
        }

        /*p_outer_letter_ = Polygon(boundaries_[0]);
        p_inner_letter_ = Polygon(boundaries_[1]);
        l_letter_ = Polygon(boundaries_[2]);
        o_outer_letter_ = Polygon(boundaries_[3]);
        o_inner_letter_ = Polygon(boundaries_[4]);*/
    }

    VectorConstView<properties::Color> getColors() const
    {
        return output_color_.constView();
    }

    void assignColors(const Vector<Point2f>& points)
    {
        output_color_.resize(points.size());

        for (size_t k = 0; k < points.size(); k++)
        {
            output_color_(k) = properties::Color{0, 0, 0};
        }

        for (size_t k = 0; k < polygons_.size(); k++)
        {
            assignForLetter(points, polygons_[k], properties::Color{255, 0, 0});
        }

        /*assignForLetter(points, p_outer_letter_, properties::Color{255, 0, 0});
        assignForLetter(points, p_inner_letter_, properties::Color{0, 0, 0});
        assignForLetter(points, l_letter_, properties::Color{0, 255, 0});
        assignForLetter(points, o_outer_letter_, properties::Color{0, 0, 255});
        assignForLetter(points, o_inner_letter_, properties::Color{0, 0, 0});*/
    }
};

class PointAssignerImg
{
private:
    ImageRGBA<std::uint8_t> img_;

    Vector<properties::Color> output_color_;

    void readShapeImage(const std::string bin_path, ImageRGBA<std::uint8_t>& output_img)
    {
        std::ifstream input(bin_path, std::ios::binary);

        std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

        std::uint16_t num_rows, num_cols;

        std::memcpy(&num_cols, buffer.data(), sizeof(std::uint16_t));
        std::memcpy(&num_rows, buffer.data() + sizeof(std::uint16_t), sizeof(std::uint16_t));

        const std::uint8_t* const img_raw_ptr = buffer.data() + 2 * sizeof(std::uint16_t);

        output_img.resize(num_rows, num_cols);

        const size_t num_element_per_channel = num_rows * num_cols;

        for (size_t ch = 0; ch < 3; ch++)
        {
            for (size_t r = 0; r < num_rows; r++)
            {
                for (size_t c = 0; c < num_cols; c++)
                {
                    const std::uint8_t pixel_val = img_raw_ptr[ch * num_element_per_channel + r * num_cols + c];
                    output_img(r, c, ch) = pixel_val;
                }
            }
        }

        for (size_t r = 0; r < num_rows; r++)
        {
            for (size_t c = 0; c < num_cols; c++)
            {
                output_img(r, c, 3) = 255;
            }
        }
    }

public:
    PointAssignerImg()
    {
        readShapeImage("/Users/danielpi/work/dvs/src/demos/tests/particles/img.bin", img_);
    }

    PointAssignerImg(const std::string& img_path)
    {
        readShapeImage(img_path, img_);
    }

    VectorConstView<properties::Color> getColors() const
    {
        return output_color_.constView();
    }

    ImageRGBA<std::uint8_t> getImage() const
    {
        return img_;
    }

    ImageRGBAConstView<std::uint8_t> getImageView() const
    {
        return img_.constView();
    }

    void assignColors(const Vector<Point2f>& points, const Vec2d min_bnd, const Vec2d max_bnd)
    {
        output_color_.resize(points.size());

        for (size_t k = 0; k < points.size(); k++)
        {
            output_color_(k) = properties::Color(0, 0, 0);
        }

        const double x_scale = img_.numCols() / (max_bnd.x - min_bnd.x);
        const double y_scale = img_.numRows() / (max_bnd.y - min_bnd.y);

        const size_t num_rows_minus_one = img_.numRows() - 1;
        const size_t num_cols_minus_one = img_.numCols() - 1;

        for (size_t k = 0; k < points.size(); k++)
        {
            const Vec2d pt{points(k).x, points(k).y};

            if ((pt.x <= min_bnd.x) || (pt.x >= max_bnd.x) || (pt.y <= min_bnd.y) || (pt.y >= max_bnd.y))
            {
                continue;
            }

            const size_t r = std::round((pt.y - min_bnd.y) * y_scale);
            const size_t c = std::round((pt.x - min_bnd.x) * x_scale);

            if ((r >= img_.numRows()) || (c >= img_.numCols()))
            {
                continue;
            }

            output_color_(k) = properties::Color{img_(num_rows_minus_one - r, c, 2),
                                                 img_(num_rows_minus_one - r, c, 1),
                                                 img_(num_rows_minus_one - r, c, 0)};
        }
    }
};

const size_t num_steps = 1200;
const Vec2f min_bnd{-4.0f, 0.0f};
const Vec2f max_bnd{4.0f, 26.0f};
const float radius = 0.025f;

class ParticleSystem
{
private:
    size_t num_particles_;
    Vec2f min_bnd_;
    Vec2f max_bnd_;
    float* x_pos_;
    float* y_pos_;

    float* x_vel_;
    float* y_vel_;

    b2World world_;
    b2ParticleSystem* particle_system_;
    Vector<properties::Color> color_;

    Vector<b2Vec2> vertices_;
    b2Body* ground_;
    b2ParticleGroup* group_;

    void placeTipAt(const float x, const float y)
    {
        b2PolygonShape shape;
        const Vector<b2Vec2> vertices{
            VectorInitializer{b2Vec2(x + -0.1, y + -0.2), b2Vec2(x + 0.0, y + 0.2), b2Vec2(x + 0.1, y + -0.2)}};
        shape.Set(vertices.data(), vertices.size());
        ground_->CreateFixture(&shape, 0.0f);
    }

public:
    ParticleSystem() = delete;
    ParticleSystem(const Vec2f min_bnd, const Vec2f max_bnd)
        : min_bnd_{min_bnd}, max_bnd_{max_bnd}, world_{b2Vec2{0.0, -9.82}}
    {
        b2ParticleSystemDef particleSystemDef;

        particleSystemDef.radius = radius;
        particleSystemDef.dampingStrength = 0.03;
        particleSystemDef.gravityScale = 0.05;
        particleSystemDef.density = 0.001;

        particle_system_ = world_.CreateParticleSystem(&particleSystemDef);

        b2BodyDef bd;
        ground_ = world_.CreateBody(&bd);

        placeTipAt(0.0f, 1.6f);
        placeTipAt(-0.4f, 1.6f);
        placeTipAt(0.4f, 1.6f);
        placeTipAt(-0.8f, 1.6f);
        placeTipAt(0.8f, 1.6f);
        placeTipAt(-1.2f, 1.6f);
        placeTipAt(1.2f, 1.6f);
        placeTipAt(-1.6f, 1.6f);
        placeTipAt(1.6f, 1.6f);

        placeTipAt(-0.2f, 1.0f);
        placeTipAt(0.2f, 1.0f);
        placeTipAt(-0.6f, 1.0f);
        placeTipAt(0.6f, 1.0f);
        placeTipAt(-1.0f, 1.0f);
        placeTipAt(1.0f, 1.0f);

        {
            b2PolygonShape shape;
            const Vector<b2Vec2> vertices{VectorInitializer{b2Vec2(-4, -2), b2Vec2(4, -2), b2Vec2(-4, -4)}};
            shape.Set(vertices.data(), vertices.size());
            ground_->CreateFixture(&shape, 0.0f);
        }

        {
            b2PolygonShape shape;
            const Vector<b2Vec2> vertices{VectorInitializer{b2Vec2(-4, 4), b2Vec2(-2, 2), b2Vec2(-2, -2)}};
            shape.Set(vertices.data(), vertices.size());
            ground_->CreateFixture(&shape, 0.0f);
        }

        {
            b2PolygonShape shape;
            const Vector<b2Vec2> vertices{VectorInitializer{b2Vec2(4, 4), b2Vec2(2, 2), b2Vec2(2, -2)}};
            shape.Set(vertices.data(), vertices.size());
            ground_->CreateFixture(&shape, 0.0f);
        }

        b2PolygonShape shape;
        shape.SetAsBox(2.0f, 2.0f);

        b2ParticleGroupDef particleGroupDef;
        particleGroupDef.flags = b2_waterParticle;
        particleGroupDef.position.Set(0.0f, 4.1f);
        particleGroupDef.shape = &shape;

        group_ = particle_system_->CreateParticleGroup(particleGroupDef);

        num_particles_ = particle_system_->GetParticleCount();

        x_pos_ = new float[particle_system_->GetParticleCount()];
        y_pos_ = new float[particle_system_->GetParticleCount()];
        x_vel_ = new float[particle_system_->GetParticleCount()];
        y_vel_ = new float[particle_system_->GetParticleCount()];

        color_.resize(particle_system_->GetParticleCount());
    }

    ~ParticleSystem() {}

    size_t numPoints() const
    {
        return num_particles_;
    }

    const b2Vec2* getParticles() const
    {
        return particle_system_->GetPositionBuffer();
    }

    void update()
    {
        const b2Vec2* const particles = particle_system_->GetPositionBuffer();
        const b2Vec2* const velocities = particle_system_->GetVelocityBuffer();

        const size_t num_particles = particle_system_->GetParticleCount();

        for (size_t k = 0; k < num_particles; k++)
        {
            x_pos_[k] = particles[k].x;
            y_pos_[k] = particles[k].y;

            /*const float vx = velocities[k].x;
            const float vy = velocities[k].y;

            const float v = std::sqrt(vx * vx + vy * vy);
            const float theta = std::atan2(vy, vx) / (M_PI * 2.0) + 0.5f;

            auto const c = color_maps::jet(theta);

            color_(k) = properties::Color{c.red, c.green, c.blue};*/
        }

        world_.Step(0.01f, 8, 3);
    }

    VectorConstView<float> getXView() const
    {
        return VectorConstView<float>{x_pos_, num_particles_};
    }

    VectorConstView<float> getYView() const
    {
        return VectorConstView<float>{y_pos_, num_particles_};
    }

    VectorConstView<properties::Color> getColorView() const
    {
        return color_.constView();
    }

    void savePointsToFile() const
    {
        const std::string file_path = "../demos/tests/points.txt";

        std::ofstream myfile;
        myfile.open(file_path);

        const VectorConstView<float> x = getXView();
        const VectorConstView<float> y = getYView();

        for (size_t k = 0; k < num_particles_; k++)
        {
            const std::string sx = std::to_string(x(k));
            const std::string sy = std::to_string(y(k));

            myfile << sx << "," << sy << " ";
        }

        myfile.close();
    }
};

Vector<Point2f> readSavedFile()
{
    const std::string text_data = readTextFile("../demos/tests/points.txt");

    return splitPointsString(text_data, 1.0, 0.0, 0.0, 1.0);
}

void saveToFile();
void runTest();

void testBasic()
{
    bool save_to_file = false;
    if (save_to_file)
    {
        saveToFile();
    }
    else
    {
        runTest();
    }
}

void runTest()
{
    const std::string project_file_path = "../../project_files/particles.duoplot";

    openProjectFile(project_file_path);

    setActiveView("p_view_0");
    clearView();
    waitForFlush();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});
    // axis({-3.0, -2.1}, {3.0, 1.0});
    axis({-2.0, -2.0}, {2.0, 2.0});

    const Vector<Point2f> saved_points = readSavedFile();

    ParticleSystem ps{min_bnd, max_bnd};
    PointAssignerImg point_assigner{};
    const auto img = point_assigner.getImage();

    point_assigner.assignColors(saved_points, Vec2d{-2.01, -2.0}, Vec2d{2.01, 0.8});

    Vector<float> xs{saved_points.size()}, ys{saved_points.size()};

    for (size_t k = 0; k < saved_points.size(); k++)
    {
        xs(k) = saved_points(k).x;
        ys(k) = saved_points(k).y;
    }

    const auto colors = point_assigner.getColors();

    VectorConstView<properties::Color> new_color_view{colors.data() + 1, colors.size() - 1U};

    if (0)
    {
        scatter(xs.constView(), ys.constView(), colors, properties::ScatterStyle::DISC, properties::PointSize(5));
        flushCurrentElement();
    }
    else
    {
        for (size_t k = 0; k < num_steps; k++)
        {
            ps.update();

            const VectorConstView<float> x = ps.getXView();
            const VectorConstView<float> y = ps.getYView();

            scatter(x, y, new_color_view, properties::ScatterStyle::DISC, properties::PointSize(17));
            flushCurrentElement();
            clearViewOnUpdate();
        }
    }
}

void testBasicOld()
{
    const std::string project_file_path = "../../project_files/particles.duoplot";

    openProjectFile(project_file_path);

    setActiveView("p_view_0");
    clearView();
    waitForFlush();
    axis({-3.0, -2.1}, {3.0, 1.0});

    const Vector<Point2f> saved_points = readSavedFile();

    ParticleSystem ps{min_bnd, max_bnd};
    PointAssigner pa{};

    pa.assignColors(saved_points);

    Vector<float> xs{saved_points.size()}, ys{saved_points.size()};

    for (size_t k = 0; k < saved_points.size(); k++)
    {
        xs(k) = saved_points(k).x;
        ys(k) = saved_points(k).y;
    }

    const auto colors = pa.getColors();

    VectorConstView<properties::Color> new_color_view{colors.data() + 1, colors.size() - 1U};

    if (0)
    {
        scatter(xs.constView(), ys.constView(), colors, properties::ScatterStyle::DISC, properties::PointSize(15));
    }
    else
    {
        for (size_t k = 0; k < num_steps; k++)
        {
            ps.update();

            const VectorConstView<float> x = ps.getXView();
            const VectorConstView<float> y = ps.getYView();

            scatter(x, y, new_color_view, properties::ScatterStyle::DISC, properties::PointSize(20));
            flushCurrentElement();
            clearViewOnUpdate();
        }
    }
}

void saveToFile()
{
    setActiveView("p_view_0");
    clearView();
    waitForFlush();
    axis({-3.0, -2.1}, {3.0, 1.0});

    ParticleSystem ps{min_bnd, max_bnd};

    for (size_t k = 0; k < num_steps; k++)
    {
        ps.update();
    }

    const VectorConstView<float> x = ps.getXView();
    const VectorConstView<float> y = ps.getYView();
    scatter(x, y, properties::Color::RED, properties::ScatterStyle::DISC, properties::PointSize(20));
    flushCurrentElement();
    clearViewOnUpdate();

    ps.savePointsToFile();
}

void testBasicTmp()
{
    const size_t num_polygon_points = 4;
    const size_t num_points = 1000;

    Vector<Point2f> polygon_points{num_polygon_points};
    Vector<float> xr{num_points}, yr{num_points};
    Vector<properties::Color> colors{num_points};

    const auto r_fun = []() -> float {
        const float r = static_cast<float>(rand() % 1001) / 500.0 - 1.0;
        return r;
    };

    for (size_t k = 0; k < num_points; k++)
    {
        xr(k) = r_fun() * 1.4;
        yr(k) = r_fun() * 1.4;
    }

    const std::vector<Vector<Point2f>> boundaries = readSVG();

    polygon_points = boundaries[0];

    Polygon polygon{polygon_points};

    for (size_t k = 0; k < num_points; k++)
    {
        if (polygon.pointIsInPolygon({xr(k), yr(k)}))
        {
            colors(k) = properties::Color(255, 0, 0);
        }
        else
        {
            colors(k) = properties::Color(0, 0, 0);
        }
    }

    setActiveView("p_view_0");
    clearView();
    axis({-1.0, -1.0}, {1.0, 1.0});

    polygon.visualize();
    scatter(xr, yr, colors, properties::PointSize(20));
}

}  // namespace particles

#endif  // DEMOS_TESTS_PARTICLES_H_
