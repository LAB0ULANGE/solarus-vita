// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

namespace solarus::launcher {
/**
 * @brief A basic updater for the Solarus Launcher.
 * Should be replaced by the more advanced Updater class from the QtQuick launcher.
 */
class BasicUpdater : public QObject {
  Q_OBJECT

public:
  enum ErrorCode {
    NoError = 0,
    JsonError = -1,
  };

  enum class Status {
    NoUpdate,
    UpdateAvailable,
    Error,
  };

  struct Result {
    Status status{ Status::NoUpdate };
    QString currentVersion;
    QString newVersion;
    QString newVersionDownloadUrl;
    int errorCode{ 0 };
  };

public:
  explicit BasicUpdater(QObject* parent = nullptr);
  virtual ~BasicUpdater() = default;

  void checkForUpdates(const QString& endpoint);

signals:
  void checkStarted();
  void checkFinished(const solarus::launcher::BasicUpdater::Result& result);

private:
  Q_SLOT void onReplyReceived(QNetworkReply* reply);

public:
  bool _checking{ false };
  QNetworkAccessManager* _manager{ nullptr };
};
} // namespace solarus::launcher
