#include "cmdl_output_window.h"

#include <QCloseEvent>
#include <QFont>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QVBoxLayout>

#include "color_utils.h"

CmdlOutputWindow::CmdlOutputWindow(QWidget* parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("Output"));
    resize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    text_edit_ = new QPlainTextEdit(this);
    text_edit_->setReadOnly(true);
    text_edit_->setFont(QFont("Courier", 11));
    layout->addWidget(text_edit_);
}

void CmdlOutputWindow::pushNewText(Color_t col, const std::string& text)
{
    QTextCharFormat fmt;
    fmt.setForeground(colorToQColor(col));

    QTextCursor cursor = text_edit_->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(QString::fromStdString(text), fmt);
    text_edit_->setTextCursor(cursor);
    text_edit_->ensureCursorVisible();
}

void CmdlOutputWindow::clear()
{
    text_edit_->clear();
}

void CmdlOutputWindow::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}
