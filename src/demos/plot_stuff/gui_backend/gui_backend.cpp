#include "duoplot/duoplot.h"

using namespace duoplot;

/*
How to run this demo
Run duoplot application with "adding_gui_elements.duoplot" autoloaded (or just a blank project?)
Project will only have one window (or no windows)
If no window, hack window new window size so that it's neat looking with the open terminal
Add two plot panes, "p0", and "p1"
Add three sliders, "slider_kp", "slider_ki", and "slider_kd"
Run the gui_backend application
*/

int main()
{
    struct SimParams
    {
        double h;
        double x0;
        double vx0;
        double L;
        double Kf;
        double Cf;
        double m;
        double Kp;
        double Ki;
        double Kd;
        double r;
        double error_bound;
    };

    SimParams sim_params{.h = 0.01,
                         .x0 = 0.5,
                         .vx0 = 0.0,
                         .L = 0.25,
                         .Kf = 5.0,
                         .Cf = 0.5,
                         .m = 0.1,
                         .Kp = 0.0,
                         .Ki = 0.0,
                         .Kd = 0.0,
                         .r = 1.0,
                         .error_bound = 0.05};

    const size_t num_elements = 1000U;
    const size_t end_idx = num_elements - 1U;

    const Vector<double> t = linspaceFromStartPointIncAndCount<double>(0.0, sim_params.h, num_elements);
    Vector<double> x(num_elements);
    Vector<double> vx(num_elements);

    const auto run_sim = [](const SimParams& sim_params, Vector<double>& x, Vector<double>& vx) -> void {
        x(0) = sim_params.x0;
        vx(0) = sim_params.vx0;

        const size_t num_elements = x.size();

        float eI = 0.0;

        for (size_t k = 0; k < num_elements - 1U; k++)
        {
            const double x_k = x(k);
            const double vx_k = vx(k);

            const double e = sim_params.r - x_k;

            eI += e * sim_params.h;

            const double F = sim_params.Kp * e - sim_params.Kd * vx_k + sim_params.Ki * eI;

            vx(k + 1) =
                vx_k + sim_params.h * (-sim_params.Kf * (x_k - sim_params.L) + F - sim_params.Cf * vx_k) / sim_params.m;
            x(k + 1) = x_k + sim_params.h * vx(k + 1);
        }
    };

    const auto find_ss = [](SimParams& sim_params, const Vector<double>& x) -> size_t {
        size_t candidate_index = 0U;

        const size_t x_size = x.size();

        for (size_t k = 1; k < x_size; k++)
        {
            const double prev_error = std::abs(x(k - 1U) - sim_params.r);
            const double current_error = std::abs(x(k) - sim_params.r);

            if (prev_error > sim_params.error_bound && current_error < sim_params.error_bound)
            {
                candidate_index = k;
            }
            else if (prev_error < sim_params.error_bound && current_error > sim_params.error_bound)
            {
                candidate_index = x_size - 1U;
            }
        }

        return candidate_index;
    };

    const auto plot_data = [&run_sim, &find_ss](const double val_kp,
                                                const double val_ki,
                                                const double val_kd,
                                                SimParams& sim_params,
                                                const Vector<double>& t,
                                                Vector<double>& x,
                                                Vector<double>& vx) -> void {
        sim_params.Kp = val_kp * 50.0;
        sim_params.Ki = val_ki * 50.0;
        sim_params.Kd = val_kd * 5.0;

        run_sim(sim_params, x, vx);

        const Vector<double> start_stop_x{VectorInitializer<double>{0.0, t(t.size() - 1U)}};
        const Vector<double> ref_vec_y{VectorInitializer<double>{sim_params.r, sim_params.r}};
        const Vector<double> l_vec_y{VectorInitializer<double>{sim_params.L, sim_params.L}};

        const size_t ss_idx = find_ss(sim_params, x);

        using Vd = Vector<double>;
        using Vid = VectorInitializer<double>;

        setActiveView("p0");

        plot(t, x, properties::LineWidth(7.0f), properties::Label("Position"));
        plot(t, vx, properties::LineWidth(7.0f), properties::Label("Velocity"));
        plot(start_stop_x,
             ref_vec_y,
             properties::LineWidth(3.0f),
             properties::Color::BLACK,
             properties::Label("Reference"));
        plot(start_stop_x,
             l_vec_y,
             properties::LineWidth(3.0f),
             properties::Color::BLACK,
             properties::Label("Position equilibrium state"));
        plot(Vd{Vid{t(ss_idx), t(ss_idx)}},
             Vd{Vid{0.0, 2.0}},
             properties::LineWidth(7.0f),
             properties::Color{255, 127, 0});
        // showLegend();
        flushCurrentElement();
        softClearView();

        setActiveView("p1");

        plot(Vd{Vid{t(0), t(end_idx)}},
             Vd{Vid{sim_params.error_bound, sim_params.error_bound}},
             properties::LineWidth(3.0f),
             properties::Color::BLUE);

        plot(Vd{Vid{t(0), t(end_idx)}},
             Vd{Vid{-sim_params.error_bound, -sim_params.error_bound}},
             properties::LineWidth(3.0f),
             properties::Color::BLUE);

        plot(Vd{Vid{t(ss_idx), t(ss_idx)}},
             Vd{Vid{-sim_params.error_bound, sim_params.error_bound}},
             properties::LineWidth(7.0f),
             properties::Color{255, 127, 0});
        plot(t, x - sim_params.r, properties::LineWidth(7.0f), properties::Label("Velocity"));

        flushCurrentElement();
        softClearView();
    };

    duoplot::gui::registerGuiCallback("slider_kp", [&](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
        const double val_kp = gui_element_handle.getNormalizedValue();
        const double val_ki =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ki").getNormalizedValue();
        const double val_kd =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kd").getNormalizedValue();

        plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);
    });

    duoplot::gui::registerGuiCallback("slider_ki", [&](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
        const double val_kp =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kp").getNormalizedValue();
        const double val_ki = gui_element_handle.getNormalizedValue();
        const double val_kd =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kd").getNormalizedValue();

        plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);
    });

    duoplot::gui::registerGuiCallback("slider_kd", [&](const duoplot::gui::SliderHandle& gui_element_handle) -> void {
        const double val_kp =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kp").getNormalizedValue();
        const double val_ki =
            duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ki").getNormalizedValue();
        const double val_kd = gui_element_handle.getNormalizedValue();

        plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);
    });

    run_sim(sim_params, x, vx);

    duoplot::gui::startGuiReceiveThread();

    setActiveView("p0");
    clearView();
    setTitle("PID Controller Simulation");
    waitForFlush();
    axis({0.0, -0.5}, {t(end_idx), 1.5});

    setActiveView("p1");
    clearView();
    setTitle("Error");
    waitForFlush();
    axis({0.0, -0.5}, {t(end_idx), 0.5});

    const double val_kp =
        duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kp").getNormalizedValue();
    const double val_ki =
        duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_ki").getNormalizedValue();
    const double val_kd =
        duoplot::gui::getGuiElementHandle<duoplot::gui::SliderHandle>("slider_kd").getNormalizedValue();

    plot_data(val_kp, val_ki, val_kd, sim_params, t, x, vx);

    while (true)
    {
        usleep(1000 * 1000 * 10);
    }
}