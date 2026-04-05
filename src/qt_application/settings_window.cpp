#include "settings_window.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QVBoxLayout>

SettingsWindow::SettingsWindow(QWidget* parent, const std::string& title, const FieldsType& fields)
    : QDialog(parent)
{
    setWindowTitle(QString::fromStdString(title));
    setMinimumWidth(400);

    QVBoxLayout* outer = new QVBoxLayout(this);

    QFormLayout* form = new QFormLayout();
    outer->addLayout(form);

    for (const auto& kv : fields)
    {
        const std::string& key = kv.first;
        const std::string& label_text = kv.second.first;
        const std::string& init_value = kv.second.second;

        QLineEdit* edit = new QLineEdit(QString::fromStdString(init_value), this);
        form->addRow(new QLabel(QString::fromStdString(label_text), this), edit);
        field_edits_[key] = edit;
    }

    QDialogButtonBox* buttons = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    outer->addWidget(buttons);

    adjustSize();
}

std::string SettingsWindow::getFieldString(const std::string& field_name) const
{
    auto it = field_edits_.find(field_name);
    if (it != field_edits_.end())
        return it->second->text().toStdString();
    return {};
}
