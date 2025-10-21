# wxWidgets Usage Analysis - main_application

This document categorizes all files in `src/main_application` based on their wxWidgets dependencies.

## Summary Statistics

- **Total files using wxWidgets**: ~40 files (headers and implementations)
- **GUI-independent files**: 74+ files in subdirectories
- **Core rendering/logic**: ~80% is framework-independent

## Files Using wxWidgets (Need Qt Replacement)

### Core Application Files
- `main.cpp` - Application entry point (wxApp)
- `main_window.h/cpp` - Main application window (wxFrame)
- `main_window_receive.cpp` - Data reception in main window
- `events.h/cpp` - Custom event definitions

### Window Management
- `gui_window.h/cpp` - Individual plot window (wxFrame)
- `gui_tab.h/cpp` - Tab management (wxNotebook)
- `graphic_window.h/cpp` - Graphics window wrapper

### GUI Elements & Controls
- `gui_element.h/cpp` - Base GUI element class
- `gui_elements.h/cpp` - Collection of GUI widgets (buttons, sliders, text boxes)
- `close_button.h/cpp` - Custom close button (wxPanel)
- `tab_button.h/cpp` - Tab button widget
- `tab_buttons.h/cpp` - Tab button manager
- `window_button.h/cpp` - Window control button
- `custom_button.h` - Custom button base class
- `color_picker.cpp` - Color selection widget

### Plotting & Visualization
- `plot_pane.h/cpp` - Main plotting area (wxGLCanvas for OpenGL)
- `background_renderer.h/cpp` - Background rendering (uses wxGLContext)

### Text Output Windows
- `cmdl_output_window.h/cpp` - Command-line output window (wxFrame)
- `topic_text_output_window.h/cpp` - Topic-based text output (wxFrame)

### Utility Windows
- `help_pane.h/cpp` - Help panel (wxPanel)
- `settings_window.h/cpp` - Settings dialog (wxFrame/wxDialog)
- `editing_silhouette.h/cpp` - Editing overlay

### System Integration
- `tray_icon.h/cpp` - System tray icon (wxTaskBarIcon)

## Files NOT Using wxWidgets (Reusable in Qt)

### Communication Layer (✓ Already reused)
```
communication/
├── data_receiver.h/cpp       # TCP server
├── received_data.h/cpp       # Data packet handling
```

### Serial Interface (✓ Framework-independent)
```
serial_interface/
├── serial_port.cpp
├── serial_interface.cpp
├── raw_data_frame.cpp
└── object_types.h
```

### Project State Management (✓ Framework-independent)
```
project_state/
├── project_settings.cpp/h
├── element_settings.cpp/h
├── plot_pane_settings.cpp/h
├── configuration_agent.cpp/h
├── save_manager.h
├── scrolling_text_settings.cpp/h
└── other_gui_settings.cpp/h
```

### Plot Objects (✓ Pure OpenGL/Data)
```
plot_objects/
├── plot_object_base/          # Base classes
├── plot2d/                    # 2D plotting
├── plot3d/                    # 3D plotting
├── fast_plot2d/               # Fast 2D
├── fast_plot3d/               # Fast 3D
├── scatter/                   # 2D scatter
├── scatter3/                  # 3D scatter
├── line_collection2/          # 2D lines
├── line_collection3/          # 3D lines
├── plot_collection2/          # 2D collections
├── plot_collection3/          # 3D collections
├── stairs/                    # Stairs plot
├── stem/                      # Stem plot
├── surf/                      # Surface plot
├── im_show/                   # Image display
├── draw_mesh/                 # Mesh rendering
├── scrolling_plot2d/          # Scrolling plot
├── screen_space_primitive/    # Screen-space rendering
└── stream_object_base/        # Stream objects
```

### Text Stream Objects (✓ Framework-independent)
```
text_stream_objects/
└── text_stream_object_base.cpp/h
```

### Axes System (✓ Pure OpenGL)
```
axes/
├── axes_interactor.h/cpp      # Interaction logic
├── axes_renderer.cpp/h        # Rendering
├── plot_box_walls.cpp/h       # Box walls
├── plot_box_silhouette.cpp/h  # Silhouette
├── plot_box_grid.cpp/h        # Grid
├── plot_pane_background.cpp/h # Background
├── zoom_rect.cpp/h            # Zoom rectangle
├── grid_numbers.cpp/h         # Grid labels
├── text_rendering.cpp/h       # Text (uses FreeType)
├── legend_renderer.cpp/h      # Legend
├── point_selection_box.cpp/h  # Selection box
├── axes_side_configuration.cpp/h
└── structures/                # Data structures
    ├── axes_limits.cpp/h
    ├── axes_settings.cpp/h
    ├── view_angles.cpp/h
    └── grid_vectors.h
```

### OpenGL Utilities (✓ Framework-independent)
```
opengl_low_level/
├── (OpenGL helper functions)
```

### Miscellaneous (✓ Framework-independent)
- `shader.cpp/h` - Shader management
- `opengl_debug.cpp/h` - OpenGL debugging
- `input_data.cpp/h` - Input handling
- `point_selection.cpp/h` - Point selection logic
- `plot_data_handler.cpp/h` - Data management
- `user_supplied_properties.cpp/h` - Property handling
- `platform_paths.cpp/h` - Path utilities
- `globals.cpp/h` - Global state
- `color.h` - Color definitions
- `filesystem.h` - Filesystem utilities
- `constants.h` - Constants
- `misc/number_formatting.cpp/h` - Number formatting
- `misc/rgb_triplet.h` - Color utilities

## Migration Strategy

### Phase 1: Core Application (✓ DONE)
- [x] main.cpp → Qt main with QApplication
- [x] MainWindow → QMainWindow
- [x] Basic menu structure
- [x] TCP communication integration

### Phase 2: Plotting Core (IN PROGRESS)
- [x] PlotPane → QOpenGLWidget
- [ ] Integrate axes system (already wx-independent)
- [ ] Integrate plot objects (already wx-independent)
- [ ] Add shader support

### Phase 3: GUI Elements
- [ ] GuiElement hierarchy → Qt widgets
- [ ] Buttons → QPushButton/custom
- [ ] Text boxes → QLineEdit/QTextEdit
- [ ] Sliders → QSlider
- [ ] Checkboxes → QCheckBox
- [ ] Radio buttons → QRadioButton
- [ ] Color picker → QColorDialog

### Phase 4: Windows & Panels
- [ ] GuiWindow → QWidget/QMainWindow
- [ ] GuiTab → QTabWidget
- [ ] Settings window → QDialog
- [ ] Help pane → QWidget
- [ ] Output windows → QDockWidget or separate windows

### Phase 5: System Integration
- [ ] Tray icon → QSystemTrayIcon
- [ ] File dialogs (already using QFileDialog)
- [ ] Message boxes → QMessageBox

## Key wxWidgets → Qt Mappings

| wxWidgets | Qt Equivalent | Notes |
|-----------|---------------|-------|
| wxApp | QApplication | Application object |
| wxFrame | QMainWindow | Top-level window |
| wxPanel | QWidget | Generic container |
| wxGLCanvas | QOpenGLWidget | OpenGL rendering |
| wxTimer | QTimer | Timer events |
| wxEvtHandler | QObject signals/slots | Event handling |
| wxButton | QPushButton | Push button |
| wxTextCtrl | QLineEdit/QTextEdit | Text input |
| wxNotebook | QTabWidget | Tabbed interface |
| wxMenuBar | QMenuBar | Menu bar |
| wxFileDialog | QFileDialog | File selection |
| wxMessageBox | QMessageBox | Message dialog |
| wxTaskBarIcon | QSystemTrayIcon | System tray |
| wxSizer | QLayout subclasses | Layout management |
| wxPoint | QPoint | Point coordinates |
| wxSize | QSize | Size dimensions |
| wxColour | QColor | Color representation |
| wxString | QString | String class |

## Estimated Effort

- **Reusable without changes**: ~60% (all rendering, data, communication)
- **Need adaptation**: ~30% (GUI elements, windows)
- **Need complete rewrite**: ~10% (custom controls, event handling)

## Notes

1. **Good news**: Most of the heavy lifting (OpenGL rendering, plot objects, data processing) is already framework-independent.

2. **Main work needed**:
   - Translating GUI element hierarchy to Qt widgets
   - Adapting event handling from wxWidgets to Qt signals/slots
   - Reimplementing custom controls

3. **Advantages of Qt**:
   - Better OpenGL integration (QOpenGLWidget)
   - More modern signal/slot system
   - Better layout management
   - More comprehensive widget library
   - Better documentation and tooling

4. **Files already created in qt_application**:
   - main.cpp ✓
   - main_window.h/cpp ✓
   - plot_pane.h/cpp ✓
   - CMakeLists.txt ✓
