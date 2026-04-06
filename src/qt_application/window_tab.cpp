#include "window_tab.h"

#include "gui_elements.h"
#include "lumos/logging/logging.h"
#include "project_state/project_settings.h"

// ---------------------------------------------------------------------------
// Helper: compute initial pixel rect from fractional element settings
// ---------------------------------------------------------------------------
static QRect getPosAndSizeInPixels(QWidget* parent, const ElementSettings* s)
{
    return QRect(
        static_cast<int>(s->x * parent->width()),
        static_cast<int>(s->y * parent->height()),
        static_cast<int>(s->width * parent->width()),
        static_cast<int>(s->height * parent->height()));
}

WindowTab::WindowTab(
    QWidget* parent_widget,
    const TabSettings& tab_settings,
    const int element_x_offset,
    const std::function<void(const char key)>& notify_main_window_key_pressed,
    const std::function<void(const char key)>& notify_main_window_key_released,
    const std::function<void(const QPoint& pos, const std::string& elem_name)>&
        notify_parent_window_right_mouse_pressed,
    const std::function<void(const std::string&)>& notify_main_window_element_deleted,
    const std::function<void()>& notify_main_window_about_modification,
    const std::function<void(const std::string&)>& on_text_output)
    : name_(tab_settings.name),
      parent_widget_(parent_widget),
      notify_main_window_key_pressed_(notify_main_window_key_pressed),
      notify_main_window_key_released_(notify_main_window_key_released),
      notify_parent_window_right_mouse_pressed_(notify_parent_window_right_mouse_pressed),
      notify_main_window_element_deleted_(notify_main_window_element_deleted),
      notify_main_window_about_modification_(notify_main_window_about_modification),
      on_text_output_(on_text_output),
      current_element_idx_(0),
      background_color_(tab_settings.background_color),
      element_x_offset_(element_x_offset)
{
    // Create GUI elements from settings
    for (const auto& element_settings : tab_settings.elements)
    {
        using ET = lumos::GuiElementType;
        switch (element_settings->type)
        {
            case ET::PlotPane:
                createNewPlotPane(element_settings);
                break;
            case ET::Button:
                if (auto s = std::dynamic_pointer_cast<ButtonSettings>(element_settings))
                    createNewButton(s);
                break;
            case ET::Slider:
                if (auto s = std::dynamic_pointer_cast<SliderSettings>(element_settings))
                    createNewSlider(s);
                break;
            case ET::Checkbox:
                if (auto s = std::dynamic_pointer_cast<CheckboxSettings>(element_settings))
                    createNewCheckbox(s);
                break;
            case ET::TextLabel:
                if (auto s = std::dynamic_pointer_cast<TextLabelSettings>(element_settings))
                    createNewTextLabel(s);
                break;
            case ET::EditableText:
                if (auto s = std::dynamic_pointer_cast<EditableTextSettings>(element_settings))
                    createNewEditableText(s);
                break;
            case ET::DropdownMenu:
                if (auto s = std::dynamic_pointer_cast<DropdownMenuSettings>(element_settings))
                    createNewDropdownMenu(s);
                break;
            case ET::RadioButtonGroup:
                if (auto s = std::dynamic_pointer_cast<RadioButtonGroupSettings>(element_settings))
                    createNewRadioButtonGroup(s);
                break;
            case ET::ListBox:
                if (auto s = std::dynamic_pointer_cast<ListBoxSettings>(element_settings))
                    createNewListBox(s);
                break;
            case ET::ScrollingText:
                if (auto s = std::dynamic_pointer_cast<ScrollingTextSettings>(element_settings))
                    createNewScrollingText(s);
                break;
            default:
                LUMOS_LOG_WARNING() << "Unknown element type encountered in WindowTab constructor";
                break;
        }
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
    auto notify_tab_about_editing = [](const QPoint&, const QSize&, bool) {};  // Phase 5 stub

    PlotPane* plot_pane = new PlotPane(parent_widget_,
                                       element_settings,
                                       background_color_,
                                       notify_main_window_key_pressed_,
                                       notify_main_window_key_released_,
                                       notify_main_window_about_modification_,
                                       notify_parent_window_right_mouse_pressed_,
                                       notify_tab_about_editing,
                                       on_text_output_);

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
    for (auto* element : getAllGuiElements())
    {
        if (auto* w = dynamic_cast<QWidget*>(element))
            w->show();
    }
}

void WindowTab::hide()
{
    for (auto* element : getAllGuiElements())
    {
        if (auto* w = dynamic_cast<QWidget*>(element))
            w->hide();
    }
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
    for (auto* pane : plot_panes_)
        pane->keyPressedElementSpecific(key);
    for (auto* elem : gui_elements_)
        elem->keyPressedElementSpecific(key);
}

void WindowTab::notifyChildrenOnKeyReleased(const char key)
{
    for (auto* pane : plot_panes_)
        pane->keyReleasedElementSpecific(key);
    for (auto* elem : gui_elements_)
        elem->keyReleasedElementSpecific(key);
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

// ---------------------------------------------------------------------------
// createNew* implementations for non-PlotPane elements
// ---------------------------------------------------------------------------

template <typename T>
static ApplicationGuiElement* createGuiElement(
    QWidget* parent_widget,
    const std::shared_ptr<typename T::SettingsType>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
{
    return new T(parent_widget, settings, key_pressed, key_released, on_modification, right_click, on_text_output);
}

ApplicationGuiElement* WindowTab::createNewButton(const std::shared_ptr<ButtonSettings>& settings)
{
    ButtonGuiElement* elem = new ButtonGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewSlider(const std::shared_ptr<SliderSettings>& settings)
{
    SliderGuiElement* elem = new SliderGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewCheckbox(const std::shared_ptr<CheckboxSettings>& settings)
{
    CheckboxGuiElement* elem = new CheckboxGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewTextLabel(const std::shared_ptr<TextLabelSettings>& settings)
{
    TextLabelGuiElement* elem = new TextLabelGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewEditableText(const std::shared_ptr<EditableTextSettings>& settings)
{
    EditableTextGuiElement* elem = new EditableTextGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewDropdownMenu(const std::shared_ptr<DropdownMenuSettings>& settings)
{
    DropdownMenuGuiElement* elem = new DropdownMenuGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewRadioButtonGroup(const std::shared_ptr<RadioButtonGroupSettings>& settings)
{
    RadioButtonGroupGuiElement* elem = new RadioButtonGroupGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewListBox(const std::shared_ptr<ListBoxSettings>& settings)
{
    ListBoxGuiElement* elem = new ListBoxGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

ApplicationGuiElement* WindowTab::createNewScrollingText(const std::shared_ptr<ScrollingTextSettings>& settings)
{
    ScrollingTextGuiElement* elem = new ScrollingTextGuiElement(
        parent_widget_, settings,
        notify_main_window_key_pressed_, notify_main_window_key_released_,
        notify_main_window_about_modification_,
        notify_parent_window_right_mouse_pressed_,
        on_text_output_);
    elem->setGeometry(getPosAndSizeInPixels(parent_widget_, settings.get()));
    elem->show();
    gui_elements_.push_back(elem);
    return elem;
}

void WindowTab::raiseElement(const std::string& handle_string)
{
    for (auto* elem : getAllGuiElements())
    {
        if (elem->getHandleString() == handle_string)
        {
            if (auto* w = dynamic_cast<QWidget*>(elem))
                w->raise();
        }
    }
}

void WindowTab::lowerElement(const std::string& handle_string)
{
    for (auto* elem : getAllGuiElements())
    {
        if (elem->getHandleString() == handle_string)
        {
            if (auto* w = dynamic_cast<QWidget*>(elem))
                w->lower();
        }
    }
}
