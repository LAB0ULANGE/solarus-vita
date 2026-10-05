// SPDX-License-Identifier: GPL-3.0-or-later
#include <Common.h>

#include <QLocale>

namespace solarus::launcher {
QString Common::languageName(const QString& langCode) {
  const auto locale = QLocale(langCode);
  auto result = locale.nativeLanguageName();
  if (!result.isEmpty()) {
    result[0] = result[0].toUpper();
  }
  if (result == "American English") {
    result = "English";
  }
  return result;
}

QIcon Common::languageIcon(const QString& langCode) {
  constexpr auto flagsDirPath = ":/solarus/launcher/resources/flags/";
  static const auto mapping = QMap<QString, QString>{
    { "en_US", "en.svg" },
    { "fr_FR", "fr.svg" },
  };
  const auto svgFileName = mapping.value(langCode);
  return svgFileName.isEmpty() ? QIcon() : QIcon(flagsDirPath + svgFileName);
}

QIcon Common::ageIcon(const Common::AgeRating value) {
  constexpr auto basePath = ":/solarus/launcher/resources/age_ratings/";
  switch (value) {
    case Common::AgeRating::All:
      return QIcon(basePath + QString("all.svg"));
    case Common::AgeRating::Restricted:
      return QIcon(basePath + QString("restricted.svg"));
    case Common::AgeRating::Warning:
      return QIcon(basePath + QString("warning.svg"));
    default:
      return QIcon();
  }
}

QString Common::darkThemeId() {
  return "Dark";
}

QString Common::lightThemeId() {
  return "Light";
}
} // namespace solarus::launcher
