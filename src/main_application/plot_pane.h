#ifndef MAIN_APPLICATION_PLOT_PANE_H_
#define MAIN_APPLICATION_PLOT_PANE_H_

#include <wx/glcanvas.h>
#include <wx/notebook.h>
#include <wx/wx.h>

#include <atomic>

#include "axes/axes.h"
#include "communication/data_receiver.h"
#include "communication/received_data.h"
#include "duoplot/internal.h"
#include "duoplot/math/math.h"
#include "gui_element.h"
#include "input_data.h"
#include "opengl_low_level/opengl_header.h"
#include "plot_data_handler.h"
#include "plot_objects/stream_object_base/stream_object_base.h"
#include "point_selection.h"
#include "serial_interface/definitions.h"
#include "serial_interface/object_types.h"

class PlotPane : public wxGLCanvas, public ApplicationGuiElement
{
private:
    wxGLContext* getContext();
    wxGLContext* m_context;

    AxesSettings axes_settings_;
    AxesInteractor axes_interactor_;
    AxesRenderer* axes_renderer_;

    bool axes_from_min_max_disabled_;
    bool axes_set_;
    bool view_set_;

    bool perspective_projection_;
    bool wait_for_flush_;

    PlotDataHandler* plot_data_handler_;
    PointSelection point_selection_;
    ShaderCollection shader_collection_;

    bool is3DFunction(const Function fcn);
    bool isImageFunction(const Function fcn);
    void initShaders();
    void clearPane();
    MouseInteractionAxis current_mouse_interaction_axis_;
    float legend_scale_factor_ = 1.0f;
    std::atomic<bool> pending_clear_;
    std::queue<std::unique_ptr<InputData>> queued_data_;
    std::queue<std::unique_ptr<InputData>> flush_queue_;

    duoplot::Vec3<double> closest_point_;
    bool should_render_point_selection_;

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void processActionQueue();
    void addPlotData(ReceivedData& received_data,
                     const PlotObjectAttributes& plot_object_attributes,
                     const UserSuppliedProperties& user_supplied_properties,
                     const std::shared_ptr<const ConvertedDataBase>& converted_data);

    wxGLAttributes getGLAttributes() const;
    std::shared_ptr<PlotPaneSettings> plot_pane_settings_;

    std::uint64_t getGuiPayloadSize() const override
    {
        return 0U;
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override {}
    void initSubscribedStreams();

    std::map<TopicId, StreamObjectBase*> subscribed_streams_;
    std::map<TopicId, std::vector<std::shared_ptr<objects::BaseObject>>> new_objects_;

public:
    PlotPane(wxNotebookPage* parent,  // TODO: wxNotebookPage is obsolete, should be wxWindow/wxFrame?
             const std::shared_ptr<ElementSettings>& element_settings,
             const RGBTripletf& tab_background_color,
             const std::function<void(const char key)>& notify_main_window_key_pressed,
             const std::function<void(const char key)>& notify_main_window_key_released,
             const std::function<void(const wxPoint pos, const std::string& elem_name)>&
                 notify_parent_window_right_mouse_pressed,
             const std::function<void()>& notify_main_window_about_modification,
             const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
                 notify_tab_about_editing,
             const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window);
    ~PlotPane() override;

    int getWidth();
    int getHeight();

    void setHandleString(const std::string& new_name) override;
    void pushQueue(std::queue<std::unique_ptr<InputData>>& new_queue);
    void pushStreamData(const TopicId topic_id, const std::vector<std::shared_ptr<objects::BaseObject>>& obj);

    void render(wxPaintEvent& evt);

    void setMinXPos(const int min_x_pos) override;
    void setSize(const wxSize& new_size) override;
    void updateSizeFromParent(const wxSize& parent_size) override;
    void addSettingsData(const ReceivedData& received_data,
                         const PlotObjectAttributes& plot_object_attributes,
                         const UserSuppliedProperties& user_supplied_properties);
    // void show() override;
    // void hide() override;
    // void destroy() override;
    void refresh();
    void setMouseInteractionType(const MouseInteractionType mit);
    void keyPressedElementSpecific(const char key) override;
    void keyReleasedElementSpecific(const char key) override;
    void waitForFlush();
    void toggleProjectionMode();
    void update();
    void mouseRightPressedGuiElementSpecific(wxMouseEvent& event) override;
    void mouseRightReleasedGuiElementSpecific(wxMouseEvent& event);
    void mouseMiddlePressed(wxMouseEvent& event);
    void mouseMiddleReleased(wxMouseEvent& event);
    void setMinimumXPos(const int new_min_x_pos);

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    // Event callback function
    void mouseMovedGuiElementSpecific(wxMouseEvent& event) override;
    void mouseLeftPressedGuiElementSpecific(wxMouseEvent& event) override;
    void mouseLeftReleasedGuiElementSpecific(wxMouseEvent& event) override;

    void bindCallbacks();

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    /*virtual void showLegend(const bool show_legend)
    {
        static_cast<void>(show_legend);
        std::cout << "Not implemented!" << std::endl;
    }*/
};

#endif  // MAIN_APPLICATION_PLOT_PANE_H_
