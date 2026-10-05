// SPDX-License-Identifier: GPL-3.0-or-later
#include <QToolBar>
#include <QPointer>

namespace solarus::launcher {
class Controller;

class ToolBar : public QToolBar {
  Q_OBJECT

public:
  explicit ToolBar(Controller* controller, QWidget* widget = nullptr);
  virtual ~ToolBar() = default;

private:
  void setupUi();

private:
  QPointer<Controller> _controller;
};
} // namespace solarus::launcher
