#ifndef DEMOS_TESTS_PARTICLES_DENSITY_H
#define DEMOS_TESTS_PARTICLES_DENSITY_H

#include <Box2D/Box2D.h>
#include <Box2D/Particle/b2Particle.h>

#include <fstream>
#include <nlohmann/json.hpp>

#include "lumos/plotting/duoplot.h"
#include "misc/rgb_triplet.h"

using namespace lumos;

namespace particles_density
{

struct ParticleEjector;

void plotBoundaries(const Vector<Point2f>& vertices, const properties::Color& c);
void scatterBoundaries(const Vector<Point2f>& vertices, const properties::Color& c);

class ParticleSystem
{
public:
    size_t num_particles_;
    Vec2f min_bnd_;
    Vec2f max_bnd_;
    float* x_pos_;
    float* y_pos_;

    b2World world_;
    b2ParticleSystem* particle_system_;

    Vector<b2Vec2> vertices_;
    b2Body* ground_;
    b2ParticleGroup* group_;
    std::vector<b2ParticleGroup*> groups_;
    std::vector<ParticleEjector> ejectors_;

    void changeDensity();


    ParticleSystem() = delete;
    ParticleSystem(const Vec2f min_bnd, const Vec2f max_bnd);
    ~ParticleSystem();

    size_t numPoints() const;

    const b2Vec2* getParticles() const;

    void update(const double dt);

    VectorConstView<float> getXView() const;
    VectorConstView<float> getYView() const;
};

void testBasic();
void runTest();
void testBasicOld();
void saveToFile();
}  // namespace particles_density

#endif  // DEMOS_TESTS_PARTICLES_DENSITY_H
