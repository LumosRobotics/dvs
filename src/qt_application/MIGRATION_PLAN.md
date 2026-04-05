# wxWidgets → Qt6 Migration Plan

**Source:** `src/main_application/` (wxWidgets, reference implementation)  
**Target:** `src/qt_application/` (Qt6, in-progress)  
**Estimated completion:** ~30–35% of feature parity reached

---

## Executive Summary

The core rendering pipeline (OpenGL via `QOpenGLWidget`, axes system, plot objects, TCP data
receiving) is working. The majority of remaining work is **UI shell completeness**: the full suite
of interactive GUI elements, window management features, project persistence, and output windows
that wxWidgets provided "for free" via its widget toolkit.

---

## Status Overview

| Area | wx lines | Qt lines | Status |
|------|----------|----------|--------|
| Main window / data router | ~1125 | ~462 | Partial — missing menus, serial, project I/O |
| GUI window (top-level frame) | ~1286 | ~432 | Partial — menus stub; element creation incomplete |
| Plot pane (OpenGL canvas) | ~973 | ~458 | Working for plot data; some callbacks missing |
| GUI element base class | ~220 + 17 KB state | ~66 | Stub — no cursor states, no serialization |
| Window tab / tab management | complex | simple | Partial — only plot panes created |
| All non-plot GUI elements | 10 files, ~2000 lines | 0 lines | Not started |
| Text output windows | 2 files | 0 files | Not started |
| Settings dialog | ~300 lines | 0 lines | Not started |
| Tray icon | ~200 lines | 0 lines | Not started |
| Project save / load | ~400 lines | 0 lines | Not started |
| Serial port UI integration | ~300 lines | plumbing only | Not started |

---

## Architectural Differences to Preserve

These design decisions from the Qt migration are **correct** and should be kept:

| Concern | wxWidgets approach | Qt approach (keep this) |
|---------|-------------------|-------------------------|
| OpenGL canvas | `wxGLCanvas` + manual `wxGLContext` | `QOpenGLWidget` — automatic context, `initializeGL / paintGL` |
| Event dispatch | `wxTimer` + `Bind(wxEVT_*)` | `QTimer::timeout` signal + override `mousePressEvent` etc. |
| Platform GL init | Guarded `#ifdef PLATFORM_APPLE_M` in `getContext()` | Qt handles internally |
| Config persistence | Custom `ConfigurationAgent` | `SettingsHandler` from LumosConfig |
| Main window visibility | Visible, owns menus | Hidden, acts as data router — GuiWindows own their menus |

---

## Phase 1 — Complete the Existing Skeleton (Blocking)

These gaps prevent basic end-to-end usage.

### 1.1 `plot_pane.cpp` — Missing callbacks

**File:** `src/qt_application/plot_pane.cpp`

The Qt `PlotPane` constructor takes fewer callbacks than the wx version. The missing ones need to be
wired up:

- **`on_create_tab_for_element`** — called when an element asks to open in a new tab; wire to
  `GuiWindow`.
- **`on_editing_silhouette`** — visual feedback while an element is being resized/moved; the Qt
  version should emit a signal or call a slot on `GuiWindow` to paint an overlay.
- **`on_text_output`** — routes text from the element to the scrolling output window. Wire to the
  `TopicTextOutputWindow` equivalent once that exists (Phase 3). For now, forward to `qDebug()`.

**Reference:** `src/main_application/plot_pane.h` constructor parameter list, lines 30–55.

### 1.2 `main_window.cpp` — Data receive path incomplete

**File:** `src/qt_application/main_window.cpp`

The wx version has `main_window_receive.cpp` (~400 lines) that handles dispatching incoming TCP
packets to the right `GuiWindow` / `WindowTab` / element. The Qt version has a `receiveData()`
method but the dispatch logic is incomplete.

- Port `src/main_application/main_window_receive.cpp` into `MainWindow::receiveData()`.
- All `Function::*` cases need to route to the correct `PlotPane` / GUI element by element name.
- Ensure the data queue per-element (`queued_data_`) is drained correctly by the timer tick.

### 1.3 Serial port UI integration

**File:** `src/qt_application/main_window.cpp` / new `main_window_serial.cpp`

The wx version's `main_window_serial.cpp` (~300 lines) handles:
- Discovering available serial ports
- Opening / closing a port on user action
- Feeding received bytes into the same deserialization pipeline as TCP

Port the logic; the `SerialInterface` class itself (`serial_interface/`) is already identical in
both targets. The UI surface needed is just a menu item or toolbar button to open a port picker.

---

## Phase 2 — GUI Elements Suite

The wx version has a complete widget toolkit extension (`gui_elements.h/cpp`). None of this exists
in the Qt version. Each element type maps to a standard Qt widget:

| wx element | Qt equivalent | Notes |
|-----------|---------------|-------|
| `ButtonGuiElement` | `QPushButton` | — |
| `SliderGuiElement` | `QSlider` + `QLabel` | Add current-value label |
| `CheckboxGuiElement` | `QCheckBox` | — |
| `TextLabelGuiElement` | `QLabel` | Static display |
| `EditableTextGuiElement` | `QLineEdit` | Emits text on Enter |
| `DropdownMenuGuiElement` | `QComboBox` | — |
| `RadioButtonGroupGuiElement` | `QButtonGroup` + `QRadioButton`s | — |
| `ListBoxGuiElement` | `QListWidget` | — |
| `ScrollingTextGuiElement` | `QPlainTextEdit` (read-only) | Append-only, auto-scroll |

### 2.1 `ApplicationGuiElement` base class

**File:** `src/qt_application/gui_element.h`

The current stub has only 3 callbacks. The wx version (`gui_element_state.h`, 17 KB) has a full
cursor-state machine and GUI payload serialization used to save/restore element positions and sizes.

Minimum additions needed:
- `getBoundingRect() / setBoundingRect()` — position and size in the tab coordinate system.
- `serialize() / deserialize()` — for project save/load (Phase 4).
- Cursor resize states (`CursorSquareState` enum, 10 states) — enables drag-to-resize. The enum
  and `updateCursorState(QPoint)` method are the main piece; actual painting happens in `GuiWindow`.

### 2.2 `WindowTab` — create all element types

**File:** `src/qt_application/window_tab.cpp`

Currently only `createNewPlotPane()` is implemented. Add:

```cpp
ApplicationGuiElement* createNewButton(const std::string& element_name,
                                       const ElementSettings& settings);
ApplicationGuiElement* createNewSlider(...);
ApplicationGuiElement* createNewCheckbox(...);
ApplicationGuiElement* createNewTextLabel(...);
ApplicationGuiElement* createNewEditableText(...);
ApplicationGuiElement* createNewDropdown(...);
ApplicationGuiElement* createNewRadioButtonGroup(...);
ApplicationGuiElement* createNewListBox(...);
ApplicationGuiElement* createNewScrollingText(...);
```

Each method creates a `Q*` widget, wraps it in the appropriate `ApplicationGuiElement` subclass,
adds it to the tab's layout, and registers it in the element map.

### 2.3 Z-order management

The wx version uses `ZOrderQueue` (in `gui_tab.h`) to track paint order of overlapping elements.
In Qt, `QWidget::raise()` / `QWidget::lower()` plus `QWidget::stackUnder()` provide the same
capability. Implement `raiseElement(name)` / `lowerElement(name)` in `WindowTab`.

---

## Phase 3 — UI Shell Features

### 3.1 Tray icon

**Reference:** `src/main_application/tray_icon.h/cpp`

Qt's `QSystemTrayIcon` is simpler than the wx equivalent:

```cpp
QSystemTrayIcon* tray = new QSystemTrayIcon(QIcon(":/icons/app.png"), this);
QMenu* menu = new QMenu();
menu->addAction("Show", this, &MainWindow::showMainWindow);
menu->addAction("Quit", qApp, &QApplication::quit);
tray->setContextMenu(menu);
tray->show();
```

Connect `QSystemTrayIcon::activated` to restore a window on double-click. The wx version also
stores the list of active `GuiWindow` names in the tray menu — replicate this by connecting
`GuiWindow` creation/destruction signals to a `rebuildTrayMenu()` slot.

### 3.2 Text output windows

**Reference:** `src/main_application/cmdl_output_window.h/cpp`,
`src/main_application/topic_text_output_window.h/cpp`

Two output windows:
- **Command-line output** (`CmdlOutputWindow`) — general log; maps to `QDockWidget` containing
  `QPlainTextEdit`.
- **Topic text output** (`TopicTextOutputWindow`) — per-element scrolling text streams; maps to a
  `QTabWidget` of `QPlainTextEdit`s, one tab per topic.

Both should live in `GuiWindow` as dockable panels toggled from the View menu.

### 3.3 Settings dialog

**Reference:** `src/main_application/settings_window.h/cpp`

Port to a `QDialog` with a `QFormLayout`. The wx version stores settings via `ConfigurationAgent`;
the Qt equivalent should use `SettingsHandler` (already integrated).

Key settings exposed: network port, font path, default window size, rendering options.

### 3.4 Help pane

**Reference:** `src/main_application/help_pane.h/cpp`

Port to a `QDockWidget` containing a `QTextBrowser` loaded from a resource file or HTML string.

---

## Phase 4 — Project Save / Load

**Reference:** `src/main_application/project_state/save_manager.h` and
`configuration_agent.h/cpp`

The wx version persists the entire GUI layout (window positions, element positions, axes limits,
colors) to an XML/JSON file via `SaveManager`. The Qt version has no save/load.

Steps:
1. Add `serialize()` / `deserialize()` to `ApplicationGuiElement` (prerequisite: Phase 2.1).
2. Implement `GuiWindow::saveLayout(QJsonObject&)` / `loadLayout(const QJsonObject&)`.
3. Implement `MainWindow::saveProject(const QString& path)` / `loadProject(const QString& path)`.
4. Wire to `QFileDialog::getSaveFileName` / `QFileDialog::getOpenFileName` in the File menu.

Use `QJsonDocument` for the file format (simpler than the wx XML approach).

---

## Phase 5 — Edit Mode & Visual Feedback

### 5.1 Editing silhouette

**Reference:** `src/main_application/editing_silhouette.h/cpp`

When the user holds the edit modifier key and hovers over an element, a resize/move overlay is
drawn on top. In Qt, implement this as a transparent `QWidget` overlay installed on the tab:

```cpp
class EditingOverlay : public QWidget {
    // set WA_TransparentForMouseEvents when not in edit mode
    // paintEvent() draws the silhouette rectangle + resize handles
};
```

Install the overlay as a child of `WindowTab` and call `raise()` so it paints on top.

### 5.2 Cursor state machine

**Reference:** `src/main_application/gui_element_state.h`

The 10-state cursor machine (`CursorSquareState`) maps screen position to resize/move intent. Port
`updateCursorState(QPoint pos)` to `ApplicationGuiElement`; call it from `GuiWindow::mouseMoveEvent`
when in edit mode. Use `QWidget::setCursor(Qt::SizeVerCursor)` etc. to set the appropriate cursor.

---

## Phase 6 — Polish & Remaining Gaps

- **`background_renderer.h/cpp`** — background gradient/pattern; port to `QPainter` calls in
  `GuiWindow::paintEvent` or as a `QOpenGLWidget` overlay.
- **`graphic_window.h/cpp`** — screenshot export; use `QOpenGLWidget::grabFramebuffer()` which
  returns a `QImage`, then `QImage::save()`.
- **Custom tab/close buttons** (`close_button.h`, `tab_button.h`, `tab_buttons.h`) — the wx version
  had hand-drawn buttons. Qt's `QTabBar` supports custom `QStyle` or `tabButton()` to add close
  buttons. Use `QTabWidget::setTabsClosable(true)` and connect `tabCloseRequested`.
- **`events.h/cpp`** — the wx custom event system for GUI-element-to-main-window communication.
  In Qt this becomes signals/slots; audit every `wxCommandEvent` usage and replace with an
  appropriate signal.

---

## Files to Create (Qt versions)

| New file | Purpose | Based on |
|----------|---------|----------|
| `gui_elements.h/cpp` | All non-plot element types | `main_application/gui_elements.h/cpp` |
| `tray_icon.h/cpp` | System tray + window menu | `main_application/tray_icon.h/cpp` |
| `settings_window.h/cpp` | Preferences dialog | `main_application/settings_window.h/cpp` |
| `help_pane.h/cpp` | Help dock widget | `main_application/help_pane.h/cpp` |
| `cmdl_output_window.h/cpp` | General log output | `main_application/cmdl_output_window.h/cpp` |
| `topic_text_output_window.h/cpp` | Per-topic text streams | `main_application/topic_text_output_window.h/cpp` |
| `editing_overlay.h/cpp` | Edit-mode resize overlay | `main_application/editing_silhouette.h/cpp` |
| `main_window_receive.cpp` | TCP packet dispatch | `main_application/main_window_receive.cpp` |
| `main_window_serial.cpp` | Serial UI integration | `main_application/main_window_serial.cpp` |

---

## Files to Significantly Update

| File | Missing additions |
|------|-----------------|
| `gui_element.h` | Bounding rect, cursor state enum, serialize interface |
| `window_tab.cpp` | All element creation methods, z-order helpers |
| `gui_window.cpp` | All element creation dispatch, context menus, output window hosting |
| `main_window.cpp` | Complete receiveData() dispatch (port main_window_receive.cpp logic) |
| `plot_pane.cpp` | Missing constructor callbacks wired up |
| `CMakeLists.txt` | Add all new source files above |

---

## What Is Already Working and Should Not Be Changed

- **Axes rendering** (`axes/axes_renderer`, `axes/grid_numbers`, `axes/legend_renderer`, etc.) —
  identical in both targets; the recent HarfBuzz text renderer upgrade applies to both.
- **All plot object types** (`plot_objects/`) — no differences; leave alone.
- **Communication layer** (`communication/`, `serial_interface/`) — identical; leave alone.
- **Project state serialization** (`project_state/`) — identical; leave alone.
- **OpenGL infrastructure** (`opengl_low_level/`, `shader.cpp`) — identical.
- **Data deserialization** (`input_data.cpp`, `plot_data_handler.cpp`) — identical.
- **`QOpenGLWidget` rendering loop** in `plot_pane.cpp` — working; do not revert to wx patterns.
- **Hidden `MainWindow` pattern** — correct Qt architecture; keep.
