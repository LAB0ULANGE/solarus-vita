// SPDX-License-Identifier: GPL-3.0-or-later
#include <QMenuBar>
#include <QPointer>

namespace solarus::launcher {
class Controller;

class MenuBar : public QMenuBar {
  Q_OBJECT

public:
  explicit MenuBar(Controller* controller, QWidget* parent = nullptr);
  virtual ~MenuBar() = default;

private:
  void setupUi();

private:
  QPointer<Controller> _controller;
};
} // namespace solarus::launcher
