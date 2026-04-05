#include "gui_window.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QCursor>
#include <QDateTime>
#include <QDebug>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPalette>
#include <QScreen>
#include <QSerialPortInfo>
#include <QSizePolicy>
#include <QSpacerItem>

#include "lumos/logging/logging.h"
#include "settings_window.h"

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

GuiWindow::GuiWindow(
    QWidget* parent,
    const WindowSettings& window_settings,
    const QString& project_name,
    const int callback_id,
    const bool project_is_saved,
    const std::function<void(const char key)>& notify_main_window_key_pressed,
    const std::function<void(const char key)>& notify_main_window_key_released,
    const std::function<std::vector<std::string>(void)>& get_all_element_names,
    const std::function<void(const std::string&)>& notify_main_window_element_deleted,
    const std::function<void(const std::string&, const std::string&)>& notify_main_window_element_name_changed,
    const std::function<void(const std::string&, const std::string&)>& notify_main_window_name_changed,
    const std::function<void()>& notify_main_window_about_modification)
    : QMainWindow(parent),
      callback_id_(callback_id),
      name_(window_settings.name),
      project_name_(project_name.toStdString()),
      project_is_saved_(project_is_saved),
      notify_main_window_key_pressed_(notify_main_window_key_pressed),
      notify_main_window_key_released_(notify_main_window_key_released),
      notify_main_window_element_deleted_(notify_main_window_element_deleted),
      get_all_element_names_(get_all_element_names),
      notify_main_window_element_name_changed_(notify_main_window_element_name_changed),
      notify_main_window_name_changed_(notify_main_window_name_changed),
      notify_main_window_about_modification_(notify_main_window_about_modification),
      current_tab_num_(0)
{
    setWindowTitle(QString::fromStdString(name_));
    setGeometry(window_settings.x, window_settings.y, window_settings.width, window_settings.height);

    // ---- Build the central widget layout ----
    // [ tab_button_panel (70px, hidden when 1 tab) | content_area (fills rest) ]
    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* h_layout = new QHBoxLayout(central);
    h_layout->setContentsMargins(0, 0, 0, 0);
    h_layout->setSpacing(0);

    // Left strip: custom tab buttons
    tab_button_panel_ = new QWidget(central);
    tab_button_panel_->setFixedWidth(70);
    tab_button_panel_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    tab_button_layout_ = new QVBoxLayout(tab_button_panel_);
    tab_button_layout_->setContentsMargins(2, 4, 2, 4);
    tab_button_layout_->setSpacing(2);
    tab_button_layout_->addStretch();  // buttons will be inserted before this stretch
    tab_button_panel_->hide();  // hidden until there are 2+ tabs

    h_layout->addWidget(tab_button_panel_);

    // Right area: parent for all WindowTab element widgets
    content_area_ = new QWidget(central);
    content_area_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    h_layout->addWidget(content_area_);

    // ---- Output & help windows ----
    cmdl_output_window_  = new CmdlOutputWindow(nullptr);
    topic_output_window_ = new TopicTextOutputWindow(nullptr);
    help_pane_           = new HelpPane(nullptr);

    // ---- Menu bar + popup menus ----
    setupMenuBar();
    setupPopupMenus();

    // ---- Text output lambda ----
    const std::string window_name_capture = name_;
    auto on_text_output = [this, window_name_capture](const std::string& text) {
        topic_output_window_->pushNewText(window_name_capture, Color_t::BLACK, text);
        topic_output_window_->show();
    };

    // ---- Right-click lambda (passed to each WindowTab) ----
    auto notify_parent_right_click = [this](const QPoint& pos, const std::string& elem_name) {
        last_clicked_item_ = elem_name;
        popup_menu_element_->exec(mapToGlobal(pos));
    };

    // ---- Create tabs from settings ----
    for (const TabSettings& tab_settings : window_settings.tabs)
    {
        WindowTab* tab = new WindowTab(
            content_area_,
            tab_settings,
            0,  // no x offset needed — content_area is already past the tab strip
            notify_main_window_key_pressed_,
            notify_main_window_key_released_,
            notify_parent_right_click,
            notify_main_window_element_deleted_,
            notify_main_window_about_modification_,
            on_text_output);

        tabs_.push_back(tab);

        // Add a tab row (label button + × close button)
        QWidget* row = new QWidget(tab_button_panel_);
        QHBoxLayout* row_layout = new QHBoxLayout(row);
        row_layout->setContentsMargins(0, 0, 0, 0);
        row_layout->setSpacing(2);

        QPushButton* btn = new QPushButton(QString::fromStdString(tab_settings.name), row);
        btn->setCheckable(true);
        const int idx = static_cast<int>(tab_buttons_qt_.size());
        connect(btn, &QPushButton::clicked, this, [this, idx]() { switchToTab(idx); });

        QPushButton* close_btn = new QPushButton("×", row);
        close_btn->setFixedSize(16, 16);
        close_btn->setToolTip(tr("Close tab"));
        connect(close_btn, &QPushButton::clicked, this, [this, idx]() {
            switchToTab(idx);
            onDeleteTab();
        });

        row_layout->addWidget(btn);
        row_layout->addWidget(close_btn);

        // Insert before the trailing stretch
        tab_button_layout_->insertWidget(tab_button_layout_->count() - 1, row);
        tab_buttons_qt_.push_back(btn);
        tab_row_widgets_.push_back(row);
    }

    // Show first tab, hide the rest
    for (int i = 0; i < static_cast<int>(tabs_.size()); ++i)
    {
        if (i == 0)
            tabs_[i]->show();
        else
            tabs_[i]->hide();
    }

    // Show tab button panel only when there are 2+ tabs
    updateTabButtonPanel();

    // Apply initial tab background color
    if (!tabs_.empty())
    {
        const RGBTripletf bg = tabs_[0]->getBackgroundColor();
        QPalette pal = content_area_->palette();
        pal.setColor(QPalette::Window, QColor::fromRgbF(bg.red, bg.green, bg.blue));
        content_area_->setAutoFillBackground(true);
        content_area_->setPalette(pal);
    }

    // Editing overlay (Phase 5)
    editing_overlay_ = new EditingOverlay(content_area_);
    installEditFiltersOnAllElements();

    LUMOS_LOG_INFO() << "GuiWindow '" << name_ << "' created with " << tabs_.size() << " tabs";
}

GuiWindow::~GuiWindow()
{
    deleteAllTabs();
    delete cmdl_output_window_;
    delete topic_output_window_;
    delete help_pane_;
    LUMOS_LOG_INFO() << "GuiWindow '" << name_ << "' destroyed";
}

void GuiWindow::deleteAllTabs()
{
    for (auto* tab : tabs_)
        delete tab;
    tabs_.clear();
}

// ---------------------------------------------------------------------------
// Tab management helpers
// ---------------------------------------------------------------------------

void GuiWindow::updateTabButtonPanel()
{
    if (tabs_.size() <= 1)
    {
        tab_button_panel_->hide();
    }
    else
    {
        tab_button_panel_->show();
        for (int i = 0; i < static_cast<int>(tab_buttons_qt_.size()); ++i)
            tab_buttons_qt_[i]->setChecked(i == current_tab_num_);
    }
}

void GuiWindow::switchToTab(int index)
{
    if (index < 0 || index >= static_cast<int>(tabs_.size()))
        return;

    // Hide current tab's widgets
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        tabs_[current_tab_num_]->hide();

    current_tab_num_ = index;
    tabs_[current_tab_num_]->show();
    tabs_[current_tab_num_]->updateSizeFromParent(content_area_->size());

    // Apply tab background color to content area
    const RGBTripletf bg = tabs_[current_tab_num_]->getBackgroundColor();
    QPalette pal = content_area_->palette();
    pal.setColor(QPalette::Window, QColor::fromRgbF(bg.red, bg.green, bg.blue));
    content_area_->setAutoFillBackground(true);
    content_area_->setPalette(pal);

    // Update button highlight
    for (int i = 0; i < static_cast<int>(tab_buttons_qt_.size()); ++i)
        tab_buttons_qt_[i]->setChecked(i == current_tab_num_);

    LUMOS_LOG_DEBUG() << "Switched to tab " << tabs_[current_tab_num_]->getName();
}

// ---------------------------------------------------------------------------
// Edit-mode helpers
// ---------------------------------------------------------------------------

void GuiWindow::installEditFilter(ApplicationGuiElement* elem)
{
    if (auto* w = dynamic_cast<QWidget*>(elem))
    {
        w->setMouseTracking(true);
        w->installEventFilter(this);
    }
}

void GuiWindow::installEditFiltersOnAllElements()
{
    for (auto* elem : getAllGuiElements())
        installEditFilter(elem);
}

// Maps a mouse position (in element-local coordinates) to a cursor resize state.
// The 10-pixel margin around edges defines the resize zones.
CursorSquareState GuiWindow::computeCursorState(const QRect& r, const QPoint& pos)
{
    constexpr int kMargin = 10;
    const bool near_left   = pos.x() <= kMargin;
    const bool near_right  = pos.x() >= r.width()  - kMargin;
    const bool near_top    = pos.y() <= kMargin;
    const bool near_bottom = pos.y() >= r.height() - kMargin;

    if (near_top    && near_left)  return CursorSquareState::RESIZE_UPPER_LEFT;
    if (near_top    && near_right) return CursorSquareState::RESIZE_UPPER_RIGHT;
    if (near_bottom && near_left)  return CursorSquareState::RESIZE_LOWER_LEFT;
    if (near_bottom && near_right) return CursorSquareState::RESIZE_LOWER_RIGHT;
    if (near_left)                 return CursorSquareState::RESIZE_CENTER_LEFT;
    if (near_right)                return CursorSquareState::RESIZE_CENTER_RIGHT;
    if (near_top)                  return CursorSquareState::RESIZE_UPPER_CENTER;
    if (near_bottom)               return CursorSquareState::RESIZE_LOWER_CENTER;
    return CursorSquareState::MOVE;
}

static Qt::CursorShape cursorForState(CursorSquareState s)
{
    switch (s)
    {
        case CursorSquareState::RESIZE_UPPER_LEFT:
        case CursorSquareState::RESIZE_LOWER_RIGHT: return Qt::SizeFDiagCursor;
        case CursorSquareState::RESIZE_UPPER_RIGHT:
        case CursorSquareState::RESIZE_LOWER_LEFT:  return Qt::SizeBDiagCursor;
        case CursorSquareState::RESIZE_CENTER_LEFT:
        case CursorSquareState::RESIZE_CENTER_RIGHT:return Qt::SizeHorCursor;
        case CursorSquareState::RESIZE_UPPER_CENTER:
        case CursorSquareState::RESIZE_LOWER_CENTER:return Qt::SizeVerCursor;
        case CursorSquareState::MOVE:               return Qt::SizeAllCursor;
        default:                                    return Qt::ArrowCursor;
    }
}

bool GuiWindow::eventFilter(QObject* obj, QEvent* event)
{
    // Only intercept mouse events on element widgets when Ctrl is held
    const QEvent::Type type = event->type();
    if (type != QEvent::MouseButtonPress   &&
        type != QEvent::MouseMove          &&
        type != QEvent::MouseButtonRelease &&
        type != QEvent::Leave)
    {
        return QMainWindow::eventFilter(obj, event);
    }

    auto* widget = dynamic_cast<QWidget*>(obj);
    if (!widget)
        return QMainWindow::eventFilter(obj, event);

    // Find the ApplicationGuiElement corresponding to this widget
    ApplicationGuiElement* target_elem = nullptr;
    for (auto* elem : getAllGuiElements())
    {
        if (dynamic_cast<QWidget*>(elem) == widget)
        {
            target_elem = elem;
            break;
        }
    }
    if (!target_elem)
        return QMainWindow::eventFilter(obj, event);

    if (type == QEvent::Leave)
    {
        if (!ctrl_drag_active_)
            widget->setCursor(Qt::ArrowCursor);
        return QMainWindow::eventFilter(obj, event);
    }

    auto* me = static_cast<QMouseEvent*>(event);
    const bool ctrl_held = me->modifiers() & Qt::ControlModifier;

    // ---- Hover: update cursor when Ctrl is held but not dragging ----
    if (type == QEvent::MouseMove && !ctrl_drag_active_)
    {
        if (ctrl_held)
        {
            const CursorSquareState state = computeCursorState(widget->rect(), me->pos());
            widget->setCursor(cursorForState(state));
        }
        else
        {
            widget->setCursor(Qt::ArrowCursor);
        }
        return QMainWindow::eventFilter(obj, event);  // don't consume — pass to widget
    }

    // ---- Press: begin Ctrl+drag ----
    if (type == QEvent::MouseButtonPress && me->button() == Qt::LeftButton && ctrl_held)
    {
        element_being_dragged_    = target_elem;
        element_rect_at_press_    = widget->geometry();  // in content_area_ coords
        mouse_content_at_press_   = widget->mapToParent(me->pos());
        drag_cursor_state_        = computeCursorState(widget->rect(), me->pos());
        ctrl_drag_active_         = true;

        widget->setCursor(cursorForState(drag_cursor_state_));
        editing_overlay_->showForRect(element_rect_at_press_);
        return true;  // consume: don't pass click to the underlying widget
    }

    // ---- Move: update while Ctrl+dragging ----
    if (type == QEvent::MouseMove && ctrl_drag_active_ && element_being_dragged_ == target_elem)
    {
        const QPoint current_content = widget->mapToParent(me->pos());
        const QPoint delta           = current_content - mouse_content_at_press_;

        QRect new_rect = element_rect_at_press_;

        switch (drag_cursor_state_)
        {
            case CursorSquareState::MOVE:
                new_rect.translate(delta);
                break;
            case CursorSquareState::RESIZE_CENTER_RIGHT:
                new_rect.setRight(element_rect_at_press_.right() + delta.x());
                break;
            case CursorSquareState::RESIZE_CENTER_LEFT:
                new_rect.setLeft(element_rect_at_press_.left() + delta.x());
                break;
            case CursorSquareState::RESIZE_LOWER_CENTER:
                new_rect.setBottom(element_rect_at_press_.bottom() + delta.y());
                break;
            case CursorSquareState::RESIZE_UPPER_CENTER:
                new_rect.setTop(element_rect_at_press_.top() + delta.y());
                break;
            case CursorSquareState::RESIZE_LOWER_RIGHT:
                new_rect.setRight(element_rect_at_press_.right()   + delta.x());
                new_rect.setBottom(element_rect_at_press_.bottom() + delta.y());
                break;
            case CursorSquareState::RESIZE_LOWER_LEFT:
                new_rect.setLeft(element_rect_at_press_.left()     + delta.x());
                new_rect.setBottom(element_rect_at_press_.bottom() + delta.y());
                break;
            case CursorSquareState::RESIZE_UPPER_RIGHT:
                new_rect.setRight(element_rect_at_press_.right()   + delta.x());
                new_rect.setTop(element_rect_at_press_.top()       + delta.y());
                break;
            case CursorSquareState::RESIZE_UPPER_LEFT:
                new_rect.setLeft(element_rect_at_press_.left()     + delta.x());
                new_rect.setTop(element_rect_at_press_.top()       + delta.y());
                break;
            default:
                break;
        }

        // Enforce minimum size
        if (new_rect.width()  < 20) new_rect.setWidth(20);
        if (new_rect.height() < 10) new_rect.setHeight(10);

        widget->setGeometry(new_rect);
        editing_overlay_->showForRect(new_rect);
        return true;
    }

    // ---- Release: finalize ----
    if (type == QEvent::MouseButtonRelease && me->button() == Qt::LeftButton && ctrl_drag_active_)
    {
        ctrl_drag_active_ = false;
        editing_overlay_->hide();
        widget->setCursor(Qt::ArrowCursor);

        // Persist fractional position back into element_settings_
        element_being_dragged_->setBoundingRect(widget->geometry(), content_area_->size());
        notify_main_window_about_modification_();

        element_being_dragged_ = nullptr;
        return true;
    }

    return QMainWindow::eventFilter(obj, event);
}

// ---------------------------------------------------------------------------
// Menu setup
// ---------------------------------------------------------------------------

void GuiWindow::setupPopupMenus()
{
    // Lambda that builds a "New Element" submenu owned by the given parent menu
    auto makeNewElementSub = [this](QMenu* parent_menu) -> QMenu* {
        QMenu* sub = new QMenu(tr("New Element"), parent_menu);
        sub->addAction(tr("Plot Pane"),          this, &GuiWindow::onNewPlotPane);
        sub->addAction(tr("Button"),             this, &GuiWindow::onNewButton);
        sub->addAction(tr("Slider"),             this, &GuiWindow::onNewSlider);
        sub->addAction(tr("Checkbox"),           this, &GuiWindow::onNewCheckbox);
        sub->addAction(tr("Text Label"),         this, &GuiWindow::onNewTextLabel);
        sub->addAction(tr("Editable Text"),      this, &GuiWindow::onNewEditableText);
        sub->addAction(tr("Dropdown Menu"),      this, &GuiWindow::onNewDropdownMenu);
        sub->addAction(tr("Radio Button Group"), this, &GuiWindow::onNewRadioButtonGroup);
        sub->addAction(tr("List Box"),           this, &GuiWindow::onNewListBox);
        sub->addAction(tr("Scrolling Text"),     this, &GuiWindow::onNewScrollingText);
        return sub;
    };

    // ---- Window/background popup ----
    popup_menu_window_ = new QMenu(this);
    popup_menu_window_->addMenu(makeNewElementSub(popup_menu_window_));
    popup_menu_window_->addSeparator();
    popup_menu_window_->addAction(tr("New Tab"),          this, &GuiWindow::onNewTab);
    popup_menu_window_->addSeparator();
    popup_menu_window_->addAction(tr("Edit Window Name"), this, &GuiWindow::onEditWindowName);
    popup_menu_window_->addAction(tr("Edit Tab Name"),    this, &GuiWindow::onEditTabName);
    popup_menu_window_->addAction(tr("Delete Tab"),       this, &GuiWindow::onDeleteTab);

    // ---- Element popup ----
    popup_menu_element_ = new QMenu(this);
    popup_menu_element_->addAction(tr("Edit Element Name"), this, &GuiWindow::onEditElementName);
    popup_menu_element_->addAction(tr("Delete Element"),    this, &GuiWindow::onDeleteElement);
    popup_menu_element_->addSeparator();
    popup_menu_element_->addMenu(makeNewElementSub(popup_menu_element_));
    popup_menu_element_->addSeparator();
    popup_menu_element_->addAction(tr("New Tab"),           this, &GuiWindow::onNewTab);
    popup_menu_element_->addAction(tr("Edit Window Name"),  this, &GuiWindow::onEditWindowName);
}

void GuiWindow::setupMenuBar()
{
    menu_bar_ = menuBar();

    // File menu
    QMenu* file_menu = menu_bar_->addMenu(tr("&File"));
    file_menu->addAction(tr("&New"),          [this]() { emit newProjectRequested(); });
    file_menu->addAction(tr("&Open..."),      [this]() { emit openProjectRequested(); });
    file_menu->addAction(tr("&Save"),         [this]() { emit saveProjectRequested(); });
    file_menu->addAction(tr("Save &As..."),   [this]() { emit saveProjectAsRequested(); });
    file_menu->addSeparator();
    file_menu->addAction(tr("New &Window"),   [this]() { emit newWindowRequested(); });
    file_menu->addSeparator();
    file_menu->addAction(tr("&Close Window"), this, &GuiWindow::close);

    // Edit menu
    QMenu* edit_menu = menu_bar_->addMenu(tr("&Edit"));
    edit_menu->addAction(tr("Edit Window &Name"), this, &GuiWindow::onEditWindowName);

    // Tab menu
    QMenu* tab_menu = menu_bar_->addMenu(tr("&Tab"));
    tab_menu->addAction(tr("&New Tab"),       this, &GuiWindow::onNewTab);
    tab_menu->addAction(tr("&Edit Tab Name"), this, &GuiWindow::onEditTabName);
    tab_menu->addAction(tr("&Delete Tab"),    this, &GuiWindow::onDeleteTab);

    // Element menu
    QMenu* element_menu = menu_bar_->addMenu(tr("&Element"));
    QMenu* new_element_submenu = element_menu->addMenu(tr("&New Element"));
    new_element_submenu->addAction(tr("New &Plot Pane"),         this, &GuiWindow::onNewPlotPane);
    new_element_submenu->addAction(tr("New &Button"),            this, &GuiWindow::onNewButton);
    new_element_submenu->addAction(tr("New &Slider"),            this, &GuiWindow::onNewSlider);
    new_element_submenu->addAction(tr("New &Checkbox"),          this, &GuiWindow::onNewCheckbox);
    new_element_submenu->addAction(tr("New &Text Label"),        this, &GuiWindow::onNewTextLabel);
    new_element_submenu->addAction(tr("New &Editable Text"),     this, &GuiWindow::onNewEditableText);
    new_element_submenu->addAction(tr("New &Dropdown Menu"),     this, &GuiWindow::onNewDropdownMenu);
    new_element_submenu->addAction(tr("New &Radio Button Group"),this, &GuiWindow::onNewRadioButtonGroup);
    new_element_submenu->addAction(tr("New &List Box"),          this, &GuiWindow::onNewListBox);
    new_element_submenu->addAction(tr("New S&crolling Text"),    this, &GuiWindow::onNewScrollingText);
    element_menu->addSeparator();
    element_menu->addAction(tr("&Edit Element Name"), this, &GuiWindow::onEditElementName);
    element_menu->addAction(tr("&Delete Element"),    this, &GuiWindow::onDeleteElement);

    // View menu
    QMenu* view_menu = menu_bar_->addMenu(tr("&View"));
    view_menu->addAction(tr("&Command Output"), this, &GuiWindow::onShowCmdlOutput);
    view_menu->addAction(tr("&Topic Output"),   this, &GuiWindow::onShowTopicOutput);
    view_menu->addSeparator();
    QAction* help_action = view_menu->addAction(tr("&Help"), this, &GuiWindow::onShowHelp);
    help_action->setShortcut(QKeySequence::HelpContents);

    // Tools menu
    QMenu* tools_menu = menu_bar_->addMenu(tr("&Tools"));
    tools_menu->addAction(tr("&Serial Port..."), this, &GuiWindow::onOpenSerialPort);
}

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------

void GuiWindow::resizeEvent(QResizeEvent* event)
{
    QMainWindow::resizeEvent(event);
    // Propagate new content-area size to the active tab's elements
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        tabs_[current_tab_num_]->updateSizeFromParent(content_area_->size());
}

void GuiWindow::contextMenuEvent(QContextMenuEvent* event)
{
    // Only fire the window popup when the click is on the content area background,
    // not on a child widget (elements call notify_parent_right_click_ themselves).
    if (childAt(event->pos()) == content_area_ || childAt(event->pos()) == nullptr)
        popup_menu_window_->exec(event->globalPos());
}

void GuiWindow::closeEvent(QCloseEvent* event)
{
    event->accept();
}

void GuiWindow::keyPressEvent(QKeyEvent* event)
{
    notifyChildrenOnKeyPressed(static_cast<char>(event->key()));
    QMainWindow::keyPressEvent(event);
}

void GuiWindow::keyReleaseEvent(QKeyEvent* event)
{
    notifyChildrenOnKeyReleased(static_cast<char>(event->key()));
    QMainWindow::keyReleaseEvent(event);
}

// ---------------------------------------------------------------------------
// Screenshot
// ---------------------------------------------------------------------------

void GuiWindow::screenshot(const std::string& base_path)
{
    raise();
    activateWindow();
    QScreen* screen = QGuiApplication::primaryScreen();
    if (!screen)
        return;
    const QPixmap pixmap = screen->grabWindow(winId());
    const QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    const QString filename = QString::fromStdString(base_path) +
                             "Screenshot_" + timestamp + "_" +
                             QString::fromStdString(name_) + ".png";
    pixmap.save(filename);
    LUMOS_LOG_INFO() << "Screenshot saved: " << filename.toStdString();
}

// ---------------------------------------------------------------------------
// Serial port dialog
// ---------------------------------------------------------------------------

void GuiWindow::onOpenSerialPort()
{
    QStringList port_names;
    for (const QSerialPortInfo& info : QSerialPortInfo::availablePorts())
        port_names << info.portName() + " - " + info.description();

    if (port_names.isEmpty())
    {
        QMessageBox::information(this, tr("Serial Port"), tr("No serial ports found."));
        return;
    }

    bool ok = false;
    const QString selected = QInputDialog::getItem(this, tr("Connect Serial Port"),
                                                   tr("Select port:"), port_names, 0, false, &ok);
    if (!ok || selected.isEmpty())
        return;

    const QString port = selected.section(" - ", 0, 0).trimmed();

    bool baud_ok = false;
    const int baudrate = QInputDialog::getInt(this, tr("Serial Baud Rate"),
                                              tr("Baud rate:"), 115200, 1200, 4000000, 100, &baud_ok);
    if (!baud_ok)
        return;

    emit serialConnectRequested(port, baudrate);
}

// ---------------------------------------------------------------------------
// Tab actions
// ---------------------------------------------------------------------------

void GuiWindow::onNewTab()
{
    bool ok;
    QString tab_name = QInputDialog::getText(this, tr("New Tab"), tr("Tab name:"),
                                             QLineEdit::Normal,
                                             QString("tab_%1").arg(tabs_.size()), &ok);
    if (!ok || tab_name.isEmpty())
        return;

    TabSettings tab_settings;
    tab_settings.name = tab_name.toStdString();

    const std::string window_name_local = name_;
    auto on_text_output_local = [this, window_name_local](const std::string& text) {
        topic_output_window_->pushNewText(window_name_local, Color_t::BLACK, text);
        topic_output_window_->show();
    };
    auto notify_parent_right_click = [this](const QPoint& pos, const std::string& elem_name) {
        last_clicked_item_ = elem_name;
        popup_menu_element_->exec(mapToGlobal(pos));
    };

    WindowTab* tab = new WindowTab(
        content_area_,
        tab_settings,
        0,
        notify_main_window_key_pressed_,
        notify_main_window_key_released_,
        notify_parent_right_click,
        notify_main_window_element_deleted_,
        notify_main_window_about_modification_,
        on_text_output_local);

    // Hide the currently active tab
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        tabs_[current_tab_num_]->hide();

    tabs_.push_back(tab);
    current_tab_num_ = static_cast<int>(tabs_.size()) - 1;
    tab->show();

    // Add tab row (label button + × close button)
    QWidget* row = new QWidget(tab_button_panel_);
    QHBoxLayout* row_layout = new QHBoxLayout(row);
    row_layout->setContentsMargins(0, 0, 0, 0);
    row_layout->setSpacing(2);

    QPushButton* btn = new QPushButton(tab_name, row);
    btn->setCheckable(true);
    const int idx = static_cast<int>(tab_buttons_qt_.size());
    connect(btn, &QPushButton::clicked, this, [this, idx]() { switchToTab(idx); });

    QPushButton* close_btn = new QPushButton("×", row);
    close_btn->setFixedSize(16, 16);
    close_btn->setToolTip(tr("Close tab"));
    connect(close_btn, &QPushButton::clicked, this, [this, idx]() {
        switchToTab(idx);
        onDeleteTab();
    });

    row_layout->addWidget(btn);
    row_layout->addWidget(close_btn);

    tab_button_layout_->insertWidget(tab_button_layout_->count() - 1, row);
    tab_buttons_qt_.push_back(btn);
    tab_row_widgets_.push_back(row);

    updateTabButtonPanel();
    notify_main_window_about_modification_();
}

void GuiWindow::onDeleteTab()
{
    if (tabs_.empty())
        return;

    const int idx = current_tab_num_;
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("Delete Tab"),
        QString("Delete tab '%1'?").arg(QString::fromStdString(tabs_[idx]->getName())),
        QMessageBox::Yes | QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    tabs_[idx]->hide();
    delete tabs_[idx];
    tabs_.erase(tabs_.begin() + idx);

    // Remove tab row (deleting the row widget also destroys the child buttons)
    tab_buttons_qt_.erase(tab_buttons_qt_.begin() + idx);
    delete tab_row_widgets_[idx];
    tab_row_widgets_.erase(tab_row_widgets_.begin() + idx);

    // Reconnect remaining buttons with correct indices
    for (int i = 0; i < static_cast<int>(tab_buttons_qt_.size()); ++i)
    {
        tab_buttons_qt_[i]->disconnect();
        connect(tab_buttons_qt_[i], &QPushButton::clicked, this, [this, i]() { switchToTab(i); });
    }

    // Select a valid tab
    current_tab_num_ = std::max(0, idx - 1);
    if (!tabs_.empty())
    {
        tabs_[current_tab_num_]->show();
        tabs_[current_tab_num_]->updateSizeFromParent(content_area_->size());
    }

    updateTabButtonPanel();
    notify_main_window_about_modification_();
}

void GuiWindow::onEditTabName()
{
    if (current_tab_num_ < 0 || current_tab_num_ >= static_cast<int>(tabs_.size()))
        return;

    bool ok;
    const QString current = QString::fromStdString(tabs_[current_tab_num_]->getName());
    const QString new_name = QInputDialog::getText(this, tr("Edit Tab Name"), tr("Tab name:"),
                                                   QLineEdit::Normal, current, &ok);
    if (!ok || new_name.isEmpty())
        return;

    tabs_[current_tab_num_]->setName(new_name.toStdString());
    tab_buttons_qt_[current_tab_num_]->setText(new_name);
    notify_main_window_about_modification_();
}

// ---------------------------------------------------------------------------
// Window name
// ---------------------------------------------------------------------------

void GuiWindow::onEditWindowName()
{
    bool ok;
    const QString new_name = QInputDialog::getText(this, tr("Edit Window Name"), tr("Window name:"),
                                                   QLineEdit::Normal,
                                                   QString::fromStdString(name_), &ok);
    if (!ok || new_name.isEmpty())
        return;

    const std::string old_name = name_;
    name_ = new_name.toStdString();
    updateLabel();
    notify_main_window_name_changed_(old_name, name_);
    notify_main_window_about_modification_();
}

// ---------------------------------------------------------------------------
// Element actions
// ---------------------------------------------------------------------------

void GuiWindow::onEditElementName()
{
    LUMOS_LOG_WARNING() << "Element name editing not yet implemented";
}

void GuiWindow::onDeleteElement()
{
    if (last_clicked_item_.empty())
        return;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        if (tabs_[current_tab_num_]->deleteElementIfItExists(last_clicked_item_))
        {
            last_clicked_item_.clear();
            notify_main_window_about_modification_();
        }
    }
}

// ---------------------------------------------------------------------------
// Accessors and helpers
// ---------------------------------------------------------------------------

int GuiWindow::getCallbackId() const { return callback_id_; }

void GuiWindow::setName(const QString& new_name)
{
    name_ = new_name.toStdString();
    updateLabel();
}

void GuiWindow::updateLabel()
{
    QString title = QString::fromStdString(project_name_);
    if (!project_is_saved_)
        title += " *";
    title += " - " + QString::fromStdString(name_);
    setWindowTitle(title);
}

WindowSettings GuiWindow::getWindowSettings() const
{
    WindowSettings settings;
    settings.name   = name_;
    settings.x      = x();
    settings.y      = y();
    settings.width  = width();
    settings.height = height();
    for (const auto* tab : tabs_)
        settings.tabs.push_back(tab->getTabSettings());
    return settings;
}

QString GuiWindow::getName() const { return QString::fromStdString(name_); }

void GuiWindow::setIsFileSavedForLabel(const bool is_saved)
{
    project_is_saved_ = is_saved;
    updateLabel();
}

void GuiWindow::setProjectName(const QString& project_name)
{
    project_name_ = project_name.toStdString();
    updateLabel();
}

ApplicationGuiElement* GuiWindow::getGuiElement(const std::string& element_handle_string) const
{
    for (const auto* tab : tabs_)
    {
        ApplicationGuiElement* elem = tab->getGuiElement(element_handle_string);
        if (elem)
            return elem;
    }
    return nullptr;
}

void GuiWindow::notifyChildrenOnKeyPressed(const char key)
{
    for (auto* tab : tabs_)
        tab->notifyChildrenOnKeyPressed(key);
}

void GuiWindow::notifyChildrenOnKeyReleased(const char key)
{
    for (auto* tab : tabs_)
        tab->notifyChildrenOnKeyReleased(key);
}

void GuiWindow::createNewPlotPane()
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        tabs_[current_tab_num_]->createNewPlotPane();
}

void GuiWindow::createNewPlotPane(const std::string& handle_string)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        tabs_[current_tab_num_]->createNewPlotPane(handle_string);
}

void GuiWindow::updateAllElements()
{
    for (auto* tab : tabs_)
        tab->updateAllElements();
}

std::vector<ApplicationGuiElement*> GuiWindow::getGuiElements() const
{
    std::vector<ApplicationGuiElement*> result;
    for (const auto* tab : tabs_)
    {
        auto v = tab->getGuiElements();
        result.insert(result.end(), v.begin(), v.end());
    }
    return result;
}

std::vector<ApplicationGuiElement*> GuiWindow::getPlotPanes() const
{
    std::vector<ApplicationGuiElement*> result;
    for (const auto* tab : tabs_)
    {
        auto v = tab->getPlotPanes();
        result.insert(result.end(), v.begin(), v.end());
    }
    return result;
}

std::vector<ApplicationGuiElement*> GuiWindow::getAllGuiElements() const
{
    std::vector<ApplicationGuiElement*> result;
    for (const auto* tab : tabs_)
    {
        auto v = tab->getAllGuiElements();
        result.insert(result.end(), v.begin(), v.end());
    }
    return result;
}

std::vector<std::string> GuiWindow::getElementNames() const
{
    std::vector<std::string> names;
    for (const auto* tab : tabs_)
    {
        auto v = tab->getElementNames();
        names.insert(names.end(), v.begin(), v.end());
    }
    return names;
}

// ---------------------------------------------------------------------------
// Helper to prompt for a unique handle string
// ---------------------------------------------------------------------------
static std::string promptUniqueHandle(GuiWindow* parent,
                                      const QString& title,
                                      const std::function<std::vector<std::string>(void)>& get_names,
                                      const std::string& default_name,
                                      bool& ok_out)
{
    ok_out = false;
    bool ok = false;
    const QString result = QInputDialog::getText(parent, title, QObject::tr("Handle string:"),
                                                 QLineEdit::Normal,
                                                 QString::fromStdString(default_name), &ok);
    if (!ok || result.isEmpty())
        return {};

    const std::string handle = result.toStdString();
    const auto existing = get_names();
    if (std::find(existing.begin(), existing.end(), handle) != existing.end())
    {
        QMessageBox::warning(parent, QObject::tr("Duplicate name"),
                             QObject::tr("An element named '%1' already exists.").arg(result));
        return {};
    }
    ok_out = true;
    return handle;
}

// ---------------------------------------------------------------------------
// onNew* slots
// ---------------------------------------------------------------------------

void GuiWindow::onNewPlotPane()
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        tabs_[current_tab_num_]->createNewPlotPane();
        notify_main_window_about_modification_();
    }
}

void GuiWindow::onNewButton()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Button"), get_all_element_names_, "button0", ok);
    if (!ok) return;

    auto settings = std::make_shared<ButtonSettings>();
    settings->handle_string = handle;
    settings->type  = lumos::GuiElementType::Button;
    settings->label = handle;
    settings->x = 0.1f; settings->y = 0.1f;
    settings->width = 0.2f; settings->height = 0.05f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewButton(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewSlider()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Slider"), get_all_element_names_, "slider0", ok);
    if (!ok) return;

    auto settings = std::make_shared<SliderSettings>();
    settings->handle_string = handle;
    settings->type = lumos::GuiElementType::Slider;
    settings->min_value = 0; settings->max_value = 100;
    settings->init_value = 50; settings->step_size = 1;
    settings->is_horizontal = true;
    settings->x = 0.1f; settings->y = 0.2f;
    settings->width = 0.3f; settings->height = 0.07f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewSlider(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewCheckbox()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Checkbox"), get_all_element_names_, "checkbox0", ok);
    if (!ok) return;

    auto settings = std::make_shared<CheckboxSettings>();
    settings->handle_string = handle;
    settings->type  = lumos::GuiElementType::Checkbox;
    settings->label = handle;
    settings->x = 0.1f; settings->y = 0.3f;
    settings->width = 0.2f; settings->height = 0.05f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewCheckbox(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewTextLabel()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Text Label"), get_all_element_names_, "label0", ok);
    if (!ok) return;

    auto settings = std::make_shared<TextLabelSettings>();
    settings->handle_string = handle;
    settings->type  = lumos::GuiElementType::TextLabel;
    settings->label = handle;
    settings->x = 0.1f; settings->y = 0.4f;
    settings->width = 0.2f; settings->height = 0.04f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewTextLabel(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewEditableText()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Editable Text"), get_all_element_names_, "edittext0", ok);
    if (!ok) return;

    auto settings = std::make_shared<EditableTextSettings>();
    settings->handle_string = handle;
    settings->type = lumos::GuiElementType::EditableText;
    settings->init_value = "";
    settings->x = 0.1f; settings->y = 0.5f;
    settings->width = 0.25f; settings->height = 0.05f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewEditableText(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewDropdownMenu()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Dropdown Menu"), get_all_element_names_, "dropdown0", ok);
    if (!ok) return;

    auto settings = std::make_shared<DropdownMenuSettings>();
    settings->handle_string = handle;
    settings->type     = lumos::GuiElementType::DropdownMenu;
    settings->elements = {"Item 1", "Item 2", "Item 3"};
    settings->x = 0.1f; settings->y = 0.6f;
    settings->width = 0.2f; settings->height = 0.05f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewDropdownMenu(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewRadioButtonGroup()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Radio Button Group"), get_all_element_names_, "radiogroup0", ok);
    if (!ok) return;

    auto settings = std::make_shared<RadioButtonGroupSettings>();
    settings->handle_string = handle;
    settings->type = lumos::GuiElementType::RadioButtonGroup;
    RadioButtonSettings rb1; rb1.label = "Option 1";
    RadioButtonSettings rb2; rb2.label = "Option 2";
    settings->radio_buttons = {rb1, rb2};
    settings->x = 0.1f; settings->y = 0.7f;
    settings->width = 0.2f; settings->height = 0.1f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewRadioButtonGroup(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewListBox()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New List Box"), get_all_element_names_, "listbox0", ok);
    if (!ok) return;

    auto settings = std::make_shared<ListBoxSettings>();
    settings->handle_string = handle;
    settings->type     = lumos::GuiElementType::ListBox;
    settings->elements = {"Item 1", "Item 2", "Item 3"};
    settings->x = 0.4f; settings->y = 0.1f;
    settings->width = 0.2f; settings->height = 0.2f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewListBox(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

void GuiWindow::onNewScrollingText()
{
    bool ok = false;
    const std::string handle = promptUniqueHandle(this, tr("New Scrolling Text"), get_all_element_names_, "scrolltext0", ok);
    if (!ok) return;

    auto settings = std::make_shared<ScrollingTextSettings>();
    settings->handle_string = handle;
    settings->type = lumos::GuiElementType::ScrollingText;
    settings->x = 0.4f; settings->y = 0.35f;
    settings->width = 0.5f; settings->height = 0.3f;

    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        ApplicationGuiElement* elem = tabs_[current_tab_num_]->createNewScrollingText(settings);
        if (elem) { installEditFilter(elem); emit guiElementCreated(elem); notify_main_window_about_modification_(); }
    }
}

// ---------------------------------------------------------------------------
// createNew*ForTab helpers
// ---------------------------------------------------------------------------

ApplicationGuiElement* GuiWindow::createNewButtonForTab(const std::shared_ptr<ButtonSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewButton(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewSliderForTab(const std::shared_ptr<SliderSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewSlider(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewCheckboxForTab(const std::shared_ptr<CheckboxSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewCheckbox(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewTextLabelForTab(const std::shared_ptr<TextLabelSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewTextLabel(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewEditableTextForTab(const std::shared_ptr<EditableTextSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewEditableText(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewDropdownMenuForTab(const std::shared_ptr<DropdownMenuSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewDropdownMenu(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewRadioButtonGroupForTab(const std::shared_ptr<RadioButtonGroupSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewRadioButtonGroup(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewListBoxForTab(const std::shared_ptr<ListBoxSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewListBox(s);
    return nullptr;
}

ApplicationGuiElement* GuiWindow::createNewScrollingTextForTab(const std::shared_ptr<ScrollingTextSettings>& s)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
        return tabs_[current_tab_num_]->createNewScrollingText(s);
    return nullptr;
}

// ---------------------------------------------------------------------------
// View menu slots
// ---------------------------------------------------------------------------

void GuiWindow::onShowCmdlOutput()
{
    cmdl_output_window_->show();
    cmdl_output_window_->raise();
    cmdl_output_window_->activateWindow();
}

void GuiWindow::onShowTopicOutput()
{
    topic_output_window_->show();
    topic_output_window_->raise();
    topic_output_window_->activateWindow();
}

void GuiWindow::onShowHelp()
{
    help_pane_->show();
    help_pane_->raise();
    help_pane_->activateWindow();
}
