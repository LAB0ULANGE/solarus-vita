// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/MainWindow.h>

#include <widgets/Console.h>
#include <widgets/MessageBox.h>
#include <widgets/PreferencesWindow.h>
#include <widgets/StatusBar.h>
#include <widgets/CustomSplitter.h>
#include <widgets/BottomPanel.h>
#include <widgets/ToolBar.h>
#include <widgets/MenuBar.h>
#include <widgets/QuestListView.h>
#include <widgets/QuestPropertiesPanel.h>
#include <widgets/DropArea.h>

#include <quests/QuestListModel.h>
#include <quests/QuestRunner.h>

#include <BasicUpdater.h>
#include <Utils.h>
#include <Preferences.h>
#include <Controller.h>

#include <QApplication>
#include <QVBoxLayout>
#include <QShortcut>
#include <QDragEnterEvent>
#include <QMimeData>
#include <QDropEvent>
#include <QFileInfo>
#include <QSysInfo>

#include <oclero/qlementine/utils/WidgetUtils.hpp>
#include <oclero/qlementine/widgets/Expander.hpp>

namespace solarus::launcher {
MainWindow::MainWindow(Controller* controller, QWidget* parent)
  : QWidget(parent)
  , _controller(controller) {
  ensurePolished();
  setMinimumSize(720, 320);
  oclero::qlementine::centerWidget(this);
  restoreGeometry(_controller->preferences()->windowGeometry());

  setWindowTitle(QApplication::applicationDisplayName());
  setupUi();

  QObject::connect(_controller, &Controller::focusOnListViewRequested, this, [this](Qt::FocusReason reason) {
    _ui.listView->setFocus(reason);
  });

  QObject::connect(_controller, &Controller::mainWindowRaiseRequested, this, [this]() {
    raise();
  });

  // Check for updates.
  QTimer::singleShot(1000, this, [this]() {
    _controller->checkForUpdates();
  });

  // Load quests.
  QTimer::singleShot(0, this, [this]() {
    _controller->loadQuests();
  });

  // Enable drag n' drop.
  setAcceptDrops(true);
}

void MainWindow::setAppIcon() {
  // On macOS, the window icon doesn't need to be changed.
  // The application will use the bundle icon, which is at the correct size,
  // and displays a specific padding to follow Apple's guidelines.
  const auto platform = QSysInfo::productType();
  if (platform == QStringLiteral("osx") || platform == QStringLiteral("macos"))
    return;

  // On the other platforms, the icon can be changed, and it doesn't need any padding.
  // (i.e. the image should take all the available space).
  static constexpr std::array<QIcon::Mode, 4> modes{
    QIcon::Mode::Normal,
    QIcon::Mode::Disabled,
    QIcon::Mode::Active,
    QIcon::Mode::Selected,
  };
  static constexpr std::array<int, 12> dimensions{ 16, 20, 24, 32, 40, 48, 64, 96, 128, 256, 512, 1024 };
  static constexpr const char* path_base(":/solarus/launcher/resources/app_icon/solarus_launcher_icon_");
  static constexpr const char* path_ext(".png");

  QIcon app_icon;
  for (const auto dimension : dimensions) {
    const auto path = path_base + QString::number(dimension) + path_ext;
    QPixmap pixmap(path);
    for (const auto mode : modes) {
      app_icon.addPixmap(pixmap, mode, QIcon::State::On);
      app_icon.addPixmap(pixmap, mode, QIcon::State::Off);
    }
  }
  QGuiApplication::setWindowIcon(app_icon);
}

void MainWindow::setupUi() {
  _ui.menuBar = new MenuBar(_controller, this);

  auto* windowLayout = new QVBoxLayout(this);
  windowLayout->setContentsMargins(0, 0, 0, 0);
  windowLayout->setSpacing(0);
  setLayout(windowLayout);

  _ui.toolBar = new ToolBar(_controller, this);

  auto* topWidget = new QWidget(this);
  {
    auto* topWidgetLayout = new QHBoxLayout(topWidget);
    topWidget->setLayout(topWidgetLayout);
    topWidgetLayout->setSpacing(0);
    topWidgetLayout->setContentsMargins(0, 0, 0, 0);

    _ui.listView = new QuestListView(_controller, this);

    _ui.propertiesPanelExpander = new oclero::qlementine::Expander(this);
    {
      _ui.propertiesPanelExpander->setOrientation(Qt::Horizontal);
      _ui.propertiesPanelExpander->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
      auto* expanderContent = new QWidget(_ui.propertiesPanelExpander);
      expanderContent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      auto* expanderLayout = new QHBoxLayout(expanderContent);
      expanderContent->setLayout(expanderLayout);
      expanderLayout->setContentsMargins(0, 0, 0, 0);
      expanderLayout->setSpacing(0);

      expanderLayout->addWidget(oclero::qlementine::makeVerticalLine(this));
      _ui.propertiesPanel = new QuestPropertiesPanel(_controller->model(), expanderContent);
      expanderLayout->addWidget(_ui.propertiesPanel);

      _ui.propertiesPanelExpander->setContent(expanderContent);
      _ui.propertiesPanelExpander->setExpanded(_controller->preferences()->appPropertiesPanelVisible());

      QObject::connect(_ui.propertiesPanelExpander, &oclero::qlementine::Expander::expandedChanged, this, [this]() {
        _controller->preferences()->setAppPropertiesPanelVisible(_ui.propertiesPanelExpander->expanded());
      });

      QObject::connect(_controller->preferences(), &Preferences::appPropertiesPanelVisibleChanged, this, [this]() {
        _ui.propertiesPanelExpander->setExpanded(_controller->preferences()->appPropertiesPanelVisible());
      });
    }

    topWidgetLayout->addWidget(_ui.listView);
    topWidgetLayout->addWidget(_ui.propertiesPanelExpander);
  }

  _ui.bottomPanel = new BottomPanel(_controller, this);

  _ui.splitter = new CustomSplitter(this);
  QObject::connect(_ui.splitter, &CustomSplitter::handleMouseReleased, this, [this]() {
    if (_ui.splitter->sizes().at(1) == 0) {
      _ui.bottomPanel->setVisible(false);
      _controller->preferences()->setAppConsoleVisible(false);
    } else {
      _ui.bottomPanel->setVisible(true);
      _controller->preferences()->setAppConsoleVisible(true);
    }
    _controller->preferences()->setWindowSplitterState(_ui.splitter->saveState());
  });
  _ui.splitter->setOrientation(Qt::Vertical);
  _ui.splitter->addWidget(topWidget);
  _ui.splitter->addWidget(_ui.bottomPanel);
  _ui.splitter->setCollapsible(0, false);
  _ui.splitter->setHandleWidth(1);

  QObject::connect(_controller->preferences(), &Preferences::appConsoleVisibleChanged, this, [this]() {
    const auto bottomVisible = _controller->preferences()->appConsoleVisible();
    if (bottomVisible) {
      _ui.bottomPanel->show();
      _ui.splitter->setSizes({ _ui.splitter->sizes().at(0), 100 });
    } else {
      _ui.bottomPanel->hide();
      _ui.splitter->setSizes({ _ui.splitter->sizes().at(0), 0 });
    }
  });

  // Restore geometry from settings.
  _ui.bottomPanel->setVisible(_controller->preferences()->appConsoleVisible());
  _ui.splitter->restoreState(_controller->preferences()->windowSplitterState());

  _ui.statusBar = new StatusBar(_controller, this);

  windowLayout->setMenuBar(_ui.menuBar);
  windowLayout->addWidget(_ui.toolBar);
  windowLayout->addWidget(_ui.splitter);
  windowLayout->addWidget(_ui.statusBar);

  auto* closeWindowShortcut = new QShortcut(QKeySequence::StandardKey::Close, this, [this]() {
    close();
  });
  closeWindowShortcut->setAutoRepeat(false);
  Q_UNUSED(closeWindowShortcut) // make clang-analyzer happy.

  _ui.dropArea = new DropArea(this);
}

void MainWindow::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  _controller->preferences()->setWindowGeometry(saveGeometry());
  _controller->preferences()->setWindowSplitterState(_ui.splitter->saveState());

  _ui.dropArea->setGeometry(QRect(0, 0, width(), height()));
}

void MainWindow::closeEvent(QCloseEvent* event) {
  QWidget::closeEvent(event);
  _controller->preferences()->setWindowGeometry(saveGeometry());
  _controller->preferences()->setWindowSplitterState(_ui.splitter->saveState());
}

void MainWindow::dragEnterEvent(QDragEnterEvent* event) {
  _ui.dropArea->animateVisibility(true);

  const auto action = event->proposedAction();
  if ((action == Qt::CopyAction || action == Qt::LinkAction)) {
    const auto accept = _controller->isMimeDataValid(event->mimeData());
    if (accept) {
      event->acceptProposedAction();
    }
  }
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent* event) {
  _ui.dropArea->animateVisibility(false);
}

void MainWindow::dropEvent(QDropEvent* event) {
  _ui.dropArea->animateVisibility(false);
  const auto action = event->proposedAction();
  if ((action == Qt::CopyAction || action == Qt::LinkAction)) {
    const auto accept = _controller->isMimeDataValid(event->mimeData());
    if (accept) {
      event->acceptProposedAction();
      const auto urls = event->mimeData()->urls();
      for (const auto& url : std::as_const(urls)) {
        if (url.isLocalFile()) {
          const auto path = url.toLocalFile();
          _controller->addQuestOrFolder(path);
        }
      }
    }
  }
}
} // namespace solarus::launcher
