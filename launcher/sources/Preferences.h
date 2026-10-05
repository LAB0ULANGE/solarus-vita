// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>
#include <QSettings>
#include <QString>
#include <QByteArray>

namespace solarus::launcher {
/**
 * @brief Unique entry point for the application preferences.
 */
class Preferences : public QObject {
  Q_OBJECT

public:
  explicit Preferences(QObject* parent = nullptr);
  virtual ~Preferences();

  void resetToDefaults();

public: // App preferences.
  QString appLanguage() const;
  void setAppLanguage(const QString& value);
  Q_SIGNAL void appLanguageChanged();

  bool appPropertiesPanelVisible() const;
  void setAppPropertiesPanelVisible(bool value);
  Q_SIGNAL void appPropertiesPanelVisibleChanged();

  bool appConsoleVisible() const;
  void setAppConsoleVisible(bool value);
  Q_SIGNAL void appConsoleVisibleChanged();

  QString appTheme() const;
  void setAppTheme(const QString& value);
  Q_SIGNAL void appThemeChanged();

  QStringList questList() const;
  void setQuestList(const QStringList& value);
  void addQuestToList(const QString& value);
  Q_SIGNAL void questListChanged();

  QString appLastOpenedPath() const;
  void setAppLastOpenedPath(const QString& path);
  Q_SIGNAL void appLastOpenedPathChanged();

  QString appLastConsoleSavePath() const;
  void setAppLastConsoleSavePath(const QString& path);
  Q_SIGNAL void appLastConsoleSavePathChanged();

  bool appWarnBeforeQuestRemoval() const;
  void setAppWarnBeforeQuestRemoval(bool value);
  Q_SIGNAL void appWarnBeforeQuestRemovalChanged();

public: // Window preferences.
  QByteArray windowGeometry() const;
  void setWindowGeometry(const QByteArray& value);

  QByteArray windowSplitterState() const;
  void setWindowSplitterState(const QByteArray& value);

public: // Quest preferences.
  bool questEnableAudio() const;
  void setQuestEnableAudio(bool value);
  Q_SIGNAL void questEnableAudioChanged();

  bool questForceSoftwareRendering() const;
  void setQuestForceSoftwareRendering(bool value);
  Q_SIGNAL void questForceSoftwareRenderingChanged();

  bool questFullScreen() const;
  void setQuestFullScreen(bool value);
  Q_SIGNAL void questFullScreenChanged();

  bool questSuspendWhenUnfocused() const;
  void setQuestSuspendWhenUnfocused(bool value);
  Q_SIGNAL void questSuspendWhenUnfocusedChanged();

private:
  QSettings _qSettings;
};
} // namespace solarus::launcher
