#ifndef MAIN_APPLICATION_TAB_VIEW_H_
#define MAIN_APPLICATION_TAB_VIEW_H_

#include <wx/button.h>
#include <wx/frame.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/wx.h>

#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

#include "gui_element.h"
#include "project_state/project_settings.h"
#include "view_base.h"

class TabView : public ViewBase<wxNotebookPage>
{
private:
    void OnKeyDown(wxKeyEvent& event)
    {
        const int key = event.GetKeyCode();
        notify_main_window_key_pressed_(key);
    }
    void OnKeyUp(wxKeyEvent& event)
    {
        const int key = event.GetKeyCode();
        notify_main_window_key_released_(key);
    }

public:
    TabView() = delete;
    TabView(wxNotebookPage* parent,
            const TabSettings& tab_settings,
            const std::function<void(const char key)>& notify_main_window_key_pressed,
            const std::function<void(const char key)>& notify_main_window_key_released);

    void newElement(const std::string& element_handle_string);
    void newElement() override;
};

#endif  // MAIN_APPLICATION_TAB_VIEW_H_
