#include "layout_tools_window.h"

#include "events.h"
#include "main_window.h"

LayoutToolsWindow::LayoutToolsWindow(wxPoint pos,
                                     wxSize size,
                                     std::function<void(const std::string&)>&& change_current_tab_name,
                                     std::function<void(const std::string&)>&& change_current_element_name,
                                     std::function<void()>&& add_new_window,
                                     std::function<void()>&& delete_window,
                                     std::function<void()>&& add_new_tab,
                                     std::function<void()>&& delete_tab,
                                     std::function<void()>&& add_new_element,
                                     std::function<void()>&& delete_element,
                                     std::function<void()>&& disable_editing)
    : wxFrame(nullptr, wxID_ANY, "Layout tools", pos, size, wxDEFAULT_FRAME_STYLE | wxSTAY_ON_TOP)
{
    this->SetMinSize(size);
    this->SetMaxSize(size);

    change_current_tab_name_ = std::move(change_current_tab_name);
    change_current_element_name_ = std::move(change_current_element_name);
    add_new_window_ = std::move(add_new_window);
    delete_window_ = std::move(delete_window);
    add_new_tab_ = std::move(add_new_tab);
    delete_tab_ = std::move(delete_tab);
    add_new_element_ = std::move(add_new_element);
    delete_element_ = std::move(delete_element);
    disable_editing_ = std::move(disable_editing);

    Bind(wxEVT_CLOSE_WINDOW, &LayoutToolsWindow::OnClose, this);

    shapes_box_ = new wxStaticBox(this, wxID_ANY, "Shapes");
    inspector_box_ = new wxStaticBox(this, wxID_ANY, "Inspector");

    wxBoxSizer* global_sizer = new wxBoxSizer(wxVERTICAL);
    global_sizer->Add(shapes_box_, 1, wxALIGN_CENTER_HORIZONTAL);
    global_sizer->Add(inspector_box_, 1, wxALIGN_CENTER_HORIZONTAL);

    setupInspector();
    setupShapes();

    this->SetSizer(global_sizer);
}

void LayoutToolsWindow::OnClose(wxCloseEvent& WXUNUSED(event))
{
    this->Hide();
    disable_editing_();
}

void LayoutToolsWindow::setupInspector()
{
    wxBoxSizer* global_sizer = new wxBoxSizer(wxVERTICAL);

    {
        wxBoxSizer* sizer_inside = new wxBoxSizer(wxVERTICAL);
        wxStaticText* tab_name_label =
            new wxStaticText(inspector_box_, wxID_ANY, "Tab name", wxDefaultPosition, wxDefaultSize);
        tab_name_ctrl_ =
            new wxTextCtrl(inspector_box_, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        tab_name_ctrl_->Bind(wxEVT_COMMAND_TEXT_UPDATED, &LayoutToolsWindow::changeCurrentTabName, this);

        sizer_inside->Add(tab_name_label, 1, wxALIGN_CENTER_HORIZONTAL);
        sizer_inside->Add(tab_name_ctrl_, 1, wxALIGN_CENTER_HORIZONTAL);
        global_sizer->Add(sizer_inside);
    }
    {
        wxBoxSizer* sizer_inside = new wxBoxSizer(wxVERTICAL);
        wxStaticText* element_name_label =
            new wxStaticText(inspector_box_, wxID_ANY, "Element name", wxDefaultPosition, wxDefaultSize);
        element_name_ctrl_ =
            new wxTextCtrl(inspector_box_, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_PROCESS_ENTER);
        element_name_ctrl_->Bind(wxEVT_COMMAND_TEXT_UPDATED, &LayoutToolsWindow::changeCurrentElementName, this);

        sizer_inside->Add(element_name_label, 1, wxALIGN_CENTER_HORIZONTAL);
        sizer_inside->Add(element_name_ctrl_, 1, wxALIGN_CENTER_HORIZONTAL);
        global_sizer->Add(sizer_inside);
    }
    inspector_box_->SetSizer(global_sizer);
}

void LayoutToolsWindow::setupShapes()
{
    wxBoxSizer* global_sizer = new wxBoxSizer(wxVERTICAL);

    {
        wxBoxSizer* sizer_inside = new wxBoxSizer(wxVERTICAL);
        wxButton* new_tab_button = new wxButton(shapes_box_, wxID_ANY, "New tab", wxDefaultPosition);
        wxButton* delete_tab_button = new wxButton(shapes_box_, wxID_ANY, "Delete tab", wxDefaultPosition);
        new_tab_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutToolsWindow::addNewTabCallback, this);
        delete_tab_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutToolsWindow::deleteTab, this);

        sizer_inside->Add(new_tab_button, 0, wxALIGN_CENTER_HORIZONTAL);
        sizer_inside->Add(delete_tab_button, 0, wxALIGN_CENTER_HORIZONTAL);
        global_sizer->Add(sizer_inside, 1, wxALIGN_CENTER_HORIZONTAL);
    }
    {
        wxBoxSizer* sizer_inside = new wxBoxSizer(wxVERTICAL);
        wxButton* new_window_button = new wxButton(shapes_box_, wxID_ANY, "New window", wxDefaultPosition);
        wxButton* delete_window_button = new wxButton(shapes_box_, wxID_ANY, "Delete window", wxDefaultPosition);
        new_window_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutToolsWindow::addNewWindowCallback, this);
        delete_window_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutToolsWindow::deleteWindow, this);

        sizer_inside->Add(new_window_button, 0, wxALIGN_CENTER_HORIZONTAL);
        sizer_inside->Add(delete_window_button, 0, wxALIGN_CENTER_HORIZONTAL);
        global_sizer->Add(sizer_inside, 1, wxALIGN_CENTER_HORIZONTAL);
    }
    {
        wxBoxSizer* sizer_inside = new wxBoxSizer(wxVERTICAL);
        wxButton* new_element_button = new wxButton(shapes_box_, wxID_ANY, "New element", wxDefaultPosition);
        wxButton* delete_element_button = new wxButton(shapes_box_, wxID_ANY, "Delete element", wxDefaultPosition);
        new_element_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutToolsWindow::newElement, this);
        delete_element_button->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &LayoutToolsWindow::deleteElement, this);

        sizer_inside->Add(new_element_button, 0, wxALIGN_CENTER_HORIZONTAL);
        sizer_inside->Add(delete_element_button, 0, wxALIGN_CENTER_HORIZONTAL);
        global_sizer->Add(sizer_inside, 1, wxALIGN_CENTER_HORIZONTAL);
    }
    shapes_box_->SetSizer(global_sizer);
}

void LayoutToolsWindow::addNewTabCallback(wxCommandEvent& WXUNUSED(event))
{
    add_new_tab_();
}

void LayoutToolsWindow::deleteTab(wxCommandEvent& WXUNUSED(event))
{
    delete_tab_();
}

void LayoutToolsWindow::addNewWindowCallback(wxCommandEvent& WXUNUSED(event))
{
    add_new_window_();
}

void LayoutToolsWindow::deleteWindow(wxCommandEvent& WXUNUSED(event))
{
    delete_window_();
}

void LayoutToolsWindow::newElement(wxCommandEvent& WXUNUSED(event))
{
    add_new_element_();
}

void LayoutToolsWindow::deleteElement(wxCommandEvent& WXUNUSED(event))
{
    delete_element_();
}

void LayoutToolsWindow::changeCurrentTabName(wxCommandEvent& event)
{
    const wxString value = event.GetString();
    change_current_tab_name_(std::string(value.mb_str()));
}

void LayoutToolsWindow::changeCurrentElementName(wxCommandEvent& event)
{
    const wxString value = event.GetString();
    change_current_element_name_(std::string(value.mb_str()));
}

void LayoutToolsWindow::setCurrentTabName(const std::string& tab_name)
{
    tab_name_ctrl_->SetEvtHandlerEnabled(false);
    tab_name_ctrl_->SetValue(tab_name);
    tab_name_ctrl_->SetEvtHandlerEnabled(true);
}

void LayoutToolsWindow::setActiveViewName(const std::string& name_of_selected)
{
    element_name_ctrl_->SetEvtHandlerEnabled(false);
    element_name_ctrl_->SetValue(name_of_selected);
    element_name_ctrl_->SetEvtHandlerEnabled(true);
}

void LayoutToolsWindow::setPosAndSize(wxPoint pos, wxSize size)
{
    this->SetPosition(pos);
    this->SetSize(size);
}
