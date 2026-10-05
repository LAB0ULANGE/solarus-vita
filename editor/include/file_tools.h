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
#ifndef SOLARUSEDITOR_FILE_TOOLS_H
#define SOLARUSEDITOR_FILE_TOOLS_H

class QRegularExpression;
class QString;

namespace SolarusEditor {

/**
 * \brief Utility functions on files and directories.
 *
 * These are general helper functions, not specific to Solarus.
 * The Quest class contains file functions specialized to Solarus quests.
 */
namespace FileTools {

void copy_recursive(const QString& src, const QString& dst);
void delete_recursive(const QString& path);
void create_directories(const QString& path);

bool replace_in_file(
    const QString& path,
    const QRegularExpression& regex,
    const QString& replacement,
    bool replace_all = true
);

void initialize_assets();
QString get_assets_path();

QString to_file_name(const QString& name);

bool is_path_valid(const QString& path);

QString cd_up_until_exists(const QString& path);

/**
 * @brief Possible errors when checking the path
 * where the new quest folder will be created.
 */
enum class NewQuestPathError {
  NoError,                /**< The path is a valid one for a new quest. */
  ParentDirDoesNotExist,  /**< The path does not exist. */
  PathIsAfile,            /**< The path exist but is a file, not a directory. */
  AlreadyAQuest,          /**< A quest already exists in this path. */
  PathNotAbsolute,        /**< The path is not absolute. */
  EmptyPath,              /**< The path is empty. */
  InvalidCharacters,      /**< The path contains invalid characters. */
};

NewQuestPathError check_new_quest_path(const QString& path);

}

}

#endif
