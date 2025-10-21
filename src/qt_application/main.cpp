#include <QApplication>
#include <iostream>

#include "duoplot/logging.h"
#include "main_window.h"

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    app.setApplicationName("Duoplot");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Duoplot");

    DUOPLOT_LOG_INFO() << "Starting Duoplot Qt application";

    std::vector<std::string> cmd_args;
    for (int k = 0; k < argc; k++)
    {
        cmd_args.emplace_back(argv[k]);
    }

    // MainWindow is hidden - it creates and manages visible GuiWindows
    MainWindow main_window(cmd_args);
    // Note: MainWindow calls hide() in its constructor, so we don't call show()
    // The GuiWindow instances it creates are shown automatically

    DUOPLOT_LOG_INFO() << "Application initialized - MainWindow (hidden) managing GuiWindows";

    int result = app.exec();

    DUOPLOT_LOG_INFO() << "Exiting Duoplot Qt application";

    return result;
}