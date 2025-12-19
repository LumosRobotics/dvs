#ifndef GUI_PANE_H
#define GUI_PANE_H

#include <wx/glcanvas.h>
#include <wx/notebook.h>
#include <wx/wx.h>

#include <atomic>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <OpenGL/gl3.h>

#include "shader.h"
#include "text_rendering.h"
#include "vertex_buffer.h"

using namespace lumos;

class GuiPane : public wxGLCanvas
{
private:
    wxGLContext* getContext();
    wxGLContext* m_context;

    ShaderCollection shader_collection_;

    wxGLAttributes getGLAttributes() const;
    TextRenderer* text_renderer_;

    void initShaders();

    wxTimer receive_timer_;

    void TimerFunc(wxTimerEvent&);

public:
    GuiPane(wxFrame* parent);
    ~GuiPane();

    void render(wxPaintEvent& evt);
    void refresh();

};

#endif  // MAIN_APPLICATION_PLOT_PANE_H_
