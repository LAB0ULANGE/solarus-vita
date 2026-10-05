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
#include "config.h"
#include "editor_exception.h"
#include "file_tools.h"
#include <solarus/core/Common.h>
#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>
#include <QTextStream>

#include <QDebug>

namespace SolarusEditor {

namespace FileTools {

namespace {
  bool assets_path_initialized = false;
  QString assets_path;
}

/**
 * @brief Determines the path to the Solarus Quest Editor assets directory.
 *
 * If the macro SOLARUSEDITOR_ASSETS_DIR is defined, it is used as the only
 * path for searching. Otherwise, the strategy below is used instead.
 *
 * The directory "assets" is searched in the following paths in this order:
 * - The directory containing the executable.
 * - On macOS, the Resources directory inside the .app bundle.
 * - The source path (macro SOLARUSEDITOR_SOURCE_PATH)
 *   (useful for developer builds).
 * - The install path (macro SOLARUSEDITOR_DATADIR_PATH).
 */
void initialize_assets() {
  QStringList potential_paths;

#ifdef SOLARUSEDITOR_ASSETS_DIR
  potential_paths << SOLARUSEDITOR_ASSETS_DIR;
#else
  const QString& executable_path = QCoreApplication::applicationDirPath();

  // Try the current directory first.
  potential_paths << executable_path + "/assets";

#ifdef Q_OS_MAC
  // On macOS, the executable is inside the .app bundle.
  potential_paths << executable_path + "/../Resources/assets";
#endif

#ifdef SOLARUSEDITOR_BINDIR_PATH
  // Try the source path if we are not running the installed executable.
  bool running_installed_executable = (executable_path == SOLARUSEDITOR_BINDIR_PATH);
#ifdef SOLARUSEDITOR_SOURCE_PATH
  if (!running_installed_executable) {
    potential_paths << SOLARUSEDITOR_SOURCE_PATH "/assets";
  }
#endif

  // Try the install path if we are running the installed executable.
#ifdef SOLARUSEDITOR_DATADIR_PATH
  if (running_installed_executable) {
    potential_paths << SOLARUSEDITOR_DATADIR_PATH "/assets";
  }
#endif
#endif
#endif

  assets_path_initialized = true;
  for (const QString& potential_path : std::as_const(potential_paths)) {
    if (QFile(potential_path).exists()) {
      assets_path = potential_path;
      return;
    }
  }
}

/**
 * @brief Returns the path to the Solarus Quest Editor assets directrory.
 * @return The assets path or an empty string if assets could not be found.
 */
QString get_assets_path() {

  if (!assets_path_initialized) {
    initialize_assets();
  }
  return assets_path;
}

/**
 * @brief Utility function to copy a file or directory with its content.
 * @param src The file or directory to copy.
 * @param dst The destination file path. It should be the name of the file
 * or directory to create.
 * @throws EditorException if the copy failed. In this case, files already
 * successfully copied are left.
 */
void copy_recursive(const QString& src, const QString& dst) {

  if (src == dst) {
    throw EditorException(QApplication::tr("Source and destination are the same: '%1'").arg(src));
  }

  QFileInfo src_info(src);
  QFileInfo dst_info(dst);

  if (!src_info.exists()) {
    throw EditorException(QApplication::tr("No such file or folder: '%1'").arg(src));
  }

  if (!src_info.isReadable()) {
    throw EditorException(QApplication::tr("Source file cannot be read: '%1'").arg(src));
  }

  if (dst_info.exists()) {
    throw EditorException(QApplication::tr("Destination already exists: '%1'").arg(dst));
  }

  if (src_info.isDir()) {

    QDir dst_dir(dst);
    dst_dir.cdUp();

    if (!dst_dir.exists()) {
      throw EditorException(QApplication::tr("No such folder: '%1'").arg(dst_dir.path()));
    }

    QString src_canonical_path = src_info.canonicalFilePath();
    QString dst_parent_canonical_path = dst_dir.canonicalPath();

    if (dst_parent_canonical_path.startsWith(src_canonical_path)) {
      throw EditorException(QApplication::tr("Cannot copy folder '%1' to one of its own subfolders: '%2'").arg(src, dst));
    }

    if (!dst_dir.mkdir(dst_info.fileName())) {
      throw EditorException(QApplication::tr("Cannot create folder '%1'").arg(dst));
    }

    QDir src_dir(src);
    const QStringList& file_names = src_dir.entryList(
          QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    for (const QString& file_name : file_names) {
      QString next_src = src + '/' + file_name;
      QString next_dst = dst + '/' + file_name;
      copy_recursive(next_src, next_dst);
    }
  }
  else {
    if (!QFile::copy(src, dst)) {
      throw EditorException(QApplication::tr("Cannot copy file '%1' to '%2'").arg(src, dst));
    }

    if (src.startsWith(":/")) {
      // Files from Qt resources are read-only. Set usual permissions now.
      QFile::setPermissions(dst,
                            QFile::ReadUser | QFile::WriteUser | QFile::ExeUser |
                            QFile::ReadGroup| QFile::ExeGroup |
                            QFile::ReadOther| QFile::ExeOther);
    }
  }
}

/**
 * @brief Deletes a file or a directory with its content.
 *
 * Does nothing if the file or directory does not exist.
 *
 * @param path The file or directory to delete.
 * @throws EditorException if the deletion failed.
 */
void delete_recursive(const QString& path) {

  QFileInfo info(path);
  if (!info.exists()) {
    return;
  }

  if (!info.isDir()) {
    // Not a directory.
    if (!QFile::remove(path)) {
      throw EditorException(QApplication::tr("Failed to delete file '%1'").arg(path));
    }
  }
  else {
    // Directory.
    QDir dir(path);
    const QStringList& file_names = dir.entryList(
          QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
    for (const QString& file_name : file_names) {
      QString child_path = path + '/' + file_name;
      delete_recursive(child_path);
    }

    if (!QDir().rmdir(path)) {
      throw EditorException(QApplication::tr("Failed to delete folder '%1'").arg(path));
    }
  }
}

/**
 * @brief Makes sure that the specified directory exists.
 *
 * Creates necessary parents directories if needed.
 *
 * @param path A directory path.
 * @throws EditorException In case of error.
 */
void create_directories(const QString& path) {

  bool success = QDir().mkpath(path);
  if (!success) {
    throw EditorException(QApplication::tr("Cannot create folder '%1'").arg(path));
  }
}

/**
 * @brief Replaces all occurences of the given pattern in a file.
 * @param path Path of the file to modify.
 * @param regexp The pattern to replace.
 * @param replacement The string to put instead of the pattern.
 * @param replace_all @c true to replace all occurences, @c false to only
 * replace the first one.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool replace_in_file(
    const QString& path,
    const QRegularExpression& regex,
    const QString& replacement,
    bool replace_all
) {
  QFile file(path);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw EditorException(QApplication::tr("Cannot open file '%1'").arg(file.fileName()));
  }
  QTextStream in(&file);
  in.setEncoding(QStringConverter::Utf8);
  QString content = in.readAll();
  file.close();

  QString old_content = content;
  if (replace_all) {
    content.replace(regex, replacement);
  } else {
    QRegularExpressionMatch match = regex.match(content);
    if (match.hasMatch()) {
      content.replace(match.capturedStart(), match.capturedLength(), replacement);
    }
  }

  if (content == old_content) {
    // No change.
    return false;
  }

  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw EditorException(QApplication::tr("Cannot open file '%1' for writing").arg(file.fileName()));
  }
  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << content;
  file.close();
  return true;
}

/**
 * @brief Modify a string so it is a safe file name on most platforms.
 * @param name The unformated name.
 * @return A string containing a safe file name.
 */
QString to_file_name(const QString& name) {
  // Characters that could cause problems in Linux, Mac and Windows.
  static const QString forbidden_characters =
      QStringLiteral("()<>\\/\"\'|&:;.?*");
  QString path = name;
  for (QChar & ch : path) {
    ch = forbidden_characters.contains(ch) ? QChar(' ') : ch.toLower();
  }
  path = path.simplified();
  path = path.replace(QChar(' '), QChar('-'));
  return path;
}

/**
 * @brief Checks if the path is a valid Windows path.
 * @param path The path to check.
 * @return True if the path is a valid one for Windows.
 */
bool is_valid_windows_path(const QString &path) {
  // List of invalid characters in Windows paths
  static const QRegularExpression invalid_chars_regex(R"([<>"\\|?*\x00-\x1F])");

  // Also check for reserved names (CON, PRN, AUX, NUL, COM1-9, LPT1-9, etc.)
  static const QRegularExpression reserved_names(
      R"((^|\\)(CON|PRN|AUX|NUL|COM[1-9]|LPT[1-9])(\..*)?($|\\))",
      QRegularExpression::CaseInsensitiveOption);

  return !path.contains(invalid_chars_regex) && !reserved_names.match(path).hasMatch();
}

/**
 * @brief Checks if the path is a valid Unix path.
 * @param path The path to check.
 * @return True if the path is a valid one for Unix.
 */
bool is_valid_unix_path(const QString &path) {
  static const QRegularExpression discouraged_chars(R"([*?\[\]$`"'|&;><!])");
  return !path.contains(discouraged_chars);
}

/**
 * @brief Checks is a path is valid for the current operating system.
 * @param path The path to check.
 * @return @c true if the path is a valid one.
 */
bool is_path_valid(const QString& path) {
#ifdef Q_OS_WIN
  if (!is_valid_windows_path(path)) {
    return false;
  }
#else
  if (!is_valid_unix_path(path)) {
    return false;
  }
#endif

  // Check for empty path.
  if (path.isEmpty()) {
    return false;
  }

  // Check for relative path markers.
  if (path.contains("/./") || path.contains("/../")) {
    return false;
  }

  // Check for trailing spaces or periods (invalid on Windows).
  if (path.endsWith(' ') || path.endsWith('.')) {
    return false;
  }

  return true;
}

/**
 * @brief Goes up until we find an existing directory.
 * @param path The starting path.
 * @return The closest existing parent directory path.
 */
QString cd_up_until_exists(const QString& path) {
  bool found = false;
  QFileInfo file_info = QFileInfo(path);
  while (!found) {
    if (file_info.exists()) {
      found = true;
    } else {
      file_info = QFileInfo(file_info.dir().absolutePath());      
    }
  }

  return found ? file_info.canonicalFilePath() : QString{};
}


/**
 * @brief Checks whether a string is a valid path for a new quest.
 * @param path The path to check.
 * @return @c true if it is valid.
 */
NewQuestPathError check_new_quest_path(const QString& path) {

  if (path.isEmpty()) {
    return NewQuestPathError::EmptyPath;
  }

  QFileInfo file_info(path);
  if (!file_info.isAbsolute()) {
    return NewQuestPathError::PathNotAbsolute;
  }

  if (!FileTools::is_path_valid(path)) {
    return NewQuestPathError::InvalidCharacters;
  }

  if (file_info.exists(path)) {
    if (file_info.isFile()) {
      return NewQuestPathError::PathIsAfile;
    }
    else {
      QDir dir = QDir(path);
      if (dir.cd("data")) {
        if (dir.exists("quest.dat")) {
          return NewQuestPathError::AlreadyAQuest;
        }
      }
    }
  }
  else {
    QDir dir = QDir(path);
    if (!dir.cdUp()) {
      return NewQuestPathError::ParentDirDoesNotExist;
    }
  }

  return NewQuestPathError::NoError;
}

}  // namespace FileTools

}  // namespace SolarusEditor
