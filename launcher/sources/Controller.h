// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QStringList>
#include <QModelIndex>

class QTranslator;
class QMimeData;

namespace oclero::qlementine {
class ThemeManager;
} // namespace oclero::qlementine

namespace solarus::launcher {
class Preferences;
class QuestListModel;
class QuestRunner;
class BasicUpdater;

/**
 * @brief The controller of the Solarus Launcher.
 * Manages the main actions of the application.
 */
class Controller : public QObject {
  Q_OBJECT

public:
  explicit Controller(QObject* parent = nullptr);
  virtual ~Controller() = default;

  void loadQuests();
  void openAddQuestDialog();
  void openAddFolderDialog();
  void addQuestOrFolder(const QString& path);
  void openPreferencesDialog();
  void openAboutDialog();
  void openContactPage();
  void openSourceCodePage();
  void openQuestPropertiesPanel();

  void removeQuest(const QString& path);
  void removeCurrentQuest();

  void playQuest(const QString& path);
  void stopQuest();
  void playCurrentQuest();
  void playStopQuest();

  void openQuestFolder(const QString& path);
  void openCurrentQuestFolder();

  void checkForUpdates();

  const QStringList& languages() const;
  static QString themeName(const QString& themeName);

  Q_SIGNAL void focusOnListViewRequested(Qt::FocusReason reason);
  Q_SIGNAL void focusOnSearchFieldRequested(Qt::FocusReason reason);
  Q_SIGNAL void mainWindowRaiseRequested();

  bool isMimeDataValid(const QMimeData* mimeData) const;

  Preferences* preferences();
  QuestListModel* model();
  QuestRunner* runner();
  BasicUpdater* updater();
  oclero::qlementine::ThemeManager* themeManager();

private:
  void setupThemeManager();
  void setupRunner();
  void loadLanguages();
  void startRunner(const QString& path);

private:
  Preferences* _preferences{ nullptr };
  QuestListModel* _model{ nullptr };
  QuestRunner* _runner{ nullptr };
  BasicUpdater* _updater{ nullptr };
  oclero::qlementine::ThemeManager* _themeManager{ nullptr };
  QStringList _languages{};
  QTranslator* _translator{ nullptr };
  QString _pendingPlayingQuestPath{};
};
} // namespace solarus::launcher
