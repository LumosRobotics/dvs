#include "help_pane.h"

#include <QCloseEvent>
#include <QVBoxLayout>

static const char* kHelpHtml = R"html(
<html>
<body>
<h2>Duoplot Help</h2>
<h3>Getting Started</h3>
<p>Duoplot receives plot data over TCP (default port 9255) from client programs
using the Duoplot API.</p>
<h3>Creating Plots</h3>
<ul>
  <li>Call <code>figure("name")</code> in your client to address a plot pane.</li>
  <li>Use <code>plot(x, y)</code>, <code>scatter(x, y)</code>, <code>surf(X, Y, Z)</code>, etc.</li>
  <li>Call <code>axis({xmin, xmax, ymin, ymax})</code> to set axis limits.</li>
</ul>
<h3>Window Layout</h3>
<ul>
  <li>Use the <b>Element</b> menu to add plot panes or GUI elements.</li>
  <li>Use the <b>Tab</b> menu to manage tabs within a window.</li>
  <li>Use the <b>Tools &gt; Serial Port…</b> menu to connect a serial device.</li>
</ul>
<h3>Keyboard Shortcuts</h3>
<ul>
  <li><b>R</b> — reset view to default</li>
  <li><b>Left drag</b> — rotate 3-D view</li>
  <li><b>Scroll</b> — zoom</li>
</ul>
</body>
</html>
)html";

HelpPane::HelpPane(QWidget* parent)
    : QWidget(parent, Qt::Window)
{
    setWindowTitle(tr("Duoplot Help"));
    resize(600, 500);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(4, 4, 4, 4);

    browser_ = new QTextBrowser(this);
    browser_->setHtml(kHelpHtml);
    layout->addWidget(browser_);
}

void HelpPane::closeEvent(QCloseEvent* event)
{
    hide();
    event->ignore();
}
