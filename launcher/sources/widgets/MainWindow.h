// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QWidget>
#include <QPointer>

#include <Controller.h>

namespace oclero::qlementine {
class Expander;
} // namespace oclero::qlementine

namespace solarus::launcher {
class MenuBar;
class ToolBar;
class QuestListView;
class CustomSplitter;
class StatusBar;
class BottomPanel;
class QuestPropertiesPanel;
class DropArea;

class MainWindow : public QWidget {
  Q_OBJECT

public:
  explicit MainWindow(Controller* controller, QWidget* parent = nullptr);
  virtual ~MainWindow() = default;

  static void setAppIcon();

private:
  void setupUi();

protected:
  void resizeEvent(QResizeEvent* event) override;
  void closeEvent(QCloseEvent* event) override;
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

private:
  QPointer<Controller> _controller{ nullptr };
  struct {
    MenuBar* menuBar{ nullptr };
    ToolBar* toolBar{ nullptr };
    QuestListView* listView{ nullptr };
    CustomSplitter* splitter{ nullptr };
    StatusBar* statusBar{ nullptr };
    BottomPanel* bottomPanel{ nullptr };
    QuestPropertiesPanel* propertiesPanel{ nullptr };
    oclero::qlementine::Expander* propertiesPanelExpander{ nullptr };
    DropArea* dropArea{ nullptr };
  } _ui;
};
} // namespace solarus::launcher
