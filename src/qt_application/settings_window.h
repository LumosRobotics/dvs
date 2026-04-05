#ifndef QT_APPLICATION_SETTINGS_WINDOW_H_
#define QT_APPLICATION_SETTINGS_WINDOW_H_

#include <QDialog>
#include <QLineEdit>

#include <map>
#include <string>
#include <utility>

// Generic settings / preferences dialog.
// The caller provides a map of { key -> (label, initial_value) } fields.
// After exec(), use getFieldString(key) to retrieve the entered values.
class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    // field_key -> (display_label, initial_value)
    using FieldsType = std::map<std::string, std::pair<std::string, std::string>>;

    SettingsWindow() = delete;
    explicit SettingsWindow(QWidget* parent, const std::string& title, const FieldsType& fields);

    std::string getFieldString(const std::string& field_name) const;

private:
    std::map<std::string, QLineEdit*> field_edits_;
};

#endif  // QT_APPLICATION_SETTINGS_WINDOW_H_
