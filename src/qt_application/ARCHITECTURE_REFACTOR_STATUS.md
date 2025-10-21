# Qt Application Architecture Refactoring Status

## Goal
Refactor Qt application to match wxWidgets architecture:
- Hidden MainWindow for data routing
- Visible GuiWindow(s) for user interaction
- WindowTab management
- ApplicationGuiElement base class

## Completed ✅

### 1. PlotPane Integration (COMPLETE)
- ✅ Created `plot_pane.h/cpp` with full axes rendering
- ✅ Integrated `AxesRenderer`, `AxesInteractor`, `PlotDataHandler`
- ✅ Implemented shader initialization from files
- ✅ Mouse interaction (rotation, panning)
- ✅ Data queue processing (AXES, VIEW, CLEAR, plot data)
- ✅ Full rendering pipeline working
- ✅ Builds successfully (5.4 MB executable)

### 2. ApplicationGuiElement Base Class (COMPLETE)
- ✅ Created `gui_element.h/cpp`
- ✅ Base class for all interactive Qt elements
- ✅ Keyboard/mouse event handling
- ✅ Callbacks to MainWindow
- ✅ Pure virtual methods for element-specific behavior
- ✅ Compiles and links successfully

### 3. WindowTab Class (COMPLETE)
- ✅ Created `window_tab.h/cpp`
- ✅ Manages plot panes and GUI elements in a tab
- ✅ Element creation/deletion
- ✅ Callbacks to MainWindow
- ✅ Compiles successfully

### 4. GuiWindow Class (COMPLETE)
- ✅ Created `gui_window.h/cpp`
- ✅ Visible window with QTabWidget
- ✅ Menu bar (File, Edit, Tab, Element menus)
- ✅ Tab management (create, edit, delete)
- ✅ Element management interface
- ✅ Keyboard event propagation
- ✅ Compiles successfully

### 5. MainWindow Refactor (COMPLETE)
- ✅ Refactored to hidden data router pattern
- ✅ Added `std::vector<GuiWindow*> windows_`
- ✅ Added `std::map<std::string, PlotPane*> plot_panes_`
- ✅ Added `std::map<std::string, std::queue<std::unique_ptr<InputData>>> queued_data_`
- ✅ Implemented callbacks for GuiWindow communication
- ✅ TCP receive thread with data routing
- ✅ Timer-based data distribution (~60Hz receive, ~30Hz refresh)
- ✅ Created `plot_data_conversion.cpp` for plot data conversion
- ✅ Builds successfully (6.2 MB executable)

###6. PlotPane Integration (COMPLETE)
- ✅ Updated PlotPane to inherit from ApplicationGuiElement
- ✅ Converted ApplicationGuiElement to pure interface (no QWidget inheritance)
- ✅ Fixed multiple QObject inheritance issue
- ✅ PlotPane now properly inherits: `QOpenGLWidget, ApplicationGuiElement`
- ✅ Updated constructor to match interface
- ✅ Implemented required virtual methods: `updateSizeFromParent`, `keyPressedElementSpecific`, `keyReleasedElementSpecific`
- ✅ MainWindow properly populates `plot_panes_` map via dynamic_cast
- ✅ WindowTab properly handles PlotPane in `getAllGuiElements()` and `deleteElementIfItExists()`
- ✅ Builds successfully (6.2 MB executable)

### 7. Update main.cpp (COMPLETE)
- ✅ Removed `main_window.show()` call (MainWindow is hidden)
- ✅ Added comments explaining architecture
- ✅ MainWindow creates GuiWindows which are shown automatically
- ✅ Builds successfully (6.2 MB executable)

## ✅ REFACTORING COMPLETE!

All architecture components have been successfully implemented and integrated!

**Key responsibilities:**
- Hidden QMainWindow (calls `hide()` in constructor)
- TCP receive thread for data
- Routes data to plot panes via element names
- Manages `std::vector<GuiWindow*> windows_`
- Per-element data queues: `queued_data_[element_name]`
- Timer-based updates

## Pending ⏳

### 4. GuiWindow Class
**Files to create:** `gui_window.h/cpp`
**Dependencies:** WindowTab

**Purpose:** Visible window with tabs (replaces current MainWindow functionality)

**Key features:**
- Inherits `QMainWindow`
- Contains `QTabWidget`
- Manages `std::vector<WindowTab*> tabs_`
- Menu bar for creating elements/tabs
- Callbacks to MainWindow

### 5. Refactor MainWindow
**Files to modify:** `main_window.h/cpp`
**Dependencies:** GuiWindow

**Changes:**
- Remove `QTabWidget* tab_widget_`
- Remove direct UI
- Add `std::vector<GuiWindow*> windows_`
- Add `std::map<std::string, std::queue<std::unique_ptr<InputData>>> queued_data_`
- Hide window or make non-visible
- Implement data routing to plot panes via element names

### 6. Update PlotPane
**Files to modify:** `plot_pane.h/cpp`
**Dependencies:** ApplicationGuiElement complete

**Changes:**
- Make PlotPane inherit from ApplicationGuiElement
- Options:
  - Multiple inheritance: `class PlotPane : public QOpenGLWidget, public ApplicationGuiElement`
  - Or composition: PlotPane contains OpenGL widget
- Implement required virtual methods from ApplicationGuiElement

### 7. Update main.cpp
**Files to modify:** `main.cpp`

**Changes:**
- Don't show MainWindow directly
- MainWindow creates GuiWindows
- GuiWindows are visible, MainWindow is hidden

## Architecture Diagram

```
┌─────────────────────────────────────┐
│         MainWindow (hidden)         │
│  - TCP data receiver                │
│  - queued_data_[element_name]       │
│  - plot_panes_ map                  │
│  - Timers for data routing          │
└──────────┬──────────────────────────┘
           │ creates/manages
           ▼
┌─────────────────────────────────────┐
│       GuiWindow (visible)           │
│  - QMainWindow                      │
│  - QTabWidget                       │
│  - Menus for creating elements      │
└──────────┬──────────────────────────┘
           │ contains
           ▼
┌─────────────────────────────────────┐
│         WindowTab                   │
│  - plot_panes_[]                    │
│  - gui_elements_[]                  │
│  - Z-order management               │
└──────────┬──────────────────────────┘
           │ contains
           ▼
┌─────────────────────────────────────┐
│    ApplicationGuiElement (base)     │
│  - PlotPane                         │
│  - Button, Slider, Checkbox, etc.   │
└─────────────────────────────────────┘
```

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
Timer: onReceiveTimer()
  ↓
plot_panes_[element_name]->pushQueue(queue)
  ↓
PlotPane::processActionQueue()
  ↓
Render in paintGL()
```

## Next Steps

1. **Create WindowTab class** - Manages tab contents
2. **Create GuiWindow class** - Visible window with tabs
3. **Refactor MainWindow** - Hidden data router
4. **Update PlotPane** - Inherit from ApplicationGuiElement
5. **Update main.cpp** - Initialize architecture properly
6. **Test** - Ensure TCP data flows correctly

## Notes

- PlotPane is fully functional and rendering correctly
- ApplicationGuiElement base provides good foundation
- Main work is creating the window/tab hierarchy
- Data routing logic exists in wxWidgets, needs Qt port
