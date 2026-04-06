#ifndef QT_APPLICATION_TAB_BUTTON_H_
#define QT_APPLICATION_TAB_BUTTON_H_

#include <QColor>
#include <QTimer>
#include <QWidget>

#include <functional>
#include <string>

#include "project_state/project_settings.h"

// Custom tab button that mimics the old wxWidgets-style flat colored tab buttons.
// Features: flat filled rectangle, hover brightening animation, hand cursor on hover.
class TabButton : public QWidget
{
    Q_OBJECT

public:
    explicit TabButton(const TabSettings& tab_settings, int id, QWidget* parent = nullptr);

    void setSelected(bool selected);
    void setLabel(const QString& text);
    bool isSelected() const { return is_selected_; }
    QString label() const { return QString::fromStdString(tab_settings_.name); }

signals:
    void tabClicked(int id);

protected:
    void paintEvent(QPaintEvent* event) override;
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void onHoverTimer();

private:
    static QColor fromRGB(const RGBTripletf& c);
    static QColor brighten(const QColor& base, float factor);

    TabSettings tab_settings_;
    int id_;

    bool is_selected_{false};
    bool is_pressed_{false};

    QColor base_normal_;
    QColor base_selected_;

    QColor current_draw_color_;  // animated during hover

    QTimer* hover_timer_;
    int hover_iter_{0};

    static constexpr int kTimerPeriodMs   = 15;
    static constexpr int kHoverIterations = 10;
    static constexpr float kBrightenFactor = 0.7f;
};

#endif  // QT_APPLICATION_TAB_BUTTON_H_
