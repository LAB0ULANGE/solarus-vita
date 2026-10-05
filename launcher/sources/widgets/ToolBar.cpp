// SPDX-License-Identifier: GPL-3.0-or-later
#include <widgets/ToolBar.h>

#include <quests/QuestListModel.h>
#include <quests/QuestRunner.h>
#include <Utils.h>
#include <BasicUpdater.h>
#include <Preferences.h>
#include <Controller.h>

#include <QApplication>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QDesktopServices>
#include <QToolButton>
#include <QMenu>
#include <QSortFilterProxyModel>

#include <oclero/qlementine/widgets/IconWidget.hpp>
#include <oclero/qlementine/widgets/LineEdit.hpp>
#include <oclero/qlementine/widgets/Switch.hpp>

namespace solarus::launcher {
namespace i18n {
static QString addQuest() {
  return QApplication::translate("SolarusLauncher", "Add Quest");
}
static QString addQuestAction() {
  return QApplication::translate("SolarusLauncher", "Add Quest…");
}
/*
static QString addQuestFolderAction() {
  return QApplication::translate("SolarusLauncher", "Add Folder…");
}
*/
static QString removeQuest() {
  return QApplication::translate("SolarusLauncher", "Remove Quest");
}
static QString play() {
  return QApplication::translate("SolarusLauncher", "Play");
}
static QString stop() {
  return QApplication::translate("SolarusLauncher", "Stop");
}
static QString playQuest() {
  return QApplication::translate("SolarusLauncher", "Play Quest");
}
static QString stopQuest() {
  return QApplication::translate("SolarusLauncher", "Stop Quest");
}
static QString search() {
  return QApplication::translate("SolarusLauncher", "Search…");
}
static QString switchTheme() {
  return QApplication::translate("SolarusLauncher", "Switch Theme");
}
static QString showHideQuestInformation() {
  return QApplication::translate("SolarusLauncher", "Show/Hide Quest Information");
}
static QString showHideConsole() {
  return QApplication::translate("SolarusLauncher", "Show/Hide Quest Console");
}
static QString preferences() {
  return QApplication::translate("SolarusLauncher", "Preferences…");
}
} // namespace i18n

ToolBar::ToolBar(Controller* controller, QWidget* parent)
  : QToolBar(parent)
  , _controller(controller) {
  setupUi();
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
}

void ToolBar::setupUi() {
  const auto defaultIconSize = style()->pixelMetric(QStyle::PM_SmallIconSize);
  setAllowedAreas(Qt::ToolBarArea::TopToolBarArea);
  setMovable(false);
  setFloatable(false);
  setIconSize(QSize(defaultIconSize, defaultIconSize));
  setBackgroundRole(QPalette::ColorRole::Window);
  setAutoFillBackground(false);
  setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonFollowStyle);

  auto* addQuestButton = new QToolButton(this);
  {
    addQuestButton->setText(i18n::addQuest());
    addQuestButton->setIcon(makeIcon(Icons16::Action_PlusCircle));
    addQuestButton->setToolTip(i18n::addQuest());
    addQuestButton->setCheckable(false);
    addQuestButton->setChecked(false);
    addQuestButton->setAutoRepeat(false);
    addQuestButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
    addQuestButton->setFocusPolicy(Qt::NoFocus);
    addWidget(addQuestButton);
    QObject::connect(addQuestButton, &QPushButton::clicked, this, [this]() {
      _controller->openAddQuestDialog();
    });

    /* Disable submenu for now, waiting for the "add folder" action to work

    {
      auto* subMenu = new QMenu(i18n::addQuest(), addQuestButton);
      addQuestButton->setMenu(subMenu);
      addQuestButton->setPopupMode(QToolButton::ToolButtonPopupMode::MenuButtonPopup);

      auto* addQuestAction = subMenu->addAction(makeIcon(Icons16::Action_PlusCircle), i18n::addQuestAction());
      addQuestAction->setShortcut(QKeySequence::StandardKey::New);
      addQuestAction->setAutoRepeat(false);
      QObject::connect(addQuestAction, &QAction::triggered, this, [this]() {
        _controller->openAddQuestDialog();
      });

      auto* addFolderAction = subMenu->addAction(makeIcon(Icons16::Action_AddFolder), i18n::addQuestFolderAction());
      addFolderAction->setShortcut(QKeySequence::StandardKey::Open);
      addFolderAction->setAutoRepeat(false);
      QObject::connect(addFolderAction, &QAction::triggered, this, [this]() {
        _controller->openAddFolderDialog();
      });
    }
    */
  }

  auto* removeQuestButton = new QToolButton(this);
  {
    removeQuestButton->setText(i18n::removeQuest());
    removeQuestButton->setIcon(makeIcon(Icons16::Action_Trash));
    removeQuestButton->setToolTip(i18n::removeQuest());
    removeQuestButton->setCheckable(false);
    removeQuestButton->setChecked(false);
    removeQuestButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    removeQuestButton->setFocusPolicy(Qt::NoFocus);
    removeQuestButton->setEnabled(false);
    removeQuestButton->setAutoRepeat(false);
    addWidget(removeQuestButton);

    QObject::connect(removeQuestButton, &QPushButton::clicked, this, [this]() {
      _controller->removeCurrentQuest();
    });

    QObject::connect(
      _controller->model(), &QuestListModel::currentQuestChanged, this, [removeQuestButton](const QString& path) {
        removeQuestButton->setEnabled(!path.isEmpty());
      });
  }

  addSeparator();

  auto* playStopQuestButton = new QToolButton(this);
  {
    playStopQuestButton->setCheckable(false);
    playStopQuestButton->setChecked(false);
    playStopQuestButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonTextBesideIcon);
    playStopQuestButton->setFocusPolicy(Qt::NoFocus);
    addWidget(playStopQuestButton);

    // Ensure the button's width does not change.
    playStopQuestButton->ensurePolished();
    playStopQuestButton->setIcon(makeIcon(Icons16::Media_Play));
    playStopQuestButton->setText(i18n::play());
    const auto w1 = playStopQuestButton->sizeHint().width();
    playStopQuestButton->setText(i18n::stop());
    const auto w2 = playStopQuestButton->sizeHint().width();
    const auto w = std::max(w1, w2);
    playStopQuestButton->setFixedWidth(w);

    QObject::connect(playStopQuestButton, &QPushButton::clicked, this, [this]() {
      _controller->playStopQuest();
    });

    const auto updatePlayStopButton = [this, playStopQuestButton]() {
      const auto hasCurrent = !_controller->model()->currentQuest().isEmpty();

      switch (_controller->runner()->state()) {
        case QuestRunner::State::Stopped:
          playStopQuestButton->setText(i18n::play());
          playStopQuestButton->setToolTip(i18n::playQuest());
          playStopQuestButton->setIcon(makeIcon(Icons16::Media_Play));
          playStopQuestButton->setEnabled(hasCurrent);
          break;
        case QuestRunner::State::Running:
          playStopQuestButton->setText(i18n::stop());
          playStopQuestButton->setToolTip(i18n::stopQuest());
          playStopQuestButton->setIcon(makeIcon(Icons16::Media_Stop));
          playStopQuestButton->setEnabled(true);
          break;
        case QuestRunner::State::Starting:
          playStopQuestButton->setEnabled(false);
        default:
          break;
      }
    };
    updatePlayStopButton();
    QObject::connect(_controller->runner(), &QuestRunner::stateChanged, this, updatePlayStopButton);
    QObject::connect(_controller->model(), &QuestListModel::currentQuestChanged, this, updatePlayStopButton);
  }

  // Spacer.
  auto* spacerWidget = new QWidget(this);
  {
    spacerWidget->setAttribute(Qt::WA_TransparentForMouseEvents);
    spacerWidget->setMinimumSize(0, 0);
    spacerWidget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Ignored);
    spacerWidget->setUpdatesEnabled(false); // No paint events.
    addWidget(spacerWidget);
  }

  auto* searchLineEdit = new oclero::qlementine::LineEdit(this);
  {
    const auto shortcut =
      QKeySequence(QKeySequence::StandardKey::Find).toString(QKeySequence::SequenceFormat::NativeText);
    searchLineEdit->setPlaceholderText(QString("%1 (%2)").arg(i18n::search(), shortcut));
    searchLineEdit->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    searchLineEdit->setFixedWidth(180);
    searchLineEdit->setClearButtonEnabled(true);
    searchLineEdit->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
    searchLineEdit->setIcon(makeIcon(Icons16::Navigation_Search));
    addWidget(searchLineEdit);

    QObject::connect(searchLineEdit, &QLineEdit::textEdited, this, [this](const QString& text) {
      _controller->model()->proxyModel()->setFilterFixedString(text);
    });

    auto* cancelAction = searchLineEdit->addAction({}, QKeySequence::StandardKey::Cancel);
    QObject::connect(cancelAction, &QAction::triggered, this, [this, searchLineEdit]() {
      searchLineEdit->clear();
      emit searchLineEdit->textEdited(searchLineEdit->text());
      emit _controller->focusOnListViewRequested(Qt::ShortcutFocusReason);
    });

    QObject::connect(searchLineEdit, &QLineEdit::returnPressed, this, [this]() {
      emit _controller->focusOnListViewRequested(Qt::ShortcutFocusReason);
    });

    QObject::connect(
      _controller, &Controller::focusOnSearchFieldRequested, this, [this, searchLineEdit](Qt::FocusReason reason) {
        searchLineEdit->setFocus(reason);
      });
  }

  auto* themeWidget = new QWidget(this);
  {
    themeWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    addWidget(themeWidget);

    auto* themeLayout = new QHBoxLayout(themeWidget);
    themeLayout->setSpacing(4);
    themeLayout->setContentsMargins(8, 0, 8, 0);
    themeWidget->setLayout(themeLayout);

    auto* lightIconWidget = new oclero::qlementine::IconWidget(makeIcon(Icons16::Misc_Sun), themeWidget);
    auto* darkIconWidget = new oclero::qlementine::IconWidget(makeIcon(Icons16::Misc_Moon), themeWidget);

    auto* themeSwitch = new oclero::qlementine::Switch(themeWidget);
    themeSwitch->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    themeSwitch->setToolTip(i18n::switchTheme());

    const auto updateThemeSwitch = [this, themeSwitch]() {
      QSignalBlocker _(themeSwitch);
      themeSwitch->setChecked(_controller->preferences()->appTheme() == Common::darkThemeId());
    };
    QObject::connect(themeSwitch, &oclero::qlementine::Switch::clicked, this, [this](auto checked) {
      _controller->preferences()->setAppTheme(checked ? Common::darkThemeId() : Common::lightThemeId());
    });
    QObject::connect(_controller->preferences(), &Preferences::appThemeChanged, this, updateThemeSwitch);

    themeLayout->addWidget(lightIconWidget);
    themeLayout->addWidget(themeSwitch);
    themeLayout->addWidget(darkIconWidget);

    updateThemeSwitch();
  }

  addSeparator();

  auto* preferencesButton = new QToolButton(this);
  {
    preferencesButton->setIcon(makeIcon(Icons16::Navigation_Settings));
    preferencesButton->setToolTip(i18n::preferences());
    preferencesButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    preferencesButton->setFocusPolicy(Qt::NoFocus);
    addWidget(preferencesButton);

    QObject::connect(preferencesButton, &QPushButton::clicked, this, [this](bool checked) {
      _controller->openPreferencesDialog();
    });
  }

  auto* toggleConsoleButton = new QToolButton(this);
  {
    toggleConsoleButton->setText(i18n::showHideConsole());
    toggleConsoleButton->setIcon(makeIcon(Icons16::Navigation_UiPanelBottom));
    toggleConsoleButton->setToolTip(i18n::showHideConsole());
    toggleConsoleButton->setCheckable(true);
    toggleConsoleButton->setChecked(_controller->preferences()->appConsoleVisible());
    toggleConsoleButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    toggleConsoleButton->setFocusPolicy(Qt::NoFocus);
    addWidget(toggleConsoleButton);

    QObject::connect(toggleConsoleButton, &QPushButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setAppConsoleVisible(checked);
    });

    QObject::connect(
      _controller->preferences(), &Preferences::appConsoleVisibleChanged, this, [this, toggleConsoleButton]() {
        QSignalBlocker _(toggleConsoleButton);
        toggleConsoleButton->setChecked(_controller->preferences()->appConsoleVisible());
      });
  }

  auto* togglePanelButton = new QToolButton(this);
  {
    togglePanelButton->setText(i18n::showHideQuestInformation());
    togglePanelButton->setIcon(makeIcon(Icons16::Navigation_UiPanelRight));
    togglePanelButton->setToolTip(i18n::showHideQuestInformation());
    togglePanelButton->setCheckable(true);
    togglePanelButton->setChecked(_controller->preferences()->appPropertiesPanelVisible());
    togglePanelButton->setToolButtonStyle(Qt::ToolButtonStyle::ToolButtonIconOnly);
    togglePanelButton->setFocusPolicy(Qt::NoFocus);
    addWidget(togglePanelButton);

    QObject::connect(togglePanelButton, &QPushButton::clicked, this, [this](bool checked) {
      _controller->preferences()->setAppPropertiesPanelVisible(checked);
    });

    QObject::connect(
      _controller->preferences(), &Preferences::appPropertiesPanelVisibleChanged, this, [this, togglePanelButton]() {
        QSignalBlocker _(togglePanelButton);
        togglePanelButton->setChecked(_controller->preferences()->appPropertiesPanelVisible());
      });
  }
}
} // namespace solarus::launcher
