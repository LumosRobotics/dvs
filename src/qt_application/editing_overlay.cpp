#include "editing_overlay.h"

#include <QPainter>
#include <QPen>

EditingOverlay::EditingOverlay(QWidget* parent) : QWidget(parent)
{
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::Widget);
    hide();
}

void EditingOverlay::showForRect(const QRect& rect)
{
    setGeometry(rect);
    raise();
    show();
    update();
}

void EditingOverlay::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, false);

    // Semi-transparent fill to indicate the element boundary
    p.fillRect(rect(), QColor(0, 0, 200, 30));

    // Dashed border
    QPen pen(QColor(0, 0, 200), 2, Qt::DashLine);
    p.setPen(pen);
    p.drawRect(rect().adjusted(1, 1, -1, -1));

    // Draw small resize handles at corners and edge midpoints
    const QColor handle_color(0, 0, 180);
    const int hs = 6;  // handle size
    auto drawHandle = [&](int cx, int cy) {
        p.fillRect(cx - hs / 2, cy - hs / 2, hs, hs, handle_color);
    };

    const int w = width(), h = height();
    drawHandle(0, 0);      drawHandle(w / 2, 0);      drawHandle(w, 0);
    drawHandle(0, h / 2);                              drawHandle(w, h / 2);
    drawHandle(0, h);      drawHandle(w / 2, h);       drawHandle(w, h);
}
