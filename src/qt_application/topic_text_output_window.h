#ifndef QT_APPLICATION_TOPIC_TEXT_OUTPUT_WINDOW_H_
#define QT_APPLICATION_TOPIC_TEXT_OUTPUT_WINDOW_H_

#include <QPlainTextEdit>
#include <QTabWidget>
#include <QWidget>

#include <map>
#include <string>

#include "color.h"

// Per-topic text output window.
// Each unique topic gets its own tab with a scrolling read-only text area.
class TopicTextOutputWindow : public QWidget
{
    Q_OBJECT

public:
    explicit TopicTextOutputWindow(QWidget* parent = nullptr);
    ~TopicTextOutputWindow() = default;

    void pushNewText(const std::string& topic, Color_t col, const std::string& text);
    void clearTopic(const std::string& topic);
    void clearAll();

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    QTabWidget* tab_widget_;
    std::map<std::string, QPlainTextEdit*> topic_tabs_;

    QPlainTextEdit* getOrCreateTab(const std::string& topic);
};

#endif  // QT_APPLICATION_TOPIC_TEXT_OUTPUT_WINDOW_H_
