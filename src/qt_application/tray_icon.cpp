#include "tray_icon.h"

#include <QApplication>
#include <QString>

#include <algorithm>

TrayIcon::TrayIcon(QObject* parent)
    : QObject(parent)
{
    tray_ = new QSystemTrayIcon(this);
    context_menu_ = new QMenu();
    windows_submenu_ = new QMenu(tr("Windows"));

    // File section
    context_menu_->addAction(tr("New"), this, [this]() {
        if (file_new_fn_) file_new_fn_();
    });
    context_menu_->addSeparator();
    context_menu_->addAction(tr("Open..."), this, [this]() {
        if (file_open_fn_) file_open_fn_();
    });
    context_menu_->addSeparator();
    context_menu_->addAction(tr("Save"), this, [this]() {
        if (file_save_fn_) file_save_fn_();
    });
    context_menu_->addAction(tr("Save As..."), this, [this]() {
        if (file_save_as_fn_) file_save_as_fn_();
    });
    context_menu_->addSeparator();

    // Windows submenu (populated dynamically)
    context_menu_->addMenu(windows_submenu_);
    context_menu_->addSeparator();

    context_menu_->addAction(tr("Quit"), qApp, &QApplication::quit);

    tray_->setContextMenu(context_menu_);
    tray_->setToolTip("Duoplot");

    connect(tray_, &QSystemTrayIcon::activated, this, &TrayIcon::onActivated);

    rebuildWindowsMenu();
}

TrayIcon::~TrayIcon()
{
    delete context_menu_;
}

void TrayIcon::setIcon(const QIcon& icon) { tray_->setIcon(icon); }

void TrayIcon::setOnFileNew(std::function<void()> fn)     { file_new_fn_    = std::move(fn); }
void TrayIcon::setOnFileOpen(std::function<void()> fn)    { file_open_fn_   = std::move(fn); }
void TrayIcon::setOnFileSave(std::function<void()> fn)    { file_save_fn_   = std::move(fn); }
void TrayIcon::setOnFileSaveAs(std::function<void()> fn)  { file_save_as_fn_= std::move(fn); }
void TrayIcon::setOnNewWindow(std::function<void()> fn)   { new_window_fn_  = std::move(fn); }
void TrayIcon::setOnPreferences(std::function<void()> fn) { preferences_fn_ = std::move(fn); }
void TrayIcon::setOnShowWindow(std::function<void(const std::string&)> fn)
{
    show_window_fn_ = std::move(fn);
}

void TrayIcon::addWindow(const std::string& window_name)
{
    window_names_.push_back(window_name);
    rebuildWindowsMenu();
}

void TrayIcon::removeWindow(const std::string& window_name)
{
    auto it = std::find(window_names_.begin(), window_names_.end(), window_name);
    if (it != window_names_.end())
        window_names_.erase(it);
    rebuildWindowsMenu();
}

void TrayIcon::show()
{
    if (QSystemTrayIcon::isSystemTrayAvailable())
        tray_->show();
}

void TrayIcon::onActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::DoubleClick)
    {
        if (!window_names_.empty() && show_window_fn_)
            show_window_fn_(window_names_.front());
    }
}

void TrayIcon::rebuildWindowsMenu()
{
    windows_submenu_->clear();

    windows_submenu_->addAction(tr("New Window"), this, [this]() {
        if (new_window_fn_) new_window_fn_();
    });
    windows_submenu_->addSeparator();

    for (const std::string& name : window_names_)
    {
        windows_submenu_->addAction(QString::fromStdString(name), this, [this, name]() {
            if (show_window_fn_) show_window_fn_(name);
        });
    }
}
