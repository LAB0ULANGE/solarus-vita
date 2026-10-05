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
#include "grid_style.h"
#include "editor_settings.h"
#include <QPalette>
#include <QSize>
#include <QFontDatabase>

namespace SolarusEditor {

// Layout keys.
const QString EditorSettings::quest_tree_visible = "quest_tree_visible";
const QString EditorSettings::quest_tree_width = "quest_tree_width";
const QString EditorSettings::console_height = "console_height";
const QString EditorSettings::map_side_width = "map_side_width";
const QString EditorSettings::map_props_height = "map_props_height";
const QString EditorSettings::tileset_side_width = "tileset_side_width";
const QString EditorSettings::sprite_side_width = "sprite_side_width";
const QString EditorSettings::sprite_preview_height = "sprite_preview_height";
const QString EditorSettings::shader_side_width = "shader_side_width";
const QString EditorSettings::shader_files_height = "shader_files_height";

// General keys.
const QString EditorSettings::theme = "theme";
const QString EditorSettings::working_directory = "working_directory";
const QString EditorSettings::current_quest = "current_quest";
const QString EditorSettings::last_quests = "last_quests";
const QString EditorSettings::last_files = "last_files";
const QString EditorSettings::last_file = "last_file";
const QString EditorSettings::restore_last_files = "restore_last_files";
const QString EditorSettings::save_files_before_running = "save_files_before_running";
const QString EditorSettings::audio = "audio";
const QString EditorSettings::quest_size = "quest_size";
const QString EditorSettings::force_software_rendering = "force_software_rendering";
const QString EditorSettings::suspend_unfocused = "suspend_unfocused";
const QString EditorSettings::last_author = "last_author";
const QString EditorSettings::last_license_data = "last_license_data";
const QString EditorSettings::last_license_script = "last_license_script";
const QString EditorSettings::last_text_searched = "last_text_searched";

// Import dialog keys.
const QString EditorSettings::import_last_source_quest = "import_last_source_quest";

// Console keys.
const QString EditorSettings::console_history = "console_history";

// Package dialog keys.
const QString EditorSettings::package_save_path = "package_dialog/save_path";

// Text editor keys.
const QString EditorSettings::font_family = "text_editor/font_family";
const QString EditorSettings::font_size = "text_editor/font_size";
const QString EditorSettings::tab_length = "text_editor/tab_length";
const QString EditorSettings::replace_tab_by_spaces =
  "text_editor/replace_tab_by_spaces";
const QString EditorSettings::external_text_editor_enabled =
  "text_editor/external_text_editor_enabled";
const QString EditorSettings::external_text_editor_cmd =
  "text_editor/external_text_editor_cmd";
const QString EditorSettings::create_scripts_with_default_code =
  "text_editor/create_scripts_with_default_code";

// Map editor keys.
const QString EditorSettings::map_main_zoom = "map_editor/main_zoom";
const QString EditorSettings::map_grid_show_at_opening =
  "map_editor/grid_show_at_opening";
const QString EditorSettings::map_grid_size = "map_editor/grid_size";
const QString EditorSettings::map_grid_style = "map_editor/grid_style";
const QString EditorSettings::map_grid_color = "map_editor/grid_color";
const QString EditorSettings::map_tileset_zoom = "map_editor/tileset_zoom";

// Sprite editor keys.
const QString EditorSettings::sprite_main_zoom = "sprite_editor/main_zoom";
const QString EditorSettings::sprite_grid_show_at_opening =
  "sprite_editor/grid_show_at_opening";
const QString EditorSettings::sprite_grid_size = "sprite_editor/grid_size";
const QString EditorSettings::sprite_grid_style = "sprite_editor/grid_style";
const QString EditorSettings::sprite_grid_color = "sprite_editor/grid_color";
const QString EditorSettings::sprite_auto_detect_grid =
  "sprite_editor/auto_detect_grid";
const QString EditorSettings::sprite_previewer_zoom =
  "sprite_editor/previewer_zoom";
const QString EditorSettings::sprite_origin_show_at_opening =
  "sprite_editor/origin_show_at_opening";
const QString EditorSettings::sprite_origin_color = "sprite_editor/origin_color";

// Tileset editor keys.
const QString EditorSettings::tileset_zoom = "tileset_editor/zoom";
const QString EditorSettings::tileset_grid_show_at_opening =
  "tileset_editor/grid_show_at_opening";
const QString EditorSettings::tileset_grid_size = "tileset_editor/grid_size";
const QString EditorSettings::tileset_grid_style = "tileset_editor/grid_style";
const QString EditorSettings::tileset_grid_color = "tileset_editor/grid_color";
const QString EditorSettings::tileset_refactoring_change_pattern_id = "tileset_editor/refactoring_change_pattern_id";

// Shader editor keys.
const QString EditorSettings::shader_preview_type = "shader_editor/preview_type";
const QString EditorSettings::shader_preview_picture_file = "shader_editor/preview_picture_file";
const QString EditorSettings::shader_preview_map_id = "shader_editor/preview_map_id";
const QString EditorSettings::shader_preview_sprite_id = "shader_editor/preview_sprite_id";
const QString EditorSettings::shader_preview_sprite_animation = "shader_editor/preview_sprite_animation";
const QString EditorSettings::shader_preview_sprite_direction = "shader_editor/preview_sprite_direction";

/**
 * @brief Get the default fixed font to use in the UI.
 * 
 * On macOS and Windows, we tweak the value returned by QFontDatabase, as the
 * default system fonts on these platforms are old and ugly...
 * 
 * @return The QFont to use for all fixed-font widgets in the user interface.
 */
QFont get_default_fixed_font() {
  QFont result = QFontDatabase::systemFont(QFontDatabase::SystemFont::FixedFont);

#ifdef Q_OS_MAC
  // On macOS, the default fixed font 'Monaco' is not very good.
  // Use another one instead if available.
  const QString better_mac_font = QString("Menlo");
  if (QFontDatabase::families().contains(better_mac_font)) {
    result = QFont(better_mac_font);
    result.setPointSize(11);
  }
#elif defined(Q_OS_WIN)
  // On Windows, the default fixed font 'Courier New' is not very good.
  // Use another one instead if available.
  const QString better_win_font = QString("Consolas");
  if (QFontDatabase::families().contains(better_win_font)) {
    result = QFont(better_win_font);
    result.setPointSize(11);
  }
#endif

  return result;
}

/**
 * @brief Creates settings.
 */
EditorSettings::EditorSettings() :
  settings() {
}

/**
 * @brief Returns the map of default values.
 * 
 * Initializes it on the first call.
 * 
 * @return The map of default values.
 */
const QMap<QString, QVariant>& EditorSettings::get_default_values() {

  static QMap<QString, QVariant> default_values{};
  static bool populated = false;

  // Populate the map only on the first call.
  if (!populated) {
    populated = true;
    const QFont default_fixed_font = get_default_fixed_font();

    default_values = QMap<QString, QVariant>{
      // Layout.
      { EditorSettings::quest_tree_visible, true },
      { EditorSettings::quest_tree_width, 300 },
      { EditorSettings::console_height, 140 },
      { EditorSettings::map_side_width, 400 },
      { EditorSettings::map_props_height, 340 },
      { EditorSettings::tileset_side_width, 400 },
      { EditorSettings::sprite_side_width, 400 },
      { EditorSettings::sprite_preview_height, 550 },
      { EditorSettings::shader_side_width, 300 },
      { EditorSettings::shader_files_height, 400 },

      // General.
      { EditorSettings::theme, "automatic" },
      { EditorSettings::working_directory, "" },
      { EditorSettings::last_quests, QStringList() },
      { EditorSettings::last_files, QStringList() },
      { EditorSettings::last_file, "" },
      { EditorSettings::restore_last_files, true },
      { EditorSettings::save_files_before_running, "ask" },
      { EditorSettings::audio, true },
      { EditorSettings::quest_size, QSize() },
      { EditorSettings::force_software_rendering, false },
      { EditorSettings::suspend_unfocused, true },

      // Import dialog.
      { EditorSettings::import_last_source_quest, "" },

      // Package dialog.
      { EditorSettings::package_save_path, "" },

      // Console.
      { EditorSettings::console_history, QStringList() },

      // Text editor.
      { EditorSettings::font_family, default_fixed_font.family() },
      { EditorSettings::font_size, default_fixed_font.pointSize() },
      { EditorSettings::tab_length, 2 },
      { EditorSettings::replace_tab_by_spaces, true },
      { EditorSettings::external_text_editor_enabled, false},
      { EditorSettings::external_text_editor_cmd, ""},

      // Map editor.
      { EditorSettings::map_main_zoom, 2.0 },
      { EditorSettings::map_grid_show_at_opening, false },
      { EditorSettings::map_grid_size, QSize(16, 16) },
      { EditorSettings::map_grid_style, static_cast<int>(GridStyle::DASHED) },
      { EditorSettings::map_grid_color, "#000000" },
      { EditorSettings::map_tileset_zoom, 2.0 },

      // Sprite editor.
      { EditorSettings::sprite_main_zoom, 2.0 },
      { EditorSettings::sprite_grid_show_at_opening, false },
      { EditorSettings::sprite_grid_size, QSize(16, 16) },
      { EditorSettings::sprite_grid_style, static_cast<int>(GridStyle::DASHED) },
      { EditorSettings::sprite_grid_color, "#000000" },
      { EditorSettings::sprite_auto_detect_grid, false },
      { EditorSettings::sprite_previewer_zoom, 2.0 },
      { EditorSettings::sprite_origin_show_at_opening, false },
      { EditorSettings::sprite_origin_color, "#0000ff" },

      // Tileset editor.
      { EditorSettings::tileset_zoom, 2.0 },
      { EditorSettings::tileset_grid_show_at_opening, false },
      { EditorSettings::tileset_grid_size, QSize(16, 16) },
      { EditorSettings::tileset_grid_style, static_cast<int>(GridStyle::DASHED) },
      { EditorSettings::tileset_grid_color, "#000000" },
      { EditorSettings::tileset_refactoring_change_pattern_id, true },

      // Shader editor.
      { EditorSettings::shader_preview_type, "picture" },
      { EditorSettings::shader_preview_picture_file, "" },
      { EditorSettings::shader_preview_map_id, "" },
      { EditorSettings::shader_preview_sprite_id, "" },
      { EditorSettings::shader_preview_sprite_animation, "" },
      { EditorSettings::shader_preview_sprite_direction, 0 },
    };
  }

  return default_values;
}

/**
 * @brief Returns a settings value.
 * @param key The key of the setting.
 * @return The value of the setting.
 */
QVariant EditorSettings::get_value(const QString& key) {

  const QMap<QString, QVariant>& default_values = get_default_values();
  if (default_values.contains(key)) {
    return settings.value(key, default_values[key]);
  }
  return settings.value(key);
}

/**
 * @brief Returns a settings boolean value.
 * @param key The key of the setting.
 * @return The boolean value of the setting.
 */
bool EditorSettings::get_value_bool(const QString& key) {

  return get_value(key).toBool();
}

/**
 * @brief Returns a settings integer value.
 * @param key The key of the setting.
 * @return The integer value of the setting.
 */
int EditorSettings::get_value_int(const QString& key) {

  return get_value(key).toInt();
}

/**
 * @brief Returns a settings double value.
 * @param key The key of the setting.
 * @return The double value of the setting.
 */
double EditorSettings::get_value_double(const QString& key) {

  return get_value(key).toDouble();
}

/**
 * @brief Returns a settings string value.
 * @param key The key of the setting.
 * @return The string value of the setting.
 */
QString EditorSettings::get_value_string(const QString& key) {

  return get_value(key).toString();
}

/**
 * @brief Returns a settings string list value.
 * @param key The key of the setting.
 * @return The string list value of the setting.
 */
QStringList EditorSettings::get_value_string_list(const QString& key) {

  return get_value(key).toStringList();
}

/**
 * @brief Returns a settings size value.
 * @param key The key of the setting.
 * @return The size value of the setting.
 */
QSize EditorSettings::get_value_size(const QString& key) {

  return get_value(key).toSize();
}

/**
 * @brief Returns a settings color value.
 * @param key The key of the setting.
 * @return The color value of the setting.
 */
QColor EditorSettings::get_value_color(const QString& key) {

  return QColor(get_value(key).toString());
}

/**
 * @brief Returns a settings default value.
 * @param key The key of the setting.
 * @return The default value of the setting.
 */
QVariant EditorSettings::get_default(const QString& key) {

  const QMap<QString, QVariant>& default_values = get_default_values();
  if (!default_values.contains(key)) {
    return QVariant();
  }
  return default_values[key];
}

/**
 * @brief Returns a settings default boolean value.
 * @param key The key of the setting.
 * @return The default boolean value of the setting.
 */
bool EditorSettings::get_default_bool(const QString& key) {

  return get_default(key).toBool();
}

/**
 * @brief Returns a settings default integer value.
 * @param key The key of the setting.
 * @return The default integer value of the setting.
 */
int EditorSettings::get_default_int(const QString& key) {

  return get_default(key).toInt();
}

/**
 * @brief Returns a settings default double value.
 * @param key The key of the setting.
 * @return The default double value of the setting.
 */
double EditorSettings::get_default_double(const QString& key) {

  return get_default(key).toDouble();
}

/**
 * @brief Returns a settings default string value.
 * @param key The key of the setting.
 * @return The default string value of the setting.
 */
QString EditorSettings::get_default_string(const QString& key) {

  return get_default(key).toString();
}

/**
 * @brief Returns a settings default size value.
 * @param key The key of the setting.
 * @return The default size value of the setting.
 */
QSize EditorSettings::get_default_size(const QString& key) {

  return get_default(key).toSize();
}

/**
 * @brief Returns a settings default color value.
 * @param key The key of the setting.
 * @return The default color value of the setting.
 */
QColor EditorSettings::get_default_color(const QString& key) {

  return QColor(get_default(key).toString());
}

/**
 * @brief Changes a settings value.
 * @param key The key of the setting.
 * @param value The new value of the setting.
 */
void EditorSettings::set_value(const QString& key, const QVariant& value) {

  settings.setValue(key, value);
}

/**
 * @brief Changes a settings color value.
 * @param key The key of the setting.
 * @param value The new color value of the setting.
 */
void EditorSettings::set_value_color(const QString& key, const QColor& value) {

  settings.setValue(key, value.name());
}

/**
 * @brief Restores all default values.
 */
void EditorSettings::restore_default() {

  const QMap<QString, QVariant>& default_values = get_default_values();
  const QStringList& keys = default_values.keys();
  for (const QString& key : keys) {
    settings.setValue(key, default_values.value(key));
  }
}

}
