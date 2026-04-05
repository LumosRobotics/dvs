#ifndef QT_APPLICATION_HELP_PANE_H_
#define QT_APPLICATION_HELP_PANE_H_

#include <QTextBrowser>
#include <QWidget>

// Floating help window with a read-only HTML text browser.
class HelpPane : public QWidget
{
    Q_OBJECT

public:
    explicit HelpPane(QWidget* parent = nullptr);
    ~HelpPane() = default;

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QTextBrowser* browser_;
};

#endif  // QT_APPLICATION_HELP_PANE_H_
