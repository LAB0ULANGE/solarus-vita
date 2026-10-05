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
#ifndef SOLARUSEDITOR_EDITOR_TABS_H
#define SOLARUSEDITOR_EDITOR_TABS_H

#include "quest_database.h"
#include <QMap>
#include <QPointer>
#include <QTabWidget>
#include <memory>

template<typename T> class QSet;
class QUndoGroup;

namespace SolarusEditor {

class Editor;
class Quest;
class Refactoring;

/**
 * \brief The main tab widget with all editors currently open.
 */
class EditorTabs : public QTabWidget {
  Q_OBJECT

public:

  explicit EditorTabs(QWidget* parent = nullptr);
  ~EditorTabs() override;

  /** 
   * @brief Protocol for internal URLs handled by the editor.
   */
  static constexpr auto INTERNAL_URL_PROTOCOL = "solaruseditor://";

  QUndoGroup& get_undo_group();

  void open_resource(
      Quest& quest, ResourceType resource_type, const QString& id);
  void open_quest_properties_editor(Quest& quest);
  void open_map_editor(
      Quest& quest, const QString& path);
  void open_tileset_editor(
      Quest& quest, const QString& path);
  void open_sprite_editor(
      Quest& quest, const QString& path);
  void open_shader_editor(
      Quest& quest, const QString& path);
  void open_image_editor(
      Quest& quest, const QString& path);
  void open_text_editor(
      Quest& quest, const QString& path);
  void open_dialogs_editor(
      Quest& quest, const QString& language_id);
  void open_strings_editor(
      Quest& quest, const QString& language_id);
  void open_welcome_editor(
      Quest& quest, const QString& path);
  QString get_last_closed_file() const;
  void reopen_last_closed_editor(Quest& quest);

  int find_editor(const QString& path);
  bool show_editor(const QString& path);

  Editor* get_editor(int index);
  Editor* get_editor();

  bool confirm_before_closing();
  bool has_unsaved_files();
  bool has_unsaved_files_other_than(const QSet<QString>& ignored_paths);
  QStringList get_unsaved_files();
  void close_without_confirmation();
  void save_open_files_list();

  void reload_settings();

signals:

  void can_cut_changed(bool can_cut);
  void can_copy_changed(bool can_copy);
  void can_paste_changed(bool can_paste);
  void can_group_ungroup_changed();
  void refactoring_requested(const Refactoring& refactoring);
  void run_map_requested(const QString& map_id);
  void clear_console();
  void log_message_to_console(const QString& log_level, const QString& message);
  void new_quest_requested();
  void open_quest_requested();
  void documentation_requested();
  void website_requested();

public slots:

  bool save_file_requested(int index);
  bool save_all_files_requested();
  bool close_file_requested(int index);
  void close_all_files_requested();
  void open_file_requested(Quest& quest, const QString& path);
  void reload_file_requested(int index);
  void file_renamed(const QString& old_path, const QString& new_path);
  void file_deleted(const QString& path);
  void open_welcome_editor_requested(Quest& quest);

  bool handle_internal_link(Quest& quest, const QString& link);

protected:

  void keyPressEvent(QKeyEvent* event) override;

private slots:

  void current_editor_changed(int index);
  void current_editor_modification_state_changed(bool clean);
  void modification_state_changed(int index, bool clean);
  void show_tabbar_context_menu(const QPoint& mouse_position);

private:

  void add_editor(std::unique_ptr<Editor> editor);
  void insert_editor(std::unique_ptr<Editor> editor, int index);
  void remove_editor(int index);

  std::map<QString, std::unique_ptr<Editor>> editors;      /**< All editors currently open,
                                                            * indexed by their file path. */
  QUndoGroup* undo_group;                                  /**< Undo/redo stacks of open files. */
  QString last_closed_file;                                /**< Path to the file from the last closed tab. */
};

}

#endif
