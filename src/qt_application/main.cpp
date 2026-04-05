#include <QApplication>
#include <QSurfaceFormat>
#include <iostream>

#include "lumos/logging/logging.h"
#include "main_window.h"

int main(int argc, char* argv[])
{
    // Request OpenGL 3.3 Core Profile globally before creating QApplication.
    // This is required on macOS (and good practice everywhere): Qt defaults to a
    // legacy 2.1 context which does not support GLSL #version 330.
    QSurfaceFormat format;
    format.setVersion(3, 3);
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setDepthBufferSize(24);
    format.setStencilBufferSize(8);
    format.setSamples(4);
    QSurfaceFormat::setDefaultFormat(format);

    QApplication app(argc, argv);

    app.setApplicationName("Duoplot");
    app.setApplicationVersion("1.0");
    app.setOrganizationName("Duoplot");

    LUMOS_LOG_INFO() << "Starting Duoplot Qt application";

    std::vector<std::string> cmd_args;
    for (int k = 0; k < argc; k++)
    {
        cmd_args.emplace_back(argv[k]);
    }

    // MainWindow is hidden - it creates and manages visible GuiWindows
    MainWindow main_window(cmd_args);
    // Note: MainWindow calls hide() in its constructor, so we don't call show()
    // The GuiWindow instances it creates are shown automatically

    LUMOS_LOG_INFO() << "Application initialized - MainWindow (hidden) managing GuiWindows";

    int result = app.exec();

    LUMOS_LOG_INFO() << "Exiting Duoplot Qt application";

    return result;
}