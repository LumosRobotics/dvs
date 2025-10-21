#include "gui_window.h"

#include <QAction>
#include <QInputDialog>
#include <QMessageBox>

#include "lumos/logging/logging.h"

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
      element_x_offset_(0),
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
    // Set window properties
    setWindowTitle(QString::fromStdString(name_));
    setGeometry(window_settings.x, window_settings.y, window_settings.width, window_settings.height);

    // Create central widget with tab widget
    tab_widget_ = new QTabWidget(this);
    setCentralWidget(tab_widget_);

    // Connect tab changed signal
    connect(tab_widget_, &QTabWidget::currentChanged, this, &GuiWindow::tabChanged);

    // Setup menu bar
    setupMenuBar();

    // Create tabs from settings
    for (const TabSettings& tab_settings : window_settings.tabs)
    {
        auto notify_parent_right_click = [this](const QPoint& pos, const std::string& elem_name) {
            last_clicked_item_ = elem_name;
            // TODO: Show popup menu
        };

        WindowTab* tab = new WindowTab(
            tab_widget_,
            tab_settings,
            element_x_offset_,
            notify_main_window_key_pressed_,
            notify_main_window_key_released_,
            notify_parent_right_click,
            notify_main_window_element_deleted_,
            notify_main_window_about_modification_);

        tabs_.push_back(tab);

        // Create a widget for this tab
        QWidget* tab_page = new QWidget();
        tab_widget_->addTab(tab_page, QString::fromStdString(tab_settings.name));
    }

    LUMOS_LOG_INFO() << "GuiWindow '" << name_ << "' created with " << tabs_.size() << " tabs";
}

GuiWindow::~GuiWindow()
{
    deleteAllTabs();
    LUMOS_LOG_INFO() << "GuiWindow '" << name_ << "' destroyed";
}

void GuiWindow::deleteAllTabs()
{
    for (auto* tab : tabs_)
    {
        delete tab;
    }
    tabs_.clear();
}

void GuiWindow::setupMenuBar()
{
    menu_bar_ = menuBar();

    // File menu
    QMenu* file_menu = menu_bar_->addMenu(tr("&File"));

    QAction* close_action = new QAction(tr("&Close Window"), this);
    connect(close_action, &QAction::triggered, this, &GuiWindow::close);
    file_menu->addAction(close_action);

    // Edit menu
    QMenu* edit_menu = menu_bar_->addMenu(tr("&Edit"));

    QAction* edit_window_name_action = new QAction(tr("Edit Window &Name"), this);
    connect(edit_window_name_action, &QAction::triggered, this, &GuiWindow::onEditWindowName);
    edit_menu->addAction(edit_window_name_action);

    // Tab menu
    QMenu* tab_menu = menu_bar_->addMenu(tr("&Tab"));

    QAction* new_tab_action = new QAction(tr("&New Tab"), this);
    connect(new_tab_action, &QAction::triggered, this, &GuiWindow::onNewTab);
    tab_menu->addAction(new_tab_action);

    QAction* edit_tab_name_action = new QAction(tr("&Edit Tab Name"), this);
    connect(edit_tab_name_action, &QAction::triggered, this, &GuiWindow::onEditTabName);
    tab_menu->addAction(edit_tab_name_action);

    QAction* delete_tab_action = new QAction(tr("&Delete Tab"), this);
    connect(delete_tab_action, &QAction::triggered, this, &GuiWindow::onDeleteTab);
    tab_menu->addAction(delete_tab_action);

    // Element menu
    QMenu* element_menu = menu_bar_->addMenu(tr("&Element"));

    QAction* new_plot_pane_action = new QAction(tr("New &Plot Pane"), this);
    connect(new_plot_pane_action, &QAction::triggered, this, &GuiWindow::onNewPlotPane);
    element_menu->addAction(new_plot_pane_action);

    element_menu->addSeparator();

    QAction* edit_element_name_action = new QAction(tr("&Edit Element Name"), this);
    connect(edit_element_name_action, &QAction::triggered, this, &GuiWindow::onEditElementName);
    element_menu->addAction(edit_element_name_action);

    QAction* delete_element_action = new QAction(tr("&Delete Element"), this);
    connect(delete_element_action, &QAction::triggered, this, &GuiWindow::onDeleteElement);
    element_menu->addAction(delete_element_action);
}

void GuiWindow::tabChanged(int index)
{
    if (index >= 0 && index < static_cast<int>(tabs_.size()))
    {
        current_tab_num_ = index;
        LUMOS_LOG_DEBUG() << "Tab changed to: " << tabs_[index]->getName();
    }
}

void GuiWindow::onNewTab()
{
    bool ok;
    QString tab_name = QInputDialog::getText(this, tr("New Tab"),
                                             tr("Tab name:"), QLineEdit::Normal,
                                             QString("tab_%1").arg(tabs_.size()), &ok);
    if (ok && !tab_name.isEmpty())
    {
        TabSettings tab_settings;
        tab_settings.name = tab_name.toStdString();

        auto notify_parent_right_click = [this](const QPoint& pos, const std::string& elem_name) {
            last_clicked_item_ = elem_name;
        };

        WindowTab* tab = new WindowTab(
            tab_widget_,
            tab_settings,
            element_x_offset_,
            notify_main_window_key_pressed_,
            notify_main_window_key_released_,
            notify_parent_right_click,
            notify_main_window_element_deleted_,
            notify_main_window_about_modification_);

        tabs_.push_back(tab);

        QWidget* tab_page = new QWidget();
        tab_widget_->addTab(tab_page, tab_name);
        tab_widget_->setCurrentIndex(tabs_.size() - 1);

        notify_main_window_about_modification_();
    }
}

void GuiWindow::onNewPlotPane()
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        tabs_[current_tab_num_]->createNewPlotPane();
        notify_main_window_about_modification_();
    }
}

void GuiWindow::onEditWindowName()
{
    bool ok;
    QString new_name = QInputDialog::getText(this, tr("Edit Window Name"),
                                             tr("Window name:"), QLineEdit::Normal,
                                             QString::fromStdString(name_), &ok);
    if (ok && !new_name.isEmpty())
    {
        std::string old_name = name_;
        name_ = new_name.toStdString();
        updateLabel();
        notify_main_window_name_changed_(old_name, name_);
        notify_main_window_about_modification_();
    }
}

void GuiWindow::onEditTabName()
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        bool ok;
        QString current_name = QString::fromStdString(tabs_[current_tab_num_]->getName());
        QString new_name = QInputDialog::getText(this, tr("Edit Tab Name"),
                                                 tr("Tab name:"), QLineEdit::Normal,
                                                 current_name, &ok);
        if (ok && !new_name.isEmpty())
        {
            tabs_[current_tab_num_]->setName(new_name.toStdString());
            tab_widget_->setTabText(current_tab_num_, new_name);
            notify_main_window_about_modification_();
        }
    }
}

void GuiWindow::onDeleteTab()
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Delete Tab",
                                     QString("Delete tab '%1'?").arg(QString::fromStdString(tabs_[current_tab_num_]->getName())),
                                     QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            delete tabs_[current_tab_num_];
            tabs_.erase(tabs_.begin() + current_tab_num_);
            tab_widget_->removeTab(current_tab_num_);
            notify_main_window_about_modification_();
        }
    }
}

void GuiWindow::onEditElementName()
{
    // TODO: Implement element name editing
    LUMOS_LOG_WARNING() << "Element name editing not yet implemented";
}

void GuiWindow::onDeleteElement()
{
    // TODO: Implement element deletion
    LUMOS_LOG_WARNING() << "Element deletion not yet implemented";
}

void GuiWindow::closeEvent(QCloseEvent* event)
{
    event->accept();
}

void GuiWindow::keyPressEvent(QKeyEvent* event)
{
    const char key = static_cast<char>(event->key());
    notifyChildrenOnKeyPressed(key);
    QMainWindow::keyPressEvent(event);
}

void GuiWindow::keyReleaseEvent(QKeyEvent* event)
{
    const char key = static_cast<char>(event->key());
    notifyChildrenOnKeyReleased(key);
    QMainWindow::keyReleaseEvent(event);
}

int GuiWindow::getCallbackId() const
{
    return callback_id_;
}

void GuiWindow::setName(const QString& new_name)
{
    name_ = new_name.toStdString();
    updateLabel();
}

void GuiWindow::updateLabel()
{
    QString title = QString::fromStdString(project_name_);
    if (!project_is_saved_)
    {
        title += " *";
    }
    title += " - " + QString::fromStdString(name_);
    setWindowTitle(title);
}

WindowSettings GuiWindow::getWindowSettings() const
{
    WindowSettings settings;
    settings.name = name_;
    settings.x = x();
    settings.y = y();
    settings.width = width();
    settings.height = height();

    for (const auto* tab : tabs_)
    {
        settings.tabs.push_back(tab->getTabSettings());
    }

    return settings;
}

QString GuiWindow::getName() const
{
    return QString::fromStdString(name_);
}

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
        ApplicationGuiElement* element = tab->getGuiElement(element_handle_string);
        if (element != nullptr)
        {
            return element;
        }
    }
    return nullptr;
}

void GuiWindow::notifyChildrenOnKeyPressed(const char key)
{
    for (auto* tab : tabs_)
    {
        tab->notifyChildrenOnKeyPressed(key);
    }
}

void GuiWindow::notifyChildrenOnKeyReleased(const char key)
{
    for (auto* tab : tabs_)
    {
        tab->notifyChildrenOnKeyReleased(key);
    }
}

void GuiWindow::createNewPlotPane()
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        tabs_[current_tab_num_]->createNewPlotPane();
    }
}

void GuiWindow::createNewPlotPane(const std::string& handle_string)
{
    if (current_tab_num_ >= 0 && current_tab_num_ < static_cast<int>(tabs_.size()))
    {
        tabs_[current_tab_num_]->createNewPlotPane(handle_string);
    }
}

void GuiWindow::updateAllElements()
{
    for (auto* tab : tabs_)
    {
        tab->updateAllElements();
    }
}

std::vector<ApplicationGuiElement*> GuiWindow::getGuiElements() const
{
    std::vector<ApplicationGuiElement*> result;
    for (const auto* tab : tabs_)
    {
        auto elements = tab->getGuiElements();
        result.insert(result.end(), elements.begin(), elements.end());
    }
    return result;
}

std::vector<ApplicationGuiElement*> GuiWindow::getPlotPanes() const
{
    std::vector<ApplicationGuiElement*> result;
    for (const auto* tab : tabs_)
    {
        auto panes = tab->getPlotPanes();
        result.insert(result.end(), panes.begin(), panes.end());
    }
    return result;
}

std::vector<ApplicationGuiElement*> GuiWindow::getAllGuiElements() const
{
    std::vector<ApplicationGuiElement*> result;
    for (const auto* tab : tabs_)
    {
        auto elements = tab->getAllGuiElements();
        result.insert(result.end(), elements.begin(), elements.end());
    }
    return result;
}

std::vector<std::string> GuiWindow::getElementNames() const
{
    std::vector<std::string> names;
    for (const auto* tab : tabs_)
    {
        auto tab_names = tab->getElementNames();
        names.insert(names.end(), tab_names.begin(), tab_names.end());
    }
    return names;
}
