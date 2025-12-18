#ifndef MAIN_APPLICATION_TRAY_ICON_H_
#define MAIN_APPLICATION_TRAY_ICON_H_

#include <wx/taskbar.h>
#include <wx/wx.h>

#include <functional>
#include <iostream>
#include <map>

#include "constants.h"
#include "lumos/logging.h"

enum
{
    PU_EXIT = 10001,
    PU_SHOW_SUB_WINDOW,
    PU_EDIT_LAYOUT,
    PU_PREFERENCES,
    PU_FILE,
    PU_FILE_NEW,
    PU_FILE_OPEN,
    PU_FILE_SAVE,
    PU_FILE_SAVE_AS,
    PU_FILE_NEW_WINDOW,
    PU_FIRST_WINDOW_ID,
    PU_LAST_WINDOW_ID = PU_FIRST_WINDOW_ID + kMaxNumWindows - 1,
    PU_SUBMAIN
};

class MyMenu : public wxMenu
{
private:
    std::function<void()> menu_teardown_function_;

public:
    MyMenu() = delete;
    MyMenu(std::function<void()>&& menu_teardown_function)
        : wxMenu{}, menu_teardown_function_{std::move(menu_teardown_function)}
    {
    }

    ~MyMenu()
    {
        menu_teardown_function_();
    }
};

#ifdef PLATFORM_APPLE_M

class CustomTaskBarIcon
{
public:
    CustomTaskBarIcon() {}

    void setOnMenuExitCallback(std::function<void()>&& exit_function) {}
    void setOnMenuFileNew(std::function<void()>&& file_new_function) {}
    void setOnMenuFileOpen(std::function<void()>&& file_open_function) {}
    void setOnMenuFileSave(std::function<void()>&& file_save_function) {}
    void setOnMenuFileSaveAs(std::function<void()>&& file_save_as_function) {}
    void setOnMenuSubWindow(std::function<void(const std::string&)>&& submenu_function) {}
    void setOnMenuPreferences(std::function<void()>&& preferences_function) {}
    void setOnMenuFileNewWindow(std::function<void()>&& new_window_function) {}
    void onMenuExit(wxCommandEvent&) {}
    void onMenuFileNew(wxCommandEvent&) {}
    void onMenuFileOpen(wxCommandEvent&) {}
    void onMenuFileSave(wxCommandEvent&) {}
    void onMenuFileSaveAs(wxCommandEvent&) {}
    void onMenuFileNewWindow(wxCommandEvent&) {}
    void onMenuPreferences(wxCommandEvent&) {}
    void onMenuSubWindow(wxCommandEvent& evt) {}
    void addNewWindow(const std::string& window_name) {}
    void removeWindow(const std::string& window_name) {}

    bool SetIcon(const wxIcon& icn, const wxString& tooltip)
    {
        return true;
    }
};

#else

class CustomTaskBarIcon : public wxTaskBarIcon
{
public:
#if defined(__WXOSX__) && wxOSX_USE_COCOA
    CustomTaskBarIcon(wxTaskBarIconType iconType = wxTBI_DEFAULT_TYPE);
#else
    CustomTaskBarIcon();
#endif

    int getNextFreeId()
    {
        LUMOS_ASSERT(free_ids_.size() != 0);
        const int val = free_ids_.back();
        free_ids_.pop_back();
        taken_ids_.push_back(val);
        return val;
    }

    void freeId(int id_to_free)
    {
        LUMOS_ASSERT(std::find(taken_ids_.begin(), taken_ids_.end(), id_to_free) != taken_ids_.end());
        taken_ids_.erase(std::find(taken_ids_.begin(), taken_ids_.end(), id_to_free));
        free_ids_.push_back(id_to_free);
    }

    void setOnMenuExitCallback(std::function<void()>&& exit_function);
    void setOnMenuFileNew(std::function<void()>&& file_new_function);
    void setOnMenuFileOpen(std::function<void()>&& file_open_function);
    void setOnMenuFileSave(std::function<void()>&& file_save_function);
    void setOnMenuFileSaveAs(std::function<void()>&& file_save_as_function);
    void setOnMenuSubWindow(std::function<void(const std::string&)>&& submenu_function);
    void setOnMenuPreferences(std::function<void()>&& preferences_function);
    void setOnMenuFileNewWindow(std::function<void()>&& new_window_function);

    void onMenuExit(wxCommandEvent&);

    void onMenuFileNew(wxCommandEvent&);
    void onMenuFileOpen(wxCommandEvent&);
    void onMenuFileSave(wxCommandEvent&);
    void onMenuFileSaveAs(wxCommandEvent&);
    void onMenuFileNewWindow(wxCommandEvent&);
    void onMenuPreferences(wxCommandEvent&);

    void onMenuSubWindow(wxCommandEvent& evt);
    void addNewWindow(const std::string& window_name);
    void removeWindow(const std::string& window_name);
    virtual wxMenu* CreatePopupMenu();

    wxDECLARE_EVENT_TABLE();

private:
    std::function<void()> exit_function_;
    std::function<void()> preferences_function_;
    std::function<void()> file_new_function_;
    std::function<void()> file_open_function_;
    std::function<void()> file_save_function_;
    std::function<void()> file_save_as_function_;
    std::function<void(const std::string&)> submenu_function_;
    std::function<void()> new_window_function_;

    std::vector<int> free_ids_;
    std::vector<int> taken_ids_;
    wxMenu* windows_submenu_;

    std::map<std::string, wxEventTypeTag<wxCommandEvent>> window_events_;
    std::map<std::string, int> window_event_ids_;
};

#endif

#endif  // MAIN_APPLICATION_TRAY_ICON_H_
