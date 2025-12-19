#include "tests/spring/spring.h"

#include <math.h>
#include <stdlib.h>

#include <iostream>
#include <string>
#include <vector>

#include "lumos/plotting/duoplot.h"

using namespace lumos;

namespace spring_structure
{

std::vector<std::vector<float>> readPositions(std::string file_name)
{
    std::ifstream read_file;
    read_file.open(file_name);

    std::string s;
    std::string delimiter = ",";
    std::string token;
    size_t pos = 0;
    std::vector<std::string> substrings;
    std::vector<float> floatcols;
    std::vector<std::vector<float>> floatdata;

    if (read_file.is_open())
    {
        while (!read_file.eof())
        {
            read_file >> s;

            if (read_file.eof())
                break;
            substrings.clear();
            floatcols.clear();

            while ((pos = s.find(delimiter)) != std::string::npos)
            {
                token = s.substr(0, pos);
                substrings.push_back(token);
                s.erase(0, pos + delimiter.length());
            }
            substrings.push_back(s);

            for (int i = 0; i < substrings.size(); i++)
            {
                floatcols.push_back(atof(substrings[i].c_str()));
            }

            floatdata.push_back(floatcols);
        }
    }
    read_file.close();

    return floatdata;
}

std::vector<std::vector<int>> readConnectivityGraph(std::string file_name)
{
    std::ifstream read_file;
    read_file.open(file_name);

    std::string s;
    std::string delimiter = ",";
    std::string token;
    size_t pos = 0;
    std::vector<std::string> substrings;
    std::vector<int> intcols;
    std::vector<std::vector<int>> intdata;

    if (read_file.is_open())
    {
        while (!read_file.eof())
        {
            read_file >> s;

            if (read_file.eof())
                break;
            substrings.clear();
            intcols.clear();

            while ((pos = s.find(delimiter)) != std::string::npos)
            {
                token = s.substr(0, pos);
                substrings.push_back(token);
                s.erase(0, pos + delimiter.length());
            }
            substrings.push_back(s);

            for (int i = 0; i < substrings.size(); i++)
            {
                intcols.push_back(atoi(substrings[i].c_str()));
            }

            intdata.push_back(intcols);
        }
    }
    read_file.close();

    return intdata;
}

void SpringStructure::update()
{
    for (size_t k = 0; k < N; k++)
    {
        idx_0 = k;
        idx_1 = N + k;
        idx_2 = 2 * N + k;

        vp[0] = P[idx_0];
        vp[1] = P[idx_1];
        vp[2] = P[idx_2];

        lim = conn[k * max_length];

        a[0] = 0.0f;
        a[1] = 0.0f;
        a[2] = 0.0f;

        A[idx_0] = 0.0f;
        A[idx_1] = 0.0f;
        A[idx_2] = 0.0f;

        Vp[idx_0] = 0.0f;
        Vp[idx_1] = 0.0f;
        Vp[idx_2] = 0.0f;

        // conn = [N x max_length] matrix
        // P = [3 x N] matrix

        for (int i = 0; i < lim; i++)
        {
            cidx = conn[k * max_length + i + 1];

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

            A[idx_0] = A[idx_0] + F * v[0];
            A[idx_1] = A[idx_1] + F * v[1];
            A[idx_2] = A[idx_2] + F * v[2];

            // Project velocities onto connection vector

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

    }  // Looping over k

    // ---------------- FRICTION ----------------

    for (size_t k = 0; k < N; k++)
    {
        idx_0 = k;
        idx_1 = N + k;
        idx_2 = 2 * N + k;

        if (P[idx_1] < friction_threshold)
        {
            v_speed = sqrt(V[idx_0] * V[idx_0] + V[idx_2] * V[idx_2]);
            if (v_speed > speed_threshold)
            {
                vp[0] = P[idx_0];
                vp[1] = P[idx_1];
                vp[2] = P[idx_2];

                lim = conn[k * max_length];

                Nf = 0.0f;

                for (int i = 0; i < lim; i++)
                {
                    cidx = conn[k * max_length + i + 1];

                    v[0] = vp[0] - P[cidx];          // difference vector x
                    v[1] = vp[1] - P[N + cidx];      // difference vector y
                    v[2] = vp[2] - P[2 * N + cidx];  // difference vector z

                    r0 = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

                    R = sqrt(r0);

                    // R = Rs[point_to_edge_index[k*max_connections + i]];
                    v[0] = v[0] / R;
                    v[1] = v[1] / R;
                    v[2] = v[2] / R;

                    F = K * (Ls[i * N + k] - R);

                    Nf = Nf + F * v[1];
                }

                if (Nf > 0.0f)
                {
                    v_0 = V[idx_0] / v_speed;
                    v_2 = V[idx_2] / v_speed;

                    A[idx_0] = A[idx_0] - v_0 * mu * Nf;
                    A[idx_2] = A[idx_2] - v_2 * mu * Nf;
                }
            }
        }  // If P[idx_1] < friction_threshold

    }  // Looping over k for friction

    // ---------------- FRICTION ----------------

    for (size_t k = 0; k < N; k++)
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

        // Floor
        if (P[idx_1] < 0.0f)
        {
            P[idx_1] = 0.0f;
            V[idx_1] = -V[idx_1];
        }

#define room_limit 15.0f

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

void SpringStructure::resetStructure()
{
    for (int c_ = 0; c_ < N; c_++)
    {
        V[c_] = 0.0f;
        V[N + c_] = 0.0f;
        V[2 * N + c_] = 0.0f;
        P[c_] = iP[c_];
        P[N + c_] = iP[N + c_];
        P[2 * N + c_] = iP[2 * N + c_];
    }
}

void SpringStructure::setVelocity(float dir[3], float amp)
{
    float r_ = sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
    dir[0] = amp * dir[0] / r_;
    dir[1] = amp * dir[1] / r_;
    dir[2] = amp * dir[2] / r_;

    for (int c_ = 0; c_ < N; c_++)
    {
        if (c_ != (N - 1))
        {
            V[c_] = V[c_] + dir[0];
            V[N + c_] = V[N + c_] + dir[1];
            V[2 * N + c_] = V[2 * N + c_] + dir[2];
        }
    }
}

void SpringStructure::calculateEdges(int* conn)
{
    int n;
    bool e = true;

    std::vector<std::vector<int>> ecg;
    std::vector<std::vector<int>> p2e_idx;

    int conn_idx;

    // Looping over all points
    for (size_t k = 0; k < N; k++)
    {
        // Number of connected points for current point
        n = conn[k * max_length];

        // Loop over every point that's connected to point k
        for (int i = 1; i < n + 1; i++)
        {
            Ne = ecg.size();

            e = true;
            conn_idx = conn[k * max_length + i];

            for (int j = 0; j < Ne; j++)
            {
                std::vector<int> ce = ecg[j];

                if (((ce[0] == k) && (ce[1] == conn_idx)) || ((ce[1] == k) && (ce[0] == conn_idx)))
                {
                    e = false;
                }
            }

            if (e)
            {
                std::vector<int> tv;
                tv.clear();
                tv.push_back(k);
                tv.push_back(conn_idx);
                ecg.push_back(tv);
            }
        }
    }

    edge_conn = (int*)malloc(sizeof(int) * 2 * ecg.size());

    for (size_t k = 0; k < ecg.size(); k++)
    {
        edge_conn[k * 2 + 0] = ecg[k][0];
        edge_conn[k * 2 + 1] = ecg[k][1];
    }

    Ne = ecg.size();

    for (size_t k = 0; k < N; k++)
    {
        std::vector<int> temp_edges;

        for (int i = 0; i < Ne; i++)
        {
            if (edge_conn[i * 2 + 0] == k || edge_conn[i * 2 + 1] == k)
            {
                temp_edges.push_back(i);
            }
        }
        p2e_idx.push_back(temp_edges);
    }

    max_connections = 0;

    for (size_t k = 0; k < p2e_idx.size(); k++)
    {
        if (p2e_idx[k].size() > max_connections)
        {
            max_connections = p2e_idx[k].size();
        }
    }

    point_to_edge_index = (int*)malloc(sizeof(int) * N * max_connections);

    for (size_t k = 0; k < p2e_idx.size(); k++)
    {
        for (int i = 0; i < p2e_idx[k].size(); i++)
        {
            point_to_edge_index[k * max_connections + i] = p2e_idx[k][i];
        }
    }
}

SpringStructure::SpringStructure(const std::string& conn_file_name,
                                 const std::string& pos_file_name,
                                 float h_,
                                 float C_,
                                 float K_,
                                 float m_,
                                 float Ca_,
                                 float x0,
                                 float y0,
                                 float z0)
{
    std::vector<std::vector<int>> intdata = readConnectivityGraph(conn_file_name);

    max_length = 0;
    this->N = intdata.size();

    for (size_t k = 0; k < intdata.size(); k++)
    {
        if (intdata[k][0] > max_length)
        {
            max_length = intdata[k][0];
        }
    }

    max_length = max_length + 1;

    conn = (int*)malloc(sizeof(int) * N * max_length);

    for (size_t k = 0; k < this->N; k++)
    {
        for (int i = 0; i < intdata[k][0] + 1; i++)
        {
            conn[k * max_length + i] = intdata[k][i];
        }
    }
    calculateEdges(conn);

    std::vector<std::vector<float>> positions = readPositions(pos_file_name);
    this->P = (float*)malloc(sizeof(float) * 3 * N);
    this->iP = (float*)malloc(sizeof(float) * 3 * N);
    this->V = (float*)calloc(3 * N, sizeof(float));
    this->A = (float*)malloc(sizeof(float) * 3 * N);

    this->Vp = (float*)malloc(sizeof(float) * 3 * N);

    for (size_t k = 0; k < positions.size(); k++)
    {
        // P = [3 x N] matrix

        P[k] = positions[k][0];
        P[N + k] = positions[k][1];
        P[2 * N + k] = positions[k][2];
    }

    Rs = (float*)malloc(sizeof(float) * Ne);

    this->m = m_;
    this->C = C_;
    this->K = K_;
    this->h = h_;
    this->Ca = Ca_;

    for (size_t k = 0; k < N; k++)
    {
        P[k] = P[k] + x0;
        P[N + k] = P[N + k] + y0;
        P[2 * N + k] = P[2 * N + k] + z0;

        iP[k] = P[k];
        iP[N + k] = P[N + k];
        iP[2 * N + k] = P[2 * N + k];

        V[k] = 0.0f;
        V[N + k] = 0.0f;
        V[2 * N + k] = 0.0f;
    }

    this->g = 9.82 * 10.0f;

    this->friction_threshold = 0.1f;
    this->speed_threshold = 0.001f;
    this->mu = 3.0f;
    this->Nf = 0.0f;
    // Need indexing vectors:
    // 1:
    // From point index to edge index
    // 2:
    // From edge index to point index

    this->Ls = (float*)malloc(sizeof(float) * (max_length - 1) * N);

    for (size_t k = 0; k < N; k++)
    {
        idx_0 = k;
        idx_1 = N + k;
        idx_2 = 2 * N + k;

        vp[0] = P[idx_0];
        vp[1] = P[idx_1];
        vp[2] = P[idx_2];

        lim = conn[k * max_length];

        // conn = [N x max_length] matrix

        for (int i = 1; i < lim + 1; i++)
        {
            cidx = conn[k * max_length + i];
            v[0] = vp[0] - P[0 * N + cidx];  // difference vector x
            v[1] = vp[1] - P[1 * N + cidx];  // difference vector y
            v[2] = vp[2] - P[2 * N + cidx];  // difference vector z
            Ls[(i - 1) * N + k] = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
        }
    }
}

void testBasic()
{
    const std::string project_file_path = "../../project_files/small_demo.duoplot";
    openProjectFile(project_file_path);

    float c = 4.0f;   // Exterior dampening
    float ca = 0.1f;  // Air resistance
    float m = 0.5f;   // Exterior mass
    float x0 = 0.0f;
    float y0 = 5.0f;
    float z0 = 0.0f;
    float K = 100000.0f;  // Exterior spring constant
    float h = 0.0001f;    // Time step

    const std::string base_path = "../demos/tests/spring/data/";

    /*
    ball_chain
    cube_125
    lattice
    rand
    snake
    sphere_163
    wheel
    */

    const std::string selection = "wheel";

    const std::string conn_file_name = base_path + "conn_" + selection + ".txt";
    const std::string pos_file_name = base_path + "pos_" + selection + ".txt";

    SpringStructure spring_structure{conn_file_name, pos_file_name, h, c, K, m, ca, x0, y0, z0};

    const size_t num_points = spring_structure.N;

    const VectorConstView<float> x{spring_structure.P, num_points};
    const VectorConstView<float> y{spring_structure.P + num_points, num_points};
    const VectorConstView<float> z{spring_structure.P + 2U * num_points, num_points};

    int lim_;
    int cidx;

    Vector<float> xl{num_points * 2U * spring_structure.max_length};
    Vector<float> yl{num_points * 2U * spring_structure.max_length};
    Vector<float> zl{num_points * 2U * spring_structure.max_length};

    xl.fill(0.0f);
    yl.fill(0.0f);
    zl.fill(0.0f);

    setActiveView("p_view_0");
    clearView();
    disableScaleOnRotation();
    disableAutomaticAxesAdjustment();
    axesSquare();
    waitForFlush();

    axis({-4.0, -4.0, -4.0}, {4.0, 4.0, 4.0});

    for (size_t k = 0; k < 1000; k++)
    {
        for (size_t u = 0; u < 50; u++)
        {
            spring_structure.update();
        }

        size_t idx = 0;
        for (size_t kl = 0; kl < num_points; kl++)
        {
            lim_ = spring_structure.conn[kl * spring_structure.max_length];

            for (size_t i = 1; i < lim_ + 1; i++)
            {
                cidx = spring_structure.conn[kl * spring_structure.max_length + i];

                const Vec3f p0{spring_structure.P[kl],
                               spring_structure.P[num_points + kl],
                               spring_structure.P[2 * num_points + kl]};
                const Vec3f p1{spring_structure.P[cidx],
                               spring_structure.P[num_points + cidx],
                               spring_structure.P[2 * num_points + cidx]};
                xl(idx) = p0.x;
                yl(idx) = p0.y;
                zl(idx) = p0.z;

                idx += 1;

                xl(idx) = p1.x;
                yl(idx) = p1.y;
                zl(idx) = p1.z;

                idx += 1;
            }
        }

        lineCollection3(xl, zl, yl, properties::Color::BLACK);
        scatter3(
            x,
            z,
            y,
            properties::ScatterStyle::DISC,
            properties::Color(0.19607843137254902 * 255.0, 0.5333333333333333 * 255.0, 0.7411764705882353 * 255.0));

        flushCurrentElement();

        clearViewOnUpdate();

        // usleep(50 * 1000);
    }
}

}  // namespace spring_structure
