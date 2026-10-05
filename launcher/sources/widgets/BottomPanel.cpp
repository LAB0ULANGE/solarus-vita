// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/BottomPanel.h>

#include <widgets/Console.h>
#include <Utils.h>
#include <Preferences.h>
#include <Controller.h>

#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QApplication>
#include <QPushButton>
#include <QPainter>
#include <QPlainTextEdit>
#include <QMenu>
#include <QFileDialog>
#include <QFile>

#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/utils/WidgetUtils.hpp>

namespace solarus::launcher {
namespace i18n {
static QString playingLabel() {
  return QApplication::translate("SolarusLauncher", "Playing:");
}
static QString closeConsole() {
  return QApplication::translate("SolarusLauncher", "Close console");
}
static QString clearConsole() {
  return QApplication::translate("SolarusLauncher", "Clear console");
}
static QString noQuestPlaying() {
  return QApplication::translate("SolarusLauncher", "No quest playing");
}
static QString copyAction() {
  return QApplication::translate("SolarusLauncher", "Copy");
}
static QString selectAllAction() {
  return QApplication::translate("SolarusLauncher", "Select All");
}
static QString clearAction() {
  return QApplication::translate("SolarusLauncher", "Clear");
}
static QString closeAction() {
  return QApplication::translate("SolarusLauncher", "Close");
}
static QString saveToFileAction() {
  return QApplication::translate("SolarusLauncher", "Save To File…");
}
} // namespace i18n

namespace {
void saveContentToFile(QPlainTextEdit* plainTextEdit, const QString& filePath) {
  QFile file(filePath);
  if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << plainTextEdit->toPlainText();
    file.close();
  } else {
    qDebug() << "Failed to save file: " << file.errorString();
    // QMessageBox::warning(
    //   nullptr, QObject::tr("Save Error"), QObject::tr("Failed to save file: %1").arg(file.errorString()));
  }
}
QString getDirectoryFromPath(const QString& path) {
  QFileInfo fileInfo(path);
  if (fileInfo.isDir()) {
    return fileInfo.absoluteFilePath();
  } else if (fileInfo.isFile()) {
    return fileInfo.absolutePath();
  }
  return QString();
}
} // namespace

class PlainTextEditBackground : public QWidget {
  using QWidget::QWidget;

protected:
  void paintEvent(QPaintEvent*) override {
    const auto* style = qobject_cast<oclero::qlementine::QlementineStyle*>(this->style());
    const auto& bgColor = style ? style->theme().backgroundColorWorkspace : palette().base();
    QPainter p(this);
    p.fillRect(rect(), bgColor);
  }
};

BottomPanel::BottomPanel(Controller* controller, QWidget* parent)
  : QWidget(parent)
  , _controller(controller) {
  setupUi();
}

void BottomPanel::setupUi() {
  auto* bottomWidgetLayout = new QVBoxLayout(this);
  setLayout(bottomWidgetLayout);
  bottomWidgetLayout->setSpacing(0);
  bottomWidgetLayout->setContentsMargins(0, 0, 0, 0);

  {
    auto* consoleToolBar = new QFrame(this);
    bottomWidgetLayout->addWidget(consoleToolBar);

    auto* consoleToolBarLayout = new QHBoxLayout(consoleToolBar);
    consoleToolBarLayout->setContentsMargins(12, 2, 4, 2);
    consoleToolBar->setLayout(consoleToolBarLayout);

    auto* iconWidget = new oclero::qlementine::IconWidget(consoleToolBar);
    consoleToolBarLayout->addWidget(iconWidget);

    auto* playingLayout = new QHBoxLayout();
    playingLayout->setContentsMargins(0, 0, 0, 0);
    playingLayout->setSpacing(4);
    consoleToolBarLayout->addLayout(playingLayout);

    auto* playingLabel = new QLabel(consoleToolBar);
    playingLabel->setText(QString("<b>%1</b>").arg(i18n::playingLabel()));
    playingLayout->addWidget(playingLabel);

    auto* consoleTitleLabel = new QLabel(consoleToolBar);
    consoleToolBarLayout->addWidget(consoleTitleLabel);
    playingLayout->addWidget(consoleTitleLabel);

    if (const auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(style())) {
      const auto updatePalette = [qlementine, playingLabel, consoleTitleLabel, iconWidget]() {
        const auto palette = qlementine->paletteForTextRole(oclero::qlementine::TextRole::Caption);
        playingLabel->setPalette(palette);
        consoleTitleLabel->setPalette(palette);
        iconWidget->setPalette(palette);
      };
      updatePalette();
      QObject::connect(qlementine, &oclero::qlementine::QlementineStyle::themeChanged, this, [updatePalette]() {
        updatePalette();
      });
    }

    const auto updateQuestToolBar = [this, playingLabel, consoleTitleLabel, iconWidget]() {
      const auto path = _controller->runner()->questFilePath();
      const auto hasQuest = !path.isEmpty();
      consoleTitleLabel->setText(hasQuest ? path : i18n::noQuestPlaying());
      playingLabel->setVisible(hasQuest);
      iconWidget->setIcon(makeIcon(hasQuest ? Icons16::Software_CommandLine : Icons16::Misc_Info));
    };
    updateQuestToolBar();

    QObject::connect(_controller->runner(), &QuestRunner::questChanged, this, updateQuestToolBar);

    consoleToolBarLayout->addStretch();

    _ui.clearConsoleButton = new QPushButton(consoleToolBar);
    _ui.clearConsoleButton->setToolTip(i18n::clearConsole());
    _ui.clearConsoleButton->setFocusPolicy(Qt::NoFocus);
    _ui.clearConsoleButton->setIconSize(QSize(12, 12));
    _ui.clearConsoleButton->setFixedSize(18, 18);
    _ui.clearConsoleButton->setFlat(true);
    _ui.clearConsoleButton->setIcon(makeIcon(Icons16::Action_Trash));
    consoleToolBarLayout->addWidget(_ui.clearConsoleButton);

    QObject::connect(_ui.clearConsoleButton, &QPushButton::clicked, this, [this]() {
      _ui.console->clear();
    });

    auto* closeButton = new QPushButton(consoleToolBar);
    closeButton->setToolTip(i18n::closeConsole());
    closeButton->setFocusPolicy(Qt::NoFocus);
    closeButton->setIconSize(QSize(12, 12));
    closeButton->setFixedSize(18, 18);
    closeButton->setFlat(true);
    closeButton->setIcon(makeIcon(Icons16::Action_Close));
    consoleToolBarLayout->addWidget(closeButton);

    QObject::connect(closeButton, &QPushButton::clicked, this, [this]() {
      _controller->preferences()->setAppConsoleVisible(false);
    });
  }
  {
    auto* line = oclero::qlementine::makeHorizontalLine(this);
    bottomWidgetLayout->addWidget(line);
  }
  {
    auto* consoleContainer = new PlainTextEditBackground(this);
    bottomWidgetLayout->addWidget(consoleContainer);

    consoleContainer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    auto* consoleContainerLayout = new QVBoxLayout(consoleContainer);
    consoleContainerLayout->setContentsMargins(0, 0, 0, 0);
    consoleContainer->setLayout(consoleContainerLayout);
    consoleContainerLayout->setSpacing(0);

    _ui.console = new Console(consoleContainer);
    _ui.console->setMinimumHeight(100);
    _ui.console->set_quest_runner(_controller->runner());
    consoleContainerLayout->addWidget(_ui.console);

    _ui.clearConsoleButton->setVisible(!_ui.console->is_empty());
    QObject::connect(_ui.console, &Console::empty_changed, this, [this]() {
      _ui.clearConsoleButton->setVisible(!_ui.console->is_empty());
    });

    auto* logView = _ui.console->log_view();
    logView->setContextMenuPolicy(Qt::ContextMenuPolicy::CustomContextMenu);
    QObject::connect(logView, &QWidget::customContextMenuRequested, this, [this, logView](const QPoint& pos) {
      QMenu menu(logView);

      auto* copyAction = new QAction(makeIcon(Icons16::Action_Copy), i18n::copyAction(), &menu);
      copyAction->setShortcut(QKeySequence::StandardKey::Copy);
      copyAction->setEnabled(!logView->textCursor().selectedText().isEmpty());
      menu.addAction(copyAction);
      QObject::connect(copyAction, &QAction::triggered, this, [this, logView]() {
        logView->copy();
      });

      auto* selectAllAction = new QAction(makeIcon(Icons16::Action_SelectAll), i18n::selectAllAction(), &menu);
      selectAllAction->setShortcut(QKeySequence::StandardKey::SelectAll);
      selectAllAction->setEnabled(!logView->toPlainText().isEmpty());
      menu.addAction(selectAllAction);
      QObject::connect(selectAllAction, &QAction::triggered, this, [this, logView]() {
        logView->selectAll();
      });

      auto* clearAction = new QAction(makeIcon(Icons16::Action_Trash), i18n::clearAction(), &menu);
      clearAction->setEnabled(!logView->toPlainText().isEmpty());
      menu.addAction(clearAction);
      QObject::connect(clearAction, &QAction::triggered, this, [this]() {
        _ui.console->clear();
      });

      menu.addSeparator();

      auto* saveToFileAction = new QAction(makeIcon(Icons16::Action_Download), i18n::saveToFileAction(), &menu);
      saveToFileAction->setEnabled(!logView->toPlainText().isEmpty());
      menu.addAction(saveToFileAction);
      QObject::connect(saveToFileAction, &QAction::triggered, this, [this]() {
        QTimer::singleShot(0, this, [this]() {
          saveConsoleContentToFile();
        });
      });

      menu.addSeparator();

      auto* closeAction = new QAction(makeIcon(Icons16::Action_Close), i18n::closeAction(), &menu);
      closeAction->setShortcut({ Qt::Key_F12 });
      QObject::connect(closeAction, &QAction::triggered, this, [this]() {
        _controller->preferences()->setAppConsoleVisible(false);
      });
      menu.addAction(closeAction);

      const auto globalPos = logView->mapToGlobal(pos);
      menu.exec(globalPos);
    });
  }
}

void BottomPanel::saveConsoleContentToFile() {
  const auto lastDir = getDirectoryFromPath(_controller->preferences()->appLastConsoleSavePath());
  const auto questName = _controller->runner()->questFilePath().section('/', -1).section('.', 0, 0);
  const auto currentDateTime = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
  const auto defaultFileName = QString("%2_%3.txt").arg(questName, currentDateTime);
  const auto defaultFilePath = lastDir + '/' + defaultFileName;

  const auto filePath = QFileDialog::getSaveFileName(
    qApp->activeWindow(), i18n::saveToFileAction(), defaultFilePath, "Text Files (*.txt);;All Files (*)");

  if (!filePath.isEmpty()) {
    saveContentToFile(_ui.console->log_view(), filePath);

    const auto saveDir = getDirectoryFromPath(QFileInfo(filePath).absolutePath());
    _controller->preferences()->setAppLastConsoleSavePath(saveDir);
  }
}
} // namespace solarus::launcher
