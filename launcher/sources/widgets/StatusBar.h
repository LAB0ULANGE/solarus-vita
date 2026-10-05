// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QStatusBar>
#include <QPointer>

class QPushButton;
class QLabel;
class QStackedWidget;

namespace solarus::launcher {
class Controller;
class MessageWidget;

class StatusBar : public QStatusBar {
  Q_OBJECT

public:
  explicit StatusBar(Controller* controller, QWidget* parent = nullptr);
  virtual ~StatusBar() = default;

private:
  void setupUi();

private:
  QPointer<Controller> _controller;
  struct {
    MessageWidget* messageWidget{ nullptr };
    QLabel* questCountLabel{ nullptr };
  } _ui;
};
} // namespace solarus::launcher
