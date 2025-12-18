#ifndef BACKGROUND_RENDERER_H
#define BACKGROUND_RENDERER_H

#include <wx/glcanvas.h>
#include <wx/notebook.h>
#include <wx/wx.h>

#include "axes/axes.h"
#include "communication/data_receiver.h"
#include "communication/received_data.h"
#include "duoplot/internal.h"
#include "duoplot/math/math.h"
#include "gui_element.h"
#include "input_data.h"
#include "opengl_low_level/opengl_header.h"
#include "plot_data_handler.h"

class BackgroundRenderer : public wxGLCanvas
{
private:
    wxGLContext* getContext();
    wxGLContext* context_;

    void initShaders();
    ShaderCollection shader_collection_;
    wxGLAttributes getGLAttributes() const;
    RGBTripletf background_color_;
    GLuint vertex_buffer_, vertex_buffer_array_;

    std::vector<GLint> dimension_handles_;
    std::vector<GLint> position_handles_;

    float* data_array_;

public:
    BackgroundRenderer(wxFrame* parent, const wxSize panel_size, const RGBTripletf& background_color);
    ~BackgroundRenderer();

    void setParentSize();

    void render(wxPaintEvent& evt);
    void updateSize();
    void setTabContent(const TabSettings& tab_settings);
};

#endif  // BACKGROUND_RENDERER_H
