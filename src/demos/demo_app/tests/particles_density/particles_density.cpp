#include "particles_density.h"

#include <array>

#include "tests/slam/color_maps.h"

namespace particles_density
{

constexpr size_t num_steps = 3200;
const Vec2f min_bnd{-4.0f, 0.0f};
const Vec2f max_bnd{4.0f, 26.0f};
constexpr float radius = 0.025f;
float start_y = -10.0f;
constexpr size_t kNumDensities{7U};

constexpr size_t kNumRows{781U};
constexpr size_t kNumCols{456U};

constexpr int32_t kKernelSize{15};
constexpr int32_t kHalfKernelSize{kKernelSize / 2};

using ArrayType = std::array<float, kNumDensities>;

struct ParticleEjector
{
    b2ParticleGroup* group_;
    b2ParticleSystem* particle_system_;
    size_t num_particles_left_;
    size_t idx_;
    size_t offset_;
    Point2f eject_pos_;
    Vec2f eject_dir_;
    float angle_;
    float cos_angle_;
    float sin_angle_;
    b2Vec2* particles_;
    b2Vec2* velocities_;

    ParticleEjector() : group_{nullptr} {}
    ParticleEjector(b2ParticleGroup* group,
                    b2ParticleSystem* particle_system,
                    const Point2f& eject_pos,
                    const Vec2f& eject_dir)
        : group_{group}, particle_system_{particle_system}, eject_pos_{eject_pos}, eject_dir_{eject_dir}
    {
        particles_ = particle_system_->GetPositionBuffer();
        velocities_ = particle_system_->GetVelocityBuffer();

        num_particles_left_ = group_->GetParticleCount();
        idx_ = 0U;
        offset_ = group_->GetBufferIndex();
        angle_ = -std::atan2(eject_dir_.y, eject_dir_.x);
        cos_angle_ = std::cos(angle_);
        sin_angle_ = std::sin(angle_);

        for (size_t k = 0; k < num_particles_left_; k++)
        {
            particles_[offset_ + k].x = k * 10.0f;
            particles_[offset_ + k].y = start_y;

            velocities_[offset_ + k].x = 0.0f;
            velocities_[offset_ + k].y = 0.0f;
        }

        start_y = start_y - 5.0f;
    }

    void eject()
    {
        if (num_particles_left_ > 0)
        {
            constexpr float delta = 0.05;
            constexpr size_t num_particles_to_emit = 5U;
            constexpr float full_width = static_cast<float>(num_particles_to_emit - 1U) * delta;
            float d = -full_width / 2.0f;

            for (size_t k = 0; k < num_particles_to_emit && num_particles_left_ > 0; k++)
            {
                const float y = d;
                d += delta;

                const float xr = sin_angle_ * y;
                const float yr = cos_angle_ * y;

                particles_[offset_ + idx_].x = eject_pos_.x + xr;
                particles_[offset_ + idx_].y = eject_pos_.y + yr;

                velocities_[offset_ + idx_].x = eject_dir_.x;
                velocities_[offset_ + idx_].y = eject_dir_.y;

                idx_++;
                num_particles_left_--;
            }
        }
    }
};

void ParticleSystem::changeDensity()
{
    auto& heaviest_grp = groups_.back();

    const size_t offset = heaviest_grp->GetBufferIndex();
    const size_t num_particles = heaviest_grp->GetParticleCount();

    float* densities = particle_system_->GetDensityBuffer();
    for (size_t k = 0; k < num_particles; k++)
    {
        densities[k + offset] = 0.0005;
    }
}

ParticleSystem::ParticleSystem(const Vec2f min_bnd, const Vec2f max_bnd)
    : min_bnd_{min_bnd}, max_bnd_{max_bnd}, world_{b2Vec2{0.0, -9.82}}
{
    b2ParticleSystemDef particleSystemDef;

    particleSystemDef.radius = radius;
    particleSystemDef.dampingStrength = 0.01;
    particleSystemDef.gravityScale = 0.15;
    particleSystemDef.density = 0.001;

    particle_system_ = world_.CreateParticleSystem(&particleSystemDef);

    b2BodyDef bd;
    ground_ = world_.CreateBody(&bd);

    {
        b2PolygonShape shape;
        const Vector<b2Vec2> vertices{VectorInitializer{b2Vec2(-4, -2), b2Vec2(4, -2), b2Vec2(-4, -4)}};
        shape.Set(vertices.data(), vertices.size());
        ground_->CreateFixture(&shape, 0.0f);
    }

    {
        b2PolygonShape shape;
        const Vector<b2Vec2> vertices{VectorInitializer{b2Vec2(-4, 18), b2Vec2(-2, 2), b2Vec2(-2, -2)}};
        shape.Set(vertices.data(), vertices.size());
        ground_->CreateFixture(&shape, 0.0f);
    }

    {
        b2PolygonShape shape;
        const Vector<b2Vec2> vertices{VectorInitializer{b2Vec2(4, 18), b2Vec2(2, 2), b2Vec2(2, -2)}};
        shape.Set(vertices.data(), vertices.size());
        ground_->CreateFixture(&shape, 0.0f);
    }

    {
        b2PolygonShape shape;
        const Vector<b2Vec2> vertices{VectorInitializer{b2Vec2(-2, 4.85), b2Vec2(2, 4.85), b2Vec2(0, 10)}};
        shape.Set(vertices.data(), vertices.size());
        ground_->CreateFixture(&shape, 0.0f);
    }

    b2PolygonShape shape;
    shape.SetAsBox(0.7f, 0.7f);

    b2ParticleGroupDef particleGroupDef;
    particleGroupDef.flags = b2_waterParticle;
    particleGroupDef.position.Set(-1.0f, 4.0f);
    particleGroupDef.shape = &shape;
    particleGroupDef.strength = 0.1;

    // std::vector<float> densities{0.001, 0.003, 0.005, 0.007, 0.09, 0.01, 0.05};
    std::vector<float> densities{0.001, 0.005, 0.01, 0.05, 0.09, 0.12, 0.15};
    assert(densities.size() == kNumDensities);

    float y_pos = 3.0;
    for (float density : densities)
    {
        particleGroupDef.density = density;
        particleGroupDef.position.Set(1.0f, y_pos);
        groups_.push_back(particle_system_->CreateParticleGroup(particleGroupDef));
        y_pos += 1.5f;
    }

    num_particles_ = particle_system_->GetParticleCount();

    for (auto& grp : groups_)
    {
        const float px = (static_cast<float>(rand() % 1001U) / 1000.0f - 0.5f) * 2.0f;
        const float py = (static_cast<float>(rand() % 1001U) / 1000.0f - 0.5f) * 2.0f + 1.0;

        const float theta = -2.0f * M_PI * static_cast<float>(rand() % 1001U) / 1000.0f;

        const float vx = std::cos(theta) * 5.0f;
        const float vy = std::sin(theta) * 5.0f;
        ejectors_.push_back(ParticleEjector{grp, particle_system_, Point2f{px, py}, Vec2f{vx, vy}});
    }

    x_pos_ = new float[particle_system_->GetParticleCount()];
    y_pos_ = new float[particle_system_->GetParticleCount()];
}

ParticleSystem::~ParticleSystem() {}

size_t ParticleSystem::numPoints() const
{
    return num_particles_;
}

const b2Vec2* ParticleSystem::getParticles() const
{
    return particle_system_->GetPositionBuffer();
}

void ParticleSystem::update(const double dt)
{
    for (auto& ej : ejectors_)
    {
        ej.eject();
    }
    const b2Vec2* const particles = particle_system_->GetPositionBuffer();

    const size_t num_particles = particle_system_->GetParticleCount();

    for (size_t k = 0; k < num_particles; k++)
    {
        x_pos_[k] = particles[k].x;
        y_pos_[k] = particles[k].y;
    }

    world_.Step(dt, 8, 3);
}

VectorConstView<float> ParticleSystem::getXView() const
{
    return VectorConstView<float>{x_pos_, num_particles_};
}

VectorConstView<float> ParticleSystem::getYView() const
{
    return VectorConstView<float>{y_pos_, num_particles_};
}

properties::Color toColor(const RGBTripletf& col)
{
    return properties::Color{static_cast<std::uint8_t>(col.red * 255.0f),
                             static_cast<std::uint8_t>(col.green * 255.0f),
                             static_cast<std::uint8_t>(col.blue * 255.0f)};
}

Matrix<float> createKernel(const size_t size, const float s, const float a)
{
    assert((size % 2) == 1);

    const int32_t size_32 = size;
    const int32_t half_size = size_32 / 2;

    Matrix<float> kernel{size, size};

    for (int32_t x = -half_size; x <= half_size; x++)
    {
        for (int32_t y = -half_size; y <= half_size; y++)
        {
            const float r = static_cast<float>(x * x + y * y);
            kernel(y + half_size, x + half_size) = a * std::exp(-s * r);
        }
    }

    return kernel;
}

void renderImage(const ParticleSystem& ps,
                 ImageRGBA<uint8_t>& img,
                 const Vec2f& scale,
                 const Vec2f& offset,
                 const std::vector<properties::Color>& colors,
                 Matrix<ArrayType>& count_mat,
                 const Matrix<float>& kernel)
{
    const int32_t num_rows = img.numRows();
    const int32_t num_cols = img.numCols();

    const float num_rows_1 = num_rows - 1;
    const float num_cols_1 = num_cols - 1;

    const Vec2f half_scale = scale / 2.0f;

    size_t idx = 0U;

    const VectorConstView<float> x = ps.getXView();
    const VectorConstView<float> y = ps.getYView();

    for (const auto grp : ps.groups_)
    {
        const size_t num_particles = grp->GetParticleCount();
        const size_t offset_idx = grp->GetBufferIndex();

        const auto col = colors[idx];

        const VectorConstView<float> xg = VectorConstView<float>{x.data() + offset_idx, num_particles};
        const VectorConstView<float> yg = VectorConstView<float>{y.data() + offset_idx, num_particles};

        const properties::Color cl = colors[idx];

        for (size_t k = 0; k < num_particles; k++)
        {
            const int32_t center_col = (num_cols_1 * (xg(k) - offset.x) / scale.x + 0.5f);
            const int32_t center_row = num_rows_1 - (num_rows_1 * (yg(k) - offset.y) / scale.y + 0.5f);

            for (int32_t x = -kHalfKernelSize; x <= kHalfKernelSize; x++)
            {
                for (int32_t y = -kHalfKernelSize; y <= kHalfKernelSize; y++)
                {
                    const int32_t column = center_col + x;
                    const int32_t row = center_row + y;

                    if (column >= 0 && column < num_cols && row >= 0 && row < num_rows)
                    {
                        ArrayType& current_array{count_mat(row, column)};
                        current_array[idx] += kernel(y + kHalfKernelSize, x + kHalfKernelSize);
                    }
                }
            }
        }

        idx++;
    }

    for (size_t r = 0; r < num_rows; r++)
    {
        for (size_t c = 0; c < num_cols; c++)
        {
            const ArrayType& current_array{count_mat(r, c)};
            // Find max and max index
            float max_val = 0.0f;
            size_t max_idx = 0U;
            for (size_t k = 0; k < kNumDensities; k++)
            {
                if (current_array[k] > max_val)
                {
                    max_val = current_array[k];
                    max_idx = k;
                }
            }

            if (max_val > 1.0f)
            {
                img(r, c, 0) = colors[max_idx].red;
                img(r, c, 1) = colors[max_idx].green;
                img(r, c, 2) = colors[max_idx].blue;
                img(r, c, 3) = 255U;
            }
        }
    }
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";

    ImageRGBA<uint8_t> img{kNumRows, kNumCols};
    ImageRGBA<uint8_t> blank_img{kNumRows, kNumCols};
    Matrix<ArrayType> count_matrix{kNumRows, kNumCols};
    Matrix<ArrayType> blank_count_matrix{kNumRows, kNumCols};

    for (size_t r = 0; r < kNumRows; r++)
    {
        for (size_t c = 0; c < kNumCols; c++)
        {
            blank_img(r, c, 0) = 0U;
            blank_img(r, c, 1) = 0U;
            blank_img(r, c, 2) = 0U;
            blank_img(r, c, 3) = 0U;

            for (size_t k = 0; k < kNumDensities; k++)
            {
                blank_count_matrix(r, c)[k] = 0.0f;
            }
        }
    }

    openProjectFile(project_file_path);

    const Matrix<float> kernel{createKernel(kKernelSize, 0.1f, 1.0f)};
    std::pair<Matrix<float>, Matrix<float>> xy_mats =
        meshGrid<float>(-1.0f, 1.0f, -1.0f, 1.0f, kKernelSize, kKernelSize);

    setActiveView("p_view_0");
    clearView();

    waitForFlush();
    setAxesBoxScaleFactor({1.0, 1.0, 1.0});

    // axis({0.0, 0.0}, {kNumCols, kNumRows});
    axis({-2.0, -3.425438 + 1.425438}, {2.0, 3.425438 + 1.425438});
    const float x_interval = 4.0f;
    const float y_interval = 2.0 * 3.425438;

    const float x_min = -2.0;
    const float y_min = -3.425438 + 1.425438;

    ParticleSystem ps{min_bnd, max_bnd};

    const VectorConstView<float> x = ps.getXView();
    const VectorConstView<float> y = ps.getYView();

    std::vector<properties::Color> colors;

    for (size_t k = 0; k < ps.groups_.size(); k++)
    {
        const float d = static_cast<float>(k) / static_cast<float>(ps.groups_.size() - 1U);
        colors.push_back(toColor(calculateColormapJetSoft(d)));
    }

    for (size_t k = 0; k < num_steps; k++)
    {
        ps.update(0.01);

        size_t idx = 0U;

        if (k < 1200)
        {
            for (const auto grp : ps.groups_)
            {
                const size_t num_particles = grp->GetParticleCount();

                const size_t offset = grp->GetBufferIndex();

                const auto col = colors[idx];

                const VectorConstView<float> xg = VectorConstView<float>{x.data() + offset, num_particles};
                const VectorConstView<float> yg = VectorConstView<float>{y.data() + offset, num_particles};

                scatter(xg,
                        yg,
                        properties::ScatterStyle::DISC,
                        properties::PointSize(10),
                        col,
                        properties::Silhouette{0U, 0U, 0U, 0.1f});

                idx++;
            }
        }
        else
        {
            if (k == 1400)
            {
                ps.changeDensity();
            }
            count_matrix = blank_count_matrix;
            img = blank_img;
            renderImage(ps, img, {x_interval, y_interval}, {x_min, y_min}, colors, count_matrix, kernel);

            imShow(img,
                   properties::Transform{
                       diagMatrix<double>({x_interval / kNumCols, y_interval / kNumRows, 1.0}),
                       rotationMatrixZ<double>(0),
                       {-x_interval / 2.0f, -(-4.85 * 0.0 + -3.425438 * 0.0 + 0.0 * 1.425438 + 2.0), 0.0}});
        }
        flushCurrentElement();
        clearViewOnUpdate();
    }
}

}  // namespace particles_density
