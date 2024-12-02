#include "gui_pane.h"

#include <wx/event.h>
#include <wx/glcanvas.h>

using namespace duoplot::internal;

wxGLAttributes GuiPane::getGLAttributes() const
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

wxGLContext* GuiPane::getContext()
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

void GuiPane::initShaders()
{
    const std::string base_path{"../resources/shaders/"};

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
    shader_collection_.simple_shader = createShader<SimpleShader>(base_path, "simple_shader");
}

void GuiPane::sendElementToBackCallback(wxCommandEvent& WXUNUSED(event))
{
    if (right_clicked_element_ != nullptr)
    {
        // Find index of the element
        const auto element_it = std::find(gui_elements_.begin(), gui_elements_.end(), right_clicked_element_);
        if (element_it == gui_elements_.end())
        {
            throw std::runtime_error("Element not found!");
        }

        const int64_t element_index = std::distance(gui_elements_.begin(), element_it);

        if (element_index == 0)
        {
            // Already at the back
            return;
        }

        std::copy_backward(
            gui_elements_.begin(), gui_elements_.begin() + element_index, gui_elements_.begin() + element_index + 1);
        gui_elements_.front() = right_clicked_element_;

        //
        const wxPoint mouse_pos = wxGetMousePosition() - this->GetScreenPosition();
        const std::vector<std::shared_ptr<GuiElement>>::reverse_iterator potentially_hovered_element =
            std::find_if(gui_elements_.rbegin(), gui_elements_.rend(), [&mouse_pos](const auto& element) {
                return element->PointIsWithin(mouse_pos);
            });

        if (potentially_hovered_element != gui_elements_.rend())
        {
            if (current_hovered_element_ != nullptr)
            {
                // current_hovered_element_->mouseExited(event);
            }
            current_hovered_element_ = *potentially_hovered_element;

            if (control_pressed_)
            {
                const ChangeDirection change_dir = current_hovered_element_->GetDirectionFromMouse(mouse_pos);
                setCursor(change_dir);
            }
            else
            {
                wxSetCursor(wxCursor(wxCURSOR_ARROW));
            }
        }
        else
        {
            // current_hovered_element_->mouseExited(event);
            current_hovered_element_ = nullptr;
            wxSetCursor(wxCursor(wxCURSOR_ARROW));
        }
    }
    else
    {
        throw std::runtime_error("No element to send to back!");
    }
}

void GuiPane::bringElementToFrontCallback(wxCommandEvent& WXUNUSED(event))
{
    if (right_clicked_element_ != nullptr)
    {
        // Find index of the element
        const auto element_it = std::find(gui_elements_.begin(), gui_elements_.end(), right_clicked_element_);
        if (element_it == gui_elements_.end())
        {
            return;
        }

        const int64_t element_index = std::distance(gui_elements_.begin(), element_it);

        if (element_index == (static_cast<int64_t>(gui_elements_.size()) - 1))
        {
            // Already at the front
            return;
        }

        std::copy(
            gui_elements_.begin() + element_index + 1, gui_elements_.end(), gui_elements_.begin() + element_index);
        gui_elements_.back() = right_clicked_element_;
    }
    else
    {
        throw std::runtime_error("No element to bring to front!");
    }
}

void GuiPane::bringElementForwardCallback(wxCommandEvent& WXUNUSED(event))
{
    if (right_clicked_element_ != nullptr)
    {
        // Find index of the element
        const auto element_it = std::find(gui_elements_.begin(), gui_elements_.end(), right_clicked_element_);
        if (element_it == gui_elements_.end())
        {
            return;
        }

        const int64_t element_index = std::distance(gui_elements_.begin(), element_it);

        if (element_index == (static_cast<int64_t>(gui_elements_.size()) - 1))
        {
            // Already at the front
            return;
        }

        std::swap(gui_elements_[element_index], gui_elements_[element_index + 1]);
    }
    else
    {
        throw std::runtime_error("No element to bring forward!");
    }
}

void GuiPane::sendElementBackwardCallback(wxCommandEvent& WXUNUSED(event))
{
    if (right_clicked_element_ != nullptr)
    {
        // Find index of the element
        const auto element_it = std::find(gui_elements_.begin(), gui_elements_.end(), right_clicked_element_);
        if (element_it == gui_elements_.end())
        {
            return;
        }

        const int64_t element_index = std::distance(gui_elements_.begin(), element_it);

        if (element_index == 0)
        {
            // Already at the back
            return;
        }
        std::swap(gui_elements_[element_index], gui_elements_[element_index - 1]);
    }
    else
    {
        throw std::runtime_error("No element to send backward!");
    }
}

GuiPane::GuiPane(wxFrame* parent) : wxGLCanvas(parent, getGLAttributes(), wxID_ANY), m_context_(getContext())
{
    control_pressed_ = false;
    left_mouse_pressed_ = false;
    right_mouse_pressed_ = false;

    popup_menu_ = new wxMenu(wxT(""));

    popup_menu_->Append(EventIds::BRING_TO_FRONT, wxT("Bring to front"));
    popup_menu_->Append(EventIds::SEND_TO_BACK, wxT("Send to back"));
    popup_menu_->Append(EventIds::BRING_FORWARD, wxT("Bring forward"));
    popup_menu_->Append(EventIds::SEND_BACKWARD, wxT("Send backward"));

    set_shader_color_ = [this](const RGBTripletf color) {
        shader_collection_.simple_shader.base_uniform_handles.vertex_color.setColor(color);
    };

    interaction_state_ = InteractionState::Hoovering;

    change_direction_at_press_ = ChangeDirection::NONE;

    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    wxGLCanvas::SetCurrent(*m_context_);
    initShaders();
    const RGBTripletf background_color{0xC9E4DEU};
    glClearColor(background_color.red, background_color.green, background_color.blue, 0.0f);

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_PROGRAM_POINT_SIZE);

    glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

    Bind(wxEVT_MENU, &GuiPane::bringElementToFrontCallback, this, EventIds::BRING_TO_FRONT);
    Bind(wxEVT_MENU, &GuiPane::sendElementToBackCallback, this, EventIds::SEND_TO_BACK);

    Bind(wxEVT_MENU, &GuiPane::bringElementForwardCallback, this, EventIds::BRING_FORWARD);
    Bind(wxEVT_MENU, &GuiPane::sendElementBackwardCallback, this, EventIds::SEND_BACKWARD);

    Bind(wxEVT_LEFT_DOWN, &GuiPane::mouseLeftPressed, this);
    Bind(wxEVT_LEFT_UP, &GuiPane::mouseLeftReleased, this);
    Bind(wxEVT_LEFT_DCLICK, &GuiPane::mouseLeftDoubleClicked, this);

    Bind(wxEVT_RIGHT_DOWN, &GuiPane::mouseRightPressed, this);
    Bind(wxEVT_RIGHT_UP, &GuiPane::mouseRightReleased, this);

    Bind(wxEVT_MOTION, &GuiPane::mouseMoved, this);
    Bind(wxEVT_ENTER_WINDOW, &GuiPane::mouseEntered, this);
    Bind(wxEVT_LEAVE_WINDOW, &GuiPane::mouseExited, this);

    Bind(wxEVT_KEY_DOWN, &GuiPane::keyPressedCallback, this);
    Bind(wxEVT_KEY_UP, &GuiPane::keyReleasedCallback, this);

    Bind(wxEVT_SIZE, &GuiPane::OnSize, this);

    Bind(wxEVT_PAINT, &GuiPane::render, this);

    shader_collection_.simple_shader.use();
    shader_collection_.simple_shader.base_uniform_handles.use_clip_plane.setInt(0);

    /*gui_elements_.push_back(
        std::make_shared<GuiElement>(10.0f, 10.0f, 50.0f, 50.0f, "top left", RGBTripletf(0.6f, 0.5f, 0.0f)));

    gui_elements_.push_back(
        std::make_shared<GuiElement>(400.0f, 10.0f, 50.0f, 50.0f, "top right", RGBTripletf(0.6f, 0.5f, 0.0f)));

    gui_elements_.push_back(
        std::make_shared<GuiElement>(10.0f, 400.0f, 50.0f, 50.0f, "bottom left", RGBTripletf(0.6f, 0.5f, 0.0f)));

    gui_elements_.push_back(
        std::make_shared<GuiElement>(400.0f, 400.0f, 50.0f, 50.0f, "bottom right", RGBTripletf(0.6f, 0.5f, 0.0f)));

    gui_elements_.push_back(std::make_shared<GuiElement>(
        10.0f, 200.0f, 50.0f, 50.0f, "Overlapping left 0", RGBTripletf(1.0f, 0.0f, 0.0f) * 0.8));

    gui_elements_.push_back(std::make_shared<GuiElement>(
        30.0f, 210.0f, 50.0f, 50.0f, "Overlapping left 1", RGBTripletf(0.0f, 1.0f, 0.0f) * 0.8));

    gui_elements_.push_back(std::make_shared<GuiElement>(
        40.0f, 220.0f, 50.0f, 50.0f, "Overlapping left 2", RGBTripletf(0.0f, 1.0f, 1.0f) * 0.8));

    gui_elements_.push_back(std::make_shared<GuiElement>(
        50.0f, 230.0f, 50.0f, 50.0f, "Overlapping left 3", RGBTripletf(1.0f, 0.0f, 1.0f) * 0.8));

    gui_elements_.push_back(std::make_shared<GuiElement>(
        200.0f, 200.0f, 50.0f, 50.0f, "Precisely adjacent left", RGBTripletf(1.0f, 0.0f, 0.0f)));

    gui_elements_.push_back(std::make_shared<GuiElement>(
        250.0f, 210.0f, 50.0f, 50.0f, "Precisely adjacent right", RGBTripletf(0.0f, 1.0f, 0.0f)));*/

    // RectangularButtonConfiguration{700.0f, 410.0f, 70.0f, 40.0f},
    // RoundedRectangleButtonConfiguration{700.0f, 410.0f, 70.0f, 40.0f, 10.0f},
    // RoundedBarButtonConfiguration{700.0f, 410.0f, 70.0f, 40.0f},
    // CircularButtonConfiguration{700.0f, 410.0f, 40.0f},
    gui_elements_.push_back(std::make_shared<Button>(
        RoundedRectangleButtonConfiguration{700.0f, 410.0f, 70.0f, 40.0f, 10.0f},
        "Button",
        RGBTripletf(1.0f, 1.0f, 1.0f),
        set_shader_color_,
        [](const uint64_t id) { std::cout << "Button pressed: " << id << std::endl; },
        [](const uint64_t id) { std::cout << "Button released: " << id << std::endl; }));

    gui_elements_.push_back(std::make_shared<Slider>(
        700.0f,
        510.0f,
        250.0f,
        60.0f,
        -13,
        10,
        34,
        Slider::SliderDirection::HORIZONTAL,
        set_shader_color_,
        "Slider",
        RGBTripletf(1.0f, 1.0f, 1.0f),
        [](const uint64_t id, const int32_t new_value) {
            std::cout << "Slider dragged, new value: " << new_value << std::endl;
        },
        [](const uint64_t id, const int32_t new_value) {
            std::cout << "Slider final value: " << new_value << std::endl;
        }));

    gui_elements_.push_back(std::make_shared<Checkbox>(400.0f,
                                                       510.0f,
                                                       120.0f,
                                                       120.0f,
                                                       "Checkbox",
                                                       RGBTripletf(1.0f, 1.0f, 1.0f),
                                                       set_shader_color_,
                                                       [](const uint64_t id, const bool new_state) {
                                                           std::cout << "Checkbox " << id
                                                                     << " state changed: " << new_state << std::endl;
                                                       }));

    current_hovered_element_ = nullptr;
    current_pressed_element_ = nullptr;

    glEnable(GL_MULTISAMPLE);

    update_timer_.Bind(wxEVT_TIMER, &GuiPane::TimerFunc, this);
    update_timer_.Start(20);
}

void GuiPane::TimerFunc(wxTimerEvent&)
{
    Refresh();
}

void GuiPane::OnSize(wxSizeEvent& event)
{
    const wxSize new_size = event.GetSize();
    // std::cout << "Size event: " << new_size.GetWidth() << " " << new_size.GetHeight() << std::endl;

    shader_collection_.simple_shader.use();
    shader_collection_.simple_shader.uniform_handles.pane_width.setFloat(static_cast<float>(new_size.GetWidth()));
    shader_collection_.simple_shader.uniform_handles.pane_height.setFloat(static_cast<float>(new_size.GetHeight()));
    shader_collection_.simple_shader.uniform_handles.shader_mode.setInt(static_cast<int>(0));

    for (auto& gui_element_ : gui_elements_)
    {
        gui_element_->UpdateSizeFromParent(new_size);
    }
}

GuiPane::~GuiPane()
{
    delete m_context_;
}

void GuiPane::refresh()
{
    Refresh();
}

void GuiPane::render(wxPaintEvent& WXUNUSED(evt))
{
    if (!IsShown())
    {
        return;
    }

    // std::cout << "Rendering" << std::endl;

    wxGLCanvas::SetCurrent(*m_context_);
    wxPaintDC(this);  // TODO: Can be removed?

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader_collection_.simple_shader.use();
    // shader_collection_.simple_shader.base_uniform_handles.vertex_color.setColor(RGBTripletf(0.6f, 0.5f, 0.0f));

    for (auto& gui_element_ : gui_elements_)
    {
        shader_collection_.simple_shader.base_uniform_handles.vertex_color.setColor(gui_element_->getColor());
        shader_collection_.simple_shader.uniform_handles.shader_mode.setInt(
            static_cast<int32_t>(gui_element_->GetShaderMode()));

        gui_element_->render();
    }

    // glFlush();
    SwapBuffers();
}

void GuiPane::UpdateSizeFromParent(const wxSize new_size) {}
