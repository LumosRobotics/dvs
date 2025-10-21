#ifndef MAIN_APPLICATION_VIEW_BASE_H_
#define MAIN_APPLICATION_VIEW_BASE_H_

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

#include "events.h"
#include "gui_element.h"
#include "project_state/project_settings.h"

class SuperBase
{
public:
    SuperBase() = default;
    virtual void resetSelectionForAllChildren() = 0;
};

template <class BaseClass> class ViewBase : public BaseClass, public SuperBase
{
protected:
    std::string name_;
    std::map<std::string, GuiElement*> gui_elements_;
    float grid_size_;
    int current_unnamed_idx_;
    bool is_editing_;
    std::string name_of_selected_element_;
    SettingsBase* settings_;
    std::function<void(const char key)> notify_main_window_key_pressed_;
    std::function<void(const char key)> notify_main_window_key_released_;

public:
    ViewBase() = delete;
    ViewBase(wxFrame* parent,
             const WindowSettings& window_settings,
             const std::function<void(const char key)>& notify_main_window_key_pressed,
             const std::function<void(const char key)>& notify_main_window_key_released);
    ViewBase(wxNotebookPage* parent,
             const TabSettings& tab_settings,
             const std::function<void(const char key)>& notify_main_window_key_pressed,
             const std::function<void(const char key)>& notify_main_window_key_released);

    std::map<std::string, GuiElement*> getGuiElements() const;
    std::string getName() const;
    virtual void setName(const std::string& new_name);
    std::vector<ElementSettings> getElementSettingsList() const;
    void deleteSelectedElement();
    void resetSelectionForAllChildren() override;
    void setSelectedElementName(const std::string& new_name);
    void mouseLeftPressed(wxMouseEvent& event);
    void setFirstElementSelected();
    std::string getSelectedElementName();
    void childModified(wxCommandEvent& event);
    void startEdit();
    void stopEdit();
    void setSize(const wxSize& new_size);
    TabSettings getTabSettings() const;
    WindowSettings getWindowSettings() const;

    virtual void newElement() = 0;
};

template <typename T>
ViewBase<T>::ViewBase(wxFrame* parent,
                      const WindowSettings& window_settings,
                      const std::function<void(const char key)>& notify_main_window_key_pressed,
                      const std::function<void(const char key)>& notify_main_window_key_released)
    : wxFrame(parent, wxID_ANY, "Figure 1"),
      notify_main_window_key_pressed_{notify_main_window_key_pressed},
      notify_main_window_key_released_{notify_main_window_key_released}
{
    grid_size_ = 1.0f;
    name_ = window_settings.getName();

    WindowSettings* st = new WindowSettings(window_settings);
    settings_ = static_cast<SettingsBase*>(st);

    this->setSize(wxSize(st->width, st->height));
    this->SetPosition(wxPoint(st->x, st->y));

    current_unnamed_idx_ = 0;
    is_editing_ = false;

    this->Bind(wxEVT_LEFT_DOWN, &ViewBase<T>::mouseLeftPressed, this);
    this->Bind(GUI_ELEMENT_CHANGED_EVENT, &ViewBase<T>::childModified, this, wxID_ANY);
}

template <typename T>
ViewBase<T>::ViewBase(wxNotebookPage* parent,
                      const TabSettings& tab_settings,
                      const std::function<void(const char key)>& notify_main_window_key_pressed,
                      const std::function<void(const char key)>& notify_main_window_key_released)
    : wxNotebookPage(parent, -1),
      notify_main_window_key_pressed_{notify_main_window_key_pressed},
      notify_main_window_key_released_{notify_main_window_key_released}
{
    grid_size_ = 1.0f;
    name_ = tab_settings.getName();

    TabSettings* st = new TabSettings(tab_settings);
    settings_ = static_cast<SettingsBase*>(st);

    current_unnamed_idx_ = 0;
    is_editing_ = false;

    this->Bind(wxEVT_LEFT_DOWN, &ViewBase<T>::mouseLeftPressed, this);
    this->Bind(GUI_ELEMENT_CHANGED_EVENT, &ViewBase<T>::childModified, this, wxID_ANY);
}

template <class BaseClass> std::string ViewBase<BaseClass>::getName() const
{
    return name_;
}

template <class BaseClass> void ViewBase<BaseClass>::setName(const std::string& new_name)
{
    name_ = new_name;
}

template <class BaseClass> std::map<std::string, GuiElement*> ViewBase<BaseClass>::getGuiElements() const
{
    return gui_elements_;
}

template <class BaseClass> TabSettings ViewBase<BaseClass>::getTabSettings() const
{
    TabSettings ts;
    ts.setName(name_);

    for (auto it : gui_elements_)
    {
        ts.pushBackElementSettings(it.second->getElementSettings());
    }

    return ts;
}

template <class BaseClass> WindowSettings ViewBase<BaseClass>::getWindowSettings() const
{
    WindowSettings* wsp = static_cast<WindowSettings*>(settings_);
    WindowSettings ws;
    ws.setName(name_);
    ws.x = wsp->x;
    ws.y = wsp->y;
    ws.width = wsp->width;
    ws.height = wsp->height;

    for (auto it : gui_elements_)
    {
        ws.pushBackElementSettings(it.second->getElementSettings());
    }

    return ws;
}

template <class BaseClass> std::vector<ElementSettings> ViewBase<BaseClass>::getElementSettingsList() const
{
    std::vector<ElementSettings> elements;

    for (auto it : gui_elements_)
    {
        elements.push_back(it.second->getElementSettings());
    }

    return elements;
}

template <class BaseClass> void ViewBase<BaseClass>::deleteSelectedElement()
{
    std::string key_to_delete = "";
    for (auto it : gui_elements_)
    {
        if (it.second->isSelected())
        {
            key_to_delete = it.first;
        }
    }

    if (key_to_delete != "")
    {
        gui_elements_[key_to_delete]->destroy();
        gui_elements_.erase(key_to_delete);
    }
}

template <class BaseClass> void ViewBase<BaseClass>::resetSelectionForAllChildren()
{
    for (auto it : gui_elements_)
    {
        it.second->resetSelection();
        it.second->refresh();
    }
}

template <class BaseClass> void ViewBase<BaseClass>::setSelectedElementName(const std::string& new_name)
{
    name_of_selected_element_ = new_name;
    for (auto it : gui_elements_)
    {
        if (it.second->isSelected())
        {
            it.second->setName(new_name);
        }
    }
}

template <class BaseClass> void ViewBase<BaseClass>::mouseLeftPressed(wxMouseEvent& WXUNUSED(event))
{
    // Since this event will only be caught if the mouse is pressed outside of an
    // element, we know we can deselect all elements.
    resetSelectionForAllChildren();
    name_of_selected_element_ = "";
    wxCommandEvent parent_event(NO_ELEMENT_SELECTED);
    wxPostEvent(this->GetParent(), parent_event);
}

template <class BaseClass> void ViewBase<BaseClass>::setFirstElementSelected()
{
    if (gui_elements_.size() > 0)
    {
        gui_elements_.begin()->second->setSelection();
        gui_elements_.begin()->second->refresh();
    }
}

template <class BaseClass> std::string ViewBase<BaseClass>::getSelectedElementName()
{
    for (auto it : gui_elements_)
    {
        if (it.second->isSelected())
        {
            name_of_selected_element_ = it.second->getName();
        }
    }
    return name_of_selected_element_;
}

template <class BaseClass> void ViewBase<BaseClass>::childModified(wxCommandEvent& WXUNUSED(event))
{
    wxCommandEvent parent_event(GUI_ELEMENT_CHANGED_EVENT);
    wxPostEvent(this->GetParent(), parent_event);
}

template <class BaseClass> void ViewBase<BaseClass>::startEdit()
{
    is_editing_ = true;
    for (auto it : gui_elements_)
    {
        it.second->setIsEditing(true);
    }
}

template <class BaseClass> void ViewBase<BaseClass>::stopEdit()
{
    is_editing_ = false;
    for (auto it : gui_elements_)
    {
        it.second->setIsEditing(false);
    }
}

template <class BaseClass> void ViewBase<BaseClass>::setSize(const wxSize& new_size)
{
    this->SetSize(new_size);
    for (auto it : gui_elements_)
    {
        it.second->updateSizeFromParent(new_size);
    }
}

#endif  // MAIN_APPLICATION_VIEW_BASE_H_
