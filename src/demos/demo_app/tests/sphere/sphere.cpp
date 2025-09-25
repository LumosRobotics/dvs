#include "tests/sphere/sphere.h"

#include <math.h>
#include <stdlib.h>

#include <iostream>

#include "debug_value_args.h"
#include "duoplot/duoplot.h"

using namespace duoplot;

namespace sphere
{

void Sphere::calculateRaiis()
{
    for (int k = 0; k < N_edges; k++)
    {
        const int idx1 = edgelist_idx1[k];
        const int idx2 = edgelist_idx2[k];

        const float p1 = P[idx1] - P[idx2];
        const float p2 = P[N + idx1] - P[N + idx2];
        const float p3 = P[2 * N + idx1] - P[2 * N + idx2];
        Rv[k] = sqrt(p1 * p1 + p2 * p2 + p3 * p3);
    }
}

void Sphere::update()
{
    Ac[0] = 0.0f;
    Ac[1] = 0.0f;
    Ac[2] = 0.0f;

    float r1, r2;
    float vproj[3];
    float vproj_center[3];
    Vpc[0] = 0.0f;
    Vpc[1] = 0.0f;
    Vpc[2] = 0.0f;
    for (k = 0; k < N; k++)
    {
        idx_0 = k;
        idx_1 = N + k;
        idx_2 = 2 * N + k;

        vp[0] = P[idx_0];
        vp[1] = P[idx_1];
        vp[2] = P[idx_2];

        a[0] = 0.0f;
        a[1] = 0.0f;
        a[2] = 0.0f;

        Vp[idx_0] = 0.0f;
        Vp[idx_1] = 0.0f;
        Vp[idx_2] = 0.0f;

        for (i = 0; i < number_of_connections[k]; i++)
        {
            cidx = subdivisor->connectivity_subdivided[k][i];

            v[0] = vp[0] - P[cidx];          // difference vector x
            v[1] = vp[1] - P[N + cidx];      // difference vector y
            v[2] = vp[2] - P[2 * N + cidx];  // difference vector z
            r0 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
            R = sqrt(r0);

            v[0] = v[0] / R;
            v[1] = v[1] / R;
            v[2] = v[2] / R;

            F = K * (Ls[i * N + k] - R);

            a[0] = a[0] + F * v[0];
            a[1] = a[1] + F * v[1];
            a[2] = a[2] + F * v[2];

            // vp = ((vv'*vc)/(vc'*vc))*vc;
            //  Project velocities onto connection vector

            r1 = (V[idx_0] * v[0] + V[idx_1] * v[1] + V[idx_2] * v[2]) / r0;
            r2 = (V[cidx] * v[0] + V[N + cidx] * v[1] + V[2 * N + cidx] * v[2]) / r0;
            r0 = r1 - r2;
            vproj[0] = r0 * v[0];
            vproj[1] = r0 * v[1];
            vproj[2] = r0 * v[2];

            Vp[idx_0] = Vp[idx_0] + vproj[0];
            Vp[idx_1] = Vp[idx_1] + vproj[1];
            Vp[idx_2] = Vp[idx_2] + vproj[2];
        }

        // Vector from point to center
        vc[0] = vp[0] - Pc[0];
        vc[1] = vp[1] - Pc[1];
        vc[2] = vp[2] - Pc[2];

        r0 = vc[0] * vc[0] + vc[1] * vc[1] + vc[2] * vc[2];  // Squared distance

        // vc is vector from center to point
        // Project outer point velocity and center point velocity onto vc

        // Center point velocity projected
        r1 = (Vc[0] * vc[0] + Vc[1] * vc[1] + Vc[2] * vc[2]) / r0;
        r2 = (V[idx_0] * vc[0] + V[idx_1] * vc[1] + V[idx_2] * vc[2]) / r0;
        r2 = r1 - r2;
        vproj_center[0] = r2 * vc[0];
        vproj_center[1] = r2 * vc[1];
        vproj_center[2] = r2 * vc[2];

        Vpc[0] = Vpc[0] + vproj_center[0];
        Vpc[1] = Vpc[1] + vproj_center[1];
        Vpc[2] = Vpc[2] + vproj_center[2];

        Rc = sqrt(vc[0] * vc[0] + vc[1] * vc[1] + vc[2] * vc[2]);

        vc[0] = vc[0] / Rc;
        vc[1] = vc[1] / Rc;
        vc[2] = vc[2] / Rc;

        Ft = Kc * (Lc - Rc);

        Fc[0] = Ft * vc[0];
        Fc[1] = Ft * vc[1];
        Fc[2] = Ft * vc[2];

        A[idx_0] = a[0] + Fc[0] + vproj_center[0] * Cc;
        A[idx_1] = a[1] + Fc[1] + vproj_center[1] * Cc;
        A[idx_2] = a[2] + Fc[2] + vproj_center[2] * Cc;

        Ac[0] = Ac[0] - Fc[0];
        Ac[1] = Ac[1] - Fc[1];
        Ac[2] = Ac[2] - Fc[2];

    }  // Looping over k

    Ac[0] = (Ac[0] - Ca * Vc[0] - Vpc[0] * Cc) / mc;
    Ac[1] = (Ac[1] - Ca * Vc[1] - Vpc[1] * Cc) / mc - g;
    Ac[2] = (Ac[2] - Ca * Vc[2] - Vpc[2] * Cc) / mc;

    Vc[0] = Vc[0] + h * Ac[0];
    Vc[1] = Vc[1] + h * Ac[1];
    Vc[2] = Vc[2] + h * Ac[2];

    Pc[0] = Pc[0] + h * Vc[0];
    Pc[1] = Pc[1] + h * Vc[1];
    Pc[2] = Pc[2] + h * Vc[2];

    // ---------------- FRICTION ----------------

    for (k = 0; k < N; k++)
    {
        idx_0 = k;
        idx_1 = N + k;
        idx_2 = 2 * N + k;

        if (P[idx_1] < friction_threshold)
        {
            v_speed = sqrt(V[idx_0] * V[idx_0] + V[idx_2] * V[idx_2]);
            if (v_speed > speed_threshold)
            {
                v_0 = V[idx_0] / v_speed;
                v_2 = V[idx_2] / v_speed;

                A[idx_0] = A[idx_0] - v_0 * mu;
                A[idx_2] = A[idx_2] - v_2 * mu;
            }
        }  // If P[idx_1] < friction_threshold

    }  // Looping over k for friction

    // ---------------- FRICTION ----------------

    if (Pc[1] < 0.0f)
    {
        Pc[1] = 0.0f;
        Vc[1] = -Vc[1];
    }

    for (int k = 0; k < N; k++)
    {
        idx_0 = k;
        idx_1 = N + k;
        idx_2 = 2 * N + k;
        A[idx_0] = (A[idx_0] - Ca * V[idx_0] - C * Vp[idx_0]) / m;
        A[idx_1] = (A[idx_1] - Ca * V[idx_1] - C * Vp[idx_1]) / m;
        A[idx_2] = (A[idx_2] - Ca * V[idx_2] - C * Vp[idx_2]) / m;

        A[idx_1] = A[idx_1] - g;

        V[idx_0] = V[idx_0] + h * A[idx_0];
        V[idx_1] = V[idx_1] + h * A[idx_1];
        V[idx_2] = V[idx_2] + h * A[idx_2];

        P[idx_0] = P[idx_0] + h * V[idx_0];
        P[idx_1] = P[idx_1] + h * V[idx_1];
        P[idx_2] = P[idx_2] + h * V[idx_2];

        if (P[idx_1] < 0.0f)
        {
            P[idx_1] = 0.0f;
            V[idx_1] = -V[idx_1];
        }

        if (P[idx_0] < -room_limit)
        {
            P[idx_0] = -room_limit;
            V[idx_0] = -V[idx_0];
        }
        else if (P[idx_0] > room_limit)
        {
            P[idx_0] = room_limit;
            V[idx_0] = -V[idx_0];
        }
        if (P[idx_2] < -room_limit)
        {
            P[idx_2] = -room_limit;
            V[idx_2] = -V[idx_2];
        }
        else if (P[idx_2] > room_limit)
        {
            P[idx_2] = room_limit;
            V[idx_2] = -V[idx_2];
        }
    }
}

void Sphere::set_velocity(float dir[3], float amp)
{
    float r_ = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
    dir[0] = amp * dir[0] / r_;
    dir[1] = amp * dir[1] / r_;
    dir[2] = amp * dir[2] / r_;

    for (int c = 0; c < N; c++)
    {
        V[c] = V[c] + dir[0];
        V[N + c] = V[N + c] + dir[1];
        V[2 * N + c] = V[2 * N + c] + dir[2];
    }
}

Sphere::Sphere() {}

Sphere::Sphere(float m_,
               float C_,
               float K_,
               float h_,
               float Kc_,
               float Cc_,
               float mc_,
               float Lc_,
               float Ca_,
               float x0,
               float y0,
               float z0,
               float vx0,
               float vy0,
               float vz0,
               int num_subdivisions)
{
    subdivisor = new Subdivisor(num_subdivisions, Lc_);

    m = m_;
    C = C_;
    K = K_;
    h = h_;
    Ca = Ca_;

    g = 9.82 * 10.0f;
    room_limit = 10.0f;

    Kc = Kc_;  // Spring constant
    Cc = Cc_;  // Dampening constant
    mc = mc_;  // Mass
    Lc = Lc_;  // Length

    friction_threshold = 0.3f;
    speed_threshold = 0.1f;
    mu = 20.0f;
    Nf = 0.0f;

    N = subdivisor->P_subdivided.size();

    A = new float[3 * N];
    V = new float[3 * N];  // Set to zero
    memset(V, 0.0f, sizeof(float) * 3 * N);
    P = new float[3 * N];

    Ac = new float[3];
    Vc = new float[3];
    Pc = new float[3];

    Vp = new float[3 * N];
    Vpc = new float[3];

    Vc[0] = 0.0f;
    Vc[1] = 0.0f;
    Vc[2] = 0.0f;

    Pc[0] = 0.0f;
    Pc[1] = y0;
    Pc[2] = 0.0f;

    for (int c = 0; c < N; c++)
    {
        Point p = subdivisor->P_subdivided[c];
        P[c] = p.x + x0;
        P[N + c] = p.y + y0;
        P[2 * N + c] = p.z + z0;
    }

    // Lengths of connecting edges, at least 5, at most 6 per point
    Ls = new float[6 * N];
    N_edges = subdivisor->edgelist_subdivided.edges.size();
    edgelist_idx1 = new int[N_edges];
    edgelist_idx2 = new int[N_edges];

    for (int k = 0; k < N_edges; k++)
    {
        edgelist_idx1[k] = subdivisor->edgelist_subdivided.edges[k].i1;
        edgelist_idx2[k] = subdivisor->edgelist_subdivided.edges[k].i2;
    }

    number_of_connections = new int[N];

    for (int k = 0; k < N; k++)
    {
        idx_0 = k;
        idx_1 = N + k;
        idx_2 = 2 * N + k;
        vp[0] = P[idx_0];
        vp[1] = P[idx_1];
        vp[2] = P[idx_2];
        number_of_connections[k] = subdivisor->num_edges_subdivided[k];

        for (int i = 0; i < number_of_connections[k]; i++)
        {
            cidx = subdivisor->connectivity_subdivided[k][i];
            v[0] = vp[0] - P[0 * N + cidx];
            v[1] = vp[1] - P[1 * N + cidx];
            v[2] = vp[2] - P[2 * N + cidx];
            Ls[i * N + k] = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }
    }

    float v[3];
    const Vec3f v3{vx0, vy0, vz0};
    v[0] = v3.x;
    v[1] = v3.y;
    v[2] = v3.z;
    set_velocity(v, v3.norm());
}

Sphere::~Sphere()
{
    delete[] number_of_connections;
    delete[] Ls;
    delete[] edgelist_idx1;
    delete[] edgelist_idx2;
    delete[] A;
    delete[] V;
    delete[] P;
    delete[] Ac;
    delete[] Vc;
    delete[] Pc;
    delete[] Vp;
    delete[] Vpc;
    delete subdivisor;
}

void testBasic()
{
    float c = 4.0f;          // Exterior dampening
    float cc = 4.0f;         // Center to exterior dampening
    float ca = 0.0f * 1.0f;  // Air resistance
    float m = 0.05f;         // Exterior mass
    float mc = 1.0f;         // Center mass
    float x0 = debug_value_args::getValue<float>("x0", 0.0f);
    float y0 = debug_value_args::getValue<float>("y0", 5.0f);
    float z0 = debug_value_args::getValue<float>("z0", 0.0f);
    float vx0 = debug_value_args::getValue<float>("vx0", 0.0f);
    float vy0 = debug_value_args::getValue<float>("vy0", 0.0f);
    float vz0 = debug_value_args::getValue<float>("vz0", 0.0f);
    float K = debug_value_args::getValue<float>("K", 10000.0f);    // Exterior spring constant
    float Kc = debug_value_args::getValue<float>("Kc", 10000.0f);  // Center to exterior spring constant
    float h = 0.0001f;                                             // Time step
    float Lc = 3.0f;                                               // Length between center and exterior
    int num_subdivisions = 3;

    Sphere sphere(m, c, K, h, Kc, cc, mc, Lc, ca, x0, y0, z0, vx0, vy0, vz0, num_subdivisions);

    const size_t num_points = sphere.N;

    const VectorConstView<float> x{sphere.P, num_points};
    const VectorConstView<float> y{sphere.P + num_points, num_points};
    const VectorConstView<float> z{sphere.P + 2U * num_points, num_points};

    Vector<float> xl{sphere.subdivisor->edgelist_subdivided.edges.size() * 2U};
    Vector<float> yl{sphere.subdivisor->edgelist_subdivided.edges.size() * 2U};
    Vector<float> zl{sphere.subdivisor->edgelist_subdivided.edges.size() * 2U};

    xl.fill(0.0f);
    yl.fill(0.0f);
    zl.fill(0.0f);

    setActiveView("p_view_0");
    clearView();
    disableScaleOnRotation();
    disableAutomaticAxesAdjustment();
    axesSquare();
    viewAngles(40, 40);
    waitForFlush();

    float zv0 = 3.0f;
    axis({-3.0, -3.0, zv0 - 3.0}, {3.0, 3.0, zv0 + 3.0});

    for (size_t k = 0; k < 1000; k++)
    {
        for (size_t u = 0; u < 50; u++)
        {
            sphere.update();
        }

        size_t idx = 0;
        for (int kl = 0; kl < sphere.subdivisor->edgelist_subdivided.edges.size(); kl++)
        {
            const int idx1 = sphere.subdivisor->edgelist_subdivided.edges[kl].i1;
            const int idx2 = sphere.subdivisor->edgelist_subdivided.edges[kl].i2;

            const Vec3f p0{sphere.P[idx1], sphere.P[num_points + idx1], sphere.P[2U * num_points + idx1]};
            const Vec3f p1{sphere.P[idx2], sphere.P[num_points + idx2], sphere.P[2U * num_points + idx2]};

            xl(idx) = p0.x;
            yl(idx) = p0.y;
            zl(idx) = p0.z;

            idx += 1;

            xl(idx) = p1.x;
            yl(idx) = p1.y;
            zl(idx) = p1.z;

            idx += 1;
        }

        lineCollection3(xl, zl, yl, properties::Color::BLACK);
        scatter3(x,
                 z,
                 y,
                 properties::DistanceFrom::xyz({0, 1, 2}, 0.0, 4.0),
                 properties::ColorMap::MAGMA,
                 properties::PointSize(13),
                 properties::ScatterStyle::DISC);

        flushCurrentElement();

        softClearView();
    }
}

}  // namespace sphere
