// SPDX-License-Identifier: GPL-3.0-or-later
#include <QListView>
#include <QPointer>

class QSortFilterProxyModel;

namespace solarus::launcher {
class Controller;

class QuestListView : public QListView {
  Q_OBJECT

public:
  explicit QuestListView(Controller* controller, QWidget* parent = nullptr);
  virtual ~QuestListView() = default;

private:
  void setupUi();

protected:
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void currentChanged(const QModelIndex& current, const QModelIndex& previous) override;
  void keyPressEvent(QKeyEvent* event) override;
  void keyReleaseEvent(QKeyEvent* event) override;
  void paintEvent(QPaintEvent* event) override;

private:
  QPointer<Controller> _controller;
  int _pressedKey{ Qt::Key::Key_unknown };
};
} // namespace solarus::launcher
