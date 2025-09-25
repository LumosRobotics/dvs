#include "tests/gui/gui.h"

namespace gui_test
{

void testBasic()
{
    const std::string project_file_path = "../../project_files/demo/gui_demo0.duoplot";

    openProjectFile(project_file_path);

    setActiveView("p_view_0");
    clearView();
    axis({-5.0, -5.0, 0.0}, {5.0, 5.0, 10.0});

    const auto plot_data = [](const double v0, const double v1, const double v2) -> void {
        const size_t n_elems = 30U;

        const std::pair<Matrix<double>, Matrix<double>> uv_mats =
            meshGrid<double>(1.0, 5.0, 1.0, 10.0, n_elems, n_elems);
        Matrix<double> u{uv_mats.first}, v{uv_mats.second};

        const Matrix<double> x = elementWiseMultiply(v0 * u, sin(v));
        const Matrix<double> y = elementWiseMultiply(-u * v2, cos(v));
        const Matrix<double> z = v * v1;

        surf(x, y, z, properties::EdgeColor(0, 0, 0), properties::ColorMap::JET_BRIGHT);
        softClearView();
    };

    duoplot::gui::registerGuiCallback("slider0",
                                      [&plot_data](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
                                          const duoplot::gui::SliderHandle slider1 =
                                              duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider1");
                                          const duoplot::gui::SliderHandle slider2 =
                                              duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider2");

                                          const double v0 = static_cast<double>(gui_element_handle.getValue()) / 100.0;
                                          const double v1 = static_cast<double>(slider1.getValue()) / 100.0;
                                          const double v2 = static_cast<double>(slider2.getValue()) / 100.0;

                                          plot_data(v0, v1, v2);
                                      });

    duoplot::gui::registerGuiCallback("slider1",
                                      [&plot_data](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
                                          const duoplot::gui::SliderHandle slider0 =
                                              duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider0");
                                          const duoplot::gui::SliderHandle slider2 =
                                              duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider2");

                                          const double v0 = static_cast<double>(slider0.getValue()) / 100.0;
                                          const double v1 = static_cast<double>(gui_element_handle.getValue()) / 100.0;
                                          const double v2 = static_cast<double>(slider2.getValue()) / 100.0;

                                          plot_data(v0, v1, v2);
                                      });

    duoplot::gui::registerGuiCallback("slider2",
                                      [&plot_data](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
                                          const duoplot::gui::SliderHandle slider0 =
                                              duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider0");
                                          const duoplot::gui::SliderHandle slider1 =
                                              duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider1");

                                          const double v0 = static_cast<double>(slider0.getValue()) / 100.0;
                                          const double v1 = static_cast<double>(slider1.getValue()) / 100.0;
                                          const double v2 = static_cast<double>(gui_element_handle.getValue()) / 100.0;

                                          plot_data(v0, v1, v2);
                                      });

    duoplot::gui::startGuiReceiveThread();

    std::cout << "Instructions:" << std::endl << "\"q\": Exits application" << std::endl;

    // Client application loop
    while (true)
    {
        std::string input;
        std::getline(std::cin, input);

        if (input == "q")
        {
            break;
        }
    }
}

struct SimParams
{
    double m;
    double ks;
    double c;
    double h;
    double freq;
    double amp;
    double x0;
    double vx0;
};

bool operator==(const SimParams& lhs, const SimParams& rhs)
{
    return (lhs.m == rhs.m) && (lhs.ks == rhs.ks) && (lhs.c == rhs.c) && (lhs.h == rhs.h) && (lhs.x0 == rhs.x0) &&
           (lhs.vx0 == rhs.vx0);
}

bool operator!=(const SimParams& lhs, const SimParams& rhs)
{
    return !(lhs == rhs);
}

void testDynamicSystem()
{
    const std::string project_file_path = "../../project_files/demo/gui_demo0.duoplot";

    openProjectFile(project_file_path);

    struct SimState
    {
        double x;
        double vx;
    };

    SimParams sim_params;
    SimState sim_state;

    sim_params.m = 0.5;
    sim_params.ks = 10.0;
    sim_params.c = 1.0;
    sim_params.h = 0.05;
    sim_params.x0 = 10.0;
    sim_params.vx0 = 0.0;

    const size_t n_laps = 7U;

    const double start_offset = -M_PI / 2.0;

    const Vector<double> tc =
        linspaceFromBoundariesAndCount(start_offset, static_cast<double>(n_laps) * 2.0 * M_PI + start_offset, 1000U);
    Vector<double> xc{tc.size()}, yc{tc.size()}, offset_y{tc.size()};

    const double circle_r = 0.5;

    for (size_t k = 0; k < tc.size(); k++)
    {
        xc(k) = circle_r * std::cos(tc(k));
        yc(k) = circle_r * std::sin(tc(k));
    }

    SimParams prev_sim_params = sim_params;

    sim_state.x = sim_params.x0;
    sim_state.vx = sim_params.vx0;

    const size_t n_elems = 100U;

    const Vector<double> t = linspaceFromBoundariesAndCount(0.0, 10.0, n_elems);
    Vector<double> x{n_elems}, vx{n_elems};

    vx(0) = sim_params.vx0;
    x(0) = sim_params.x0;

    setActiveView("p_view_0");
    clearView();
    // axis({-5.0, -5.0, 0.0}, {5.0, 5.0, 10.0});
    axis({0.0, -32.0, -1.0}, {10.0, 32.0, 1.0});

    setActiveView("p_view_1");
    clearView();
    // axis({-5.0, -5.0, 0.0}, {5.0, 5.0, 10.0});
    axis({-2.0, -32.0, -1.0}, {2.0, 32.0, 1.0});

    const auto set_data = [&n_elems](Vector<double>& x, Vector<double>& vx, const SimParams& sim_params) -> void {
        for (size_t k = 1; k < n_elems; k++)
        {
            // m * x'' = k * x - c * x'
            vx(k) = vx(k - 1) - sim_params.h * (x(k - 1) * sim_params.ks + vx(k - 1) * sim_params.c) / sim_params.m;
            x(k) = x(k - 1) + sim_params.h * vx(k - 1);
        }
    };

    const auto mv = [](const int val) -> double { return static_cast<double>(val) / 100.0; };

    duoplot::gui::registerGuiCallback("slider_m", [&sim_params, &mv](const duoplot::gui::SliderHandle& s_m) -> void {
        // const duoplot::gui::SliderHandle s_m =
        // duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_m");
        const duoplot::gui::SliderHandle s_c =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_c");
        const duoplot::gui::SliderHandle s_ks =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ks");
        const duoplot::gui::SliderHandle s_freq =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_freq");
        const duoplot::gui::SliderHandle s_amp =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_amp");

        sim_params.m = mv(s_m.getValue()) * 10.0 + 0.5;
        sim_params.c = 2.0 * mv(s_c.getValue()) + 1.0;
        sim_params.ks = 20.0 * mv(s_ks.getValue()) + 1.0;
        sim_params.freq = 10.0 * mv(s_freq.getValue()) + 0.5;
        sim_params.amp = 100.0 * mv(s_amp.getValue()) + 1.0;
    });

    duoplot::gui::registerGuiCallback("slider_c", [&sim_params, &mv](const duoplot::gui::SliderHandle& s_c) -> void {
        const duoplot::gui::SliderHandle s_m =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_m");
        // const duoplot::gui::SliderHandle s_c =
        // duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_c");
        const duoplot::gui::SliderHandle s_ks =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ks");
        const duoplot::gui::SliderHandle s_freq =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_freq");
        const duoplot::gui::SliderHandle s_amp =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_amp");

        sim_params.m = mv(s_m.getValue()) * 10.0 + 0.5;
        sim_params.c = 2.0 * mv(s_c.getValue()) + 1.0;
        sim_params.ks = 20.0 * mv(s_ks.getValue()) + 1.0;
        sim_params.freq = 10.0 * mv(s_freq.getValue()) + 0.5;
        sim_params.amp = 100.0 * mv(s_amp.getValue()) + 1.0;
    });

    duoplot::gui::registerGuiCallback("slider_ks", [&sim_params, &mv](const duoplot::gui::SliderHandle& s_ks) -> void {
        const duoplot::gui::SliderHandle s_m =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_m");
        const duoplot::gui::SliderHandle s_c =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_c");
        // const duoplot::gui::SliderHandle s_ks =
        // duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ks");
        const duoplot::gui::SliderHandle s_freq =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_freq");
        const duoplot::gui::SliderHandle s_amp =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_amp");

        sim_params.m = mv(s_m.getValue()) * 10.0 + 0.5;
        sim_params.c = 2.0 * mv(s_c.getValue()) + 1.0;
        sim_params.ks = 20.0 * mv(s_ks.getValue()) + 1.0;
        sim_params.freq = 10.0 * mv(s_freq.getValue()) + 0.5;
        sim_params.amp = 100.0 * mv(s_amp.getValue()) + 1.0;
    });

    duoplot::gui::registerGuiCallback(
        "slider_freq", [&sim_params, &mv](const duoplot::gui::SliderHandle& s_freq) -> void {
            const duoplot::gui::SliderHandle s_m =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_m");
            const duoplot::gui::SliderHandle s_c =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_c");
            const duoplot::gui::SliderHandle s_ks =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ks");
            // const duoplot::gui::SliderHandle s_freq =
            // duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_freq");
            const duoplot::gui::SliderHandle s_amp =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_amp");

            sim_params.m = mv(s_m.getValue()) * 10.0 + 0.5;
            sim_params.c = 2.0 * mv(s_c.getValue()) + 1.0;
            sim_params.ks = 20.0 * mv(s_ks.getValue()) + 1.0;
            sim_params.freq = 10.0 * mv(s_freq.getValue()) + 0.5;
            sim_params.amp = 100.0 * mv(s_amp.getValue()) + 1.0;
        });

    duoplot::gui::registerGuiCallback(
        "slider_amp", [&sim_params, &mv](const duoplot::gui::SliderHandle& s_amp) -> void {
            const duoplot::gui::SliderHandle s_m =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_m");
            const duoplot::gui::SliderHandle s_c =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_c");
            const duoplot::gui::SliderHandle s_ks =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ks");
            const duoplot::gui::SliderHandle s_freq =
                duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_freq");
            // const duoplot::gui::SliderHandle s_amp =
            // duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_amp");

            sim_params.m = mv(s_m.getValue()) * 10.0 + 0.5;
            sim_params.c = 2.0 * mv(s_c.getValue()) + 1.0;
            sim_params.ks = 20.0 * mv(s_ks.getValue()) + 1.0;
            sim_params.freq = 10.0 * mv(s_freq.getValue()) + 0.5;
            sim_params.amp = 100.0 * mv(s_amp.getValue()) + 1.0;
        });

    duoplot::gui::startGuiReceiveThread();

    std::cout << "Instructions:" << std::endl << "\"q\": Exits application" << std::endl;

    double tv = 0.0;

    Vector<double> xv{2U}, yv{2U};
    xv(0) = 0.0;
    xv(1) = 0.0;

    const double t_size_m_1 = static_cast<double>(tc.size() - 1U);
    // Client application loop
    while (true)
    {
        setActiveView("p_view_0");
        set_data(x, vx, sim_params);
        plot(t, x, properties::LineWidth(5.0));
        softClearView();

        const double F = sim_params.amp * std::sin(sim_params.freq * tv);
        tv += sim_params.h;

        sim_state.vx = sim_state.vx -
                       sim_params.h * (sim_state.x * sim_params.ks + sim_state.vx * sim_params.c + F) / sim_params.m;
        sim_state.x = sim_state.x + sim_params.h * sim_state.vx;

        for (size_t k = 0; k < tc.size(); k++)
        {
            const double kf = sim_state.x * static_cast<double>(k) / t_size_m_1;
            offset_y(k) = yc(k) + kf + circle_r;
        }

        yv(0) = sim_state.x;
        yv(1) = sim_state.x;

        setActiveView("p_view_1");
        plot(xc, offset_y, properties::LineWidth(5.0));
        scatter(xv, yv, properties::PointSize(50.0), properties::ScatterStyle::DISC);
        softClearView();
        usleep(1000U * 30U);

        prev_sim_params = sim_params;
    }
}

}  // namespace gui_test
