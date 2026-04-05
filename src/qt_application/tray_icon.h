#ifndef QT_APPLICATION_TRAY_ICON_H_
#define QT_APPLICATION_TRAY_ICON_H_

#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QSystemTrayIcon>

#include <functional>
#include <string>
#include <vector>

// Application system-tray icon.
// Provides a context menu with File actions and a Windows submenu.
// Callbacks are set via setOn*() before calling show().
class TrayIcon : public QObject
{
    Q_OBJECT

public:
    explicit TrayIcon(QObject* parent = nullptr);
    ~TrayIcon();

    void setIcon(const QIcon& icon);

    void setOnFileNew(std::function<void()> fn);
    void setOnFileOpen(std::function<void()> fn);
    void setOnFileSave(std::function<void()> fn);
    void setOnFileSaveAs(std::function<void()> fn);
    void setOnNewWindow(std::function<void()> fn);
    void setOnPreferences(std::function<void()> fn);
    void setOnShowWindow(std::function<void(const std::string&)> fn);

    void addWindow(const std::string& window_name);
    void removeWindow(const std::string& window_name);

    void show();

private slots:
    void onActivated(QSystemTrayIcon::ActivationReason reason);

private:
    void rebuildWindowsMenu();

    QSystemTrayIcon* tray_;
    QMenu*           context_menu_;
    QMenu*           windows_submenu_;

    std::function<void()>                    file_new_fn_;
    std::function<void()>                    file_open_fn_;
    std::function<void()>                    file_save_fn_;
    std::function<void()>                    file_save_as_fn_;
    std::function<void()>                    new_window_fn_;
    std::function<void()>                    preferences_fn_;
    std::function<void(const std::string&)>  show_window_fn_;

    std::vector<std::string> window_names_;
};

#endif  // QT_APPLICATION_TRAY_ICON_H_
