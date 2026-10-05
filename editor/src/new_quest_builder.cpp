/*
 * Copyright (C) 2014-2021 Christopho, Solarus - http://www.solarus-games.org
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
#include "file_tools.h"
#include "new_quest_builder.h"
#include "quest.h"
#include "quest_properties.h"
#include "version.h"
#include <QApplication>
#include <QDir>
#include <QFile>
#include <QUuid>

namespace SolarusEditor {

namespace NewQuestBuilder {

/**
 * @brief Create a minimal data directory for a blank quest.
 * @param quest_path Root path of the quest to create. Must exist.
 * The data directory will be created there.
 * @throws EditorException If the files creation failed.
 */
static void create_blank_quest(const QString& quest_path) {

  // Create the data directory.
  QDir quest_dir(quest_path);
  if (!quest_dir.mkdir("data")) {
    throw EditorException(QApplication::tr(
      "Could not create data directory."));
  }

  // Create the Quest Properties file. (Quest doesn't exist yet.)
  QFile quest_file(quest_dir.absoluteFilePath("data/quest.dat"));
  if (quest_file.open(QIODevice::WriteOnly)) {
    // A very minimal version is created. Updating properties overwrites it.
    QTextStream out(&quest_file);
    out << QString("quest{ solarus_version = \"%1\", title = \"%2\", }\n")
      .arg(SOLARUSEDITOR_VERSION_WITHOUT_PATCH, QApplication::tr("Untitled Quest"));
    quest_file.close();
  } else {
    throw EditorException(QApplication::tr(
      "Could not create quest properties."));
  }

  // Create the quest database file. (Avoids some error messages.)
  QFile database_file(quest_dir.absoluteFilePath("data/project_db.dat"));
  if (database_file.open(QIODevice::WriteOnly)) {
    // This is basically a 'touch' command to create an empty file.
    database_file.close();
  }
  // This error is recoverable, so don't throw.
}

/**
 * @brief Creates initial files of a new quest.
 * @param config The configuration.
 * @throws EditorException If the files creation failed.
 */
void create_initial_quest_files(const NewQuestConfig& config) {

  // Create the quest directory if required.
  QDir quest_dir(config.quest_path);
  if (!quest_dir.exists()) {
    FileTools::create_directories(config.quest_path);
  }

  // Create the data directory, quest properties and quest database.
  switch (config.mode) {
  case BLANK_QUEST:
    create_blank_quest(config.quest_path);
    break;
  case COPY_INITIAL_QUEST:
    {
      // Create files from the assets directory.
      const QString& assets_path = FileTools::get_assets_path();
      if (assets_path.isEmpty()) {
        throw EditorException(QApplication::tr("Could not find the assets directory.\nMake sure that Solarus Editor is properly installed."));
      }
      FileTools::copy_recursive(assets_path + "/initial_quest/data", config.quest_path + "/data");
    }
    break;
  }

  // Make sure all resource directories exist.
  Quest quest(config.quest_path);
  for (ResourceType resource_type : Solarus::EnumInfo<ResourceType>::enums()) {
    quest.create_dir_if_not_exists(quest.get_resource_path(resource_type));
  }

  // Initialize the write directory to a unique id so that the game is directly playable.
  QuestProperties properties(quest);
  QString uid_string = QUuid::createUuid().toString();
  uid_string = uid_string.mid(1, uid_string.size() - 2);
  properties.set_write_dir(uid_string);
  properties.set_title(config.quest_name);
  properties.save();
}

}

}
