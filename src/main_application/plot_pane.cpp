#include "plot_pane.h"

#include <wx/event.h>
#include <wx/glcanvas.h>

#include "axes/axes.h"
#include "axes/axes_side_configuration.h"
#include "duoplot/enumerations.h"
#include "duoplot/math/math.h"
#include "events.h"
#include "platform_paths.h"
#include "plot_objects/stream_objects/stream_objects.h"

using namespace duoplot::internal;

wxGLAttributes PlotPane::getGLAttributes() const
{
    wxGLAttributes attrs;
    attrs.PlatformDefaults().RGBA().DoubleBuffer().Depth(16).Samplers(4).SampleBuffers(1).EndList();

    if (!wxGLCanvas::IsDisplaySupported(attrs))
    {
        attrs.Reset();
        attrs.PlatformDefaults().RGBA().DoubleBuffer().Depth(16).EndList();

        if (!wxGLCanvas::IsDisplaySupported(attrs))
        {
            attrs.Reset();
            attrs.PlatformDefaults().Defaults().EndList();

            if (!wxGLCanvas::IsDisplaySupported(attrs))
            {
                throw std::runtime_error("Unable to set acceptable wxGLAttributes!");
            }
        }
    }

    return attrs;
}

wxGLContext* PlotPane::getContext()
{
#ifdef PLATFORM_APPLE_M
    wxGLContextAttrs cxtAttrs;
    cxtAttrs.PlatformDefaults().CoreProfile().OGLVersion(3, 2).EndList();
    // https://stackoverflow.com/questions/41145024/wxwidgets-and-modern-opengl-3-3
    return new wxGLContext(this, NULL, &cxtAttrs);
#endif

#ifdef PLATFORM_LINUX_M
    return new wxGLContext(this);
#endif
}

template <typename T> T createShader(const std::string& base_path, const std::string& shader_name)
{
    const std::string v_path = base_path + shader_name + ".vs";
    const std::string f_path = base_path + shader_name + ".fs";

    return T{v_path, f_path, ShaderSource::FILE};
}

void PlotPane::initShaders()
{
    const std::string base_path{getResourcesPathString() + "/shaders/"};

    shader_collection_.plot_box_shader = createShader<ShaderBase>(base_path, "plot_box_shader");
    shader_collection_.pane_background_shader = createShader<ShaderBase>(base_path, "pane_background");
    shader_collection_.text_shader = createShader<TextShader>(base_path, "text");
    shader_collection_.basic_plot_shader = createShader<ShaderBase>(base_path, "basic_plot_shader");
    shader_collection_.plot_2d_shader = createShader<Plot2DShader>(base_path, "plot_2d_shader");
    shader_collection_.plot_3d_shader = createShader<Plot3DShader>(base_path, "plot_3d_shader");
    shader_collection_.img_plot_shader = createShader<ImShowShader>(base_path, "img");
    shader_collection_.scatter_shader = createShader<ScatterShader>(base_path, "scatter_shader");
    shader_collection_.draw_mesh_shader = createShader<DrawMeshShader>(base_path, "draw_mesh_shader");
    shader_collection_.legend_shader = createShader<ShaderBase>(base_path, "legend_shader");
    shader_collection_.screen_space_shader = createShader<ShaderBase>(base_path, "screen_space_shader");
}

PlotPane::PlotPane(
    wxWindow* parent,
    const std::shared_ptr<ElementSettings>& element_settings,
    const RGBTripletf& tab_background_color,
    const std::function<void(const char key)>& notify_main_window_key_pressed,
    const std::function<void(const char key)>& notify_main_window_key_released,
    const std::function<void(const wxPoint pos, const std::string& elem_name)>&
        notify_parent_window_right_mouse_pressed,
    const std::function<void()>& notify_main_window_about_modification,
    const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>& notify_tab_about_editing,
    const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window)
    : wxGLCanvas(parent, getGLAttributes()),
      ApplicationGuiElement{element_settings,
                            notify_main_window_key_pressed,
                            notify_main_window_key_released,
                            notify_parent_window_right_mouse_pressed,
                            notify_main_window_about_modification,
                            notify_tab_about_editing,
                            push_text_to_cmdl_output_window},
      m_context(getContext()),
      axes_interactor_(axes_settings_, getWidth(), getHeight()),
      plot_pane_settings_{std::dynamic_pointer_cast<PlotPaneSettings>(element_settings)}
{
    pending_clear_ = false;
    perspective_projection_ =
        (plot_pane_settings_->projection_mode == PlotPaneSettings::ProjectionMode::PERSPECTIVE) ? true : false;

    axes_interactor_.setViewAnglesSnapAngle(plot_pane_settings_->snap_view_to_axes ? (5.0 * M_PI / 180.0) : 0.0);

    should_render_point_selection_ = false;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    Show();

    wait_for_flush_ = false;
    left_mouse_pressed_ = false;

    bindCallbacks();

    wxGLCanvas::SetCurrent(*m_context);
    initShaders();

    axes_renderer_ = new AxesRenderer(shader_collection_, *plot_pane_settings_, tab_background_color);
    plot_data_handler_ = new PlotDataHandler(shader_collection_);

    axes_set_ = false;
    view_set_ = false;
    axes_from_min_max_disabled_ = false;
    axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::UNCHANGED);

    Bind(wxEVT_RIGHT_DOWN, &ApplicationGuiElement::mouseRightPressed, this);
    Bind(wxEVT_MIDDLE_DOWN, &PlotPane::mouseMiddlePressed, this);

    Bind(wxEVT_MIDDLE_UP, &PlotPane::mouseMiddleReleased, this);
    Bind(wxEVT_RIGHT_UP, &ApplicationGuiElement::mouseRightReleased, this);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);
    current_mouse_interaction_axis_ = MouseInteractionAxis::ALL;

    initSubscribedStreams();
}

void PlotPane::pushStreamData(const TopicId topic_id, const std::vector<std::shared_ptr<objects::BaseObject>>& objects)
{
    if (new_objects_.find(topic_id) == new_objects_.end())
    {
        new_objects_[topic_id] = objects;
    }
    else
    {
        new_objects_[topic_id].insert(new_objects_[topic_id].end(), objects.begin(), objects.end());
    }
}

void PlotPane::initSubscribedStreams()
{
    for (const SubscribedStreamSettings& subscribed_stream : plot_pane_settings_->subscribed_streams)
    {
        if (subscribed_stream.stream_type == StreamType::PLOT)
        {
            subscribed_streams_[subscribed_stream.topic_id] = new Plot2DStream(subscribed_stream, shader_collection_);
        }
        else if (subscribed_stream.stream_type == StreamType::PLOT3D)
        {
            std::cout << "Creating plot3d" << std::endl;
        }
        else if (subscribed_stream.stream_type == StreamType::SCATTER)
        {
            subscribed_streams_[subscribed_stream.topic_id] = new ScatterStream(subscribed_stream, shader_collection_);
        }
        else if (subscribed_stream.stream_type == StreamType::STAIRS)
        {
            subscribed_streams_[subscribed_stream.topic_id] = new StairsStream(subscribed_stream, shader_collection_);
        }
        else if (subscribed_stream.stream_type == StreamType::SCATTER3D)
        {
            std::cout << "Creating scatter3d" << std::endl;
        }
        else
        {
            std::cout << "Unknown stream type!" << std::endl;
        }
    }
}

void PlotPane::updateSizeFromParent(const wxSize& parent_size)
{
    setElementPositionAndSize();
}

void PlotPane::setMinimumXPos(const int new_min_x_pos)
{
    minimum_x_pos_ = new_min_x_pos;
}

void PlotPane::setSize(const wxSize& new_size)
{
    // axes_renderer_->setWindowSize(new_size.GetWidth(), new_size.GetHeight());
#ifdef PLATFORM_LINUX_M
    // This seems to be needed on linux platforms
    glViewport(0, 0, new_size.GetWidth(), new_size.GetHeight());
#endif
    this->SetSize(new_size);
}

void PlotPane::bindCallbacks()
{
    Bind(wxEVT_LEFT_DOWN, &ApplicationGuiElement::mouseLeftPressed, this);
    Bind(wxEVT_MOTION, &ApplicationGuiElement::mouseMovedOverItem, this);
    Bind(wxEVT_LEFT_UP, &ApplicationGuiElement::mouseLeftReleased, this);

    Bind(wxEVT_KEY_DOWN, &ApplicationGuiElement::keyPressedCallback, this);
    Bind(wxEVT_KEY_UP, &ApplicationGuiElement::keyReleasedCallback, this);
    Bind(wxEVT_PAINT, &PlotPane::render, this);
    Bind(wxEVT_ENTER_WINDOW, &ApplicationGuiElement::mouseEnteredElement, this);
    Bind(wxEVT_LEAVE_WINDOW, &ApplicationGuiElement::mouseLeftElement, this);
}

PlotPane::~PlotPane()
{
    delete plot_data_handler_;
    delete m_context;
}

void PlotPane::addSettingsData(const ReceivedData& received_data,
                               const PlotObjectAttributes& plot_object_attributes,
                               const UserSuppliedProperties& user_supplied_properties)
{
    const duoplot::internal::CommunicationHeader& hdr = received_data.getCommunicationHeader();
    const Function fcn = hdr.getFunction();

    if (fcn == Function::AXES_2D)
    {
        axes_set_ = true;

        const std::pair<Vec3d, Vec3d> axes_bnd =
            hdr.get(CommunicationHeaderObjectType::AXIS_MIN_MAX_VEC).as<std::pair<Vec3d, Vec3d>>();
        axes_interactor_.setAxesLimits(Vec2d(axes_bnd.first.x, axes_bnd.first.y),
                                       Vec2d(axes_bnd.second.x, axes_bnd.second.y));
    }
    else if (fcn == Function::AXES_3D)
    {
        axes_set_ = true;

        const std::pair<Vec3d, Vec3d> axes_bnd =
            hdr.get(CommunicationHeaderObjectType::AXIS_MIN_MAX_VEC).as<std::pair<Vec3d, Vec3d>>();
        axes_interactor_.setAxesLimits(Vec3d(axes_bnd.first.x, axes_bnd.first.y, axes_bnd.first.z),
                                       Vec3d(axes_bnd.second.x, axes_bnd.second.y, axes_bnd.second.z));
    }
    else if (fcn == Function::PROPERTIES_EXTENSION)
    {
        plot_data_handler_->propertiesExtension(plot_object_attributes.id, user_supplied_properties);
    }
    else if (fcn == Function::PROPERTIES_EXTENSION_MULTIPLE)
    {
        plot_data_handler_->propertiesExtensionMultiple(received_data);
    }
    else if (fcn == Function::CURRENT_ELEMENT_AS_IMAGE_VIEW)
    {
        axes_interactor_.setIsImageView(true);
    }
    else if (fcn == Function::DELETE_PLOT_OBJECT)
    {
        const ItemId id = hdr.get(CommunicationHeaderObjectType::ITEM_ID).as<ItemId>();
        plot_data_handler_->deletePlotObject(id);
        point_selection_.deletePlotObject(id);
    }
    else if (fcn == Function::GLOBAL_ILLUMINATION)
    {
        axes_renderer_->activateGlobalIllumination(hdr.get(CommunicationHeaderObjectType::VEC3).as<Vec3d>());
    }
    else if (fcn == Function::VIEW)
    {
        view_set_ = true;
        const float azimuth = hdr.get(internal::CommunicationHeaderObjectType::AZIMUTH).as<float>();
        const float elevation = hdr.get(internal::CommunicationHeaderObjectType::ELEVATION).as<float>();

        const float azimuth_rad = azimuth * M_PI / 180.0f;
        const float elevation_rad = elevation * M_PI / 180.0f;

        axes_interactor_.setViewAngles(azimuth_rad, elevation_rad);
    }
    else if (fcn == Function::CLEAR)
    {
        clearPane();
    }
    else if (fcn == Function::SHOW_LEGEND)
    {
        axes_interactor_.showLegend(true);
    }
    else if (fcn == Function::WAIT_FOR_FLUSH)
    {
        wait_for_flush_ = true;
    }
    else if (fcn == Function::FLUSH_ELEMENT)
    {
        // Do nothing...
    }
    else if (fcn == Function::AXES_SQUARE)
    {
        axes_renderer_->setAxesSquare(true);
    }
    else if (fcn == Function::DISABLE_SCALE_ON_ROTATION)
    {
        axes_renderer_->setScaleOnRotation(false);
    }
    else if (fcn == Function::SOFT_CLEAR)
    {
        plot_data_handler_->softClear();
        point_selection_.softClear();
        should_render_point_selection_ = false;
    }
    else if (fcn == Function::DISABLE_AXES_FROM_MIN_MAX)
    {
        axes_from_min_max_disabled_ = true;
    }
    else if (fcn == Function::SET_AXES_BOX_SCALE_FACTOR)
    {
        const Vec3d scale_vec = hdr.get(CommunicationHeaderObjectType::VEC3).as<Vec3d>() * 3.0;
        axes_renderer_->setAxesBoxScaleFactor(scale_vec);
    }
    else if (fcn == Function::SET_TITLE)
    {
        const std::string pane_title =
            hdr.get(CommunicationHeaderObjectType::TITLE_STRING).as<properties::Label>().data;
        axes_renderer_->setTitle(pane_title);
    }
    else if (fcn == Function::SET_OBJECT_TRANSFORM)
    {
        const Vec3d translation_vec = hdr.get(CommunicationHeaderObjectType::TRANSLATION_VECTOR).as<Vec3d>();
        const MatrixFixed<double, 3, 3> rotation_mat =
            hdr.get(CommunicationHeaderObjectType::ROTATION_MATRIX).as<MatrixFixed<double, 3, 3>>();

        const ItemId id = hdr.get(CommunicationHeaderObjectType::ITEM_ID).as<ItemId>();

        const MatrixFixed<double, 3, 3> scale =
            hdr.get(CommunicationHeaderObjectType::SCALE_MATRIX).as<MatrixFixed<double, 3, 3>>();

        const MatrixFixed<double, 3, 3> inv_rotation_mat = rotation_mat.transposed();
        plot_data_handler_->setTransform(id, inv_rotation_mat, translation_vec, scale);
    }
    else
    {
        throw std::runtime_error("Invalid function!");
    }
}

void PlotPane::pushQueue(std::queue<std::unique_ptr<InputData>>& new_queue)
{
    const size_t queue_size = new_queue.size();

    for (size_t k = 0; k < queue_size; k++)
    {
        queued_data_.push(std::move(new_queue.front()));
        new_queue.pop();
    }
}

void PlotPane::update()
{
    if (!queued_data_.empty() || new_objects_.size() > 0U)
    {
        Refresh();
    }
}

bool PlotPane::isImageFunction(const Function fcn)
{
    return fcn == Function::IM_SHOW;
}

bool PlotPane::is3DFunction(const Function fcn)
{
    return (fcn == Function::DRAW_LINE3D) || (fcn == Function::PLANE_XY) || (fcn == Function::PLANE_XZ) ||
           (fcn == Function::PLANE_YZ) || (fcn == Function::DRAW_MESH) ||
           (fcn == Function::DRAW_MESH_SEPARATE_VECTORS) || (fcn == Function::DRAW_TRIANGLES_3D) ||
           (fcn == Function::DRAW_TRIANGLE_3D) || (fcn == Function::SURF) || (fcn == Function::PLOT3) ||
           (fcn == Function::SCATTER3) || (fcn == Function::DRAW_LINE_BETWEEN_POINTS_3D) ||
           (fcn == Function::POLYGON_FROM_4_POINTS) || (fcn == Function::DRAW_TILES) ||
           (fcn == Function::PLOT_COLLECTION3) || (fcn == Function::FAST_PLOT3) || (fcn == Function::LINE_COLLECTION3);
}

void PlotPane::waitForFlush()
{
    wait_for_flush_ = true;
}

void PlotPane::toggleProjectionMode()
{
    perspective_projection_ = !perspective_projection_;
    plot_pane_settings_->projection_mode = perspective_projection_ ? PlotPaneSettings::ProjectionMode::PERSPECTIVE
                                                                   : PlotPaneSettings::ProjectionMode::ORTHOGRAPHIC;
    Refresh();
}

/*void PlotPane::showLegend(const bool show_legend)
{
    axes_interactor_.showLegend(show_legend);
    if (!wait_for_flush_)
    {
        Refresh();
    }
}*/

// void PlotPane::destroy()
// {
//     this->Destroy();
// }

void PlotPane::setHandleString(const std::string& new_name)
{
    plot_pane_settings_->handle_string = new_name;
    Refresh();
}

void PlotPane::mouseLeftPressedGuiElementSpecific(wxMouseEvent& event)
{
    const wxPoint mouse_pos_at_press_local = event.GetPosition();

    left_mouse_pressed_ = true;

    const Vec2f mouse_pos_at_press_normalized{
        Vec2f(mouse_pos_at_press_local.x, mouse_pos_at_press_local.y)
            .elementWiseDivide(Vec2f(element_size_at_press_.x, element_size_at_press_.y))};

    axes_interactor_.registerMousePressed(mouse_pos_at_press_normalized);

    if (wxGetKeyState(WXK_SHIFT))
    {
        shift_pressed_at_mouse_press_ = true;
        axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::ROTATE);
    }

    Refresh();
}

void PlotPane::mouseLeftReleasedGuiElementSpecific(wxMouseEvent& event)
{
    const wxPoint mouse_local_pos{event.GetPosition()};
    const wxSize size_now = this->GetSize();

    axes_interactor_.registerMouseReleased(
        Vec2f(mouse_local_pos.x, mouse_local_pos.y).elementWiseDivide(Vec2f(size_now.x, size_now.y)));

    left_mouse_pressed_ = false;

    if (shift_pressed_at_mouse_press_)
    {
        shift_pressed_at_mouse_press_ = false;
        axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::UNCHANGED);
    }

    Refresh();
}

void PlotPane::mouseMiddlePressed(wxMouseEvent& event)
{
    const wxPoint current_point = event.GetPosition();

    if (wxGetKeyState(WXK_SHIFT))
    {
        shift_pressed_at_mouse_press_ = true;
        axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::PAN);
    }
}

void PlotPane::mouseMiddleReleased(wxMouseEvent& event)
{
    if (shift_pressed_at_mouse_press_)
    {
        shift_pressed_at_mouse_press_ = false;
        axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::UNCHANGED);
    }

    Refresh();
}

void PlotPane::mouseRightPressedGuiElementSpecific(wxMouseEvent& event)
{
    if (wxGetKeyState(WXK_SHIFT))
    {
        axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::ZOOM);
    }
}

void PlotPane::mouseRightReleasedGuiElementSpecific(wxMouseEvent& event)
{
    if (event.RightIsDown())
    {
        if (shift_pressed_at_mouse_press_)
        {
            shift_pressed_at_mouse_press_ = false;
            axes_interactor_.setOverriddenMouseInteractionType(MouseInteractionType::UNCHANGED);
        }

        Refresh();
    }
}

void PlotPane::mouseMovedGuiElementSpecific(wxMouseEvent& event)
{
    const wxPoint current_mouse_position_local = event.GetPosition();

    if (wxGetKeyState(static_cast<wxKeyCode>('l')) && event.LeftIsDown())
    {
        // Adjust plot box size
        const Vec2d diff(current_mouse_pos_.x - previous_mouse_pos_.x, current_mouse_pos_.y - previous_mouse_pos_.y);
        Vec3d scale_factor = axes_renderer_->getAxesBoxScaleFactor();
        if (diff.x < 0)
        {
            scale_factor = scale_factor * 0.99;
        }
        else
        {
            scale_factor = scale_factor * 1.01;
        }

        axes_renderer_->setAxesBoxScaleFactor(scale_factor);
    }
    else if ((event.LeftIsDown() || event.MiddleIsDown() || event.RightIsDown()))
    {
        if (event.LeftIsDown() && control_pressed_at_mouse_press_)
        {
            adjustPaneSizeOnMouseMoved();
        }
        else if (shift_pressed_at_mouse_press_)
        {
            axes_interactor_.registerMouseDragInput(current_mouse_interaction_axis_,
                                                    current_mouse_position_local.x,
                                                    current_mouse_position_local.y,
                                                    current_mouse_pos_.x - previous_mouse_pos_.x,
                                                    current_mouse_pos_.y - previous_mouse_pos_.y);
        }
        else if (event.LeftIsDown())
        {
            axes_interactor_.registerMouseDragInput(current_mouse_interaction_axis_,
                                                    current_mouse_position_local.x,
                                                    current_mouse_position_local.y,
                                                    current_mouse_pos_.x - previous_mouse_pos_.x,
                                                    current_mouse_pos_.y - previous_mouse_pos_.y);
        }

        Refresh();
    }
    /*else
    {
        if (wxGetKeyState(WXK_COMMAND))
        {
            setCursorDependingOnMousePos(current_mouse_position_local);
        }
    }*/

    // previous_mouse_pos_ = current_mouse_pos_;
}

void PlotPane::setMouseInteractionType(const MouseInteractionType mit)
{
    axes_interactor_.setMouseInteractionType(mit);
}

void PlotPane::keyPressedElementSpecific(const char key)
{
    if (wxGetKeyState(static_cast<wxKeyCode>('1')) && wxGetKeyState(static_cast<wxKeyCode>('2')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::XY;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('2')) && wxGetKeyState(static_cast<wxKeyCode>('3')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::YZ;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('1')) && wxGetKeyState(static_cast<wxKeyCode>('3')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::XZ;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('1')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::X;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('2')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::Y;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('3')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::Z;
    }

    if (wxGetKeyState(static_cast<wxKeyCode>('r')) || wxGetKeyState(static_cast<wxKeyCode>('R')))
    {
        axes_interactor_.setMouseInteractionType(MouseInteractionType::ROTATE);
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('z')) || wxGetKeyState(static_cast<wxKeyCode>('Z')))
    {
        axes_interactor_.setMouseInteractionType(MouseInteractionType::ZOOM);
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('p')) || wxGetKeyState(static_cast<wxKeyCode>('P')))
    {
        axes_interactor_.setMouseInteractionType(MouseInteractionType::PAN);
    }
    // else if (wxGetKeyState(static_cast<wxKeyCode>('s')) || wxGetKeyState(static_cast<wxKeyCode>('S')))
    // {
    //     axes_interactor_.setMouseInteractionType(MouseInteractionType::POINT_SELECTION);
    // }

    Refresh();
}

void PlotPane::keyReleasedElementSpecific(const char key)
{
    current_mouse_interaction_axis_ = MouseInteractionAxis::ALL;

    if (wxGetKeyState(static_cast<wxKeyCode>('1')) && wxGetKeyState(static_cast<wxKeyCode>('2')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::XY;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('2')) && wxGetKeyState(static_cast<wxKeyCode>('3')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::YZ;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('1')) && wxGetKeyState(static_cast<wxKeyCode>('3')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::XZ;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('1')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::X;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('2')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::Y;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('3')))
    {
        current_mouse_interaction_axis_ = MouseInteractionAxis::Z;
    }

    Refresh();
}

void PlotPane::setMinXPos(const int min_x_pos)
{
    minimum_x_pos_ = min_x_pos;
    setElementPositionAndSize();
}

// Returns window width in pixels
int PlotPane::getWidth()
{
    return GetSize().x;
}

// Returns window height in pixels
int PlotPane::getHeight()
{
    return GetSize().y;
}

MouseInteractionType getMouseInteractionType()
{
    if (wxGetKeyState(static_cast<wxKeyCode>('p')) || wxGetKeyState(static_cast<wxKeyCode>('P')))
    {
        return MouseInteractionType::PAN;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('r')) || wxGetKeyState(static_cast<wxKeyCode>('R')))
    {
        return MouseInteractionType::ROTATE;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('z')) || wxGetKeyState(static_cast<wxKeyCode>('Z')))
    {
        return MouseInteractionType::ZOOM;
    }
    else
    {
        return MouseInteractionType::UNCHANGED;
    }
}

void PlotPane::updateElementSettings(const std::map<std::string, std::string>& new_settings)
{
    plot_pane_settings_->handle_string = new_settings.at("handle_string");
    if (new_settings.count("title") > 0U)
    {
        plot_pane_settings_->title = new_settings.at("title");
    }
}

bool viewShouldBeReset()
{
    if (wxGetKeyState(static_cast<wxKeyCode>('c')))
    {
        return true;
    }
    else
    {
        return false;
    }
}

void PlotPane::addPlotData(ReceivedData& received_data,
                           const PlotObjectAttributes& plot_object_attributes,
                           const UserSuppliedProperties& user_supplied_properties,
                           const std::shared_ptr<const ConvertedDataBase>& converted_data)
{
    const CommunicationHeader& hdr = received_data.getCommunicationHeader();

    plot_data_handler_->addData(hdr, plot_object_attributes, user_supplied_properties, received_data, converted_data);
    point_selection_.addData(hdr, plot_object_attributes, user_supplied_properties, converted_data);

    internal::Function fcn = hdr.getFunction();

    // TODO: The fundamental design of this min/max needs to be changed.
    // For some types of plot data, it shouldn't be executed if axes is set,
    // but for some it should always be executed, for drawMesh for example,
    // to correctly set the color map min max values
    // And if it has already been executed once for a plot data, it shouldn't need
    // to be executed again. Also, if there are multiple color map plot datas
    // they should get their color map min max values from a global min/max of
    // all the plot datas
    // The whole getMinMaxVectors logic in plot_datas should probably also be redesigned
    const std::pair<Vec3d, Vec3d> min_max = plot_data_handler_->getMinMaxVectors();

    if ((!axes_set_) && (!axes_from_min_max_disabled_))
    {
        const Vec3d mean_vec = (min_max.second + min_max.first) / 2.0;

        const Vec3d min_vec = (min_max.first - mean_vec) * 1.001 + mean_vec;
        const Vec3d max_vec = (min_max.second - mean_vec) * 1.001 + mean_vec;

        axes_interactor_.setAxesLimits(min_vec, max_vec);
    }
    if ((!view_set_) && (!axes_from_min_max_disabled_))
    {
        if (is3DFunction(fcn))
        {
            const float azimuth = -64.0f * M_PI / 180.0f;
            const float elevation = 34.0f * M_PI / 180.0f;

            axes_interactor_.setViewAngles(azimuth, elevation);
        }
        else if (isImageFunction(fcn))
        {
            const float azimuth = 0.0f * M_PI / 180.0f;
            const float elevation = -90.0f * M_PI / 180.0f;

            axes_interactor_.setViewAngles(azimuth, elevation);
            view_set_ = true;  // Let imshow be dominant once used
        }
        else
        {
            const float azimuth = 0.0f * M_PI / 180.0f;
            const float elevation = 90.0f * M_PI / 180.0f;

            axes_interactor_.setViewAngles(azimuth, elevation);
        }
    }
}

void PlotPane::clearPane()
{
    plot_data_handler_->clear();
    point_selection_.clear();
    should_render_point_selection_ = false;
    pending_clear_ = true;
    axes_set_ = false;
    view_set_ = false;
    wait_for_flush_ = false;
    axes_from_min_max_disabled_ = false;

    axes_interactor_.setIsImageView(false);
    axes_interactor_.setViewAngles(0, M_PI);
    axes_interactor_.setAxesLimits(Vec3d(-1.0, -1.0, -1.0), Vec3d(1.0, 1.0, 1.0));
    axes_interactor_.showLegend(false);
    axes_renderer_->resetGlobalIllumination();
    axes_renderer_->setAxesBoxScaleFactor(Vec3d{2.5, 2.5, 2.5});
    axes_renderer_->setScaleOnRotation(true);
    axes_renderer_->setAxesSquare(false);
    axes_renderer_->setTitle("");
}

void PlotPane::processActionQueue()
{
    while (!queued_data_.empty())
    {
        const internal::Function fcn = queued_data_.front()->getFunction();

        if (wait_for_flush_)
        {
            if (fcn == Function::FLUSH_ELEMENT)
            {
                while (!flush_queue_.empty())
                {
                    const internal::Function inner_fcn = flush_queue_.front()->getFunction();

                    if (isPlotDataFunction(inner_fcn))
                    {
                        auto [received_data, plot_object_attributes, user_supplied_properties, converted_data] =
                            flush_queue_.front()->moveAllData();
                        addPlotData(received_data, plot_object_attributes, user_supplied_properties, converted_data);
                    }
                    else
                    {
                        auto [received_data, plot_object_attributes, user_supplied_properties] =
                            flush_queue_.front()->moveAllDataButConvertedData();
                        addSettingsData(received_data, plot_object_attributes, user_supplied_properties);
                    }

                    flush_queue_.pop();
                }
                queued_data_.pop();
                break;
            }
            else if (fcn == Function::CLEAR)
            {
                clearPane();
                while (!flush_queue_.empty())
                {
                    flush_queue_.pop();
                }
                queued_data_.pop();
            }
            else
            {
                flush_queue_.push(std::move(queued_data_.front()));
                queued_data_.pop();
            }
        }
        else
        {
            if (isPlotDataFunction(fcn))
            {
                auto [received_data, plot_object_attributes, user_supplied_properties, converted_data] =
                    queued_data_.front()->moveAllData();

                addPlotData(received_data, plot_object_attributes, user_supplied_properties, converted_data);
            }
            else
            {
                auto [received_data, plot_object_attributes, user_supplied_properties] =
                    queued_data_.front()->moveAllDataButConvertedData();
                addSettingsData(received_data, plot_object_attributes, user_supplied_properties);
            }

            queued_data_.pop();

            if (fcn == Function::SOFT_CLEAR)
            {
                break;
            }
        }
    }
}

void PlotPane::refresh()
{
    Refresh();
}

void PlotPane::render(wxPaintEvent& WXUNUSED(evt))
{
    if (!IsShown())
    {
        return;
    }

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this);  // TODO: Can be removed?

    processActionQueue();

    for (auto& [topic_id, objects] : new_objects_)
    {
        if (subscribed_streams_.find(topic_id) == subscribed_streams_.end())
        {
            continue;
        }

        subscribed_streams_.find(topic_id)->second->appendNewData(objects);
    }
    new_objects_.clear();

    /*if (pending_clear_)
    {
        pending_clear_ = false;
        plot_data_handler_->clear();
    }*/

    glEnable(GL_MULTISAMPLE);

    const RGBTripletf color_vec{plot_pane_settings_->background_color};
    glClearColor(color_vec.red, color_vec.green, color_vec.blue, 0.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (viewShouldBeReset())
    {
        axes_interactor_.resetView();
    }

    axes_interactor_.updateWindowSize(getWidth(), getHeight());

    if (wxGetKeyState(static_cast<wxKeyCode>('y')) || wxGetKeyState(static_cast<wxKeyCode>('Y')))
    {
        legend_scale_factor_ -= 0.05;
    }
    else if (wxGetKeyState(static_cast<wxKeyCode>('u')) || wxGetKeyState(static_cast<wxKeyCode>('U')))
    {
        legend_scale_factor_ += 0.05;
    }

    const Vec2f pane_size(GetSize().x, GetSize().y);

    const AxesSideConfiguration axes_side_configuration{axes_interactor_.getViewAngles(), perspective_projection_};

    const Vec2f mouse_pos_at_press(mouse_pos_at_press_.x, mouse_pos_at_press_.y);
    const Vec2f current_mouse_pos(current_mouse_pos_.x, current_mouse_pos_.y);

    axes_renderer_->updateStates(axes_interactor_.getAxesLimits(),
                                 axes_interactor_.getViewAngles(),
                                 axes_interactor_.getQueryPoint(),
                                 axes_interactor_.generateGridVectors(),
                                 axes_side_configuration,
                                 perspective_projection_,
                                 getWidth(),
                                 getHeight(),
                                 mouse_pos_at_press.elementWiseDivide(pane_size),
                                 current_mouse_pos.elementWiseDivide(pane_size),
                                 axes_interactor_.getMouseInteractionType(),
                                 left_mouse_pressed_,
                                 axes_interactor_.shouldDrawZoomRect(),
                                 axes_interactor_.getShowLegend(),
                                 legend_scale_factor_,
                                 plot_data_handler_->getLegendStrings(),
                                 plot_pane_settings_->handle_string,
                                 current_mouse_interaction_axis_);

    axes_renderer_->render();
    glEnable(GL_DEPTH_TEST);  // TODO: Put in "plotBegin" and "plotEnd"?
    axes_renderer_->plotBegin();

    plot_data_handler_->render();
    for (auto& [topic_id, stream] : subscribed_streams_)
    {
        stream->render();
    }

    axes_renderer_->plotEnd();

    if (axes_interactor_.getQueryPoint().has_query_point)
    {
        bool has_closest_point = false;

        std::tie(closest_point_, has_closest_point) = point_selection_.getClosestPoint(
            axes_renderer_->getLine(), axes_interactor_.getQueryPoint().has_query_point);

        if (has_closest_point)
        {
            should_render_point_selection_ = true;
        }
    }

    if (should_render_point_selection_)
    {
        axes_renderer_->renderPointSelection(closest_point_);
    }

    glDisable(GL_DEPTH_TEST);

    // glFlush();
    SwapBuffers();
}
