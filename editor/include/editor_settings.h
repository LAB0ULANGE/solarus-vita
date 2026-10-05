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
#ifndef SOLARUSEDITOR_EDITOR_SETTINGS_H
#define SOLARUSEDITOR_EDITOR_SETTINGS_H

#include <QSettings>

namespace SolarusEditor {

/**
 * @brief Manages settings of the quest editor.
 */
class EditorSettings {

public:

  // Layout keys.
  static const QString quest_tree_visible;
  static const QString quest_tree_width;
  static const QString console_height;
  static const QString map_side_width;
  static const QString map_props_height;
  static const QString tileset_side_width;
  static const QString sprite_side_width;
  static const QString sprite_preview_height;
  static const QString shader_side_width;
  static const QString shader_files_height;

  // General keys.
  static const QString theme;
  static const QString working_directory;
  static const QString current_quest;
  static const QString last_quests;
  static const QString last_files;
  static const QString last_file;
  static const QString restore_last_files;
  static const QString save_files_before_running;
  static const QString audio;
  static const QString quest_size;
  static const QString force_software_rendering;
  static const QString suspend_unfocused;
  static const QString last_author;
  static const QString last_license_data;
  static const QString last_license_script;
  static const QString last_text_searched;

  // Console keys.
  static const QString console_history;

  // Import dialog keys.
  static const QString import_last_source_quest;

  // Package dialog keys.
  static const QString package_save_path;

  // Text editor keys.
  static const QString font_family;
  static const QString font_size;
  static const QString tab_length;
  static const QString replace_tab_by_spaces;
  static const QString external_text_editor_enabled;
  static const QString external_text_editor_cmd;
  static const QString create_scripts_with_default_code;

  // Map editor keys.
  static const QString map_main_zoom;
  static const QString map_grid_show_at_opening;
  static const QString map_grid_size;
  static const QString map_grid_style;
  static const QString map_grid_color;
  static const QString map_tileset_zoom;

  // Sprite editor keys.
  static const QString sprite_main_zoom;
  static const QString sprite_grid_show_at_opening;
  static const QString sprite_grid_size;
  static const QString sprite_grid_style;
  static const QString sprite_grid_color;
  static const QString sprite_auto_detect_grid;
  static const QString sprite_previewer_zoom;
  static const QString sprite_origin_show_at_opening;
  static const QString sprite_origin_color;

  // Tileset editor keys.
  static const QString tileset_zoom;
  static const QString tileset_grid_show_at_opening;
  static const QString tileset_grid_size;
  static const QString tileset_grid_style;
  static const QString tileset_grid_color;
  static const QString tileset_refactoring_change_pattern_id;

  // Shader editor keys.
  static const QString shader_preview_type;
  static const QString shader_preview_picture_file;
  static const QString shader_preview_map_id;
  static const QString shader_preview_sprite_id;
  static const QString shader_preview_sprite_animation;
  static const QString shader_preview_sprite_direction;

  EditorSettings();

  QVariant get_value(const QString& key);
  bool get_value_bool(const QString& key);
  int get_value_int(const QString& key);
  double get_value_double(const QString& key);
  QString get_value_string(const QString& key);
  QStringList get_value_string_list(const QString& key);
  QSize get_value_size(const QString& key);
  QColor get_value_color(const QString& key);

  QVariant get_default(const QString& key);
  bool get_default_bool(const QString& key);
  int get_default_int(const QString& key);
  double get_default_double(const QString& key);
  QString get_default_string(const QString& key);
  QSize get_default_size(const QString& key);
  QColor get_default_color(const QString& key);

  void set_value(const QString& key, const QVariant& value);
  void set_value_color(const QString& key, const QColor& value);

  void restore_default();

private:
  const QMap<QString, QVariant>& get_default_values();

  QSettings settings;  /**< The wrapped settings. */
};

}

#endif
