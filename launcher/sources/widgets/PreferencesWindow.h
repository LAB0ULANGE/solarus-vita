// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QDialog>
#include <QPointer>

namespace solarus::launcher {
class Controller;

class PreferencesWindow : public QDialog {
  Q_OBJECT

public:
  explicit PreferencesWindow(Controller* controller, QWidget* parent = nullptr);
  virtual ~PreferencesWindow() = default;

protected:
  void mouseReleaseEvent(QMouseEvent*) override;

private:
  void setupUi();

  QPointer<Controller> _controller;
};
} // namespace solarus::launcher
