/*
 * Copyright (C) 2014-2018 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "editor_exception.h"
#include "quest.h"
#include "quest_properties.h"
#include "size.h"

namespace SolarusEditor {

namespace {

/**
 * @brief Converts a vector of strings to a QStringList (QVector<QString>).
 * @param list The vector to convert.
 * @return The QStringList.
 */
QStringList to_qstring_list(const std::vector<std::string>& list) {

  QStringList result;
  result.reserve(list.size());
  std::transform(list.begin(), list.end(), std::back_inserter(result),
                 [](const std::string &str) { return QString::fromStdString(str); });
  return result;
}

/**
 * @brief Converts a QStringList (QVector<QString>) to a vector of strings.
 * @param list The QStringList to convert.
 * @return The vector of strings.
 */
std::vector<std::string> to_string_vector(const QStringList& list) {

  std::vector<std::string> result;
  result.reserve(list.size());
  std::transform(list.begin(), list.end(), std::back_inserter(result),
                 [](const QString &str) { return str.toStdString(); });
  return result;
}

} // anynomous namespace

/**
 * @brief Creates quest properties for the specified quest.
 * @param quest The quest.
 */
QuestProperties::QuestProperties(Quest& quest) :
  quest(quest) {

  connect(&quest, &Quest::root_path_changed,
          this, &QuestProperties::reload);
  reload();
}

/**
 * @brief Reads quest.dat into this object.
 */
void QuestProperties::reload() {

  if (!quest.is_valid() || !quest.exists()) {
    return;
  }

  QString file_name = quest.get_properties_path();
  if (!properties.import_from_file(file_name.toLocal8Bit().toStdString())) {
    throw EditorException(tr("Cannot open file '%1'").arg(file_name));
  }
}

/**
 * @brief Saves the properties to the quest.dat file of the quest.
 * @throws EditorException If the save operation failed.
 */
void QuestProperties::save() const {

  if (!quest.is_valid() || !quest.exists()) {
    throw EditorException(tr("No quest"));
  }

  QString file_name = quest.get_properties_path();
  if (!properties.export_to_file(file_name.toLocal8Bit().toStdString())) {
    throw EditorException(tr("Cannot write file '%1'").arg(file_name));
  }
}

/**
 * @brief Returns the Solarus compatibility version of the quest.
 * @return The Solarus version.
 */
QString QuestProperties::get_solarus_version() const {

  return QString::fromStdString(properties.get_solarus_version());
}

/**
 * @brief Returns the Solarus compatibility version of the quest, without patch number.
 * @return The Solarus version without patch number.
 */
QString QuestProperties::get_solarus_version_without_patch() const {

  QString version = get_solarus_version();

  if (version.isEmpty()) {
    return version;
  }

  int dot_index_1 = static_cast<int>(version.indexOf('.'));
  int dot_index_2 = static_cast<int>(version.indexOf('.', dot_index_1 + 1));
  if (dot_index_2 != -1) {
    // Remove the patch version (it does not break compatibility).
    version = version.section('.', 0, -2);
  }
  return version;
}

/**
 * @brief Returns the write directory of the quest.
 * @return The write directory.
 */
QString QuestProperties::get_write_dir() const {

  return QString::fromStdString(properties.get_quest_write_dir());
}

/**
 * @brief Changes the write directory of the quest.
 * @param write_dir The new write directory.
 */
void QuestProperties::set_write_dir(const QString& write_dir) {

  QString old_write_dir = get_write_dir();
  if (write_dir == old_write_dir) {
    return;
  }

  properties.set_quest_write_dir(write_dir.toStdString());
  emit write_dir_changed(write_dir);
}

/**
 * @brief Returns the title of the quest.
 * @return The title.
 */
QString QuestProperties::get_title() const {

  return QString::fromStdString(properties.get_title());
}

/**
 * @brief Changes the title of the quest.
 * @param title The new title.
 */
void QuestProperties::set_title(const QString& title) {

  QString old_title = get_title();
  if (title == old_title) {
    return;
  }

  properties.set_title(title.toStdString());
  emit title_changed(title);
}

/**
 * @brief Returns the one-line description of the quest.
 * @return The one-line description.
 */
QString QuestProperties::get_short_description() const {

  return QString::fromStdString(properties.get_short_description());
}

/**
 * @brief Changes the one-line description of the quest.
 * @param short_description The one-line description.
 */
void QuestProperties::set_short_description(const QString& short_description) {

  QString old_short_description = get_short_description();
  if (short_description == old_short_description) {
    return;
  }

  properties.set_short_description(short_description.toStdString());
  emit short_description_changed(short_description);
}

/**
 * @brief Returns the one-line description of the quest.
 * @return The one-line description.
 */
QString QuestProperties::get_long_description() const {

  return QString::fromStdString(properties.get_long_description());
}

/**
 * @brief Changes the one-line description of the quest.
 * @param long_description The one-line description.
 */
void QuestProperties::set_long_description(const QString& long_description) {

  QString old_long_description = get_long_description();
  if (long_description == old_long_description) {
    return;
  }

  properties.set_long_description(long_description.toStdString());
  emit long_description_changed(long_description);
}

/**
 * @brief Returns the author of the quest.
 * @return The author.
 */
QString QuestProperties::get_author() const {

  return QString::fromStdString(properties.get_author());
}

/**
 * @brief Changes the author of the quest.
 * @param author The author.
 */
void QuestProperties::set_author(const QString& author) {

  QString old_author = get_author();
  if (author == old_author) {
    return;
  }

  properties.set_author(author.toStdString());
  emit author_changed(author);
}

/**
 * @brief Returns the version of the quest.
 * @return The version.
 */
QString QuestProperties::get_quest_version() const {

  return QString::fromStdString(properties.get_quest_version());
}

/**
 * @brief Changes the version of the quest.
 * @param quest_version The version.
 */
void QuestProperties::set_quest_version(const QString& quest_version) {

  QString old_quest_version = get_quest_version();
  if (quest_version == old_quest_version) {
    return;
  }

  properties.set_quest_version(quest_version.toStdString());
  emit quest_version_changed(quest_version);
}

/**
 * @brief Returns the quest initial release date.
 * @return The release date or an invalid date.
 */
QDate QuestProperties::get_initial_release_date() const {

  QString date_string = QString::fromStdString(properties.get_initial_release_date());
  return QDate::fromString(date_string, "yyyyMMdd");
}


/**
 * @brief Changes the quest initial release date.
 * @param release_date The release date or an invalid date.
 */
void QuestProperties::set_initial_release_date(const QDate &initial_release_date) {

  const QDate old_intial_release_date = get_initial_release_date();
  if (initial_release_date == old_intial_release_date) {
    return;
  }

  QString date_string;
  if (initial_release_date.isValid()) {
    date_string = initial_release_date.toString("yyyyMMdd");
  }
  properties.set_initial_release_date(date_string.toStdString());
  emit initial_release_date_changed(initial_release_date);
}

/**
 * @brief Returns the website of the quest.
 * @return The website.
 */
QString QuestProperties::get_website() const {

  return QString::fromStdString(properties.get_website());
}

/**
 * @brief Changes the website of the quest.
 * @param website The website.
 */
void QuestProperties::set_website(const QString& website) {

  QString old_website = get_website();
  if (website == old_website) {
    return;
  }

  properties.set_website(website.toStdString());
  emit website_changed(website);
}

/**
 * @brief Returns the quest release date.
 * @return The release date or an invalid date.
 */
QDate QuestProperties::get_release_date() const {

  QString date_string = QString::fromStdString(properties.get_release_date());
  return QDate::fromString(date_string, "yyyyMMdd");
}

/**
 * @brief Changes the quest release date.
 * @param release_date The release date or an invalid date.
 */
void QuestProperties::set_release_date(const QDate& release_date) {

  QDate old_release_date = get_release_date();
  if (release_date == old_release_date) {
    return;
  }

  QString date_string;
  if (release_date.isValid()) {
    date_string = release_date.toString("yyyyMMdd");
  }
  properties.set_release_date(date_string.toStdString());
  emit release_date_changed(release_date);
}

/**
 * @brief Returns the normal size of the quest.
 * @return The normal size.
 */
QSize QuestProperties::get_normal_quest_size() const {

  return Size::to_qsize(properties.get_normal_quest_size());
}

/**
 * @brief Changes the normal size of the quest.
 * @param size The new normal size.
 */
void QuestProperties::set_normal_quest_size(const QSize& size) {

  QSize old_size = get_normal_quest_size();
  if (size == old_size) {
    return;
  }

  properties.set_normal_quest_size(Size::to_solarus_size(size));
  emit normal_size_changed(size);
}

/**
 * @brief Returns the minimum quest size of the quest.
 * @return The minimum quest size.
 */
QSize QuestProperties::get_min_quest_size() const {

  return Size::to_qsize(properties.get_min_quest_size());
}

/**
 * @brief Changes the minimum size of the quest.
 * @param size The new minimum size.
 */
void QuestProperties::set_min_quest_size(const QSize& size) {

  QSize old_size = get_min_quest_size();
  if (size == old_size) {
    return;
  }

  properties.set_min_quest_size(Size::to_solarus_size(size));
  emit min_size_changed(size);
}

/**
 * @brief Returns the maximum quest size of the quest.
 * @return The maximum quest size.
 */
QSize QuestProperties::get_max_quest_size() const {

  return Size::to_qsize(properties.get_max_quest_size());
}

/**
 * @brief Changes the maximum size of the quest.
 * @param size The new maximum size.
 */
void QuestProperties::set_max_quest_size(const QSize& size) {

  QSize old_size = get_max_quest_size();
  if (size == old_size) {
    return;
  }

  properties.set_max_quest_size(Size::to_solarus_size(size));
  emit max_size_changed(size);
}

/**
 * @brief Returns the license of the quest.
 * @return The quest license.
 */
QString QuestProperties::get_license() const {

  return QString::fromStdString(properties.get_license());
}

/**
 * @brief Changes the license of the quest.
 * @param license The quest license.
 */
void QuestProperties::set_license(const QString &license) {

  const QString old_license = get_license();
  if (old_license == license) {
    return;
  }

  properties.set_license(license.toStdString());
  emit license_changed(license);
}

/**
 * @brief Returns the languages officially supported by the quest.
 * @return the quest languages.
 */
QStringList QuestProperties::get_languages() const {

  QStringList languages;
  const std::vector<std::string> quest_languages = properties.get_languages();
  languages.reserve(quest_languages.size());
  std::transform(quest_languages.begin(), quest_languages.end(), std::back_inserter(languages),
                 [] (const std::string &quest_language){
                   return QString::fromStdString(quest_language);
  });
  return languages;
}

/**
 * @brief Changes the languages of the quest.
 * @param languages The quest languages.
 */
void QuestProperties::set_languages(const QStringList &languages) {

  const QStringList old_languages = get_languages();
  if (old_languages == languages) {
    return;
  }

  properties.set_languages(to_string_vector(languages));
  emit languages_changed(languages);
}

/**
 * @brief Returns the minimum players needed to play the quest.
 * @return The quest minimum players.
 */
int QuestProperties::get_min_players() const {

  return static_cast<int>(properties.get_min_players());
}

/**
 * @brief Changes the minimum players needed to play the quest.
 * @param min_players The quest minimum players.
 */
void QuestProperties::set_min_players(int min_players) {

  const int old_min_players = get_min_players();
  if (old_min_players == min_players) {
    return;
  }

  properties.set_min_players(std::max(1, min_players));
  emit min_players_changed(min_players);
}

/**
 * @brief Returns the maximum players allowed to play the quest.
 * @return The quest maximum players.
 */
int QuestProperties::get_max_players() const {

  return static_cast<int>(properties.get_max_players());
}

/**
 * @brief Changes the maximum players allowed to play the quest.
 * @param max_players The quest maximum players.
 */
void QuestProperties::set_max_players(int max_players) {

  const int old_max_players = get_max_players();
  if (old_max_players == max_players) {
    return;
  }

  properties.set_max_players(std::max(1, max_players));
  emit max_players_changed(max_players);
}

/**
 * @brief Returns the genres of the quest.
 * @return The quest genres.
 */
QStringList QuestProperties::get_genres() const {

  return to_qstring_list(properties.get_genres());
}

/**
 * @brief Changes the genres of the quest.
 * @param genres The quest genres.
 */
void QuestProperties::set_genres(const QStringList &genres) {

  const QStringList old_genres = get_genres();
  if (old_genres == genres) {
    return;
  }

  properties.set_genres(to_string_vector(genres));
  emit genres_changed(genres);
}

/**
 * @brief Returns wether the quest has dynamic timestep enabled.
 * @return true if quest is dynamic timestep enabled
 */
bool QuestProperties::is_dynamic_timestep() const {
  return properties.is_dynamic_timestep();
}

/**
 * @brief Sets wether the quest has dynamic timestep
 * @param enable true to enable dynamic timestep for the quest
 */
void QuestProperties::set_dynamic_timestep(bool enable) {
  auto old = properties.is_dynamic_timestep();
  if(old == enable) {
    return;
  }

  properties.set_dynamic_timestep(enable);
  emit dynamic_timestep_changed(enable);
}

/**
 * @brief Returns wether the quest has subpixel camera enabled.
 * @return true if quest is dynamic timestep enabled
 */
bool QuestProperties::is_subpixel_camera() const {
  return properties.is_subpixel_camera();
}

/**
 * @brief Sets wether the quest has subpixel camera
 * @param enable true to enable subpixel camera for the quest
 */
void QuestProperties::set_subpixel_camera(bool enable) {
  auto old = properties.is_subpixel_camera();
  if(old == enable) {
    return;
  }

  properties.set_subpixel_camera(enable);
  emit subpixel_camera_changed(enable);
}

}
