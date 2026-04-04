#include "gui_pane.h"

#include <wx/event.h>
#include <wx/glcanvas.h>

using namespace lumos::internal;

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
    const std::string base_path{"/Users/danielpi/work/dvs/src/resources/shaders/"};

    shader_collection_.plot_box_shader = createShader<ShaderBase>(base_path, "plot_box_shader");
    shader_collection_.pane_background_shader = createShader<ShaderBase>(base_path, "pane_background");
    shader_collection_.text_shader = createShader<TextShader>(base_path, "text_test");
    shader_collection_.new_text_shader = createShader<NewTextShader>(base_path, "new_text_shader");
    shader_collection_.basic_plot_shader = createShader<ShaderBase>(base_path, "basic_plot_shader");
    shader_collection_.plot_2d_shader = createShader<Plot2DShader>(base_path, "plot_2d_shader");
    shader_collection_.plot_3d_shader = createShader<Plot3DShader>(base_path, "plot_3d_shader");
    shader_collection_.img_plot_shader = createShader<ImShowShader>(base_path, "img");
    shader_collection_.scatter_shader = createShader<ScatterShader>(base_path, "scatter_shader");
    shader_collection_.draw_mesh_shader = createShader<DrawMeshShader>(base_path, "draw_mesh_shader");
    shader_collection_.legend_shader = createShader<ShaderBase>(base_path, "legend_shader");
    shader_collection_.simple_shader = createShader<SimpleShader>(base_path, "simple_shader");
}

GuiPane::GuiPane(wxFrame* parent)
    : wxGLCanvas(parent, getGLAttributes()),
      m_context(getContext())
{
    SetBackgroundStyle(wxBG_STYLE_CUSTOM);

    Show();

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this);  // TODO: Can be removed?
    initShaders();
    text_renderer_ = new text_renderer();
    text_renderer_->init("/Users/danielpi/work/dvs/src/resources/fonts/Roboto-Regular.ttf");
    text_renderer_->add_text("Length = 98.76532101", glm::vec2(-0.8f, -0.3f), glm::vec3(1.0f, 1.0f, 0.0f), 45.0f, 0.0004f);
    text_renderer_->add_text("Big text!", glm::vec2(-1.5f, -0.3f), glm::vec3(1.0f, 1.0f, 0.0f), 0.0f, 0.002f);
    text_renderer_->add_text("Pulled Pork Sandwich!!", glm::vec2(-0.4f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), -15.0f, 0.0003f);
    text_renderer_->add_text("Hello", glm::vec2(-0.8f, 0.3f), glm::vec3(1.0f, 1.0f, 1.0f), 5.0f, 0.0003f);
    text_renderer_->build();

    glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
    
    glEnable(GL_DEPTH_CLAMP);
    // initFreetype();

    // glUniform1i(shader_collection_.text_shader.uniform_handles.text_sampler, 0);

    shader_collection_.new_text_shader.use();
    
    // glEnable(GL_MULTISAMPLE);
    // glEnable(GL_PROGRAM_POINT_SIZE);

    // glHint(GL_MULTISAMPLE_FILTER_HINT_NV, GL_NICEST);

    Bind(wxEVT_PAINT, &GuiPane::render, this);

    receive_timer_.Bind(wxEVT_TIMER, &GuiPane::TimerFunc, this);
    receive_timer_.Start(20);
}

void GuiPane::TimerFunc(wxTimerEvent&)
{
    Refresh();
}

GuiPane::~GuiPane()
{
    delete m_context;
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

    wxGLCanvas::SetCurrent(*m_context);
    wxPaintDC(this);

    glClearColor(0.1, 0.5, 0.5, 1.0);

    glClear(GL_COLOR_BUFFER_BIT);
    // glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*glUniform3f(shader_collection_.text_shader.uniform_handles.text_color, 0.0, 0.0, 0.0);
    wxSize size = GetSize();
    // std::cout << "Size: " << size.GetWidth() << " " << size.GetHeight() << std::endl;
    shader_collection_.text_shader.uniform_handles.pane_width.setFloat(size.GetWidth());
    shader_collection_.text_shader.uniform_handles.pane_height.setFloat(size.GetHeight());

    // text_renderer_->renderTextFromLeftCenter("A,a\"^jlghyk-lp1q2{3}4[5]6(78)9@0", -0.95, 0.0, 0.0003f, 500, 478);
    text_renderer_->renderTextTest('A', -0.95, -0.95, 0.0003f, 500, 478);*/

    text_renderer_->render(1.0f);

    /*float size = 0.0005f, y = -0.9f;

    for(size_t k = 0; k < 10; k++)
    {
        text_renderer_->renderTextFromLeftCenter("Hello", -0.8, y, size, 500, 478);
        size += 0.0005f;
        y += 0.1f;
    }*/

    SwapBuffers();
}
