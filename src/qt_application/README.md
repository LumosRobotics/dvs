# Duoplot Qt Application

This is the Qt-based implementation of the Duoplot visualization application, replacing the wxWidgets implementation.

## Structure

### Core Files

- **main.cpp** - Application entry point with QApplication setup
- **main_window.h/cpp** - Main application window (equivalent to wxWidgets MainWindow)
- **plot_pane.h/cpp** - OpenGL-based plotting widget (equivalent to wxWidgets PlotPane)

### Key Features

1. **TCP Communication**: Reuses the existing DataReceiver from main_application for TCP-based plot commands
2. **OpenGL Rendering**: Uses QOpenGLWidget for hardware-accelerated 3D plotting
3. **Multi-threaded**: Separate thread for TCP reception to avoid blocking the UI
4. **Timer-based Updates**:
   - Receive timer (~60 Hz) for processing incoming data
   - Refresh timer (~30 Hz) for updating displays

### Architecture

```
main.cpp
  └─> QApplication
       └─> MainWindow
            ├─> TCP Receive Thread (DataReceiver)
            ├─> Receive Timer (processes queued data)
            ├─> Refresh Timer (updates UI)
            └─> PlotPane(s) (OpenGL widgets)
```

### Building

The Qt application is included in the main CMake build:

```bash
cd src/build
cmake ..
make qt_duoplot
./qt_duoplot
```

### Current Implementation Status

**Completed:**
- Basic Qt application structure
- Main window with menu bar (File, Window menus)
- TCP communication integration
- OpenGL-based PlotPane with mouse interaction (rotation, zoom)
- Basic 3D axes rendering

**TODO:**
- Plot object rendering (plot2d, plot3d, scatter, etc.)
- Project save/load functionality
- Multiple window/tab support
- GUI elements (buttons, sliders, text fields)
- Settings window
- All plot types from main_application
- Text rendering with FreeType
- Shaders integration
- Color picker
- Legend rendering

### Migration from wxWidgets

Key differences from the wxWidgets implementation:

| Feature | wxWidgets | Qt |
|---------|-----------|-----|
| OpenGL Widget | wxGLCanvas | QOpenGLWidget |
| Timer | wxTimer | QTimer |
| Menu Bar | wxMenuBar | QMenuBar |
| Signals/Slots | wxEvtHandler | Qt signals/slots (MOC) |
| File Dialog | wxFileDialog | QFileDialog |
| Threading | std::thread | std::thread (same) |

### Dependencies

- Qt6 (Core, Widgets, OpenGL)
- OpenGL/GLUT
- C++17
- Existing Duoplot communication layer

### Notes

- The application reuses the DataReceiver and ReceivedData classes from main_application
- OpenGL rendering uses legacy fixed-function pipeline (matches wxWidgets version)
- Future work should consider modern OpenGL with shaders
