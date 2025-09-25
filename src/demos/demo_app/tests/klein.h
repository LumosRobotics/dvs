#ifndef DEMOS_TESTS_KLEIN_H_
#define DEMOS_TESTS_KLEIN_H_

#include "duoplot/duoplot.h"

using namespace duoplot;

namespace klein
{

struct KleinParams
{
    struct XParams
    {
        float a;
        float b;
        float c;
        float d;
        float e;
        float f;
    };
    struct YParams
    {
        float a;
        float b;
        float c;
        float d;
        float e;
        float f;
        float g;
        float h;
        float i;
        float j;
    };
    struct ZParams
    {
        float a;
        float b;
        float c;
    };

    XParams x;
    YParams y;
    ZParams z;

    KleinParams()
    {
        x.a = -2.0f / 15.0f;
        x.b = 3.0f;
        x.c = 30.0f;
        x.d = 90.0f;
        x.e = 60.0f;
        x.f = 5.0f;

        y.a = -1.0 / 15.0f;
        y.b = 3.0f;
        y.c = 3.0f;
        y.d = 48.0f;
        y.e = 48.0f;
        y.f = 60.0f;
        y.g = 5.0f;
        y.h = 5.0f;
        y.i = 80.0f;
        y.j = 80.0f;

        z.a = -2.0 / 15.0f;
        z.b = 3.0f;
        z.c = 5.0f;
    }
};

void evalKlein(const Matrix<float>& u,
               const Matrix<float>& v,
               Matrix<float>& x,
               Matrix<float>& y,
               Matrix<float>& z,
               const float u_offset,
               const float v_offset,
               const KleinParams& klein_params,
               const Matrix<float>& r_mat)
{
    const size_t nu = u.numRows();
    const size_t nv = u.numCols();

    const KleinParams& kp = klein_params;

    for (size_t ku = 0; ku < nu; ku++)
    {
        for (size_t kv = 0; kv < nv; kv++)
        {
            const float u_val = u(ku, kv) + u_offset;
            const float v_val = v(ku, kv) + v_offset;

            const float cos_u = std::cos(u_val);
            const float sin_u = std::sin(u_val);
            const float cos_v = std::cos(v_val);
            const float sin_v = std::sin(v_val);

            const float cos2_u = cos_u * cos_u;
            const float cos3_u = cos2_u * cos_u;
            const float cos4_u = cos3_u * cos_u;
            const float cos5_u = cos4_u * cos_u;
            const float cos6_u = cos5_u * cos_u;
            const float cos7_u = cos6_u * cos_u;

            Point3f p;

            p.x = kp.x.a * cos_u *
                  (kp.x.b * cos_v - kp.x.c * sin_u + kp.x.d * cos4_u * sin_u - kp.x.e * cos6_u * sin_u +
                   kp.x.f * cos_u * cos_v * sin_u);

            p.y = kp.y.a * sin_u *
                  (kp.y.b * cos_v - kp.y.c * cos2_u * cos_v - kp.y.d * cos4_u * cos_v + kp.y.e * cos6_u * cos_v -
                   kp.y.f * sin_u + kp.y.g * cos_u * cos_v * sin_u - kp.y.h * cos3_u * cos_v * sin_u -
                   kp.y.i * cos5_u * cos_v * sin_u + kp.y.j * cos7_u * cos_v * sin_u);

            p.z = kp.z.a * (kp.z.b + kp.z.c * cos_u * sin_u) * sin_v;
            const Point3f pr = r_mat * p;
            x(ku, kv) = pr.x;
            y(ku, kv) = pr.y;
            z(ku, kv) = pr.z;
        }
    }
}

void testBasic()
{
    const size_t nu = 50, nv = 50;

    std::pair<Matrix<float>, Matrix<float>> uv_mats = meshGrid<float>(0.0f, M_PI, 0.0f, 2.0f * M_PI, nv, nu);

    Matrix<float> x(nu, nv), y(nu, nv), z(nu, nv);

    Matrix<float>& u_mat = uv_mats.first;
    Matrix<float>& v_mat = uv_mats.second;

    KleinParams klein_params{};

    VectorConstView<float> vx(x.data(), nu * nv);
    VectorConstView<float> vy(y.data(), nu * nv);
    VectorConstView<float> vz(z.data(), nu * nv);

    setActiveView("p_view_0");
    clearView();

    evalKlein(u_mat, v_mat, x, y, z, 0.0f, 0.0f, klein_params, unitMatrix<float>(3, 3));

    scatter3(vx, vy, vz, properties::Color::RED);
    plot3(vx, vy, vz, properties::Color::BLUE);
}

void testAdvanced0()
{
    const size_t nu = 50, nv = 50;
    const size_t num_its = 100;

    std::pair<Matrix<float>, Matrix<float>> uv_mats = meshGrid<float>(0.0f, M_PI, 0.0f, 2.0f * M_PI, nv, nu);

    Matrix<float> x(nu, nv), y(nu, nv), z(nu, nv);

    Matrix<float>& u_mat = uv_mats.first;
    Matrix<float>& v_mat = uv_mats.second;

    KleinParams klein_params{};

    VectorConstView<float> vx(x.data(), nu * nv);
    VectorConstView<float> vy(y.data(), nu * nv);
    VectorConstView<float> vz(z.data(), nu * nv);

    setActiveView("p_view_0");
    clearView();
    waitForFlush();
    float u_offset = 0.0f, v_offset = 0.0f;

    for (size_t k = 0; k < num_its; k++)
    {
        evalKlein(u_mat, v_mat, x, y, z, u_offset, v_offset, klein_params, unitMatrix<float>(3, 3));
        u_offset += 0.1;

        scatter3(vx, vy, vz, properties::Color::RED);
        plot3(vx, vy, vz, properties::Color::BLUE);
        // axis({-130.0, 0.0, -1.0}, {110.0, 4.0, 1.0});
        flushCurrentElement();

        usleep(50000);

        softClearView();
    }
}

void testAdvanced1()
{
    const size_t nu = 50, nv = 50;
    const size_t num_triangles = nu * nv * 2;
    const size_t num_its = 1;

    Vector<IndexTriplet> indices(num_triangles);

    std::pair<Matrix<float>, Matrix<float>> uv_mats = meshGrid<float>(0.0f, M_PI, 0.0f, 2.0f * M_PI, nv, nu);

    Matrix<float> x(nu, nv), y(nu, nv), z(nu, nv);

    Matrix<float>& u_mat = uv_mats.first;
    Matrix<float>& v_mat = uv_mats.second;

    KleinParams klein_params{};

    VectorConstView<float> vx(x.data(), nu * nv);
    VectorConstView<float> vy(y.data(), nu * nv);
    VectorConstView<float> vz(z.data(), nu * nv);

    indices.fill(IndexTriplet{0, 0, 0});

    size_t idx = 0U;
    for (size_t ku = 0; ku < (nu - 1U); ku++)
    {
        for (size_t kv = 0; kv < (nv - 1U); kv++)
        {
            const size_t vec_idx = ku * nv + kv;
            indices(idx).i0 = vec_idx;
            indices(idx).i1 = vec_idx + 1;
            indices(idx).i2 = vec_idx + nv;

            idx += 1;

            indices(idx).i0 = vec_idx + nv + 1;
            indices(idx).i1 = vec_idx + nv;
            indices(idx).i2 = vec_idx + 1;

            idx += 1;
        }
    }

    setActiveView("p_view_0");
    clearView();
    float u_offset = 0.0f, v_offset = 0.0f;

    float t = 0.0f;

    float ax = 0.0f, ay = 0.0f, az = 0.0f;

    globalIllumination({2, 2, 2});

    for (size_t k = 0; k < num_its; k++)
    {
        const Matrix<float> r_mat = rotationMatrixZ(az) * rotationMatrixY(ay) * rotationMatrixX(ax);
        // const Matrix<float> r_mat = unitMatrix<float>(3, 3);
        evalKlein(u_mat, v_mat, x, y, z, u_offset, v_offset, klein_params, r_mat);
        // drawMesh(vx, vy, vz, indices.view(), properties::EdgeColor::NONE, properties::FaceColor(255, 0, 74));
        // drawMesh(vx, vy, vz, indices.view(), properties::EdgeColor(0, 255, 0), properties::FaceColor(255, 0, 74));
        // drawMesh(vx, vy, vz, indices.view(), properties::EdgeColor::NONE, properties::ColorMap::HSV);
        // drawMesh(vx, vy, vz, indices.view(), properties::EdgeColor::NONE, properties::ColorMap::JET);
        // drawMesh(vx, vy, vz, indices.view(), properties::EdgeColor::NONE, properties::ColorMap::VIRIDIS);
        drawMesh(vx, vy, vz, indices.constView(), properties::EdgeColor::NONE, properties::ColorMap::MAGMA);
        // drawMesh(vx, vy, vz, indices.view(), properties::EdgeColor(0, 255, 0), properties::FaceColor::None());
        // drawMesh(vx, vy, vz, indices.view(), properties::EdgeColor::NONE, properties::FaceColor(233, 116, 74));

        // ax += 0.05;
        // ay += 0.02;

        // klein_params.x.a = KleinParams().x.a + std::sin(t) * 0.1;

        // u_offset += 0.01;

        t += 0.02f;
        ax = 6.0 * sin(t);
        ay = 6.0 * cos(t);

        axis(Vec3d{-3.5, 0.0, -2.5}, Vec3d{3.5, 5.0, 2.5});
        // azimuth = azimuth > 180.0f ? -180.0f : azimuth + 0.2f;
        // view(azimuth, 30);
        usleep(10000);

        softClearView();
    }
}

}  // namespace klein

#endif  // DEMOS_TESTS_KLEIN_H_
