#include "window_tab.h"

#include "lumos/logging/logging.h"
#include "project_state/project_settings.h"

WindowTab::WindowTab(
    QWidget* parent_widget,
    const TabSettings& tab_settings,
    const int element_x_offset,
    const std::function<void(const char key)>& notify_main_window_key_pressed,
    const std::function<void(const char key)>& notify_main_window_key_released,
    const std::function<void(const QPoint& pos, const std::string& elem_name)>&
        notify_parent_window_right_mouse_pressed,
    const std::function<void(const std::string&)>& notify_main_window_element_deleted,
    const std::function<void()>& notify_main_window_about_modification)
    : name_(tab_settings.name),
      parent_widget_(parent_widget),
      notify_main_window_key_pressed_(notify_main_window_key_pressed),
      notify_main_window_key_released_(notify_main_window_key_released),
      notify_parent_window_right_mouse_pressed_(notify_parent_window_right_mouse_pressed),
      notify_main_window_element_deleted_(notify_main_window_element_deleted),
      notify_main_window_about_modification_(notify_main_window_about_modification),
      current_element_idx_(0),
      background_color_(tab_settings.background_color),
      element_x_offset_(element_x_offset)
{
    // Create GUI elements from settings
    for (const auto& element_settings : tab_settings.elements)
    {
        if (element_settings->type == lumos::GuiElementType::PlotPane)
        {
            createNewPlotPane(element_settings);
        }
        // TODO: Add other element types (buttons, sliders, etc.) when implemented
    }

    LUMOS_LOG_INFO() << "WindowTab '" << name_ << "' created with "
                       << plot_panes_.size() << " plot panes";
}

WindowTab::~WindowTab()
{
    // Clean up all plot panes
    for (auto* plot_pane : plot_panes_)
    {
        delete plot_pane;
    }
    plot_panes_.clear();

    // Clean up other GUI elements
    for (auto* element : gui_elements_)
    {
        delete element;
    }
    gui_elements_.clear();

    LUMOS_LOG_INFO() << "WindowTab '" << name_ << "' destroyed";
}

std::vector<ApplicationGuiElement*> WindowTab::getPlotPanes() const
{
    std::vector<ApplicationGuiElement*> result;
    for (auto* pane : plot_panes_)
    {
        result.push_back(pane);
    }
    return result;
}

std::vector<ApplicationGuiElement*> WindowTab::getGuiElements() const
{
    return gui_elements_;
}

std::vector<ApplicationGuiElement*> WindowTab::getAllGuiElements() const
{
    std::vector<ApplicationGuiElement*> result;

    // Add plot panes (they now inherit from ApplicationGuiElement)
    for (auto* pane : plot_panes_)
    {
        result.push_back(pane);
    }

    for (auto* element : gui_elements_)
    {
        result.push_back(element);
    }

    return result;
}

void WindowTab::updateAllElements()
{
    // Update plot panes (they're QWidgets with update() method)
    for (auto* pane : plot_panes_)
    {
        pane->update();
    }

    // Update other GUI elements (when implemented, they'll also be QWidgets)
    // for (auto* element : gui_elements_)
    // {
    //     // Cast to QWidget and call update() when element types are added
    // }
}

void WindowTab::setMinXPos(const int min_x_pos)
{
    for (auto* element : getAllGuiElements())
    {
        element->setMinXPos(min_x_pos);
    }
}

void WindowTab::createNewPlotPane()
{
    std::string handle_string = "p" + std::to_string(current_element_idx_);
    current_element_idx_++;
    createNewPlotPane(handle_string);
}

void WindowTab::createNewPlotPane(const std::string& element_handle_string)
{
    auto element_settings = std::make_shared<PlotPaneSettings>();
    element_settings->handle_string = element_handle_string;
    element_settings->type = lumos::GuiElementType::PlotPane;
    element_settings->x = 10.0f;
    element_settings->y = 10.0f;
    element_settings->width = 600.0f;
    element_settings->height = 400.0f;

    createNewPlotPane(element_settings);
}

void WindowTab::createNewPlotPane(const std::shared_ptr<ElementSettings>& element_settings)
{
    PlotPane* plot_pane = new PlotPane(parent_widget_,
                                       element_settings,
                                       notify_main_window_key_pressed_,
                                       notify_main_window_key_released_,
                                       notify_main_window_about_modification_);

    plot_pane->setGeometry(
        static_cast<int>(element_settings->x) + element_x_offset_,
        static_cast<int>(element_settings->y),
        static_cast<int>(element_settings->width),
        static_cast<int>(element_settings->height)
    );

    plot_pane->show();
    plot_panes_.push_back(plot_pane);

    LUMOS_LOG_INFO() << "Created plot pane: " << element_settings->handle_string;
}

void WindowTab::show()
{
    // Show plot panes (they're QWidgets)
    for (auto* pane : plot_panes_)
    {
        pane->show();
    }

    // Show other GUI elements (when implemented, they'll also be QWidgets)
    // for (auto* element : gui_elements_)
    // {
    //     // Cast to QWidget when element types are added
    // }
}

void WindowTab::hide()
{
    // Hide plot panes (they're QWidgets)
    for (auto* pane : plot_panes_)
    {
        pane->hide();
    }

    // Hide other GUI elements (when implemented, they'll also be QWidgets)
    // for (auto* element : gui_elements_)
    // {
    //     // Cast to QWidget when element types are added
    // }
}

void WindowTab::updateSizeFromParent(const QSize new_size) const
{
    for (auto* element : getAllGuiElements())
    {
        element->updateSizeFromParent(new_size);
    }
}

RGBTripletf WindowTab::getBackgroundColor() const
{
    return background_color_;
}

std::string WindowTab::getName() const
{
    return name_;
}

TabSettings WindowTab::getTabSettings() const
{
    TabSettings settings;
    settings.name = name_;
    settings.background_color = background_color_;

    // Collect element settings
    for (auto* element : getAllGuiElements())
    {
        settings.elements.push_back(element->getElementSettings());
    }

    return settings;
}

ApplicationGuiElement* WindowTab::getGuiElement(const std::string& element_handle_string) const
{
    for (auto* element : getAllGuiElements())
    {
        if (element->getHandleString() == element_handle_string)
        {
            return element;
        }
    }
    return nullptr;
}

void WindowTab::notifyChildrenOnKeyPressed(const char key)
{
    // Key events are handled via Qt's event system, no need to manually propagate
}

void WindowTab::notifyChildrenOnKeyReleased(const char key)
{
    // Key events are handled via Qt's event system, no need to manually propagate
}

bool WindowTab::deleteElementIfItExists(const std::string& element_handle_string)
{
    // Check plot panes
    for (auto it = plot_panes_.begin(); it != plot_panes_.end(); ++it)
    {
        if ((*it)->getHandleString() == element_handle_string)
        {
            delete *it;
            plot_panes_.erase(it);
            notify_main_window_element_deleted_(element_handle_string);
            return true;
        }
    }

    // Check other GUI elements
    for (auto it = gui_elements_.begin(); it != gui_elements_.end(); ++it)
    {
        if ((*it)->getHandleString() == element_handle_string)
        {
            delete *it;
            gui_elements_.erase(it);
            notify_main_window_element_deleted_(element_handle_string);
            return true;
        }
    }

    return false;
}

bool WindowTab::elementWithNameExists(const std::string& element_handle_string)
{
    return getGuiElement(element_handle_string) != nullptr;
}

std::vector<std::string> WindowTab::getElementNames() const
{
    std::vector<std::string> names;

    for (auto* element : getAllGuiElements())
    {
        names.push_back(element->getHandleString());
    }

    return names;
}

void WindowTab::setName(const std::string& new_name)
{
    name_ = new_name;
}
