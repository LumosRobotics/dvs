#ifndef QT_APPLICATION_PLOT_PANE_H_
#define QT_APPLICATION_PLOT_PANE_H_

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>

#include <memory>
#include <vector>
#include <queue>
#include <atomic>

#include "axes/axes_interactor.h"
#include "axes/axes_renderer.h"
#include "axes/structures/axes_settings.h"
#include "plot_data_handler.h"
#include "point_selection.h"
#include "shader.h"
#include "input_data.h"
#include "gui_element.h"

class PlotPane : public QOpenGLWidget, public ApplicationGuiElement, protected QOpenGLFunctions
{
    Q_OBJECT

private:
    // Axes system
    AxesSettings axes_settings_;
    AxesInteractor axes_interactor_;
    AxesRenderer* axes_renderer_;
    bool axes_set_;
    bool view_set_;
    bool perspective_projection_;

    // Plot data
    PlotDataHandler* plot_data_handler_;
    PointSelection point_selection_;
    ShaderCollection shader_collection_;

    // Data queues
    std::queue<std::unique_ptr<InputData>> queued_data_;
    std::atomic<bool> pending_clear_;

    // Mouse interaction
    QPoint last_mouse_pos_;
    bool is_rotating_;
    bool is_panning_;
    bool is_zooming_;
    bool shift_pressed_at_mouse_press_;
    bool should_render_point_selection_;

    // Callbacks to parent
    std::function<void(const QPoint& pos, const QSize& size, bool is_editing)> notify_tab_about_editing_;

    // Helper methods
    void initShaders();
    void processActionQueue();
    void clearPane();

    // OpenGL initialization
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    // Mouse events
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;

public:
    PlotPane(QWidget* parent,
             const std::shared_ptr<ElementSettings>& element_settings,
             const std::function<void(const char key)>& notify_main_window_key_pressed,
             const std::function<void(const char key)>& notify_main_window_key_released,
             const std::function<void()>& notify_main_window_about_modification,
             const std::function<void(const QPoint& pos, const std::string& elem_name)>& notify_parent_right_click,
             const std::function<void(const QPoint& pos, const QSize& size, bool is_editing)>& notify_tab_about_editing,
             const std::function<void(const std::string&)>& on_text_output);
    ~PlotPane();

    // Data management
    void pushQueue(std::queue<std::unique_ptr<InputData>>& new_queue);
    void addPlotData(ReceivedData& received_data,
                     const PlotObjectAttributes& plot_object_attributes,
                     const UserSuppliedProperties& user_supplied_properties,
                     const std::shared_ptr<const ConvertedDataBase>& converted_data);
    void clear();

    // ApplicationGuiElement interface implementation
    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override;
    void keyReleasedElementSpecific(const char key) override;
};

#endif  // QT_APPLICATION_PLOT_PANE_H_
