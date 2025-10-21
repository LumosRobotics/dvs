#include "tab_view.h"

#include "events.h"
#include "globals.h"
#include "plot_pane.h"

TabView::TabView(wxNotebookPage* parent,
                 const TabSettings& tab_settings,
                 const std::function<void(const char key)>& notify_main_window_key_pressed,
                 const std::function<void(const char key)>& notify_main_window_key_released)
    : ViewBase<wxNotebookPage>(parent, tab_settings, notify_main_window_key_pressed, notify_main_window_key_released)
{
    const std::vector<ElementSettings> elements = settings_->getElementSettingsList();

    for (const auto& elem : elements)
    {
        GuiElement* const ge = new PlotPane(dynamic_cast<wxWindow*>(this),
                                            elem,
                                            grid_size_,
                                            notify_main_window_key_pressed_,
                                            notify_main_window_key_released_);

        ge->updateSizeFromParent(this->GetSize());
        gui_elements_[elem.name] = ge;
    }

    if (gui_elements_.size() > 0)
    {
        gui_elements_.begin()->second->setIsSelected();
    }

    Bind(wxEVT_KEY_UP, &TabView::OnKeyDown, this);
    Bind(wxEVT_KEY_DOWN, &TabView::OnKeyUp, this);

    this->Show();
}

void TabView::newElement(const std::string& element_handle_string)
{
    ElementSettings elem;
    elem.x = 0;
    elem.y = 0;
    elem.width = 0.3;
    elem.height = 0.3;
    elem.name = element_handle_string;

    GuiElement* const ge = new PlotPane(dynamic_cast<wxNotebookPage*>(this),
                                        elem,
                                        grid_size_,
                                        notify_main_window_key_pressed_,
                                        notify_main_window_key_released_);

    ge->updateSizeFromParent(this->GetSize());
    if (is_editing_)
    {
        ge->setIsEditing(true);
    }
    gui_elements_[elem.name] = ge;
}

void TabView::newElement()
{
    ElementSettings elem;
    elem.x = 0;
    elem.y = 0;
    elem.width = 0.3;
    elem.height = 0.3;
    elem.name = "new-element-" + std::to_string(current_unused_element_idx++);

    GuiElement* const ge = new PlotPane(dynamic_cast<wxNotebookPage*>(this),
                                        elem,
                                        grid_size_,
                                        notify_main_window_key_pressed_,
                                        notify_main_window_key_released_);

    ge->updateSizeFromParent(this->GetSize());
    if (is_editing_)
    {
        ge->setIsEditing(true);
    }
    gui_elements_[elem.name] = ge;
}
