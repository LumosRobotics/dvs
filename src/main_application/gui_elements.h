#ifndef MAIN_APPLICATION_GUI_ELEMENTS_H_
#define MAIN_APPLICATION_GUI_ELEMENTS_H_

#include <wx/notebook.h>
#include <wx/textctrl.h>
#include <wx/wx.h>

#include <functional>
#include <memory>
#include <queue>

#include "communication/received_data.h"
#include "duoplot/enumerations.h"
#include "duoplot/math/math.h"
#include "gui_element.h"
#include "project_state/project_settings.h"

class ButtonGuiElement : public wxButton, public ApplicationGuiElement
{
private:
    bool is_pressed_;
    bool publish_to_local_;
    bool publish_to_serial_;

public:
    ButtonGuiElement(wxFrame* parent,
                     const std::shared_ptr<ElementSettings>& element_settings,
                     const std::function<void(const char key)>& notify_main_window_key_pressed,
                     const std::function<void(const char key)>& notify_main_window_key_released,
                     const std::function<void(const wxPoint pos, const std::string& elem_name)>&
                         notify_parent_window_right_mouse_pressed,
                     const std::function<void()>& notify_main_window_about_modification,
                     const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
                         notify_tab_about_editing,
                     const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
                     const wxPoint& pos,
                     const wxSize& size);

    void mouseLeftPressed(wxMouseEvent& event);
    void mouseLeftReleased(wxMouseEvent& event);
    void mouseMovedOverItem(wxMouseEvent& event);
    void buttonEvent(wxCommandEvent& event);

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    std::uint64_t getGuiPayloadSize() const override
    {
        return sizeof(std::int8_t);
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        output_array.fillWithStaticType(static_cast<std::uint8_t>(is_pressed_));
    }

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void keyPressedElementSpecific(const char key) override
    {
        static_cast<void>(key);
    }

    void keyReleasedElementSpecific(const char key) override
    {
        static_cast<void>(key);
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        this->SetSize(new_size);
    }

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    void mouseLeftPressedGuiElementSpecific(wxMouseEvent& event) override
    {
        is_pressed_ = true;
    }

    void mouseLeftReleasedGuiElementSpecific(wxMouseEvent& event) override
    {
        is_pressed_ = false;
    }

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<ButtonState> button_state =
            std::make_shared<ButtonState>(element_settings_->handle_string, is_pressed_);

        return button_state;
    }

    void setLabel(const std::string& new_label) override;

    void sendDataToSerialInterface() const;
};

class SliderGuiElement : public wxSlider, public ApplicationGuiElement
{
private:
    std::int32_t slider_value_;
    wxStaticText* value_text_;
    wxStaticText* min_text_;
    wxStaticText* max_text_;
    bool is_horizontal_;

    bool publish_to_local_;
    bool publish_to_serial_;

    int getStyle(const std::shared_ptr<ElementSettings>& element_settings) const;

public:
    SliderGuiElement(wxFrame* parent,
                     const std::shared_ptr<ElementSettings>& element_settings,
                     const std::function<void(const char key)>& notify_main_window_key_pressed,
                     const std::function<void(const char key)>& notify_main_window_key_released,
                     const std::function<void(const wxPoint pos, const std::string& elem_name)>&
                         notify_parent_window_right_mouse_pressed,
                     const std::function<void()>& notify_main_window_about_modification,
                     const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
                         notify_tab_about_editing,
                     const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
                     const wxPoint& pos,
                     const wxSize& size);

    void sliderEvent(wxCommandEvent& event);

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<SliderState> slider_state =
            std::make_shared<SliderState>(element_settings_->handle_string,
                                          std::dynamic_pointer_cast<SliderSettings>(element_settings_)->min_value,
                                          std::dynamic_pointer_cast<SliderSettings>(element_settings_)->max_value,
                                          std::dynamic_pointer_cast<SliderSettings>(element_settings_)->step_size,
                                          this->GetValue(),
                                          true);

        return slider_state;
    }

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void setElementPositionAndSize() override;

    std::uint64_t getGuiPayloadSize() const override
    {
        return 4U * sizeof(std::int32_t);
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        const std::int32_t step_size{std::dynamic_pointer_cast<SliderSettings>(element_settings_)->step_size};
        const std::int32_t min_value{std::dynamic_pointer_cast<SliderSettings>(element_settings_)->min_value};
        const std::int32_t max_value{std::dynamic_pointer_cast<SliderSettings>(element_settings_)->max_value};

        output_array.fillWithStaticType(min_value);
        output_array.fillWithStaticType(max_value);
        output_array.fillWithStaticType(step_size);
        output_array.fillWithStaticType(slider_value_);
    }

    void mouseMovedOverItem(wxMouseEvent& event);
    void mouseLeftReleased(wxMouseEvent& event);
    void mouseLeftPressed(wxMouseEvent& event);

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        wxSize final_size{new_size};
        final_size.SetWidth(std::max(min_x_size_, std::min(max_x_size_, final_size.GetWidth())));
        final_size.SetHeight(std::max(min_y_size_, std::min(max_y_size_, final_size.GetHeight())));
        this->SetSize(final_size);
    }

    wxSize getSizeInternal(const std::shared_ptr<ElementSettings>& element_settings, const wxFrame* const parent) const
    {
        const bool is_horizontal = std::dynamic_pointer_cast<SliderSettings>(element_settings)->is_horizontal;
        const wxSize parent_size = parent->GetSize();

        wxSize new_size;

        if (is_horizontal)
        {
            const float width = element_settings->width * parent_size.x;
            const float height = element_settings->height * parent_size.y;

            new_size.x = std::max(width, 70.0f);
            new_size.y = std::max(height, 30.0f);
        }
        else
        {
            const float width = element_settings->width * parent_size.x;
            const float height = element_settings->height * parent_size.y;

            new_size.x = std::max(height, 70.0f);
            new_size.y = std::max(width, 30.0f);
        }

        return new_size;
    }

    void hide() override
    {
        Hide();
        value_text_->Hide();
        min_text_->Hide();
        max_text_->Hide();
    }

    void show() override
    {
        Show();
        value_text_->Show();
        min_text_->Show();
        max_text_->Show();
    }

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }
};

class CheckboxGuiElement : public wxCheckBox, public ApplicationGuiElement
{
private:
public:
    CheckboxGuiElement(wxFrame* parent,
                       const std::shared_ptr<ElementSettings>& element_settings,
                       const std::function<void(const char key)>& notify_main_window_key_pressed,
                       const std::function<void(const char key)>& notify_main_window_key_released,
                       const std::function<void(const wxPoint pos, const std::string& elem_name)>&
                           notify_parent_window_right_mouse_pressed,
                       const std::function<void()>& notify_main_window_about_modification,
                       const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
                           notify_tab_about_editing,
                       const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
                       const wxPoint& pos,
                       const wxSize& size);

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    std::uint64_t getGuiPayloadSize() const override
    {
        return sizeof(std::int8_t);
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        const bool is_checked = this->IsChecked();
        output_array.fillWithStaticType(static_cast<std::uint8_t>(is_checked));
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        this->SetSize(new_size);
    }

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    void checkBoxCallback(wxCommandEvent& event);

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<CheckboxState> checkbox_state =
            std::make_shared<CheckboxState>(element_settings_->handle_string, this->GetValue());

        return checkbox_state;
    }
};

class TextLabelGuiElement : public wxStaticText, public ApplicationGuiElement
{
private:
    std::string label_;

public:
    TextLabelGuiElement(wxFrame* parent,
                        const std::shared_ptr<ElementSettings>& element_settings,
                        const std::function<void(const char key)>& notify_main_window_key_pressed,
                        const std::function<void(const char key)>& notify_main_window_key_released,
                        const std::function<void(const wxPoint pos, const std::string& elem_name)>&
                            notify_parent_window_right_mouse_pressed,
                        const std::function<void()>& notify_main_window_about_modification,
                        const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
                            notify_tab_about_editing,
                        const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
                        const wxPoint& pos,
                        const wxSize& size);

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    std::uint64_t getGuiPayloadSize() const override
    {
        return sizeof(std::int8_t) + label_.length();
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        output_array.fillWithStaticType(static_cast<std::uint8_t>(label_.length()));
        output_array.fillWithDataFromPointer(label_.data(), label_.length());
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        this->SetSize(new_size);
    }

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    void checkBoxCallback(wxCommandEvent& event);

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<TextLabelState> text_label_state =
            std::make_shared<TextLabelState>(element_settings_->handle_string, this->GetLabel().ToStdString());

        return text_label_state;
    }

    void setLabel(const std::string& new_label);
};

class ListBoxGuiElement : public wxListBox, public ApplicationGuiElement
{
private:
    std::vector<std::string> elements_;
    std::string selected_element_;

public:
    ListBoxGuiElement(wxFrame* parent,
                      const std::shared_ptr<ElementSettings>& element_settings,
                      const std::function<void(const char key)>& notify_main_window_key_pressed,
                      const std::function<void(const char key)>& notify_main_window_key_released,
                      const std::function<void(const wxPoint pos, const std::string& elem_name)>&
                          notify_parent_window_right_mouse_pressed,
                      const std::function<void()>& notify_main_window_about_modification,
                      const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
                          notify_tab_about_editing,
                      const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
                      const wxPoint& pos,
                      const wxSize& size);

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    std::uint64_t getGuiPayloadSize() const override
    {
        std::uint64_t total_num_bytes = sizeof(std::uint8_t) + selected_element_.length();

        total_num_bytes += sizeof(std::uint16_t);  // Number of elements

        for (const auto& element : elements_)
        {
            total_num_bytes += sizeof(std::uint8_t) + element.length();
        }
        return total_num_bytes;
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        output_array.fillWithStaticType(static_cast<std::uint8_t>(selected_element_.length()));
        output_array.fillWithDataFromPointer(selected_element_.data(), selected_element_.length());

        output_array.fillWithStaticType(static_cast<std::uint16_t>(elements_.size()));

        for (const auto& element : elements_)
        {
            output_array.fillWithStaticType(static_cast<std::uint8_t>(element.length()));
            output_array.fillWithDataFromPointer(element.data(), element.length());
        }
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        this->SetSize(new_size);
    }

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    void listBoxCallback(wxCommandEvent& event);

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<ListBoxState> list_box_state =
            std::make_shared<ListBoxState>(element_settings_->handle_string, elements_, selected_element_);

        return list_box_state;
    }
};

class EditableTextGuiElement : public wxTextCtrl, public ApplicationGuiElement
{
private:
    std::string text_;
    bool enter_pressed_;

public:
    EditableTextGuiElement(
        wxFrame* parent,
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void(const wxPoint pos, const std::string& elem_name)>&
            notify_parent_window_right_mouse_pressed,
        const std::function<void()>& notify_main_window_about_modification,
        const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
            notify_tab_about_editing,
        const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
        const wxPoint& pos,
        const wxSize& size);

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    std::uint64_t getGuiPayloadSize() const override
    {
        const std::uint64_t total_num_bytes = sizeof(std::uint8_t) + sizeof(std::uint8_t) + text_.length();

        return total_num_bytes;
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        output_array.fillWithStaticType(static_cast<std::uint8_t>(enter_pressed_));
        output_array.fillWithStaticType(static_cast<std::uint8_t>(text_.length()));
        output_array.fillWithDataFromPointer(text_.data(), text_.length());
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        this->SetSize(new_size);
    }

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    void editableTextCallback(wxCommandEvent& event);
    void editableTextEnterPressedCallback(wxCommandEvent& event);

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<EditableTextState> editable_text_state =
            std::make_shared<EditableTextState>(element_settings_->handle_string, enter_pressed_, text_);

        return editable_text_state;
    }
};

class DropdownMenuGuiElement : public wxComboBox, public ApplicationGuiElement
{
private:
    std::vector<std::string> elements_;
    std::string selected_element_;

public:
    DropdownMenuGuiElement(
        wxFrame* parent,
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void(const wxPoint pos, const std::string& elem_name)>&
            notify_parent_window_right_mouse_pressed,
        const std::function<void()>& notify_main_window_about_modification,
        const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
            notify_tab_about_editing,
        const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
        const wxPoint& pos,
        const wxSize& size);

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    std::uint64_t getGuiPayloadSize() const override
    {
        std::uint64_t total_num_bytes = sizeof(std::uint8_t) + selected_element_.length();

        total_num_bytes += sizeof(std::uint16_t);  // Number of elements

        for (const auto& element : elements_)
        {
            total_num_bytes += sizeof(std::uint8_t) + element.length();
        }
        return total_num_bytes;
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        output_array.fillWithStaticType(static_cast<std::uint8_t>(selected_element_.length()));
        output_array.fillWithDataFromPointer(selected_element_.data(), selected_element_.length());

        output_array.fillWithStaticType(static_cast<std::uint16_t>(elements_.size()));

        for (const auto& element : elements_)
        {
            output_array.fillWithStaticType(static_cast<std::uint8_t>(element.length()));
            output_array.fillWithDataFromPointer(element.data(), element.length());
        }
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        this->SetSize(new_size);
    }

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    void dropdownCallback(wxCommandEvent& event);

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<DropdownMenuState> drop_down_menu_state =
            std::make_shared<DropdownMenuState>(element_settings_->handle_string, elements_, selected_element_);

        return drop_down_menu_state;
    }
};

class RadioButtonGroupGuiElement : public wxRadioBox, public ApplicationGuiElement
{
private:
    std::vector<std::string> buttons_;
    std::int32_t selected_idx_;

public:
    RadioButtonGroupGuiElement(
        wxFrame* parent,
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void(const wxPoint pos, const std::string& elem_name)>&
            notify_parent_window_right_mouse_pressed,
        const std::function<void()>& notify_main_window_about_modification,
        const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
            notify_tab_about_editing,
        const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
        const wxPoint& pos,
        const wxSize& size);

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    std::uint64_t getGuiPayloadSize() const override
    {
        std::uint64_t total_num_bytes = 0U;

        total_num_bytes += sizeof(std::uint16_t);  // Number of buttons
        total_num_bytes += sizeof(std::uint32_t);  // Selected button

        for (const auto& button : buttons_)
        {
            total_num_bytes += sizeof(std::uint8_t) + button.length();
        }

        return total_num_bytes;
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        output_array.fillWithStaticType(selected_idx_);

        output_array.fillWithStaticType(static_cast<std::uint16_t>(buttons_.size()));

        for (const auto& button : buttons_)
        {
            output_array.fillWithStaticType(static_cast<std::uint8_t>(button.length()));
            output_array.fillWithDataFromPointer(button.data(), button.length());
        }
    }

    wxArrayString getRadioButtons(const std::shared_ptr<ElementSettings>& element_settings) const
    {
        const std::shared_ptr<RadioButtonGroupSettings> radio_button_group_settings =
            std::dynamic_pointer_cast<RadioButtonGroupSettings>(element_settings);
        wxArrayString radio_buttons_to_add{};

        for (const auto& radio_button_attribute : radio_button_group_settings->radio_buttons)
        {
            radio_buttons_to_add.Add(radio_button_attribute.label);
        }

        return radio_buttons_to_add;
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override
    {
        this->SetSize(new_size);
    }

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    void radioButtonGroupCallback(wxCommandEvent& event);

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        std::shared_ptr<RadioButtonGroupState> radio_button_group_state =
            std::make_shared<RadioButtonGroupState>(element_settings_->handle_string, buttons_, selected_idx_);

        return radio_button_group_state;
    }
};

class ScrollingTextGuiElement : public wxTextCtrl, public ApplicationGuiElement
{
private:
    std::map<TopicId, wxColor> topic_id_to_color_;
    wxColor black_color_;
    bool print_timestamp_;
    bool print_topic_id_;

public:
    ScrollingTextGuiElement(
        wxFrame* parent,
        const std::shared_ptr<ElementSettings>& element_settings,
        const std::function<void(const char key)>& notify_main_window_key_pressed,
        const std::function<void(const char key)>& notify_main_window_key_released,
        const std::function<void(const wxPoint pos, const std::string& elem_name)>&
            notify_parent_window_right_mouse_pressed,
        const std::function<void()>& notify_main_window_about_modification,
        const std::function<void(const wxPoint& pos, const wxSize& size, const bool is_editing)>&
            notify_tab_about_editing,
        const std::function<void(const Color_t, const std::string&)>& push_text_to_cmdl_output_window,
        const wxPoint& pos,
        const wxSize& size);

    void setMinXPos(const int min_x_pos) override
    {
        minimum_x_pos_ = min_x_pos;
        setElementPositionAndSize();
    }

    void updateElementSettings(const std::map<std::string, std::string>& new_settings) override;

    void keyPressedElementSpecific(const char key) override {}

    void keyReleasedElementSpecific(const char key) override {}

    std::uint64_t getGuiPayloadSize() const override
    {
        std::uint64_t total_num_bytes = 0U;

        /*total_num_bytes += sizeof(std::uint16_t);  // Number of buttons
        total_num_bytes += sizeof(std::uint32_t);  // Selected button

        for (const auto& button : buttons_)
        {
            total_num_bytes += sizeof(std::uint8_t) + button.length();
        }*/

        return total_num_bytes;
    }

    void fillGuiPayload(FillableUInt8Array& output_array) const override
    {
        /*output_array.fillWithStaticType(selected_idx_);

        output_array.fillWithStaticType(static_cast<std::uint16_t>(buttons_.size()));

        for (const auto& button : buttons_)
        {
            output_array.fillWithStaticType(static_cast<std::uint8_t>(button.length()));
            output_array.fillWithDataFromPointer(button.data(), button.length());
        }*/
    }

    void updateSizeFromParent(const wxSize& parent_size) override
    {
        setElementPositionAndSize();
    }

    wxPoint getPosition() const override
    {
        return this->GetPosition();
    }

    wxSize getSize() const override
    {
        return this->GetSize();
    }

    wxWindow* getParent() const override
    {
        return this->GetParent();
    }

    void setPosition(const wxPoint& new_pos) override
    {
        this->SetPosition(new_pos);
    }

    void setSize(const wxSize& new_size) override;

    void hide() override
    {
        Hide();
    }

    void show() override
    {
        Show();
    }

    void pushNewText(const TopicId topic_id, const std::vector<std::pair<uint64_t, std::string>>& strings);

    std::shared_ptr<GuiElementState> getGuiElementState() const override
    {
        return nullptr;
    }
};

#endif  // MAIN_APPLICATION_GUI_ELEMENT_H_
