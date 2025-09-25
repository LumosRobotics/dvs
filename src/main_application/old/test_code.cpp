
void plotSomething(int i)
{
    Vector<double> x(10), y(10), z(10);

    for (size_t k = 0; k < 10; k++)
    {
        x(k) = k;
        y(k) = 2 * x(k);
        z(k) = 2 * std::sin(x(k) + static_cast<double>(i));
    }
    plot3(x, y, z, properties::Color(12, 14, 55), properties::LineWidth(1), properties::PointSize(137));
}

void testBasic()
{
    Vector<double> x(20), y(20), z(20);

    for (size_t k = 0; k < 20; k++)
    {
        x(k) = k;
        y(k) = 2 * x(k);
        z(k) = 2 * std::sin(x(k));
    }

    setActiveView("view_00");
    sleepMS(20);
    viewAngles(22.4f, 0.14f);
    sleepMS(20);
    axis({-1.1, -2.2, -3.3}, {4.4, 5.5, 6.6});
    sleepMS(20);
    plot(x,
         y,
         properties::Color(12, 14, 55),
         properties::Alpha(137),
         properties::Label("SimpleLabel"),
         properties::LineStyle("-*"),
         properties::LineWidth(1),
         properties::EdgeColor(55, 21, 7),
         properties::FaceColor(52, 26, 3),
         properties::ColorMap::magma(),
         properties::PointSize(137));
    sleepMS(20);
    setActiveView("view_01");
    sleepMS(20);
    plot3(x,
          y,
          z,
          properties::Color(12, 14, 55),
          properties::Alpha(137),
          properties::Label("SimpleLabel"),
          properties::LineStyle("-*"),
          properties::LineWidth(1),
          properties::EdgeColor(55, 21, 7),
          properties::FaceColor(52, 26, 3),
          properties::ColorMap::magma(),
          properties::PointSize(137));
    sleepMS(20);
    setActiveView("view_02");
    sleepMS(20);
    plot3(x,
          y,
          z,
          properties::Color(12, 14, 55),
          properties::Alpha(137),
          properties::Label("SimpleLabel"),
          properties::LineStyle("-*"),
          properties::LineWidth(1),
          properties::EdgeColor(55, 21, 7),
          properties::FaceColor(52, 26, 3),
          properties::ColorMap::magma(),
          properties::PointSize(137));
    sleepMS(20);
    setActiveView("view_00");

    for (int i = 0; i < 100; i++)
    {
        sleepMS(20);
        plotSomething(i);
    }
}

void testDifferentViews()
{
    const std::vector<std::string> views = {"view_100",
                                            "view_101",
                                            "view_200",
                                            "view_201",
                                            "view_00",
                                            "view_01",
                                            "view_02",
                                            "view_10",
                                            "view_11",
                                            "view_20",
                                            "view_21",
                                            "view_22",
                                            "view_23",
                                            "view_24",
                                            "view_25",
                                            "view_26",
                                            "view_27"};

    const size_t num_elements = 50;
    Vector<double> x(num_elements), y(num_elements), z(num_elements);

    auto transf = [&](const double offset) {
        double t = offset;
        for (size_t k = 0; k < num_elements; k++)
        {
            x(k) = 10.0 * cos(t) + 20.0;
            y(k) = 10.0 * sin(t) + 20.0;
            z(k) = t;
            t = t + 0.1;
        }
    };

    double offset = 0.0;

    for (size_t k = 0; k < 200; k++)
    {
        const std::string current_view = views[k % views.size()];
        setActiveView(current_view);
        softClearView();

        axis({10.0, 10.0, 0.3}, {32.0, 32.0, 100.6});
        plot3(x, y, z, properties::Color(212, 14, 55), properties::LineWidth(1));
        offset = offset + 0.3;
        transf(offset);
    }
}

void testCube()
{
    setActiveView("view_00");
    clearView();
    const double x = 1.0, y = 1.0, z = 1.0, d = 0.00001;
    axis({-x - d, -y - d, -z - d}, {x + d, y + d, z + d});
    drawPolygonFrom4Points({x, y, z}, {-x, y, z}, {-x, -y, z}, {x, -y, z});
    drawPolygonFrom4Points({x, y, -z}, {-x, y, -z}, {-x, -y, -z}, {x, -y, -z});

    drawPolygonFrom4Points({x, y, z}, {x, -y, z}, {x, -y, -z}, {x, y, -z});
    drawPolygonFrom4Points({-x, y, z}, {-x, -y, z}, {-x, -y, -z}, {-x, y, -z});

    drawPolygonFrom4Points({x, y, z}, {-x, y, z}, {-x, y, -z}, {x, y, -z});
    drawPolygonFrom4Points({x, -y, z}, {-x, -y, z}, {-x, -y, -z}, {x, -y, -z});
}

void testMultipleStuff()
{
    const size_t num_elements = 50;
    Vector<float> x(num_elements), y(num_elements), z(num_elements);

    double t = 0.0;

    for (size_t k = 0; k < num_elements; k++)
    {
        x(k) = 10.0 * cos(t);
        y(k) = 10.0 * sin(t);
        z(k) = t;
        t = t + 0.3;
    }

    setActiveView("view_00");
    clearView();

    viewAngles(-106.5f, 16.3f);
    axis({-5.0, -5.0, -5.0}, {5.0, 5.0, 5.0});
    plot3(x, y, z, properties::Color(12, 14, 55), properties::LineWidth(1));
    scatter3(x, y, z, properties::Color(255, 0, 0), properties::PointSize(3));
}

void testDrawMeshAdvanced()
{
    using tp = double;

    const int num_rows = 20, num_cols = 25;
    const int num_triangles = (num_rows - 1) * (num_cols - 1) * 2;

    Vector<Point3<tp>> vertices(num_rows * num_cols);
    Vector<IndexTriplet> indices(num_triangles);

    for (int r = 0; r < num_rows; r++)
    {
        for (int c = 0; c < num_cols; c++)
        {
            const double x = static_cast<double>(c - num_cols / 2) * 0.1 + 0.05;
            const double y = static_cast<double>(r - num_rows / 2) * 0.1 + 0.05;
            const double r0 = 10.0 * (x * x + y * y);
            const double z = std::sin(r0) / r0;

            vertices(r * num_cols + c) = Point3<tp>(x * 100.0, y * 100.0, z * 100.0);
        }
    }

    int idx = 0;
    for (int r = 0; r < (num_rows - 1); r++)
    {
        for (int c = 0; c < (num_cols - 1); c++)
        {
            const int idx0 = r * num_cols + c;
            const int idx1 = r * num_cols + c + 1;
            const int idx2 = (r + 1) * num_cols + c + 1;
            const int idx3 = (r + 1) * num_cols + c;
            indices(idx) = IndexTriplet(idx0, idx1, idx2);
            indices(idx + 1) = IndexTriplet(idx0, idx3, idx2);
            idx += 2;
        }
    }

    setActiveView("view_00");
    clearView();
    drawMesh(vertices, indices, properties::EdgeColor(0, 0, 0), properties::FaceColor(12, 244, 244));
}
