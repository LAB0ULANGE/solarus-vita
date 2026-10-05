// SPDX-License-Identifier: GPL-3.0-or-later
#include <Controller.h>

#include <Preferences.h>
#include <quests/QuestRunner.h>
#include <quests/QuestListModel.h>
#include <widgets/MessageBox.h>
#include <widgets/PreferencesWindow.h>
#include <Utils.h>
#include <BasicUpdater.h>
#include <SolarusLauncherConfig.h>

#include <oclero/qlementine/style/QlementineStyle.hpp>
#include <oclero/qlementine/style/ThemeManager.hpp>
#include <oclero/qlementine/widgets/AboutDialog.hpp>

#include <QTimer>
#include <QStandardPaths>
#include <QFileDialog>
#include <QApplication>
#include <QDesktopServices>
#include <QTranslator>
#include <QMimeData>

namespace solarus::launcher {
namespace i18n {
static QString addQuest() {
  return QApplication::translate("SolarusLauncher", "Add Quest");
}
static QString addQuestFolder() {
  return QApplication::translate("SolarusLauncher", "Add Quest Folder");
}
static QString questRemovalConfirmation() {
  return QApplication::translate("SolarusLauncher", "Do you want to remove this Solarus Quest?");
}
static QString questRemovalDescription() {
  return QApplication::translate(
    "SolarusLauncher", "The quest will be removed from Solarus Launcher index, but will be kept on disk.");
}
static QString aboutWindowTitle(const QString& appName) {
  return QApplication::translate("SolarusLauncher", "About %1").arg(appName);
}
static QString allRightsReserved() {
  return QApplication::translate("SolarusLauncher", "All rights reserved.");
}
static QString appDescription() {
  return QApplication::translate("SolarusLauncher", "A graphical user interface to launch and manage Solarus quests.");
}
static QString license() {
  return QApplication::translate("SolarusLauncher", "Licensed under GPL v3 and CC-BY-SA 4.0.");
}
static QString solarusQuests() {
  return QApplication::translate("SolarusLauncher", "Solarus Quests");
}
static QString themeName(const QString themeId) {
  if (themeId == Common::darkThemeId()) {
    return QApplication::translate("SolarusLauncher", "Dark");
  } else if (themeId == Common::lightThemeId()) {
    return QApplication::translate("SolarusLauncher", "Light");
  } else {
    return themeId;
  }
}
static QString questRunnerErrorTitle() {
  return QApplication::translate("SolarusLauncher", "An error occurred while running the quest");
}
static QString questRunnerErrorText(const QuestRunner::ErrorCode error) {
  switch (error) {
    case QuestRunner::ErrorCode::ProcessReadError:
      return QApplication::translate("SolarusLauncher", "The process had a read error.");
    case QuestRunner::ErrorCode::ProcessWriteError:
      return QApplication::translate("SolarusLauncher", "The process had a write error.");
    case QuestRunner::ErrorCode::ProcessCrashed:
      return QApplication::translate("SolarusLauncher", "The process crashed.");
    case QuestRunner::ErrorCode::ProcessFailedToStart:
      return QApplication::translate("SolarusLauncher", "The process failed to start.");
    case QuestRunner::ErrorCode::ProcessTimedOut:
      return QApplication::translate("SolarusLauncher", "The process no longer responds.");
    case QuestRunner::ErrorCode::UnknownError:
      return QApplication::translate("SolarusLauncher", "Unknown error.");
    default:
      return "";
  }
}
} // namespace i18n

namespace {
QString findBestLanguage(const QString& desiredLanguage, const QStringList& availableLanguages) {
  if (availableLanguages.size() > 0) {
    if (!desiredLanguage.isEmpty()) {
      for (const auto& lang : {
             // User's language in settings.
             desiredLanguage,
             // User's system language.
             QLocale().name(),
             // Fallback to English (US).
             QLocale(QLocale::English, QLocale::UnitedStates).name(),
           }) {
        const auto languageIndex = availableLanguages.indexOf(desiredLanguage);
        if (languageIndex != -1) {
          return desiredLanguage;
        }
      }
    }

    // Fallback: use first available language.
    return availableLanguages.front();
  }

  return QString{};
}
} // namespace

Controller::Controller(QObject* parent)
  : QObject(parent)
  , _preferences(new Preferences(this))
  , _model(new QuestListModel(this))
  , _runner(new QuestRunner(this))
  , _updater(new BasicUpdater(this)) {
  setupThemeManager();
  loadLanguages();
  setupRunner();

  // Save/load quest list from settings.
  QObject::connect(_model, &QuestListModel::questListChanged, this, [this]() {
    const auto questPathList = _model->questPathList();
    _preferences->setQuestList(questPathList);
  });
}

void Controller::setupThemeManager() {
  auto* qlementine = qobject_cast<oclero::qlementine::QlementineStyle*>(qApp->style());
  _themeManager = new oclero::qlementine::ThemeManager(qlementine, this);
  _themeManager->loadDirectory(":/solarus/launcher/resources/themes");

  const auto theme = _preferences->appTheme();
  _themeManager->setCurrentTheme(theme);

  QObject::connect(_preferences, &Preferences::appThemeChanged, this, [this]() {
    const auto theme = _preferences->appTheme();
    _themeManager->setCurrentTheme(theme);
  });
  QObject::connect(_themeManager, &oclero::qlementine::ThemeManager::currentThemeChanged, this, [this]() {
    const auto theme = _themeManager->currentTheme();
    _preferences->setAppTheme(theme);
  });
}

void Controller::setupRunner() {
  QObject::connect(_runner, &QuestRunner::stateChanged, this, [this]() {
    const auto isStopped = _runner->state() == QuestRunner::State::Stopped;
    if (isStopped) {
      if (_pendingPlayingQuestPath.isEmpty()) {
        _model->setCurrentPlayingQuest({});
      } else {
        _model->setCurrentPlayingQuest(_pendingPlayingQuestPath);
        const auto toStart = _pendingPlayingQuestPath;
        _pendingPlayingQuestPath = {};
        QTimer::singleShot(0, this, [this, toStart]() {
          startRunner(toStart);
        });
      }
    }
  });

  // Initialize.
  _runner->setSuspendWhenUnfocused(_preferences->questSuspendWhenUnfocused());
  _runner->setAudioEnabled(_preferences->questEnableAudio());
  _runner->setFullScreen(_preferences->questFullScreen());
  _runner->setForceSoftwareRendering(_preferences->questForceSoftwareRendering());

  // Connect to changes.
  QObject::connect(_preferences, &Preferences::questSuspendWhenUnfocusedChanged, this, [this]() {
    _runner->setSuspendWhenUnfocused(_preferences->questSuspendWhenUnfocused());
  });
  QObject::connect(_preferences, &Preferences::questEnableAudioChanged, this, [this]() {
    _runner->setAudioEnabled(_preferences->questEnableAudio());
  });
  QObject::connect(_preferences, &Preferences::questFullScreenChanged, this, [this]() {
    _runner->setFullScreen(_preferences->questFullScreen());
  });
  QObject::connect(_preferences, &Preferences::questForceSoftwareRenderingChanged, this, [this]() {
    _runner->setForceSoftwareRendering(_preferences->questForceSoftwareRendering());
  });

  // Handle errors and crashes.
  QObject::connect(_runner, &QuestRunner::errorRaised, this, [this](QuestRunner::ErrorCode errorCode) {
    emit mainWindowRaiseRequested();

    QTimer::singleShot(0, this, [this, errorCode]() {
      auto* msgBox = new MessageBox(qApp->activeWindow());
      msgBox->setType(MessageBox::Type::Error);
      msgBox->setTitle(i18n::questRunnerErrorTitle());
      msgBox->setButtons(MessageBox::Button::Ok);
      msgBox->setText(i18n::questRunnerErrorText(errorCode));
      msgBox->show();
    });
  });
}

void Controller::loadLanguages() {
  constexpr auto sourceDirPath = ":/i18n/";
  constexpr auto fileName = SOLARUSLAUNCHER_QM_PREFIX;
  constexpr auto separator = "_";

  constexpr auto filterFlags = QDir::Filter::NoDotAndDotDot | QDir::Filter::Files | QDir::Filter::Readable;
  constexpr auto sortFlags = QDir::SortFlag::Name;
  const auto sourceDirectory = QDir(sourceDirPath);
  const auto filePrefix = QString(fileName) + separator;
  const auto entries = sourceDirectory.entryInfoList({ QStringLiteral("*.qm") }, filterFlags, sortFlags);
  for (const auto& entry : std::as_const(entries)) {
    if (!entry.baseName().startsWith(QStringLiteral("qt"))) {
      const auto baseName = entry.baseName();
      const auto hasPrefix = !filePrefix.isEmpty() && baseName.startsWith(filePrefix);
      if (hasPrefix) {
        const auto& languageName = baseName.mid(filePrefix.length());
        _languages.append(languageName);
      }
    }
  }

  // Find best language.
  const auto userLang = findBestLanguage(_preferences->appLanguage(), _languages);
  _preferences->setAppLanguage(userLang);
  _translator = new QTranslator(this);
  if (_translator->load(QLocale(userLang), sourceDirectory.absolutePath() + "/" + fileName, separator, sourceDirPath)) {
    qApp->installTranslator(_translator);
  }

  // const auto locale = QLocale::system();
  // QLocale::setDefault(locale);
}

void Controller::loadQuests() {
  _model->setQuestPathList(_preferences->questList());
}

void Controller::openAddQuestDialog() {
  QTimer::singleShot(0, this, [this]() {
    static const auto solarusFiles = QString("%1 (*.solarus)");
    const auto filePath = QFileDialog::getOpenFileName(qApp->activeWindow(), i18n::addQuest(),
      _preferences->appLastOpenedPath(), solarusFiles.arg(i18n::solarusQuests()));
    if (!filePath.isEmpty()) {
      QTimer::singleShot(500, this, [this, filePath]() {
        addQuestOrFolder(filePath);
      });
    }
    _preferences->setAppLastOpenedPath(filePath);
  });
}

void Controller::openAddFolderDialog() {
  QTimer::singleShot(0, this, [this]() {
    const auto dirPath = QFileDialog::getExistingDirectory(
      qApp->activeWindow(), i18n::addQuestFolder(), _preferences->appLastOpenedPath());
    if (!dirPath.isEmpty()) {
      addQuestOrFolder(dirPath);
    }
    _preferences->setAppLastOpenedPath(dirPath);
  });
}

void Controller::addQuestOrFolder(const QString& path) {
  const QFileInfo fileInfo(path);
  if (fileInfo.isFile() && path.endsWith(".solarus")) {
    _model->addQuest(path);
  } else if (fileInfo.isDir()) {
    _model->addQuestFolder(path);
  }
}

void Controller::removeQuest(const QString& questFilePath) {
  if (!questFilePath.isEmpty()) {
    const auto showMsgBox = _preferences->appWarnBeforeQuestRemoval();
    if (showMsgBox) {
      auto* msgBox = new MessageBox(qApp->activeWindow());
      msgBox->setType(MessageBox::Type::Warning);
      msgBox->setTitle(i18n::questRemovalConfirmation());
      msgBox->setText(i18n::questRemovalDescription() + QString("<br/><br/><b>%1</b>").arg(questFilePath));
      msgBox->setButtons(MessageBox::Button::Cancel | MessageBox::Button::Yes);
      msgBox->setButtonIcon(MessageBox::Button::Yes, makeIcon(Icons16::Action_Trash));
      msgBox->setCheckBox("Show this message next time", true);
      QObject::connect(msgBox, &MessageBox::finished, this, [this, questFilePath, msgBox](int result) {
        const auto button = MessageBox::buttonResult(result);
        if (button == MessageBox::Yes) {
          const auto showMsgBoxNextTime = msgBox->checkBoxChecked();
          _preferences->setAppWarnBeforeQuestRemoval(showMsgBoxNextTime);

          _model->removeQuest(questFilePath);
        }
      });
      msgBox->show();
    } else {
      _model->removeQuest(questFilePath);
    }
  }
}

void Controller::removeCurrentQuest() {
  const auto questFilePath = _model->currentQuest();
  removeQuest(questFilePath);
}

void Controller::playQuest(const QString& questFilePath) {
  if (_runner->state() == QuestRunner::State::Stopped) {
    _pendingPlayingQuestPath = {};
    startRunner(questFilePath);
  } else if (_runner->questFilePath() != questFilePath) {
    _pendingPlayingQuestPath = questFilePath;
    stopQuest();
  }
}

void Controller::stopQuest() {
  _runner->stop();
  _model->setCurrentPlayingQuest({});
}

void Controller::playCurrentQuest() {
  const auto questFilePath = _model->currentQuest();
  playQuest(questFilePath);
}

void Controller::startRunner(const QString& questFilePath) {
  // Find the index of the quest. If not found, it means that the quest
  // is not indexed but started from a doube-click on the file.
  const auto row = _model->questRow(questFilePath);
  if (row >= 0) {
    _model->setCurrentPlayingQuest(questFilePath);
  } else {
    _model->setCurrentPlayingQuest({});
  }

  // Check if the file exists and is a quest file.
  const auto extension = QString(".%1").arg(SOLARUSLAUNCHER_DOCUMENT_EXTENSION);
  if (QFile::exists(questFilePath) && questFilePath.endsWith(extension)) {
    _runner->start(questFilePath);
  }
}

void Controller::openQuestFolder(const QString& questFilePath) {
  if (!questFilePath.isEmpty()) {
    const auto questFileInfo = QFileInfo(questFilePath);
    const auto questDirUrl = QUrl::fromLocalFile(questFileInfo.absoluteDir().absolutePath());
    QDesktopServices::openUrl(questDirUrl);
  }
}

void Controller::openCurrentQuestFolder() {
  const auto questFilePath = _model->currentQuest();
  openQuestFolder(questFilePath);
}

void Controller::openPreferencesDialog() {
  auto* window = new PreferencesWindow(this, qApp->activeWindow());
  window->show();
}

void Controller::openAboutDialog() {
  auto* dialog = new oclero::qlementine::AboutDialog(qApp->activeWindow());
  dialog->setIcon(QIcon(":/solarus/launcher/resources/app_icon/solarus_launcher_icon.ico"));
  dialog->setWindowTitle(i18n::aboutWindowTitle(QApplication::applicationDisplayName()));
  dialog->setWebsiteUrl(SOLARUSLAUNCHER_HOMEPAGE_URL);
  dialog->setDescription(i18n::appDescription());
  dialog->setLicense(i18n::license());
  dialog->setCopyright(QString("%1 %2").arg(SOLARUSLAUNCHER_COPYRIGHT, i18n::allRightsReserved()));
  for (const auto [tooltip, url, icon] : {
         std::make_tuple("X", SOLARUSLAUNCHER_LINKS_X, Icons16::Brand_X),
         std::make_tuple("Mastodon", SOLARUSLAUNCHER_LINKS_MASTODON, Icons16::Brand_MastodonFill),
         std::make_tuple("YouTube", SOLARUSLAUNCHER_LINKS_YOUTUBE, Icons16::Brand_YoutubeFill),
         std::make_tuple("GitLab", SOLARUSLAUNCHER_LINKS_SOURCE_CODE, Icons16::Brand_GitlabFill),
       }) {
    dialog->addSocialMediaLink(tooltip, url, makeIcon(icon));
  }
  dialog->show();
}

void Controller::playStopQuest() {
  if (_runner->state() == QuestRunner::State::Stopped) {
    const auto questFilePath = _model->currentQuest();
    playQuest(questFilePath);
  } else {
    stopQuest();
  }
}

void Controller::openContactPage() {
  QDesktopServices::openUrl(QUrl(SOLARUSLAUNCHER_LINKS_CONTACT));
}

void Controller::openSourceCodePage() {
  QDesktopServices::openUrl(QUrl(SOLARUSLAUNCHER_LINKS_SOURCE_CODE));
}

void Controller::checkForUpdates() {
  _updater->checkForUpdates(SOLARUSLAUNCHER_UPDATE_ENDPOINT);
}

const QStringList& Controller::languages() const {
  return _languages;
}

QString Controller::themeName(const QString& themeId) {
  return i18n::themeName(themeId);
}

bool Controller::isMimeDataValid(const QMimeData* mimeData) const {
  auto result = false;
  if (mimeData) {
    const auto containFiles = mimeData->hasUrls();
    if (containFiles) {
      const auto& urls = mimeData->urls();
      const auto atLeastAValidPath = std::any_of(urls.begin(), urls.end(), [](const QUrl& url) {
        if (url.isLocalFile()) {
          const auto path = url.toLocalFile();
          const auto fileInfo = QFileInfo(path);
          const auto extension = QString(".%1").arg(SOLARUSLAUNCHER_DOCUMENT_EXTENSION);
          return fileInfo.isDir() || (fileInfo.isFile() && path.endsWith(extension));
        }
        return false;
      });
      result = atLeastAValidPath;
    }
  }
  return result;
}

Preferences* Controller::preferences() {
  return _preferences;
}

QuestListModel* Controller::model() {
  return _model;
}

QuestRunner* Controller::runner() {
  return _runner;
}

BasicUpdater* Controller::updater() {
  return _updater;
}

oclero::qlementine::ThemeManager* Controller::themeManager() {
  return _themeManager;
}
} // namespace solarus::launcher
