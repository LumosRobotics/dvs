#ifndef MAIN_APPLICATION_LAYOUT_TOOLS_WINDOW_H_
#define MAIN_APPLICATION_LAYOUT_TOOLS_WINDOW_H_

#include <unistd.h>
#include <wx/button.h>
#include <wx/frame.h>
#include <wx/grid.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/wx.h>

#include <csignal>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <tuple>
#include <utility>
#include <vector>

class LayoutToolsWindow : public wxFrame
{
private:
    wxStaticBox* shapes_box_;
    wxStaticBox* inspector_box_;
    wxTextCtrl* tab_name_ctrl_;
    wxTextCtrl* element_name_ctrl_;

    std::function<void(const std::string&)> change_current_tab_name_;
    std::function<void(const std::string&)> change_current_element_name_;
    std::function<void()> add_new_window_;
    std::function<void()> delete_window_;
    std::function<void()> add_new_tab_;
    std::function<void()> delete_tab_;
    std::function<void()> add_new_element_;
    std::function<void()> delete_element_;
    std::function<void()> disable_editing_;

public:
    LayoutToolsWindow() = default;
    LayoutToolsWindow(wxPoint pos,
                      wxSize size,
                      std::function<void(const std::string&)>&& change_current_tab_name,
                      std::function<void(const std::string&)>&& change_current_element_name,
                      std::function<void()>&& add_new_window,
                      std::function<void()>&& delete_window,
                      std::function<void()>&& add_new_tab,
                      std::function<void()>&& delete_tab,
                      std::function<void()>&& add_new_element,
                      std::function<void()>&& delete_element,
                      std::function<void()>&& disable_editing);

    void addNewTabCallback(wxCommandEvent& event);
    void deleteTab(wxCommandEvent& event);
    void addNewWindowCallback(wxCommandEvent& event);
    void deleteWindow(wxCommandEvent& event);
    void newElement(wxCommandEvent& event);
    void deleteElement(wxCommandEvent& event);
    void changeCurrentTabName(wxCommandEvent& event);
    void changeCurrentElementName(wxCommandEvent& event);

    void setPosAndSize(wxPoint pos, wxSize size);
    void setupInspector();
    void setupShapes();

    void setCurrentTabName(const std::string& tab_name);
    void setActiveViewName(const std::string& name_of_selected);

    virtual void OnClose(wxCloseEvent& event);
};

#endif  // MAIN_APPLICATION_LAYOUT_TOOLS_WINDOW_H_
