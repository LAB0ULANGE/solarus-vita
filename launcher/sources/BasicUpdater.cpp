// SPDX-License-Identifier: GPL-3.0-or-later
#include <BasicUpdater.h>

#include <QDesktopServices>
#include <QApplication>
#include <QNetworkRequest>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QJsonDocument>
#include <QVersionNumber>
#include <QUrl>

namespace solarus::launcher {
namespace {
static QVersionNumber getCurrentVersion() {
  return QVersionNumber::fromString(QApplication::applicationVersion());
}

static BasicUpdater::Result fromGitLabApiV4LatestRelease(const QJsonDocument& doc) {
  const auto currentVersion = getCurrentVersion();

  auto result = BasicUpdater::Result{
    BasicUpdater::Status::Error,
    currentVersion.toString(),
    {},
    {},
    BasicUpdater::ErrorCode::JsonError,
  };

  if (doc.isObject()) {
    const auto jsonObj = doc.object();
    if (jsonObj.contains("tag_name")) {
      auto tagName = jsonObj.value("tag_name").toString();
      if (!tagName.isEmpty()) {
        if (tagName.startsWith('v', Qt::CaseInsensitive)) {
          tagName.remove(0, 1);
        }
        const auto latestVersion = QVersionNumber::fromString(tagName);
        if (!latestVersion.isNull()) {
          if (latestVersion > currentVersion) {
            if (jsonObj.contains("_links")) {
              const auto linksObj = jsonObj.value("_links").toObject();
              if (linksObj.contains("self")) {
                const auto releasePageUrl = linksObj.value("self").toString();
                result = {
                  BasicUpdater::Status::UpdateAvailable,
                  currentVersion.toString(),
                  latestVersion.toString(),
                  releasePageUrl,
                  BasicUpdater::ErrorCode::NoError,
                };
              }
            }
          } else {
            result = {
              BasicUpdater::Status::NoUpdate,
              currentVersion.toString(),
              currentVersion.toString(),
              {},
              BasicUpdater::ErrorCode::NoError,
            };
          }
        }
      }
    }
  }
  return result;
}
} // namespace

BasicUpdater::BasicUpdater(QObject* parent)
  : QObject(parent)
  , _manager(new QNetworkAccessManager(this)) {
  QObject::connect(_manager, &QNetworkAccessManager::finished, this, &BasicUpdater::onReplyReceived);
}

void BasicUpdater::checkForUpdates(const QString& endpoint) {
  if (!_checking) {
    _checking = true;
    emit checkStarted();
    auto request = QNetworkRequest(QUrl(endpoint));
    request.setAttribute(QNetworkRequest::RedirectPolicyAttribute, QNetworkRequest::SameOriginRedirectPolicy);
    request.setTransferTimeout(5000);
    _manager->get(request);
  }
}

void BasicUpdater::onReplyReceived(QNetworkReply* reply) {
  _checking = false;
  const auto error = reply->error();
  if (error == QNetworkReply::NoError) {
    const auto responseData = reply->readAll();
    const auto jsonDoc = QJsonDocument::fromJson(responseData);
    const auto result = fromGitLabApiV4LatestRelease(jsonDoc);
    emit checkFinished(result);
  } else {
    const auto result = BasicUpdater::Result{
      BasicUpdater::Status::Error,
      getCurrentVersion().toString(),
      {},
      {},
      error,
    };
    emit checkFinished(result);
  }
  reply->deleteLater();
}
} // namespace solarus::launcher
