#ifndef QT_APPLICATION_GUI_ELEMENTS_H_
#define QT_APPLICATION_GUI_ELEMENTS_H_

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QVBoxLayout>
#include <QWidget>
#include <QContextMenuEvent>

#include <functional>
#include <memory>
#include <string>

#include "gui_element.h"
#include "project_state/project_settings.h"

// ---------------------------------------------------------------------------
// ButtonGuiElement
// ---------------------------------------------------------------------------
class ButtonGuiElement : public QPushButton, public ApplicationGuiElement
{
    Q_OBJECT
public:
    ButtonGuiElement(QWidget* parent,
                     const std::shared_ptr<ElementSettings>& settings,
                     const std::function<void(const char)>& key_pressed,
                     const std::function<void(const char)>& key_released,
                     const std::function<void()>& on_modification,
                     const std::function<void(const QPoint&, const std::string&)>& right_click,
                     const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    void setLabel(const std::string& new_label) override { setText(QString::fromStdString(new_label)); }
    QRect getBoundingRect() const override { return geometry(); }

    std::uint64_t getGuiPayloadSize() const override { return sizeof(std::uint8_t); }
    void fillGuiPayload(FillableUInt8Array& out) const override
    {
        out.fillWithStaticType(static_cast<std::uint8_t>(isDown() ? 1U : 0U));
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// SliderGuiElement
// ---------------------------------------------------------------------------
class SliderGuiElement : public QWidget, public ApplicationGuiElement
{
    Q_OBJECT
    QSlider* slider_;
    QLabel* value_label_;

public:
    SliderGuiElement(QWidget* parent,
                     const std::shared_ptr<ElementSettings>& settings,
                     const std::function<void(const char)>& key_pressed,
                     const std::function<void(const char)>& key_released,
                     const std::function<void()>& on_modification,
                     const std::function<void(const QPoint&, const std::string&)>& right_click,
                     const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    QRect getBoundingRect() const override { return geometry(); }

    int getValue() const { return slider_->value(); }

    std::uint64_t getGuiPayloadSize() const override { return 4U * sizeof(std::int32_t); }
    void fillGuiPayload(FillableUInt8Array& out) const override
    {
        const auto* sl = dynamic_cast<const SliderSettings*>(element_settings_.get());
        out.fillWithStaticType(static_cast<std::int32_t>(sl ? sl->min_value  : 0));
        out.fillWithStaticType(static_cast<std::int32_t>(sl ? sl->max_value  : 100));
        out.fillWithStaticType(static_cast<std::int32_t>(sl ? sl->step_size  : 1));
        out.fillWithStaticType(static_cast<std::int32_t>(slider_->value()));
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// CheckboxGuiElement
// ---------------------------------------------------------------------------
class CheckboxGuiElement : public QCheckBox, public ApplicationGuiElement
{
    Q_OBJECT
public:
    CheckboxGuiElement(QWidget* parent,
                       const std::shared_ptr<ElementSettings>& settings,
                       const std::function<void(const char)>& key_pressed,
                       const std::function<void(const char)>& key_released,
                       const std::function<void()>& on_modification,
                       const std::function<void(const QPoint&, const std::string&)>& right_click,
                       const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    void setLabel(const std::string& new_label) override { setText(QString::fromStdString(new_label)); }
    QRect getBoundingRect() const override { return geometry(); }
    bool isChecked() const { return QCheckBox::isChecked(); }

    std::uint64_t getGuiPayloadSize() const override { return sizeof(std::uint8_t); }
    void fillGuiPayload(FillableUInt8Array& out) const override
    {
        out.fillWithStaticType(static_cast<std::uint8_t>(QCheckBox::isChecked() ? 1U : 0U));
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// TextLabelGuiElement
// ---------------------------------------------------------------------------
class TextLabelGuiElement : public QLabel, public ApplicationGuiElement
{
    Q_OBJECT
public:
    TextLabelGuiElement(QWidget* parent,
                        const std::shared_ptr<ElementSettings>& settings,
                        const std::function<void(const char)>& key_pressed,
                        const std::function<void(const char)>& key_released,
                        const std::function<void()>& on_modification,
                        const std::function<void(const QPoint&, const std::string&)>& right_click,
                        const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    void setLabel(const std::string& new_label) override { setText(QString::fromStdString(new_label)); }
    QRect getBoundingRect() const override { return geometry(); }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// EditableTextGuiElement
// ---------------------------------------------------------------------------
class EditableTextGuiElement : public QLineEdit, public ApplicationGuiElement
{
    Q_OBJECT
public:
    EditableTextGuiElement(QWidget* parent,
                           const std::shared_ptr<ElementSettings>& settings,
                           const std::function<void(const char)>& key_pressed,
                           const std::function<void(const char)>& key_released,
                           const std::function<void()>& on_modification,
                           const std::function<void(const QPoint&, const std::string&)>& right_click,
                           const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    QRect getBoundingRect() const override { return geometry(); }

    std::uint64_t getGuiPayloadSize() const override
    {
        const std::string t = text().toStdString();
        return sizeof(std::uint8_t) + sizeof(std::uint8_t) + t.size();  // enter_pressed + len + text
    }
    void fillGuiPayload(FillableUInt8Array& out) const override
    {
        const std::string t = text().toStdString();
        out.fillWithStaticType(static_cast<std::uint8_t>(0U));  // enter_pressed = false for state sync
        out.fillWithStaticType(static_cast<std::uint8_t>(t.size()));
        out.fillWithDataFromPointer(t.data(), t.size());
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// DropdownMenuGuiElement
// ---------------------------------------------------------------------------
class DropdownMenuGuiElement : public QComboBox, public ApplicationGuiElement
{
    Q_OBJECT
public:
    DropdownMenuGuiElement(QWidget* parent,
                           const std::shared_ptr<ElementSettings>& settings,
                           const std::function<void(const char)>& key_pressed,
                           const std::function<void(const char)>& key_released,
                           const std::function<void()>& on_modification,
                           const std::function<void(const QPoint&, const std::string&)>& right_click,
                           const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    QRect getBoundingRect() const override { return geometry(); }

    std::uint64_t getGuiPayloadSize() const override
    {
        const std::string sel = currentText().toStdString();
        std::uint64_t sz = sizeof(std::uint8_t) + sel.size() + sizeof(std::uint16_t);
        for (int i = 0; i < count(); ++i)
            sz += sizeof(std::uint8_t) + itemText(i).toStdString().size();
        return sz;
    }
    void fillGuiPayload(FillableUInt8Array& out) const override
    {
        const std::string sel = currentText().toStdString();
        out.fillWithStaticType(static_cast<std::uint8_t>(sel.size()));
        out.fillWithDataFromPointer(sel.data(), sel.size());
        out.fillWithStaticType(static_cast<std::uint16_t>(count()));
        for (int i = 0; i < count(); ++i)
        {
            const std::string item = itemText(i).toStdString();
            out.fillWithStaticType(static_cast<std::uint8_t>(item.size()));
            out.fillWithDataFromPointer(item.data(), item.size());
        }
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// RadioButtonGroupGuiElement
// ---------------------------------------------------------------------------
class RadioButtonGroupGuiElement : public QWidget, public ApplicationGuiElement
{
    Q_OBJECT
    QButtonGroup* button_group_;

public:
    RadioButtonGroupGuiElement(QWidget* parent,
                               const std::shared_ptr<ElementSettings>& settings,
                               const std::function<void(const char)>& key_pressed,
                               const std::function<void(const char)>& key_released,
                               const std::function<void()>& on_modification,
                               const std::function<void(const QPoint&, const std::string&)>& right_click,
                               const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    QRect getBoundingRect() const override { return geometry(); }

    std::uint64_t getGuiPayloadSize() const override
    {
        // selected_idx (int32) + num_buttons (uint16) + for each: len (uint8) + text
        std::uint64_t sz = sizeof(std::int32_t) + sizeof(std::uint16_t);
        const auto* rbg = dynamic_cast<const RadioButtonGroupSettings*>(element_settings_.get());
        if (rbg)
            for (const auto& rb : rbg->radio_buttons)
                sz += sizeof(std::uint8_t) + rb.label.size();
        return sz;
    }
    void fillGuiPayload(FillableUInt8Array& out) const override
    {
        const std::int32_t sel_idx = static_cast<std::int32_t>(
            button_group_ ? button_group_->checkedId() : 0);
        out.fillWithStaticType(sel_idx);
        const auto* rbg = dynamic_cast<const RadioButtonGroupSettings*>(element_settings_.get());
        const std::uint16_t num = rbg ? static_cast<std::uint16_t>(rbg->radio_buttons.size()) : 0U;
        out.fillWithStaticType(num);
        if (rbg)
        {
            for (const auto& rb : rbg->radio_buttons)
            {
                out.fillWithStaticType(static_cast<std::uint8_t>(rb.label.size()));
                out.fillWithDataFromPointer(rb.label.data(), rb.label.size());
            }
        }
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// ListBoxGuiElement
// ---------------------------------------------------------------------------
class ListBoxGuiElement : public QListWidget, public ApplicationGuiElement
{
    Q_OBJECT
public:
    ListBoxGuiElement(QWidget* parent,
                      const std::shared_ptr<ElementSettings>& settings,
                      const std::function<void(const char)>& key_pressed,
                      const std::function<void(const char)>& key_released,
                      const std::function<void()>& on_modification,
                      const std::function<void(const QPoint&, const std::string&)>& right_click,
                      const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    QRect getBoundingRect() const override { return geometry(); }

    std::uint64_t getGuiPayloadSize() const override
    {
        const auto* sel_item = currentItem();
        const std::string sel = sel_item ? sel_item->text().toStdString() : std::string{};
        std::uint64_t sz = sizeof(std::uint8_t) + sel.size() + sizeof(std::uint16_t);
        for (int i = 0; i < count(); ++i)
            sz += sizeof(std::uint8_t) + item(i)->text().toStdString().size();
        return sz;
    }
    void fillGuiPayload(FillableUInt8Array& out) const override
    {
        const auto* sel_item = currentItem();
        const std::string sel = sel_item ? sel_item->text().toStdString() : std::string{};
        out.fillWithStaticType(static_cast<std::uint8_t>(sel.size()));
        out.fillWithDataFromPointer(sel.data(), sel.size());
        out.fillWithStaticType(static_cast<std::uint16_t>(count()));
        for (int i = 0; i < count(); ++i)
        {
            const std::string it = item(i)->text().toStdString();
            out.fillWithStaticType(static_cast<std::uint8_t>(it.size()));
            out.fillWithDataFromPointer(it.data(), it.size());
        }
    }

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

// ---------------------------------------------------------------------------
// ScrollingTextGuiElement
// ---------------------------------------------------------------------------
class ScrollingTextGuiElement : public QPlainTextEdit, public ApplicationGuiElement
{
    Q_OBJECT
public:
    ScrollingTextGuiElement(QWidget* parent,
                            const std::shared_ptr<ElementSettings>& settings,
                            const std::function<void(const char)>& key_pressed,
                            const std::function<void(const char)>& key_released,
                            const std::function<void()>& on_modification,
                            const std::function<void(const QPoint&, const std::string&)>& right_click,
                            const std::function<void(const std::string&)>& on_text_output);

    void updateSizeFromParent(const QSize& parent_size) override;
    void keyPressedElementSpecific(const char key) override {}
    void keyReleasedElementSpecific(const char key) override {}
    QRect getBoundingRect() const override { return geometry(); }

    void pushNewText(const std::string& text);

protected:
    void contextMenuEvent(QContextMenuEvent* event) override;
};

#endif  // QT_APPLICATION_GUI_ELEMENTS_H_
