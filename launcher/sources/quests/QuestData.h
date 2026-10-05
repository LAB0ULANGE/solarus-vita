// SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include <QString>
#include <QDate>
#include <QVersionNumber>
#include <QStringList>
#include <QUrl>
#include <QPixmap>

#include <Common.h>

namespace solarus::launcher {
struct QuestData {
  bool isValid{ false };
  QString path;
  QString title;
  QStringList authors;
  QDate initialReleaseDate;
  QDate latestReleaseDate;
  QString description;
  QVersionNumber version;
  QVersionNumber engineVersion;
  QStringList licenses;
  QStringList languages;
  uint minPlayers{ 0 };
  uint maxPlayers{ 0 };
  QStringList genres;
  QUrl website;
  Common::AgeRating ageRating{ Common::AgeRating::Unknown };
  Common::Controls controls{ Common::Control::None };
  QString id;
  QPixmap thumbnail;
};
} // namespace solarus::launcher

Q_DECLARE_METATYPE(solarus::launcher::QuestData)
