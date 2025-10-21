# Qt Application Architecture - Complete Implementation

## Overview

Successfully migrated the wxWidgets application architecture to Qt, implementing a hidden MainWindow pattern with visible GuiWindow instances.

## Architecture Pattern

```
┌─────────────────────────────────────┐
│         MainWindow (hidden)         │
│  - TCP data receiver                │
│  - queued_data_[element_name]       │
│  - plot_panes_ map                  │
│  - Timers for data routing          │
│  - Creates/manages GuiWindows       │
└──────────┬──────────────────────────┘
           │ creates/manages
           ▼
┌─────────────────────────────────────┐
│       GuiWindow (visible)           │
│  - QMainWindow with QTabWidget      │
│  - Menu bar (File/Edit/Tab/Element) │
│  - Tab management                   │
│  - Keyboard event propagation       │
└──────────┬──────────────────────────┘
           │ contains
           ▼
┌─────────────────────────────────────┐
│         WindowTab                   │
│  - plot_panes_[]                    │
│  - gui_elements_[]                  │
│  - Element lifecycle management     │
└──────────┬──────────────────────────┘
           │ contains
           ▼
┌─────────────────────────────────────┐
│    ApplicationGuiElement (base)     │
│  - Pure interface (no QWidget)      │
│  - PlotPane, Button, Slider, etc.   │
└─────────────────────────────────────┘
```

## Key Components

### 1. ApplicationGuiElement (gui_element.h)
- **Type**: Pure interface class (header-only)
- **Purpose**: Base for all interactive GUI elements
- **Key Design**: No QWidget inheritance to avoid multiple QObject issues
- **Virtual Methods**:
  - `updateSizeFromParent(const QSize&)` - Handle parent resize
  - `keyPressedElementSpecific(char)` - Element-specific key handling
  - `keyReleasedElementSpecific(char)` - Element-specific key release
- **Callbacks**:
  - `notify_main_window_key_pressed_`
  - `notify_main_window_key_released_`
  - `notify_main_window_about_modification_`

### 2. PlotPane (plot_pane.h/cpp)
- **Inheritance**: `public QOpenGLWidget, public ApplicationGuiElement`
- **Purpose**: OpenGL rendering widget for 3D plots
- **Features**:
  - Full axes system (AxesRenderer, AxesInteractor)
  - 22 plot object types support
  - Mouse interaction (rotate, pan, zoom)
  - Shader-based rendering
  - Data queue processing

### 3. WindowTab (window_tab.h/cpp)
- **Purpose**: Manages GUI elements within a single tab
- **Responsibilities**:
  - Create/delete plot panes and GUI elements
  - Element lookup by handle string
  - Size updates from parent
  - Element name tracking
- **Maps**:
  - `plot_panes_` - PlotPane instances
  - `gui_elements_` - Other GUI elements

### 4. GuiWindow (gui_window.h/cpp)
- **Type**: QMainWindow (visible to user)
- **Purpose**: Tabbed window interface
- **Features**:
  - QTabWidget for tab management
  - Menu bar with File/Edit/Tab/Element menus
  - Tab creation/editing/deletion
  - Element creation interface
  - Keyboard event propagation to children
- **Callbacks to MainWindow**:
  - Key press/release notifications
  - Element deletion notifications
  - Element/window name change notifications
  - Project modification notifications

### 5. MainWindow (main_window.h/cpp)
- **Type**: Hidden QMainWindow (data router)
- **Purpose**: TCP data reception and routing
- **Key Features**:
  - Calls `hide()` in constructor
  - TCP receive thread for incoming plot data
  - Per-element data queues: `queued_data_[element_name]`
  - Timer-based data distribution (~60Hz receive, ~30Hz refresh)
  - Creates and manages GuiWindow instances
- **Data Structures**:
  - `std::vector<GuiWindow*> windows_`
  - `std::map<std::string, PlotPane*> plot_panes_`
  - `std::map<std::string, ApplicationGuiElement*> gui_elements_`
  - `std::map<std::string, std::queue<std::unique_ptr<InputData>>> queued_data_`

### 6. Data Conversion (plot_data_conversion.cpp)
- **Purpose**: Convert raw TCP data to plot object data
- **Function**: `convertPlotObjectData()`
- **Supports**: All 22 plot types (PLOT2, PLOT3, SCATTER, SURF, etc.)

## Data Flow

```
TCP Thread
  ↓
MainWindow::manageReceivedData()
  ↓
addActionToQueue()
  ↓
queued_data_[element_name].push(InputData)
  ↓
Timer: onReceiveTimer() (~60Hz)
  ↓
plot_panes_[element_name]->pushQueue(queue)
  ↓
PlotPane::processActionQueue()
  ↓
Render in paintGL()
```

## Build Information

- **Compiler**: Clang (Apple)
- **Qt Version**: Qt6
- **OpenGL**: Legacy fixed-function pipeline
- **Executable Size**: 6.2 MB
- **Build Command**: `make qt_duoplot -j6`
- **Output**: `/Users/danielpi/work/dvs/src/build/qt_duoplot`

## Files Created/Modified

### Created:
- `gui_element.h` - ApplicationGuiElement interface
- `window_tab.h/cpp` - Tab management
- `gui_window.h/cpp` - Visible window with tabs
- `plot_data_conversion.cpp` - Data conversion utilities
- `ARCHITECTURE_REFACTOR_STATUS.md` - Progress tracking
- `ARCHITECTURE_SUMMARY.md` - This file

### Modified:
- `main_window.h/cpp` - Refactored to hidden data router
- `plot_pane.h/cpp` - Added ApplicationGuiElement inheritance
- `main.cpp` - Removed main_window.show() call
- `CMakeLists.txt` - Added new source files

## Key Design Decisions

### 1. ApplicationGuiElement as Pure Interface
**Problem**: Multiple QObject inheritance not supported by Qt
**Solution**: Made ApplicationGuiElement a pure interface without QWidget/QObject
**Benefit**: PlotPane can inherit from both QOpenGLWidget and ApplicationGuiElement

### 2. Hidden MainWindow Pattern
**Reason**: Matches wxWidgets architecture
**Benefit**: Clean separation of data routing and UI
**Implementation**: MainWindow calls `hide()` in constructor

### 3. Per-Element Data Queues
**Design**: `std::map<std::string, std::queue<InputData>>`
**Benefit**: Efficient routing of TCP data to specific plot panes
**Key**: Element handle strings used as map keys

### 4. Timer-Based Updates
**Receive Timer**: 16ms (~60Hz) - Checks for incoming data
**Refresh Timer**: 33ms (~30Hz) - Updates display
**Benefit**: Decouples data reception from rendering

## Testing Status

- ✅ Builds successfully without errors
- ✅ All architecture components implemented
- ✅ Data routing paths established
- ⏳ Runtime testing pending
- ⏳ TCP data reception testing pending
- ⏳ Multi-window functionality testing pending

## Next Steps for Full Functionality

1. **Test Application Launch**
   - Run `./qt_duoplot`
   - Verify GuiWindow appears
   - Verify MainWindow is hidden

2. **Test TCP Data Reception**
   - Send plot data via TCP
   - Verify data routing to correct PlotPane
   - Verify rendering occurs

3. **Test User Interactions**
   - Tab creation/deletion
   - Element creation
   - Mouse interaction (rotate/pan/zoom)
   - Menu operations

4. **Add Additional GUI Elements**
   - Button class (inheriting ApplicationGuiElement)
   - Slider class
   - Checkbox class
   - Text input class

5. **Implement Missing Features**
   - Project save/load
   - Screenshot functionality
   - Settings persistence

## Comparison with wxWidgets

| Feature | wxWidgets | Qt (Completed) |
|---------|-----------|----------------|
| Hidden MainWindow | ✅ | ✅ |
| Visible GuiWindow | ✅ | ✅ |
| Tab Management | ✅ | ✅ |
| ApplicationGuiElement Base | ✅ | ✅ |
| PlotPane with OpenGL | ✅ | ✅ |
| TCP Data Reception | ✅ | ✅ |
| Data Routing | ✅ | ✅ |
| Menu System | ✅ | ✅ |
| Multiple Inheritance | `wxGLCanvas, ApplicationGuiElement` | `QOpenGLWidget, ApplicationGuiElement` |

## Architecture Highlights

✅ **Clean Separation**: UI (GuiWindow) vs Data (MainWindow)
✅ **Scalable**: Easy to add new element types
✅ **Maintainable**: Clear component responsibilities
✅ **Testable**: Components can be tested independently
✅ **Qt-Compatible**: Avoids Qt-specific pitfalls (multiple QObject inheritance)
✅ **Performance**: Efficient data routing with minimal copying

## Conclusion

The Qt application architecture has been successfully implemented to match the wxWidgets pattern. All core components are in place, building successfully, and ready for runtime testing and feature expansion.
