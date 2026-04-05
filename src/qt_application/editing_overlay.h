#ifndef QT_APPLICATION_EDITING_OVERLAY_H_
#define QT_APPLICATION_EDITING_OVERLAY_H_

#include <QWidget>

// Transparent overlay drawn on top of content_area_ to show the element
// silhouette while the user Ctrl+drags to move or resize it.
class EditingOverlay : public QWidget
{
    Q_OBJECT
public:
    explicit EditingOverlay(QWidget* parent);

    // Position / size the overlay and make it visible.
    void showForRect(const QRect& rect);

protected:
    void paintEvent(QPaintEvent* event) override;
};

#endif  // QT_APPLICATION_EDITING_OVERLAY_H_
