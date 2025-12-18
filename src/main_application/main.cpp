#include <CoreFoundation/CoreFoundation.h>
#include <unistd.h>
#include <wx/wx.h>

#include <iostream>

#include "debug_value_args.h"
#include "lumos/logging.h"
#include "main_window.h"

class MainApp : public wxApp
{
private:
public:
    virtual bool OnInit();
    virtual int OnExit();
};

MainWindow* main_window;

IMPLEMENT_APP(MainApp)

bool MainApp::OnInit()
{
    debug_value_args::parseArgs(argc, argv);

    wxImage::AddHandler(new wxPNGHandler);
    wxImage::AddHandler(new wxICOHandler);

    std::vector<std::string> cmd_args;
    for (int k = 0; k < wxAppConsole::argc; k++)
    {
        cmd_args.emplace_back(wxAppConsole::argv[k].mb_str());
    }

    main_window = new MainWindow(cmd_args);

    // SetTopWindow(main_window);

    return true;
}

int MainApp::OnExit()
{
    main_window->destroy();
    LUMOS_LOG_INFO() << "Exit from MainApp::OnExit!";
    return this->wxApp::OnExit();
}
