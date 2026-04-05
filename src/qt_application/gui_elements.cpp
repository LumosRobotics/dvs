#include "gui_elements.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "lumos/plotting/constants.h"
#include "lumos/plotting/fillable_uint8_array.h"
#include "lumos/plotting/internal.h"

// ---------------------------------------------------------------------------
// Helper: compute pixel rect from fractional element settings
// ---------------------------------------------------------------------------
static QRect computeGeometry(const QSize& parent_size, const ElementSettings* s)
{
    return QRect(
        static_cast<int>(s->x * parent_size.width()),
        static_cast<int>(s->y * parent_size.height()),
        static_cast<int>(s->width * parent_size.width()),
        static_cast<int>(s->height * parent_size.height()));
}

// ============================= ButtonGuiElement =============================

ButtonGuiElement::ButtonGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QPushButton(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    auto* btn_settings = dynamic_cast<ButtonSettings*>(settings.get());
    if (btn_settings)
    {
        setText(QString::fromStdString(btn_settings->label));
    }

    connect(this, &QPushButton::clicked, this, [this]() {
        sendGuiData();
        if (notify_main_window_about_modification_)
            notify_main_window_about_modification_();
    });
}

void ButtonGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void ButtonGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
}

// ============================= SliderGuiElement =============================

SliderGuiElement::SliderGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QWidget(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    auto* sl_settings = dynamic_cast<SliderSettings*>(settings.get());

    const bool horizontal = sl_settings ? sl_settings->is_horizontal : true;
    slider_ = new QSlider(horizontal ? Qt::Horizontal : Qt::Vertical, this);

    if (sl_settings)
    {
        slider_->setMinimum(sl_settings->min_value);
        slider_->setMaximum(sl_settings->max_value);
        slider_->setValue(sl_settings->init_value);
        slider_->setSingleStep(sl_settings->step_size);
        slider_->setPageStep(sl_settings->step_size);
    }

    value_label_ = new QLabel(QString::number(slider_->value()), this);
    value_label_->setAlignment(Qt::AlignCenter);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(slider_);
    layout->addWidget(value_label_);
    setLayout(layout);

    connect(slider_, &QSlider::valueChanged, this, [this](int value) {
        value_label_->setText(QString::number(value));
        sendGuiData();
        if (notify_main_window_about_modification_)
            notify_main_window_about_modification_();
    });
}

void SliderGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void SliderGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
}

// ============================= CheckboxGuiElement ===========================

CheckboxGuiElement::CheckboxGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QCheckBox(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    auto* cb_settings = dynamic_cast<CheckboxSettings*>(settings.get());
    if (cb_settings)
    {
        setText(QString::fromStdString(cb_settings->label));
    }

    connect(this, &QCheckBox::stateChanged, this, [this](int) {
        sendGuiData();
        if (notify_main_window_about_modification_)
            notify_main_window_about_modification_();
    });
}

void CheckboxGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void CheckboxGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
}

// ============================= TextLabelGuiElement ==========================

TextLabelGuiElement::TextLabelGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QLabel(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    auto* lbl_settings = dynamic_cast<TextLabelSettings*>(settings.get());
    if (lbl_settings)
    {
        setText(QString::fromStdString(lbl_settings->label));
    }
}

void TextLabelGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void TextLabelGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
}

// ============================= EditableTextGuiElement =======================

EditableTextGuiElement::EditableTextGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QLineEdit(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    auto* et_settings = dynamic_cast<EditableTextSettings*>(settings.get());
    if (et_settings)
    {
        setText(QString::fromStdString(et_settings->init_value));
    }

    connect(this, &QLineEdit::returnPressed, this, [this]() {
        // For EditableText, enter_pressed=true when sent on Return key
        const std::string t = text().toStdString();
        const std::uint64_t total =
            sizeof(std::uint8_t) +  // type
            sizeof(std::uint8_t) +  // handle len
            element_settings_->handle_string.size() +
            sizeof(std::uint32_t) +  // payload size
            sizeof(std::uint8_t) +   // enter_pressed = 1
            sizeof(std::uint8_t) +   // text len
            t.size();
        FillableUInt8Array arr{total};
        arr.fillWithStaticType(static_cast<std::uint8_t>(element_settings_->type));
        const std::uint8_t hl = static_cast<std::uint8_t>(element_settings_->handle_string.size());
        arr.fillWithStaticType(hl);
        arr.fillWithDataFromPointer(element_settings_->handle_string.data(), hl);
        arr.fillWithStaticType(static_cast<std::uint32_t>(2U + t.size()));
        arr.fillWithStaticType(static_cast<std::uint8_t>(1U));  // enter_pressed = true
        arr.fillWithStaticType(static_cast<std::uint8_t>(t.size()));
        arr.fillWithDataFromPointer(t.data(), t.size());
        lumos::internal::sendThroughTcpInterface(arr.view(), lumos::internal::kGuiTcpPortNum);

        if (on_text_output_)
            on_text_output_(t);
        if (notify_main_window_about_modification_)
            notify_main_window_about_modification_();
    });
}

void EditableTextGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void EditableTextGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
    else
        QLineEdit::contextMenuEvent(event);
}

// ============================= DropdownMenuGuiElement =======================

DropdownMenuGuiElement::DropdownMenuGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QComboBox(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    auto dd_settings = std::dynamic_pointer_cast<DropdownMenuSettings>(settings);
    if (dd_settings)
    {
        for (const auto& item : dd_settings->elements)
        {
            addItem(QString::fromStdString(item));
        }

        if (!dd_settings->initially_selected_item.empty())
        {
            const int idx = findText(QString::fromStdString(dd_settings->initially_selected_item));
            if (idx >= 0)
                setCurrentIndex(idx);
        }
    }

    connect(this, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int) {
        sendGuiData();
        if (notify_main_window_about_modification_)
            notify_main_window_about_modification_();
    });
}

void DropdownMenuGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void DropdownMenuGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
}

// ============================= RadioButtonGroupGuiElement ===================

RadioButtonGroupGuiElement::RadioButtonGroupGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QWidget(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    button_group_ = new QButtonGroup(this);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    auto rbg_settings = std::dynamic_pointer_cast<RadioButtonGroupSettings>(settings);
    if (rbg_settings)
    {
        int id = 0;
        for (const auto& rb_setting : rbg_settings->radio_buttons)
        {
            QRadioButton* rb = new QRadioButton(QString::fromStdString(rb_setting.label), this);
            button_group_->addButton(rb, id++);
            layout->addWidget(rb);
        }
        // Select first by default
        if (!rbg_settings->radio_buttons.empty())
        {
            if (auto* first = button_group_->button(0))
                first->setChecked(true);
        }
    }

    setLayout(layout);

    connect(button_group_, QOverload<int>::of(&QButtonGroup::idClicked), this, [this](int) {
        sendGuiData();
        if (notify_main_window_about_modification_)
            notify_main_window_about_modification_();
    });
}

void RadioButtonGroupGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void RadioButtonGroupGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
}

// ============================= ListBoxGuiElement ============================

ListBoxGuiElement::ListBoxGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QListWidget(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    auto lb_settings = std::dynamic_pointer_cast<ListBoxSettings>(settings);
    if (lb_settings)
    {
        for (const auto& item : lb_settings->elements)
        {
            addItem(QString::fromStdString(item));
        }
    }

    connect(this, &QListWidget::itemSelectionChanged, this, [this]() {
        sendGuiData();
        if (notify_main_window_about_modification_)
            notify_main_window_about_modification_();
    });
}

void ListBoxGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void ListBoxGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
    else
        QListWidget::contextMenuEvent(event);
}

// ============================= ScrollingTextGuiElement ======================

ScrollingTextGuiElement::ScrollingTextGuiElement(
    QWidget* parent,
    const std::shared_ptr<ElementSettings>& settings,
    const std::function<void(const char)>& key_pressed,
    const std::function<void(const char)>& key_released,
    const std::function<void()>& on_modification,
    const std::function<void(const QPoint&, const std::string&)>& right_click,
    const std::function<void(const std::string&)>& on_text_output)
    : QPlainTextEdit(parent),
      ApplicationGuiElement(settings, key_pressed, key_released, on_modification, right_click, on_text_output)
{
    setReadOnly(true);
}

void ScrollingTextGuiElement::updateSizeFromParent(const QSize& parent_size)
{
    setGeometry(computeGeometry(parent_size, element_settings_.get()));
}

void ScrollingTextGuiElement::pushNewText(const std::string& text)
{
    appendPlainText(QString::fromStdString(text));
    ensureCursorVisible();
}

void ScrollingTextGuiElement::contextMenuEvent(QContextMenuEvent* event)
{
    if (notify_parent_right_click_)
        notify_parent_right_click_(event->globalPos(), element_settings_->handle_string);
    else
        QPlainTextEdit::contextMenuEvent(event);
}
