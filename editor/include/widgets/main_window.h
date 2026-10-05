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
#ifndef SOLARUSEDITOR_MAIN_WINDOW_H
#define SOLARUSEDITOR_MAIN_WINDOW_H

#include "widgets/settings_dialog.h"
#include "quest.h"
#include "ui_main_window.h"
#include <solarus/entities/EntityType.h>
#include "quest_runner.h"
#include <QMainWindow>

class QToolButton;

namespace SolarusEditor {

class Editor;
class PairSpinBox;
class Refactoring;

using EntityType = Solarus::EntityType;

/**
 * @brief Main window of the quest editor.
 */
class MainWindow : public QMainWindow {
  Q_OBJECT

public:

  MainWindow(QWidget* parent);
  ~MainWindow();

  void initialize_geometry_on_screen();
  Quest& get_quest();
  void close_quest();
  bool open_quest(const QString& quest_path);
  void open_file(Quest& quest, const QString& path);
  void open_welcome();
  Editor* get_current_editor();
  void run_quest(const QString& map_id);

private slots:

  // Menu actions.
  void on_action_new_quest_triggered();
  void on_action_load_quest_triggered();
  void on_action_close_quest_triggered();
  void on_action_reopen_closed_tab_triggered();
  void on_action_save_triggered();
  void on_action_save_all_triggered();
  void on_action_close_triggered();
  void on_action_close_all_triggered();
  void on_action_import_triggered();
  void on_action_open_quest_properties_triggered();
  void on_action_package_quest_triggered();
  void on_action_exit_triggered();
  void on_action_cut_triggered();
  void on_action_copy_triggered();
  void on_action_paste_triggered();
  void on_action_group_triggered();
  void on_action_ungroup_triggered();
  void on_action_select_all_triggered();
  void on_action_unselect_all_triggered();
  void on_action_find_triggered();
  void on_action_run_quest_triggered();
  void on_action_run_map_triggered();
  void on_action_stop_music_triggered();
  void on_action_show_grid_triggered();
  void on_action_show_quest_files_triggered();
  void on_action_show_console_triggered();
  void change_grid_size();
  void on_action_show_layer_0_triggered();
  void on_action_show_layer_1_triggered();
  void on_action_show_layer_2_triggered();
  void on_action_show_traversables_triggered();
  void on_action_show_obstacles_triggered();
  void on_action_export_to_image_triggered();
  void on_action_settings_triggered();
  void on_action_website_triggered();
  void on_action_doc_triggered();
  void on_action_about_triggered();
  void on_action_welcome_triggered();

  void current_editor_changed(int index);
  void rename_file_requested(Quest& quest, const QString& path);
  void refactoring_requested(const Refactoring& refactoring);

  void update_zoom();
  void update_grid_visibility();
  void update_grid_size();
  void update_layer_range();
  void update_layer_locking(int layer);
  void update_layers_locking();
  void update_layer_visibility(int layer);
  void update_layers_visibility();
  void update_traversables_visibility();
  void update_obstacles_visibility();
  void update_entity_type_visibility(EntityType entity_type);
  void update_entity_types_visibility();
  void update_grouping_actions();

  void update_run_quest();
  void quest_running();
  void quest_finished();

  void current_music_changed(const QString& music_id);
  void update_music_actions();
  void selected_path_changed(const QString& path);

  void quest_tree_resized(int pos, int index);

  void reload_settings();

protected:

  void closeEvent(QCloseEvent* event) override;
  bool eventFilter(QObject* target, QEvent* evt) override;

private:

  bool confirm_before_closing();
  bool confirm_upgrade_quest(const QString& old_format);
  void update_title();
  void upgrade_quest();
  void add_quest_to_recent_list();

  void update_recent_quests_menu();
  QMenu* create_zoom_menu();
  void update_show_layers_menu();
  void update_lock_layers_menu();
  QMenu* create_show_entities_menu();

  bool is_quest_files_visible() const;
  void set_quest_files_visible(bool quest_files_visible);

  bool is_console_visible() const;
  void set_console_visible(bool console_visible);
  void log_message_to_console(const QString& log_level, const QString& message);

  void refactor_map_id(const QString& map_id_before, const QString& map_id_after);
  bool update_destination_map_in_map(
      const QString& map_id,
      const QString& map_id_before,
      const QString& map_id_after
  );
  void refactor_tileset_id(const QString& tileset_id_before, const QString& tileset_id_after);
  bool update_tileset_in_map(
      const QString& map_id,
      const QString& tileset_id_before,
      const QString& tileset_id_after
  );
  void refactor_music_id(const QString& music_id_before, const QString& music_id_after);
  bool update_music_in_map(
      const QString& map_id,
      const QString& music_id_before,
      const QString& music_id_after
  );
  void refactor_enemy_id(const QString& enemy_id_before, const QString& enemy_id_after);
  bool update_enemy_breed_in_map(
      const QString& map_id,
      const QString& enemy_id_before,
      const QString& enemy_id_after
  );
  void refactor_custom_entity_id(const QString& custom_entity_id_before, const QString& custom_entity_id_after);
  bool update_custom_entity_model_in_map(
      const QString& map_id,
      const QString& custom_entity_id_before,
      const QString& custom_entity_id_after
  );
  void refactor_image_file(const QString& image_path_before, const QString& image_path_after);
  bool update_image_in_sprite(
      const QString& sprite_id,
      const QString& image_before,
      const QString& image_after
  );

  Ui::MainWindow ui;              /**< The main window widgets. */
  Quest quest;                    /**< The current quest open if any. */
  QuestRunner quest_runner;       /**< The executor of the current quest. */

  QMenu* recent_quests_menu;      /**< The menu to open a recent quest. */
  QMenu* zoom_menu;               /**< The zoom menu. */
  QToolButton* zoom_button;       /**< The zoom toolbar button. */
  QMap<double, QAction*>
      zoom_actions;               /**< Action of each zoom value. */
  PairSpinBox* grid_size;         /**< The grid size. */

  QMenu* show_layers_menu;        /**< The menu with the visibility of all layers. */
  QToolButton*
      show_layers_button;         /**< The "more layers" toolbar button. */
  QAction* show_layers_action;    /**< Action of the "more layer" toolbar button. */
  QMap<QString, QAction*>
      show_layers_subactions;     /**< Actions in the show layer menu.
                                   * There is one action for each layer,
                                   * plus two special actions "Show all" and "Hide all".
                                   * The key is the layer as a string or
                                   * "action_show_all" or "action_hide_all". */
  QMenu* lock_layers_menu;        /**< The menu with the lock status of all layers. */
  QMap<int, QAction*>
      lock_layers_subactions;     /**< Actions in the lock layer menu.
                                   * There is one action for each layer.
                                   * The key is the layer. */

  QMenu* show_entities_menu;      /**< The menu with the visibility of all entity types. */
  QToolButton*
      show_entities_button;       /**< The entity type visilibity toolbar button. */
  QMap<QString, QAction*>
      show_entities_subactions;   /**< Actions in the show entity type menu.
                                   * There is one action for each entity type,
                                   * plus two special actions "Show all" and "Hide all".
                                   * The key is the entity type name or
                                   * "action_show_all" or "action_hide_all". */

  QMap<QString, QAction*>
      common_actions;             /**< Actions available to all editors. */

  SettingsDialog settings_dialog; /**< The settings dialog. */
};

}

#endif
