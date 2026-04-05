#ifndef QT_APPLICATION_CMDL_OUTPUT_WINDOW_H_
#define QT_APPLICATION_CMDL_OUTPUT_WINDOW_H_

#include <QPlainTextEdit>
#include <QWidget>

#include <string>

#include "color.h"

// General command-line / log output window.
// Displayed as a floating window; closing hides rather than destroys it.
class CmdlOutputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit CmdlOutputWindow(QWidget* parent = nullptr);
    ~CmdlOutputWindow() = default;

    void pushNewText(Color_t col, const std::string& text);
    void clear();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QPlainTextEdit* text_edit_;
};

#endif  // QT_APPLICATION_CMDL_OUTPUT_WINDOW_H_
