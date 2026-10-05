// SPDX-License-Identifier: GPL-3.0-or-later
#include <QWidget>
#include <QPointer>

class QPushButton;

namespace solarus::launcher {
class Console;
class Controller;

class BottomPanel : public QWidget {
  Q_OBJECT

public:
  explicit BottomPanel(Controller* controller, QWidget* parent = nullptr);
  virtual ~BottomPanel() = default;

private:
  void setupUi();

private:
  void saveConsoleContentToFile();

  QPointer<Controller> _controller;
  struct {
    Console* console{ nullptr };
    QPushButton* clearConsoleButton{ nullptr };
  } _ui;
};
} // namespace solarus::launcher
