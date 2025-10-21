#include "plot_pane.h"

#include <QOpenGLContext>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#include "axes/axes_side_configuration.h"
#include "duoplot/enumerations.h"
#include "duoplot/logging.h"
#include "platform_paths.h"
#include "project_state/project_settings.h"

PlotPane::PlotPane(QWidget* parent,
                   const std::shared_ptr<ElementSettings>& element_settings,
                   const std::function<void(const char key)>& notify_main_window_key_pressed,
                   const std::function<void(const char key)>& notify_main_window_key_released,
                   const std::function<void()>& notify_main_window_about_modification)
    : QOpenGLWidget(parent),
      ApplicationGuiElement(element_settings,
                           notify_main_window_key_pressed,
                           notify_main_window_key_released,
                           notify_main_window_about_modification),
      axes_interactor_(axes_settings_, width(), height()),
      axes_renderer_(nullptr),
      axes_set_(false),
      view_set_(false),
      perspective_projection_(true),
      plot_data_handler_(nullptr),
      pending_clear_(false),
      is_rotating_(false),
      is_panning_(false),
      is_zooming_(false),
      shift_pressed_at_mouse_press_(false),
      should_render_point_selection_(false)
{
    setFocusPolicy(Qt::StrongFocus);
    setMouseTracking(true);

    DUOPLOT_LOG_INFO() << "PlotPane created with handle: " << element_settings->handle_string;
}

PlotPane::~PlotPane()
{
    makeCurrent();

    // Clean up resources
    if (axes_renderer_ != nullptr)
    {
        delete axes_renderer_;
    }

    if (plot_data_handler_ != nullptr)
    {
        delete plot_data_handler_;
    }

    doneCurrent();

    DUOPLOT_LOG_INFO() << "PlotPane destroyed";
}

void PlotPane::initializeGL()
{
    initializeOpenGLFunctions();

    // Set clear color
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);

    // Enable depth testing
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Enable line smoothing
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    // Enable multisampling and point size
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    // Initialize shaders
    initShaders();

    // Create default plot pane settings for now
    // TODO: Make this configurable via MainWindow settings
    PlotPaneSettings plot_pane_settings;
    plot_pane_settings.handle_string = "plot_pane";
    plot_pane_settings.projection_mode = PlotPaneSettings::ProjectionMode::PERSPECTIVE;
    plot_pane_settings.background_color = RGBTripletf(0.15f, 0.15f, 0.15f);

    RGBTripletf tab_background_color(0.2f, 0.2f, 0.2f);

    // Create axes renderer
    axes_renderer_ = new AxesRenderer(shader_collection_, plot_pane_settings, tab_background_color);

    // Create plot data handler
    plot_data_handler_ = new PlotDataHandler(shader_collection_);

    DUOPLOT_LOG_INFO() << "OpenGL initialized in PlotPane";
}

void PlotPane::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
    axes_interactor_.updateWindowSize(w, h);
}

void PlotPane::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (pending_clear_)
    {
        clearPane();
        pending_clear_ = false;
    }

    processActionQueue();

    if (axes_set_ && view_set_)
    {
        const Vec2f pane_size(width(), height());

        // Create axes side configuration based on view angles
        const AxesSideConfiguration axes_side_configuration{axes_interactor_.getViewAngles(), perspective_projection_};

        // Update axes renderer state
        axes_renderer_->updateStates(axes_interactor_.getAxesLimits(),
                                     axes_interactor_.getViewAngles(),
                                     axes_interactor_.getQueryPoint(),
                                     axes_interactor_.generateGridVectors(),
                                     axes_side_configuration,
                                     perspective_projection_,
                                     static_cast<float>(width()),
                                     static_cast<float>(height()),
                                     Vec2f(last_mouse_pos_.x(), last_mouse_pos_.y()).elementWiseDivide(pane_size),
                                     Vec2f(last_mouse_pos_.x(), last_mouse_pos_.y()).elementWiseDivide(pane_size),
                                     axes_interactor_.getMouseInteractionType(),
                                     is_rotating_ || is_panning_,
                                     axes_interactor_.shouldDrawZoomRect(),
                                     axes_interactor_.getShowLegend(),
                                     1.0f,  // legend_scale_factor
                                     plot_data_handler_ != nullptr ? plot_data_handler_->getLegendStrings() : std::vector<LegendProperties>(),
                                     "plot_pane",
                                     MouseInteractionAxis::ALL);

        axes_renderer_->render();
        glEnable(GL_DEPTH_TEST);
        axes_renderer_->plotBegin();

        if (plot_data_handler_ != nullptr)
        {
            plot_data_handler_->render();
        }

        axes_renderer_->plotEnd();

        if (should_render_point_selection_)
        {
            // TODO: Implement point selection rendering
            // point_selection_.render();
        }

        glDisable(GL_DEPTH_TEST);
    }
}

void PlotPane::initShaders()
{
    const std::string base_path{getResourcesPathString() + "/shaders/"};

    auto createShader = [&base_path](const std::string& shader_name) -> ShaderBase {
        const std::string v_path = base_path + shader_name + ".vs";
        const std::string f_path = base_path + shader_name + ".fs";
        return ShaderBase{v_path, f_path, ShaderSource::FILE};
    };

    shader_collection_.plot_box_shader = createShader("plot_box_shader");
    shader_collection_.pane_background_shader = createShader("pane_background");
    shader_collection_.text_shader = TextShader{base_path + "text.vs", base_path + "text.fs", ShaderSource::FILE};
    shader_collection_.basic_plot_shader = createShader("basic_plot_shader");
    shader_collection_.plot_2d_shader = Plot2DShader{base_path + "plot_2d_shader.vs", base_path + "plot_2d_shader.fs", ShaderSource::FILE};
    shader_collection_.plot_3d_shader = Plot3DShader{base_path + "plot_3d_shader.vs", base_path + "plot_3d_shader.fs", ShaderSource::FILE};
    shader_collection_.img_plot_shader = ImShowShader{base_path + "img.vs", base_path + "img.fs", ShaderSource::FILE};
    shader_collection_.scatter_shader = ScatterShader{base_path + "scatter_shader.vs", base_path + "scatter_shader.fs", ShaderSource::FILE};
    shader_collection_.draw_mesh_shader = DrawMeshShader{base_path + "draw_mesh_shader.vs", base_path + "draw_mesh_shader.fs", ShaderSource::FILE};
    shader_collection_.legend_shader = createShader("legend_shader");
    shader_collection_.screen_space_shader = createShader("screen_space_shader");
}

void PlotPane::processActionQueue()
{
    using namespace duoplot::internal;

    while (!queued_data_.empty())
    {
        const Function fcn = queued_data_.front()->getFunction();

        if (fcn == Function::AXES_2D || fcn == Function::AXES_3D)
        {
            axes_set_ = true;
            auto [received_data, plot_object_attributes, user_supplied_properties] =
                queued_data_.front()->moveAllDataButConvertedData();

            const CommunicationHeader& hdr = received_data.getCommunicationHeader();
            const std::pair<Vec3d, Vec3d> axes_bnd =
                hdr.get(CommunicationHeaderObjectType::AXIS_MIN_MAX_VEC).as<std::pair<Vec3d, Vec3d>>();

            if (fcn == Function::AXES_2D)
            {
                axes_interactor_.setAxesLimits(Vec2d(axes_bnd.first.x, axes_bnd.first.y),
                                               Vec2d(axes_bnd.second.x, axes_bnd.second.y));
            }
            else
            {
                axes_interactor_.setAxesLimits(Vec3d(axes_bnd.first.x, axes_bnd.first.y, axes_bnd.first.z),
                                               Vec3d(axes_bnd.second.x, axes_bnd.second.y, axes_bnd.second.z));
            }
        }
        else if (fcn == Function::VIEW)
        {
            view_set_ = true;
            auto [received_data, plot_object_attributes, user_supplied_properties] =
                queued_data_.front()->moveAllDataButConvertedData();

            const CommunicationHeader& hdr = received_data.getCommunicationHeader();
            const float azimuth = hdr.get(CommunicationHeaderObjectType::AZIMUTH).as<float>();
            const float elevation = hdr.get(CommunicationHeaderObjectType::ELEVATION).as<float>();

            const float azimuth_rad = azimuth * M_PI / 180.0f;
            const float elevation_rad = elevation * M_PI / 180.0f;

            axes_interactor_.setViewAngles(azimuth_rad, elevation_rad);
        }
        else if (fcn == Function::CLEAR)
        {
            pending_clear_ = true;
        }
        else if (isPlotDataFunction(fcn))
        {
            // Plot object data - pass to plot data handler
            if (plot_data_handler_ != nullptr)
            {
                auto [received_data, plot_object_attributes, user_supplied_properties, converted_data] =
                    queued_data_.front()->moveAllData();

                const CommunicationHeader& hdr = received_data.getCommunicationHeader();
                plot_data_handler_->addData(hdr, plot_object_attributes, user_supplied_properties,
                                           received_data, converted_data);
            }
        }

        queued_data_.pop();
    }

    if (axes_set_ && view_set_)
    {
        const int window_width = width();
        const int window_height = height();
        axes_interactor_.updateWindowSize(window_width, window_height);
    }
}

void PlotPane::clearPane()
{
    if (plot_data_handler_ != nullptr)
    {
        plot_data_handler_->clear();
    }
    axes_set_ = false;
    view_set_ = false;
}

void PlotPane::mousePressEvent(QMouseEvent* event)
{
    last_mouse_pos_ = event->pos();

    const Vec2f mouse_pos_normalized{
        static_cast<float>(last_mouse_pos_.x()) / static_cast<float>(width()),
        static_cast<float>(last_mouse_pos_.y()) / static_cast<float>(height())
    };

    if (event->button() == Qt::LeftButton)
    {
        is_rotating_ = true;
        axes_interactor_.registerMousePressed(mouse_pos_normalized);

        if (event->modifiers() & Qt::ShiftModifier)
        {
            shift_pressed_at_mouse_press_ = true;
            axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::ROTATE);
        }
    }
    else if (event->button() == Qt::MiddleButton)
    {
        is_panning_ = true;
        axes_interactor_.registerMousePressed(mouse_pos_normalized);

        if (event->modifiers() & Qt::ShiftModifier)
        {
            shift_pressed_at_mouse_press_ = true;
            axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::PAN);
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        // Right-click: zoom mode with shift
        if (event->modifiers() & Qt::ShiftModifier)
        {
            is_zooming_ = true;
            shift_pressed_at_mouse_press_ = true;
            axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::ZOOM);
            axes_interactor_.registerMousePressed(mouse_pos_normalized);
            DUOPLOT_LOG_DEBUG() << "Right-click with shift: zoom mode enabled";
        }
        else
        {
            DUOPLOT_LOG_DEBUG() << "Right-click at position: (" << event->pos().x() << ", " << event->pos().y() << ")";
        }
    }
}

void PlotPane::mouseMoveEvent(QMouseEvent* event)
{
    QPoint current_pos = event->pos();

    if (is_rotating_ || is_panning_ || is_zooming_)
    {
        axes_interactor_.registerMouseDragInput(MouseInteractionAxis::ALL,
                                                 current_pos.x(),
                                                 current_pos.y(),
                                                 current_pos.x() - last_mouse_pos_.x(),
                                                 current_pos.y() - last_mouse_pos_.y());
        update();
    }

    last_mouse_pos_ = current_pos;
}

void PlotPane::mouseReleaseEvent(QMouseEvent* event)
{
    QPoint current_pos = event->pos();

    const Vec2f mouse_pos_normalized{
        static_cast<float>(current_pos.x()) / static_cast<float>(width()),
        static_cast<float>(current_pos.y()) / static_cast<float>(height())
    };

    if (event->button() == Qt::LeftButton)
    {
        is_rotating_ = false;
        axes_interactor_.registerMouseReleased(mouse_pos_normalized);

        if (shift_pressed_at_mouse_press_)
        {
            shift_pressed_at_mouse_press_ = false;
            axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::UNCHANGED);
        }
    }
    else if (event->button() == Qt::MiddleButton)
    {
        is_panning_ = false;
        axes_interactor_.registerMouseReleased(mouse_pos_normalized);

        if (shift_pressed_at_mouse_press_)
        {
            shift_pressed_at_mouse_press_ = false;
            axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::UNCHANGED);
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        if (is_zooming_)
        {
            is_zooming_ = false;
            axes_interactor_.registerMouseReleased(mouse_pos_normalized);

            if (shift_pressed_at_mouse_press_)
            {
                shift_pressed_at_mouse_press_ = false;
                axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::UNCHANGED);
            }
        }
        update();
    }
}

void PlotPane::wheelEvent(QWheelEvent* event)
{
    // Mouse wheel zooming is handled via keyboard 'z' + mouse drag in the original implementation
    // For now, we'll just trigger a repaint
    // TODO: Implement proper scroll wheel zoom if desired
    update();
}

void PlotPane::pushQueue(std::queue<std::unique_ptr<InputData>>& new_queue)
{
    while (!new_queue.empty())
    {
        queued_data_.push(std::move(new_queue.front()));
        new_queue.pop();
    }
    update();
}

void PlotPane::addPlotData(ReceivedData& received_data,
                           const PlotObjectAttributes& plot_object_attributes,
                           const UserSuppliedProperties& user_supplied_properties,
                           const std::shared_ptr<const ConvertedDataBase>& converted_data)
{
    if (plot_data_handler_ != nullptr)
    {
        const duoplot::internal::CommunicationHeader& hdr = received_data.getCommunicationHeader();
        plot_data_handler_->addData(hdr, plot_object_attributes, user_supplied_properties,
                                   received_data, converted_data);
        update();
    }
}

void PlotPane::clear()
{
    pending_clear_ = true;
    update();
}

void PlotPane::updateSizeFromParent(const QSize& parent_size)
{
    // Calculate size based on element settings
    const int new_width = static_cast<int>(element_settings_->width * parent_size.width());
    const int new_height = static_cast<int>(element_settings_->height * parent_size.height());
    const int new_x = static_cast<int>(element_settings_->x * parent_size.width());
    const int new_y = static_cast<int>(element_settings_->y * parent_size.height());

    setGeometry(new_x, new_y, new_width, new_height);
}

void PlotPane::keyPressedElementSpecific(const char key)
{
    // Handle plot-specific key presses
    // TODO: Implement key bindings for view manipulation, etc.
    DUOPLOT_LOG_DEBUG() << "PlotPane key pressed: " << key;
}

void PlotPane::keyReleasedElementSpecific(const char key)
{
    // Handle plot-specific key releases
    DUOPLOT_LOG_DEBUG() << "PlotPane key released: " << key;
}
