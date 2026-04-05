#include "topic_text_output_window.h"

#include <QCloseEvent>
#include <QFont>
#include <QTextCharFormat>
#include <QTextCursor>
#include <QVBoxLayout>

#include "color_utils.h"

TopicTextOutputWindow::TopicTextOutputWindow(QWidget* parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("Topic Output"));
    resize(800, 600);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    tab_widget_ = new QTabWidget(this);
    layout->addWidget(tab_widget_);
}

QPlainTextEdit* TopicTextOutputWindow::getOrCreateTab(const std::string& topic)
{
    auto it = topic_tabs_.find(topic);
    if (it != topic_tabs_.end())
    {
        return it->second;
    }

    QPlainTextEdit* edit = new QPlainTextEdit(tab_widget_);
    edit->setReadOnly(true);
    edit->setFont(QFont("Courier", 11));
    tab_widget_->addTab(edit, QString::fromStdString(topic));
    topic_tabs_[topic] = edit;
    return edit;
}

void TopicTextOutputWindow::pushNewText(const std::string& topic, Color_t col, const std::string& text)
{
    QPlainTextEdit* edit = getOrCreateTab(topic);

    QTextCharFormat fmt;
    fmt.setForeground(colorToQColor(col));

    QTextCursor cursor = edit->textCursor();
    cursor.movePosition(QTextCursor::End);
    cursor.insertText(QString::fromStdString(text), fmt);
    edit->setTextCursor(cursor);
    edit->ensureCursorVisible();
}

void TopicTextOutputWindow::clearTopic(const std::string& topic)
{
    auto it = topic_tabs_.find(topic);
    if (it != topic_tabs_.end())
    {
        it->second->clear();
    }
}

void TopicTextOutputWindow::clearAll()
{
    for (auto& pair : topic_tabs_)
    {
        pair.second->clear();
    }
}

void TopicTextOutputWindow::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}
