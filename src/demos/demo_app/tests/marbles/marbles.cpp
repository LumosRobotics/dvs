#include "tests/marbles/marbles.h"

#include <functional>

#include "tests/slam/color_maps.h"
#include "tests/sphere/sphere_subdivision/point.h"
#include "tests/sphere/sphere_subdivision/subdivisor.h"

namespace marbles
{

double azimuth = -179.0;

struct MarbleState
{
    Vec3d pos;
    Vec3d vel;
    Vec2d dir;
    double theta;

    MarbleState() : pos({0.0, 0.0, 0.0}), vel({0.0, 0.0, 0.0}), theta(0.0) {}
    MarbleState(const Vec3d& pos_, const Vec3d& vel_, const double theta_) : pos(pos_), vel(vel_), theta(theta_) {}
};

struct Marble
{
    Vector<IndexTriplet> indices;
    Vector<double> x;
    Vector<double> y;
    Vector<double> z;

    Marble() = delete;
    Marble(const int num_subdivisions)
    {
        sphere::Subdivisor subdivisor{num_subdivisions, 1.0};
        int N = subdivisor.P_subdivided.size();

        x.resize(N);
        y.resize(N);
        z.resize(N);

        for (int c = 0; c < N; c++)
        {
            sphere::Point p = subdivisor.P_subdivided[c];
            x(c) = p.x;
            y(c) = p.y;
            z(c) = p.z;
        }

        indices.resize(subdivisor.facelist_subdivided.faces.size());
        for (int c = 0; c < subdivisor.facelist_subdivided.faces.size(); c++)
        {
            const sphere::Face f = subdivisor.facelist_subdivided.faces[c];
            const sphere::Edge e1 = subdivisor.edgelist_subdivided.edges[f.i1];
            const sphere::Edge e2 = subdivisor.edgelist_subdivided.edges[f.i2];
            const sphere::Edge e3 = subdivisor.edgelist_subdivided.edges[f.i3];

            std::vector<int> ind = {e1.i1, e1.i2, e2.i1, e2.i2, e3.i1, e3.i2};

            std::sort(ind.begin(), ind.end());
            ind.erase(std::unique(ind.begin(), ind.end()), ind.end());

            indices(c) = IndexTriplet(ind[0], ind[1], ind[2]);
        }
    }
};

class Marbles
{
public:
    Marble marble_;
    double min_z_;
    double max_z_;
    double z_range_;
    std::vector<Vector<properties::Color>> colors_;
    std::vector<MarbleState> marble_states_;
    std::vector<ItemId> ids_;

public:
    Marbles() = delete;
    Marbles(const int num_marbles, const int num_subdivisions) : marble_(num_subdivisions)
    {
        for (std::uint16_t k = 0; k < num_marbles; k++)
        {
            ids_.push_back(static_cast<ItemId>(k));
        }

        min_z_ = marble_.z(0);
        max_z_ = marble_.z(0);

        for (size_t k = 0; k < marble_.z.size(); k++)
        {
            min_z_ = std::min(min_z_, marble_.z(k));
            max_z_ = std::max(max_z_, marble_.z(k));
        }
        z_range_ = max_z_ - min_z_;

        colors_.resize(num_marbles);

        for (int k = 0; k < num_marbles; k++)
        {
            Vector<properties::Color>& col_vec = colors_[k];
            col_vec.resize(marble_.indices.size());

            std::function<RGBTripletf(float)> colormap_func = calculateColormapViridis;

            int kr = k % num_marbles;

            if (k == 0)
            {
                // colormap_func = calculateColormapRainbow;
            }
            else if (k == 1)
            {
                colormap_func = calculateColormapMagma;
            }
            else if (k == 2)
            {
                colormap_func = calculateColormapViridis;
            }
            else if (k == 3)
            {
                // colormap_func = calculateColormapJet;
            }
            else if (k == 4)
            {
                colormap_func = calculateColormapRainbowPastel;
            }
            else if (k == 5)
            {
                colormap_func = calculateColormapJetSoft;
            }
            else if (k == 6)
            {
                colormap_func = calculateColormapJetBright;
            }

            for (size_t i = 0; i < marble_.indices.size(); i++)
            {
                const IndexTriplet& it{marble_.indices(i)};
                const double z0 = marble_.z(it.i0);
                const double z1 = marble_.z(it.i1);
                const double z2 = marble_.z(it.i2);
                const double z_val = (z0 + z1 + z2) / 3.0;
                const double norm_val = (z_val - min_z_) / z_range_;
                const RGBTripletf c = colormap_func(norm_val);
                col_vec(i) = properties::Color(c.red * 255, c.green * 255, c.blue * 255);
            }
        }

        for (size_t k = 0; k < ids_.size(); k++)
        {
            Vector<properties::Color>& col_vec = colors_[k];

            drawMesh(marble_.x, marble_.y, marble_.z, marble_.indices, col_vec, ids_[k], properties::EdgeColor::NONE);
        }

        for (size_t k = 0; k < ids_.size(); k++)
        {
            MarbleState ms;

            ms.vel = {randFunc(), randFunc(), randFunc() + 1.0};
            // ms.vel = {0.0, 0.0, randFunc() + 1.0};
            ms.pos = {2.0 * randFunc(), 2.0 * randFunc(), randFunc() + 3.0};
            ms.dir.x = ms.vel.x;
            ms.dir.y = ms.vel.y;

            marble_states_.push_back(ms);
        }
    }

    ~Marbles() {}

    void updateMarbleStates(const double h)
    {
        for (size_t k = 0; k < ids_.size(); k++)
        {
            MarbleState& ms = marble_states_[k];

            const Vec3d acc = {0.0, 0.0, -9.81};
            ms.vel = ms.vel + acc * h;
            ms.pos = ms.pos + ms.vel * h;

            if (ms.pos.z < 0.0)
            {
                ms.pos.z = -ms.pos.z;
                ms.vel.z = -ms.vel.z;
            }
        }
    }

    double randFunc()
    {
        return 2.0 * (static_cast<double>(rand() % 1001) / 1000.0 - 0.5);
    }

    void run()
    {
        double theta = 0.0;
        const double h = 0.01;

        for (size_t i = 0; i < 1000; i++)
        {
            std::vector<PropertySet> props;

            updateMarbleStates(h);

            for (size_t k = 0; k < ids_.size(); k++)
            {
                MarbleState& ms = marble_states_[k];

                const double ang = std::atan2(ms.dir.y, ms.dir.x) + M_PI / 2.0;
                const Matrix<double> rot = rotationMatrixY<double>(theta) * rotationMatrixZ<double>(ang);

                props.emplace_back(ids_[k], properties::Transform{diagMatrix<double>({1.0, 1.0, 1.0}), rot, ms.pos});
            };

            setProperties(props);

            theta += 0.01;
            viewAngles(azimuth, 35);
            azimuth += 0.3;
            if (azimuth > 180.0)
            {
                azimuth = -179.0;
            }
            flushCurrentElement();
            usleep(1000U * 10U);
        }
    }
};

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

    plot(x, y, properties::Color(0, 0, 0), properties::ZOffset(-1.0f), properties::LineWidth(2.0f));
    // TODO: CHange line width for this demo:
    // shader_collection_.plot_2d_shader.uniform_handles.half_line_width.setFloat(line_width_ / 20.0f);
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    setActiveView("p_view_0");
    clearView();

    // globalIllumination({2.0, 2.0, 2.0});
    axesSquare();
    viewAngles(azimuth, 35);
    // waitForFlush();

    disableScaleOnRotation();
    axis({-5.0, -5.0, -1.0}, {5.0, 5.0, 9.0});
    // viewAngles(-90, 90);

    Marbles marbles(10, 4);
    drawGrid();
    marbles.run();
}

}  // namespace marbles
