#include "tab_button.h"

#include <QMouseEvent>
#include <QPainter>

TabButton::TabButton(const TabSettings& tab_settings, int id, QWidget* parent)
    : QWidget(parent), tab_settings_(tab_settings), id_(id)
{
    base_normal_   = fromRGB(tab_settings_.button_normal_color);
    base_selected_ = fromRGB(tab_settings_.button_selected_color);
    current_draw_color_ = base_normal_;

    setFixedHeight(30);
    setCursor(Qt::PointingHandCursor);

    hover_timer_ = new QTimer(this);
    hover_timer_->setInterval(kTimerPeriodMs);
    connect(hover_timer_, &QTimer::timeout, this, &TabButton::onHoverTimer);
}

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

QColor TabButton::fromRGB(const RGBTripletf& c)
{
    return QColor::fromRgbF(
        static_cast<double>(c.red),
        static_cast<double>(c.green),
        static_cast<double>(c.blue));
}

QColor TabButton::brighten(const QColor& base, float factor)
{
    // Multiply each channel by (1 + factor), clamped to 255.
    const float r = std::min(base.red()   * (1.0f + factor), 255.0f);
    const float g = std::min(base.green() * (1.0f + factor), 255.0f);
    const float b = std::min(base.blue()  * (1.0f + factor), 255.0f);
    return QColor(static_cast<int>(r), static_cast<int>(g), static_cast<int>(b));
}

// ---------------------------------------------------------------------------
// Public
// ---------------------------------------------------------------------------

void TabButton::setLabel(const QString& text)
{
    tab_settings_.name = text.toStdString();
    update();
}

void TabButton::setSelected(bool selected)
{
    is_selected_ = selected;
    is_pressed_  = false;
    hover_timer_->stop();
    hover_iter_ = 0;
    current_draw_color_ = is_selected_ ? base_selected_ : base_normal_;
    setCursor(is_selected_ ? Qt::ArrowCursor : Qt::PointingHandCursor);
    update();
}

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------

void TabButton::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.fillRect(rect(), current_draw_color_);

    p.setPen(Qt::black);
    const int text_h = p.fontMetrics().height();
    p.drawText(5, rect().height() / 2 - text_h / 2 + p.fontMetrics().ascent() - 3,
               label());
}

void TabButton::enterEvent(QEnterEvent* /*event*/)
{
    if (is_selected_)
        return;
    hover_iter_ = 0;
    hover_timer_->start();
}

void TabButton::leaveEvent(QEvent* /*event*/)
{
    hover_timer_->stop();
    hover_iter_ = 0;
    current_draw_color_ = is_selected_ ? base_selected_ : base_normal_;
    update();
}

void TabButton::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && !is_selected_)
    {
        is_pressed_ = true;
        current_draw_color_ = fromRGB(tab_settings_.button_clicked_color);
        update();
    }
}

void TabButton::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton && is_pressed_)
    {
        is_pressed_ = false;
        update();
        emit tabClicked(id_);
    }
}

// ---------------------------------------------------------------------------
// Timer slot
// ---------------------------------------------------------------------------

void TabButton::onHoverTimer()
{
    if (hover_iter_ >= kHoverIterations)
    {
        hover_timer_->stop();
        return;
    }

    // Interpolate from base toward brightened version (index 0..60 out of 100)
    const float t = static_cast<float>(hover_iter_ * 6) / 100.0f;
    const QColor bright = brighten(is_selected_ ? base_selected_ : base_normal_, kBrightenFactor);
    const QColor& base  = is_selected_ ? base_selected_ : base_normal_;

    const int r = base.red()   + static_cast<int>(t * (bright.red()   - base.red()));
    const int g = base.green() + static_cast<int>(t * (bright.green() - base.green()));
    const int b = base.blue()  + static_cast<int>(t * (bright.blue()  - base.blue()));
    current_draw_color_ = QColor(r, g, b);

    hover_iter_++;
    update();
}
