// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/MenuBar.h>

#include <Preferences.h>
#include <Utils.h>
#include <Controller.h>
#include <BasicUpdater.h>
#include <quests/QuestListModel.h>

#include <QApplication>
#include <QActionGroup>

#include <oclero/qlementine/style/ThemeManager.hpp>

namespace solarus::launcher {
namespace i18n {
static QString addQuestAction() {
  return QApplication::translate("SolarusLauncher", "Add Quest…");
}
/*
static QString addQuestFolderAction() {
  return QApplication::translate("SolarusLauncher", "Add Folder…");
}
*/
static QString preferences() {
  return QApplication::translate("SolarusLauncher", "Preferences…");
}
static QString quit() {
  return QApplication::translate("SolarusLauncher", "Quit");
}
static QString fileMenu() {
  return QApplication::translate("SolarusLauncher", "File");
}
static QString viewMenu() {
  return QApplication::translate("SolarusLauncher", "View");
}
static QString helpMenu() {
  return QApplication::translate("SolarusLauncher", "Help");
}
static QString contact() {
  return QApplication::translate("SolarusLauncher", "Contact");
}
static QString sourceCode() {
  return QApplication::translate("SolarusLauncher", "Source Code");
}
static QString about() {
  return QApplication::translate("SolarusLauncher", "About…");
}
static QString switchTheme() {
  return QApplication::translate("SolarusLauncher", "Switch Theme");
}
static QString theme() {
  return QApplication::translate("SolarusLauncher", "Theme");
}
static QString questMenu() {
  return QApplication::translate("SolarusLauncher", "Quest");
}
static QString showContaingFolder() {
  return QApplication::translate("SolarusLauncher", "Open Containing Folder");
}
static QString removeQuest() {
  return QApplication::translate("SolarusLauncher", "Remove Quest");
}
static QString reloadQuestsAction() {
  return QApplication::translate("SolarusLauncher", "Reload Quests");
}
static QString playQuest() {
  return QApplication::translate("SolarusLauncher", "Play Quest");
}
static QString stopQuest() {
  return QApplication::translate("SolarusLauncher", "Stop Currently Running Quest");
}
static QString showHideQuestInformation() {
  return QApplication::translate("SolarusLauncher", "Show/Hide Quest Information");
}
static QString showHideConsole() {
  return QApplication::translate("SolarusLauncher", "Show/Hide Quest Console");
}
static QString search() {
  return QApplication::translate("SolarusLauncher", "Search…");
}
static QString checkForUpdates() {
  return QApplication::translate("SolarusLauncher", "Check for Updates");
}
} // namespace i18n

MenuBar::MenuBar(Controller* controller, QWidget* parent)
  : QMenuBar(parent)
  , _controller(controller) {
  setupUi();
}

void MenuBar::setupUi() {
  setNativeMenuBar(true);

  const auto macOS = QSysInfo::productType() == "macos";

  auto* fileMenu = addMenu(i18n::fileMenu());
  {
    fileMenu->setSeparatorsCollapsible(true);

    auto* addQuestAction = fileMenu->addAction(
      makeIcon(Icons16::Action_PlusCircle, macOS), i18n::addQuestAction(), QKeySequence::StandardKey::New, [this]() {
        _controller->openAddQuestDialog();
      });
    addQuestAction->setAutoRepeat(false);

    /*
    auto* addFolderAction = fileMenu->addAction(makeIcon(Icons16::Action_AddFolder, macOS),
      i18n::addQuestFolderAction(), QKeySequence::StandardKey::Open, [this]() {
        _controller->openAddFolderDialog();
      });
    addFolderAction->setAutoRepeat(false);
    */

#ifdef __APPLE__
    const auto removeQuestShortcut = QKeySequence(Qt::Key_Backspace);
#else
    const auto removeQuestShortcut = QKeySequence(QKeySequence::StandardKey::Delete);
#endif

    auto* removeQuestAction =
      fileMenu->addAction(makeIcon(Icons16::Action_Trash, macOS), i18n::removeQuest(), removeQuestShortcut, [this]() {
        _controller->removeCurrentQuest();
      });
    removeQuestAction->setAutoRepeat(false);
    const auto canRemove = !_controller->model()->currentQuest().isEmpty();
    removeQuestAction->setEnabled(canRemove);
    QObject::connect(
      _controller->model(), &QuestListModel::currentQuestChanged, this, [removeQuestAction](const QString& path) {
        removeQuestAction->setEnabled(!path.isEmpty());
      });

    auto* refreshAction = fileMenu->addAction(makeIcon(Icons16::Action_Refresh, macOS), i18n::reloadQuestsAction(),
      QKeySequence(Qt::SHIFT | Qt::Key_R), [this]() {
        _controller->loadQuests();
      });
    refreshAction->setAutoRepeat(false);

    fileMenu->addSeparator();

    auto* playQuestAction = fileMenu->addAction(
      makeIcon(Icons16::Media_Play, macOS), i18n::playQuest(), QKeySequence{ Qt::Key_F5 }, [this]() {
        _controller->playCurrentQuest();
      });
    playQuestAction->setAutoRepeat(false);

    auto* stopQuestAction = fileMenu->addAction(
      makeIcon(Icons16::Media_Stop, macOS), i18n::stopQuest(), QKeySequence{ Qt::Key_F5 }, [this]() {
        _controller->stopQuest();
      });
    stopQuestAction->setAutoRepeat(false);

    const auto updatePlayStopActions = [this, playQuestAction, stopQuestAction]() {
      const auto* model = _controller->model();
      const auto currentQuest = model->currentQuest();
      const auto playingQuest = model->currentPlayingQuest();

      playQuestAction->setEnabled(currentQuest != playingQuest);
      stopQuestAction->setEnabled(!playingQuest.isEmpty());
    };
    updatePlayStopActions();
    QObject::connect(_controller->model(), &QuestListModel::currentQuestChanged, this, updatePlayStopActions);
    QObject::connect(_controller->model(), &QuestListModel::currentPlayingQuestChanged, this, updatePlayStopActions);

    fileMenu->addSeparator();

    auto* showContainingFolderAction = fileMenu->addAction(
      makeIcon(Icons16::File_FolderOpen, macOS), i18n::showContaingFolder(), QKeySequence{}, [this]() {
        _controller->openCurrentQuestFolder();
      });
    showContainingFolderAction->setAutoRepeat(false);

    auto* searchAction = fileMenu->addAction(
      makeIcon(Icons16::Navigation_Search, macOS), i18n::search(), QKeySequence::StandardKey::Find, [this]() {
        emit _controller->focusOnSearchFieldRequested(Qt::ShortcutFocusReason);
      });
    searchAction->setAutoRepeat(false);

    fileMenu->addSeparator();

    auto* preferencesAction = fileMenu->addAction(makeIcon(Icons16::Navigation_Settings, macOS), i18n::preferences(),
      QKeySequence::StandardKey::Preferences, [this]() {
        _controller->openPreferencesDialog();
      });
    preferencesAction->setMenuRole(QAction::MenuRole::PreferencesRole);
    preferencesAction->setAutoRepeat(false);

    fileMenu->addSeparator();

#ifdef Q_OS_WIN
    // QKeySequence::Quit is empty on Windows.
    const auto quitShortcut = QKeySequence(Qt::CTRL | Qt::Key_Q);
#else
    const auto quitShortcut = QKeySequence(QKeySequence::Quit);
#endif
    auto* quitAction = fileMenu->addAction(makeIcon(Icons16::Action_Close, macOS), i18n::quit(), quitShortcut, []() {
      qApp->quit();
    });
    quitAction->setMenuRole(QAction::MenuRole::QuitRole);
    quitAction->setAutoRepeat(false);
  }

  auto* viewMenu = addMenu(i18n::viewMenu());
  {
    viewMenu->setSeparatorsCollapsible(true);

    auto* themeMenu = viewMenu->addMenu(i18n::theme());
    themeMenu->setIcon(makeIcon(Icons16::Misc_PaintPalette, macOS));

    auto* themeActionGroup = new QActionGroup(themeMenu);
    themeActionGroup->setExclusive(true);

    auto* themeManager = _controller->themeManager();
    const auto& themes = themeManager->themes();
    const auto currentTheme = themeManager->currentTheme();

    for (const auto& theme : themes) {
      const auto name = theme.meta.name;
      const auto icon =
        name == Common::darkThemeId() ? makeIcon(Icons16::Misc_Moon, macOS) : makeIcon(Icons16::Misc_Sun, macOS);
      const auto text = Controller::themeName(theme.meta.name);
      auto* action = themeMenu->addAction(icon, text);
      action->setData(name);
      action->setCheckable(true);
      action->setAutoRepeat(false);
      themeActionGroup->addAction(action);
      action->setChecked(name == currentTheme);

      QObject::connect(action, &QAction::triggered, this, [this, name, themeManager](bool checked) {
        if (checked) {
          themeManager->setCurrentTheme(name);
        }
      });
      QObject::connect(_controller->preferences(), &Preferences::appThemeChanged, this, [this, name, action]() {
        QSignalBlocker _(action);
        action->setChecked(name == _controller->preferences()->appTheme());
      });
    }

    auto* swapThemeAction = viewMenu->addAction(
      makeIcon(Icons16::Action_Swap, macOS), i18n::switchTheme(), { Qt::CTRL | Qt::Key_T }, [this]() {
        _controller->themeManager()->setNextTheme();
      });
    swapThemeAction->setAutoRepeat(false);

    viewMenu->addSeparator();

    {
      auto* action = viewMenu->addAction(makeIcon(Icons16::Navigation_UiPanelBottom, macOS), i18n::showHideConsole(),
        { Qt::Key_F12 }, [this](bool checked) {
          _controller->preferences()->setAppConsoleVisible(checked);
        });
      action->setCheckable(true);
      action->setAutoRepeat(false);
      action->setChecked(_controller->preferences()->appConsoleVisible());

      QObject::connect(_controller->preferences(), &Preferences::appConsoleVisibleChanged, this, [this, action]() {
        QSignalBlocker _(action);
        action->setChecked(_controller->preferences()->appConsoleVisible());
      });
    }

    {
      auto* action = viewMenu->addAction(makeIcon(Icons16::Navigation_UiPanelRight, macOS),
        i18n::showHideQuestInformation(), { Qt::Key_F10 }, [this](bool checked) {
          _controller->preferences()->setAppPropertiesPanelVisible(checked);
        });
      action->setCheckable(true);
      action->setAutoRepeat(false);
      action->setChecked(_controller->preferences()->appPropertiesPanelVisible());

      QObject::connect(
        _controller->preferences(), &Preferences::appPropertiesPanelVisibleChanged, this, [this, action]() {
          QSignalBlocker _(action);
          action->setChecked(_controller->preferences()->appPropertiesPanelVisible());
        });
    }
  }

  auto* helpMenu = addMenu(i18n::helpMenu());
  {
    helpMenu->setSeparatorsCollapsible(true);

    auto* contactAction =
      helpMenu->addAction(makeIcon(Icons16::Misc_Mail, macOS), i18n::contact(), QKeySequence{}, [this]() {
        _controller->openContactPage();
      });
    contactAction->setAutoRepeat(false);

    auto* sourceCodeAction =
      helpMenu->addAction(makeIcon(Icons16::File_FileScript, macOS), i18n::sourceCode(), QKeySequence{}, [this]() {
        _controller->openSourceCodePage();
      });
    sourceCodeAction->setAutoRepeat(false);

    helpMenu->addSeparator();

    {
      auto* checkForUpdateAction =
        helpMenu->addAction(makeIcon(Icons16::Action_Update, macOS), i18n::checkForUpdates(), QKeySequence{}, [this]() {
          _controller->checkForUpdates();
        });
      checkForUpdateAction->setMenuRole(QAction::MenuRole::ApplicationSpecificRole);
      checkForUpdateAction->setAutoRepeat(false);

      QObject::connect(_controller->updater(), &BasicUpdater::checkStarted, this, [this, checkForUpdateAction]() {
        checkForUpdateAction->setEnabled(false);
      });
      QObject::connect(_controller->updater(), &BasicUpdater::checkFinished, this,
        [this, checkForUpdateAction](const BasicUpdater::Result& result) {
          checkForUpdateAction->setEnabled(true);
        });
    }

    auto* aboutAction =
      helpMenu->addAction(makeIcon(Icons16::Misc_Info, macOS), i18n::about(), QKeySequence{}, [this]() {
        _controller->openAboutDialog();
      });
    aboutAction->setMenuRole(QAction::MenuRole::AboutRole);
    aboutAction->setAutoRepeat(false);
  }
}
} // namespace solarus::launcher
