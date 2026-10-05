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
#include "widgets/dialogs_editor.h"
#include "widgets/editor_tabs.h"
#include "widgets/gui_tools.h"
#include "widgets/image_editor.h"
#include "widgets/map_editor.h"
#include "widgets/shader_editor.h"
#include "widgets/sprite_editor.h"
#include "widgets/quest_properties_editor.h"
#include "widgets/strings_editor.h"
#include "widgets/text_editor.h"
#include "widgets/tileset_editor.h"
#include "widgets/welcome_editor.h"
#include "editor_exception.h"
#include "editor_settings.h"
#include "editor_style.h"
#include "quest.h"
#include <QFileInfo>
#include <QKeyEvent>
#include <QSet>
#include <QUndoGroup>
#include <QUndoStack>
#include <QProcess>
#include <QStringList>
#include <QTabBar>

namespace SolarusEditor {

/**
 * @brief Creates an editor tab widget.
 * @param parent The parent object or nullptr.
 */
EditorTabs::EditorTabs(QWidget* parent):
  QTabWidget(parent),
  undo_group(new QUndoGroup(this)) {

  QTabBar* tab_bar = new QTabBar();
  tab_bar->setTabsClosable(true);

  setTabBar(tab_bar);

  setMovable(true);
  setFocusPolicy(Qt::StrongFocus);

  connect(tab_bar, &QTabBar::tabCloseRequested,
          this, &EditorTabs::close_file_requested);
  connect(tab_bar, &QTabBar::currentChanged,
          this, &EditorTabs::current_editor_changed);

  connect(tab_bar, &QTabBar::customContextMenuRequested, this, &EditorTabs::show_tabbar_context_menu);
}

/**
 * @brief Destructor.
 */
EditorTabs::~EditorTabs() {
  disconnect(tabBar(), &QTabBar::currentChanged,
             nullptr, nullptr);
}

/**
 * @brief Returns the undo/redo group of all open files.
 * @return The undo/redo group of all open files.
 */
QUndoGroup& EditorTabs::get_undo_group() {
  return *undo_group;
}

/**
 * @brief Shows a tab to edit the specified resource element.
 * @param quest A Solarus quest.
 * @param resource_type A type of resource.
 * @param id Id of the resource element to open in this quest.
 */
void EditorTabs::open_resource(
    Quest& quest, ResourceType resource_type, const QString& id) {

  switch (resource_type) {

  case ResourceType::MAP:
    // Open the map data file.
    open_map_editor(quest, quest.get_map_data_file_path(id));
    break;

  case ResourceType::TILESET:
    // Open the tileset data file.
    open_tileset_editor(quest, quest.get_tileset_data_file_path(id));
    break;

  case ResourceType::LANGUAGE:
    // Open the dialogs file.
    open_dialogs_editor(quest, id);
    break;

  case ResourceType::SPRITE:
    // Open the sprite file.
    open_sprite_editor(quest, quest.get_sprite_path(id));
    break;

  case ResourceType::ITEM:
    // Open the item script.
    open_text_editor(quest, quest.get_item_script_path(id));
    break;

  case ResourceType::ENEMY:
    // Open the enemy script.
    open_text_editor(quest, quest.get_enemy_script_path(id));
    break;

  case ResourceType::ENTITY:
    // Open the custom entity model script.
    open_text_editor(quest, quest.get_entity_script_path(id));
    break;

  case ResourceType::SHADER:
    // Open the shader file.
    open_shader_editor(quest, quest.get_shader_data_file_path(id));
    break;

  case ResourceType::MUSIC:
  case ResourceType::SOUND:
  case ResourceType::FONT:
    // These resource types cannot be edited.
    break;
  }
}

/**
 * @brief Shows a tab to edit the quest properties.
 * @param quest A Solarus quest.
 */
void EditorTabs::open_quest_properties_editor(Quest& quest) {

  // Find the existing tab if any.
  int index = find_editor(quest.get_properties_path());
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new QuestPropertiesEditor(quest)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with an image editor.
 *
 * The file should be a PNG image.
 *
 * @param quest A Solarus quest.
 * @param path Path of the PNG file to open.
 */
void EditorTabs::open_image_editor(
    Quest& quest, const QString& path) {

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new ImageEditor(quest, path)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with a text editor.
 *
 * The file may be a Lua script.
 *
 * @param quest A Solarus quest.
 * @param path Path of the Lua file to open.
 */
void EditorTabs::open_text_editor(
    Quest& quest, const QString& path) {

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  EditorSettings settings;

  if (settings.get_value_bool(EditorSettings::external_text_editor_enabled)) {
      //Should open the external editor instead
      QString project_path = quest.get_root_path();
      QString cmd_str = settings.get_value_string(EditorSettings::external_text_editor_cmd);
#ifdef Q_OS_WIN
      // For Windows, use startDetached with the full command string
      cmd_str.replace("%f",path).replace("%p",project_path);
      if(QProcess::startDetached(cmd_str)) {
          return;
      }
#else
      // For Linux/Mac, replace placeholders in each argument after splitting
      QStringList cmd_args = cmd_str.split(' ', Qt::SkipEmptyParts);
      for (QString &arg : cmd_args) {
          arg.replace("%f", path);
          arg.replace("%p", project_path);
      }
      if (!cmd_args.isEmpty()) {
          QString cmd_prog = cmd_args.takeFirst();
          if (QProcess::startDetached(cmd_prog, cmd_args)) {
              return;
          }
      }
#endif
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new TextEditor(quest, path)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with a map editor.
 * @param quest A Solarus quest.
 * @param path Path of the map data file to open.
 */
void EditorTabs::open_map_editor(
    Quest& quest, const QString& path) {

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new MapEditor(quest, path)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with a tileset editor.
 * @param quest A Solarus quest.
 * @param path Path of the tileset data file to open.
 */
void EditorTabs::open_tileset_editor(
    Quest& quest, const QString& path) {

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new TilesetEditor(quest, path)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with a sprite editor.
 * @param quest A Solarus quest.
 * @param path Path of the sprite data file to open.
 */
void EditorTabs::open_sprite_editor(
    Quest& quest, const QString& path) {

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new SpriteEditor(quest, path)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with a shader editor.
 * @param quest A Solarus quest.
 * @param path Path of the shader data file to open.
 */
void EditorTabs::open_shader_editor(
    Quest& quest, const QString& path) {

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new ShaderEditor(quest, path)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with a language dialogs editor.
 * @param quest A Solarus quest.
 * @param path Path of the dialogs file to open.
 */
void EditorTabs::open_dialogs_editor(Quest& quest, const QString& language_id) {

  // Get the strings file path.
  QString path = quest.get_dialogs_path(language_id);

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new DialogsEditor(quest, language_id)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens a file with a language strings list editor.
 * @param quest A Solarus quest.
 * @param language_id Language id of the strings file to open.
 */
void EditorTabs::open_strings_editor(
    Quest& quest, const QString& language_id) {

  // Get the strings file path.
  QString path = quest.get_strings_path(language_id);

  if (!quest.is_in_root_path(path)) {
    // Not a file of this quest.
    return;
  }

  // Find the existing tab if any.
  int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new StringsEditor(quest, language_id)));
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Opens the welcome editor.
 * @param quest Unused but mandatory quest parameter to fit the Editor Tabs API.
 * @param path Path of the welcome editor to open.
 */
void EditorTabs::open_welcome_editor(Quest& quest, const QString& path) {
  // Find the existing tab if any.
  const int index = find_editor(path);
  if (index != -1) {
    // Already open.
    setCurrentIndex(index);
    return;
  }

  try {
    add_editor(std::unique_ptr<Editor>(new WelcomeEditor(quest, path)));
  }
  catch (const EditorException &ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Returns the path to the file of the last closed tab.
 */
QString EditorTabs::get_last_closed_file() const {
  return last_closed_file;
}

/**
 * @brief Reopens the last closed tab.
 * @param quest The quest.
 */
void EditorTabs::reopen_last_closed_editor(Quest& quest) {

  if (!last_closed_file.isEmpty()) {
    const QString file_path = last_closed_file;
    last_closed_file.clear();
    open_file_requested(quest, file_path);
  }
}

/**
 * @brief Creates a new tab and shows it.
 * @param editor The editor to put in the new tab.
 */
void EditorTabs::add_editor(std::unique_ptr<Editor> editor) {
  insert_editor(std::move(editor), count());
  setCurrentIndex(count() - 1);
}

/**
 * @brief Creates a new tab.
 * @param editor The editor to put in the new tab.
 * @param index Index of the tab to add.
 */
void EditorTabs::insert_editor(std::unique_ptr<Editor> editor, int index) {

  QUndoStack* undo_stack = &editor->get_undo_stack();
  undo_group->addStack(undo_stack);

  QString path = editor->get_file_path();
  insertTab(index, editor.get(), editor->get_icon(), editor->get_title());
  setTabToolTip(index, editor->get_file_path());

  // Show an asterisk in tab title when a file is modified.
  connect(undo_stack, &QUndoStack::cleanChanged,
          this, &EditorTabs::current_editor_modification_state_changed);

  connect(editor.get(), &Editor::can_cut_changed,
          this, &EditorTabs::can_cut_changed);
  connect(editor.get(), &Editor::can_copy_changed,
          this, &EditorTabs::can_copy_changed);
  connect(editor.get(), &Editor::can_paste_changed,
          this, &EditorTabs::can_paste_changed);
  connect(editor.get(), &Editor::can_group_ungroup_changed,
          this, &EditorTabs::can_group_ungroup_changed);
  connect(editor.get(), &Editor::clear_console,
          this, &EditorTabs::clear_console);
  connect(editor.get(), &Editor::log_message_to_console,
          this, &EditorTabs::log_message_to_console);
  connect(editor.get(), &Editor::open_file_requested,
          this, &EditorTabs::open_file_requested);
  connect(editor.get(), &Editor::refactoring_requested,
          this, &EditorTabs::refactoring_requested);
  connect(editor.get(), &Editor::run_map_requested,
          this, &EditorTabs::run_map_requested);
  connect(editor.get(), &Editor::new_quest_requested,
          this, &EditorTabs::new_quest_requested);
  connect(editor.get(), &Editor::open_quest_requested,
          this, &EditorTabs::open_quest_requested);
  connect(editor.get(), &Editor::documentation_requested,
          this, &EditorTabs::documentation_requested);
  connect(editor.get(), &Editor::website_requested,
          this, &EditorTabs::website_requested);

  editors.emplace(path, std::move(editor));
}

/**
 * @brief Closes the editor at the specified index without confirmation.
 * @param index An editor index.
 */
void EditorTabs::remove_editor(int index) {

  Editor* editor = get_editor(index);
  editor->about_to_be_closed();
  QString path = editor->get_file_path();

  undo_group->removeStack(&editor->get_undo_stack());

  removeTab(index);
  editors.erase(path);
}

/**
 * @brief Returns the editor at the specified index.
 * @param index An editor index.
 * @return The editor at this index in the tab bar or nullptr.
 */
Editor* EditorTabs::get_editor(int index) {

  if (index == -1 || index >= count()) {
    return nullptr;
  }

  return static_cast<Editor*>(widget(index));
}

/**
 * @brief Returns the editor currently open if any
 * @return The current editor.
 * Returns nullptr if there is no tab.
 */
Editor* EditorTabs::get_editor() {

  int index = currentIndex();
  if (index == -1) {
    return nullptr;
  }

  return get_editor(index);
}

/**
 * @brief Returns the index of an editor in the tabs.
 * @param path Path of a file to find the editor of.
 * @return The index of the editor or -1 if the file is not open.
 */
int EditorTabs::find_editor(const QString& path) {

  auto it = editors.find(path);
  if (it == editors.end()) {
    return -1;
  }

  Editor* editor = it->second.get();
  return indexOf(editor);
}

/**
 * @brief If the specified file is open in a tab, sets it as the current tab.
 * @param path Path of a file to show the editor of.
 * @return @c true if the file is open in an editor.
 */
bool EditorTabs::show_editor(const QString& path) {

  auto it = editors.find(path);
  if (it == editors.end()) {
    return -1;
  }

  Editor* editor = it->second.get();
  setCurrentWidget(editor);
  return true;
}

/**
 * @brief Slot called when the user attempts to save a file.
 * @param index Index of the tab to save.
 * @return @c true in case of success.
 */
bool EditorTabs::save_file_requested(int index) {

  Editor* editor = get_editor(index);
  if (editor == nullptr) {
    return false;
  }

  try {
    editor->save();
    editor->get_undo_stack().setClean();
    modification_state_changed(index, true);
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
    return false;
  }

  return true;
}

/**
 * @brief Slot called when the user attempts to save all tabs.
 * @return @c true in case of success.
 */
bool EditorTabs::save_all_files_requested() {

  bool success = true;
  for (int i = 0; i < count(); ++i) {
    success = success && save_file_requested(i);
  }
  return success;
}

/**
 * @brief Slot called when the user attempts to open a file.
 * @param quest The quest that holds this file.
 * @param path Path of the file to open.
 */
void EditorTabs::open_file_requested(Quest& quest, const QString& path) {

  if (path.isEmpty()) {
    return;
  }

  // Handle internal links first.
  if (path.startsWith(INTERNAL_URL_PROTOCOL)) {
    if (handle_internal_link(quest, path)) {
      return;
    }
  }

  QFileInfo file_info(path);
  QString canonical_path = file_info.canonicalFilePath();
  if (!quest.is_in_root_path(canonical_path)) {
    // Not a file of this quest.
    return;
  }

  ResourceType resource_type;
  QString element_id;
  if (quest.is_resource_element(canonical_path, resource_type, element_id)) {
    // A resource element declared in the quest.
    // Possibly a map data file, an enemy Lua script,
    // a language directory, etc.
    open_resource(quest, resource_type, element_id);
  }
  else if (quest.is_dialogs_file(canonical_path, element_id)) {
    open_dialogs_editor(quest, element_id);
  }
  else if (quest.is_strings_file(canonical_path, element_id)) {
    open_strings_editor(quest, element_id);
  }
  else if (quest.is_image(canonical_path)) {
    // A PNG image.
    open_image_editor(quest, canonical_path);
  }
  else if (quest.is_script(canonical_path)) {
    // A Lua script that is not a resource element.
    open_text_editor(quest, canonical_path);
  }
  else if (quest.is_shader_code_file(canonical_path)) {
    // A GLSL file.
    open_text_editor(quest, canonical_path);
  }
  else if (quest.is_properties_path(canonical_path)) {
    // Opening quest.dat directly.
    open_quest_properties_editor(quest);
  }
  else if (quest.is_data_path(canonical_path)) {
    // Opening the quest root also opens quest.dat.
    open_quest_properties_editor(quest);
  }
}

/**
 * @brief Handles an internal link.
 *
 * Internal links start with the "solaruseditor://" protocol.
 *
 * @param quest Unused but mandatory quest parameter to fit the Editor Tabs API.
 * @param link The link to handle.
 * @return @c true if the link was handled.
 */
bool EditorTabs::handle_internal_link(Quest& quest, const QString& link) {
  QString trimmed_link = link.mid(strlen(INTERNAL_URL_PROTOCOL));
  if (trimmed_link == "welcome") {
    // solaruseditor://welcome : Open the welcome editor.
    open_welcome_editor(quest, link);
    return true;
  }

  return false;
}

/**
 * @brief Slot called when the user attempts to close a tab.
 * @param index Index of the tab to close.
 * @return @c true if the tab was closed.
 */
bool EditorTabs::close_file_requested(int index) {

  Editor* editor = get_editor(index);
  if (editor != nullptr && editor->confirm_before_closing()) {
    last_closed_file = editor->get_file_path();
    remove_editor(index);
    return true;
  }
  return false;
}

/**
 * @brief Slot called when the user attempts to close all tabs.
 */
void EditorTabs::close_all_files_requested() {

  if (confirm_before_closing()) {
    close_without_confirmation();
  }
}

/**
 * @brief Slot called when the user wants to reload the file of a tab.
 * @param index Index of the tab to reload.
 */
void EditorTabs::reload_file_requested(int index) {

  int active_editor_index = currentIndex();

  Editor* editor = get_editor(index);
  if (editor == nullptr) {
    return;
  }
  Quest& quest = editor->get_quest();
  QString path = editor->get_file_path();

  close_file_requested(index);
  open_file_requested(quest, path);

  editor = get_editor(count() - 1);

  removeTab(count() - 1);
  insertTab(index, editor, editor->get_icon(), editor->get_title());
  setTabToolTip(index, editor->get_file_path());

  setCurrentIndex(active_editor_index);
}

/**
 * @brief Slot called when a quest file of has been renamed.
 *
 * The corresponding tab is closed if any.
 *
 * @param old_path Old path of the file.
 * @param new_path New path after renaming.
 */
void EditorTabs::file_renamed(const QString& old_path, const QString& new_path) {

  Q_UNUSED(new_path);

  if (get_editor() == nullptr) {
    return;
  }

  Quest& quest = get_editor()->get_quest();
  ResourceType resource_type;
  QString language_id;

  QString path = old_path;
  if (quest.is_potential_resource_element(path, resource_type, language_id) &&
      resource_type == ResourceType::LANGUAGE) {

    int index = find_editor(quest.get_strings_path(language_id));
    if (index != -1) {
      remove_editor(index);
    }
    path = quest.get_dialogs_path(language_id);
  }

  int index = find_editor(path);
  if (index != -1) {
    remove_editor(index);
  }
}

/**
 * @brief Slot called when a quest file of has been deleted.
 *
 * The corresponding tab is closed if any.
 *
 * @param path Path of the deleted file.
 */
void EditorTabs::file_deleted(const QString& path) {

  int index = find_editor(path);
  if (index != -1) {
    remove_editor(index);
  }
}

/**
 * @brief Slot called when the user wants to open the welcome editor.
 * 
 * @param quest Unused but mandatory quest parameter to fit the Editor Tabs API.
 */
void EditorTabs::open_welcome_editor_requested(Quest& quest) {
  open_file_requested(
      quest,
      QString(INTERNAL_URL_PROTOCOL) + "welcome");
}

/**
 * @brief Function called when the user wants to close all editors.
 *
 * This function does not close any editor, it only lets the user save them.
 *
 * @return @c false to cancel the closing operation.
 */
bool EditorTabs::confirm_before_closing() {

  for (int i = 0; i < count(); ++i) {

    Editor* editor = get_editor(i);
    if (!editor->confirm_before_closing()) {
      return false;
    }
  }

  return true;
}

/**
 * @brief Returns whether at least one editor has unsaved modifications.
 * @return @c true if there are unsaved tabs.
 */
bool EditorTabs::has_unsaved_files() {

  return has_unsaved_files_other_than(QSet<QString>());
}

/**
 * @brief Returns whether at least one editor has unsaved modifications
 * and is not in the given list.
 * @param ignored_paths File paths to ignore in this check.
 * @return @c true if at least an open file not in the list is unsaved.
 */
bool EditorTabs::has_unsaved_files_other_than(const QSet<QString>& ignored_paths) {

  for (int i = 0; i < count(); ++i) {
    const Editor* editor = get_editor(i);
    if (ignored_paths.contains(editor->get_file_path())) {
      continue;
    }
    if (!editor->get_undo_stack().isClean()) {
      return true;
    }
  }

  return false;
}

/**
 * @brief Returns the paths of unsaved files in open editors.
 * @return The unsaved file paths.
 */
QStringList EditorTabs::get_unsaved_files() {

  QStringList unsaved_paths;
  for (int i = 0; i < count(); ++i) {
    const Editor* editor = get_editor(i);
    if (!editor->get_undo_stack().isClean()) {
      unsaved_paths << editor->get_file_path();
    }
  }

  return unsaved_paths;
}

/**
 * @brief Closes all editors without confirmation.
 */
void EditorTabs::close_without_confirmation() {

  for (int i = count() - 1; i >= 0; --i) {
    remove_editor(i);
  }
}

/**
 * @brief Reloads settings of all editors.
 */
void EditorTabs::reload_settings() {

  for (int i = 0; i < count(); ++i) {
    get_editor(i)->reload_settings();
  }
}

/**
 * @brief Slot called when the current tab changes.
 * @param index Index of the new current tab.
 */
void EditorTabs::current_editor_changed(int index) {

  Q_UNUSED(index);

  Editor* editor = get_editor();
  if (editor == nullptr) {
    get_undo_group().setActiveStack(nullptr);
    emit can_cut_changed(false);
    emit can_copy_changed(false);
    emit can_paste_changed(false);
    emit can_group_ungroup_changed();
  }
  else {
    get_undo_group().setActiveStack(&editor->get_undo_stack());
    emit can_cut_changed(editor->can_cut());
    emit can_copy_changed(editor->can_copy());
    emit can_paste_changed(editor->can_paste());
    emit can_group_ungroup_changed();
    editor->setFocus();
  }
}

/**
 * @brief Saves the list of open tabs.
 */
void EditorTabs::save_open_files_list() {

  EditorSettings settings;
  QStringList last_files;
  for (int i = 0; i < count(); ++i) {

    Editor* editor = get_editor(i);
    last_files << editor->get_file_path();
  }

  settings.set_value(EditorSettings::last_files, last_files);

  Editor* editor = get_editor();
  QString file_path = (editor == nullptr) ? QString() : editor->get_file_path();
  settings.set_value(EditorSettings::last_file, file_path);
}

/**
 * @brief Slot called when the is-modified state of the current tab has changed.
 * @param clean @c true if the file is now clean, @c false if it is now
 * modified.
 */
void EditorTabs::current_editor_modification_state_changed(bool clean) {

  modification_state_changed(currentIndex(), clean);
}

/**
 * @brief Slot called when the is-modified state of a tab has changed.
 * @param index Index of a tab.
 * @param clean @c true if the file is now clean, @c false if it is now
 * modified.
 */
void EditorTabs::modification_state_changed(int index, bool clean) {

  Editor* editor = get_editor(index);
  if (editor == nullptr) {
    return;
  }

  QString title = editor->get_title();
  if (!clean) {
    title += '*';
  }
  setTabText(index, title);
}

/**
 * @brief Slot called when the context menu of the QTabBar needs to be displayed.
 * @param mouse_position The mouse position relative to the QTabBar.
 */
void EditorTabs::show_tabbar_context_menu(const QPoint& mouse_position) {
  QTabBar* tab_bar = tabBar();
  const int index = tab_bar->tabAt(mouse_position);
  if (index != -1) {
    QMenu menu(this);

    EditorStyle::setAutoIconColor(&menu, EditorStyle::AutoIconColor::TextColor);

    QAction *action_close = new QAction(
        QIcon(":/images/icon_close_tab.svg"),
        tr("Close"), &menu);
    // action_close->setShortcut({QKeySequence::StandardKey::Close});
    connect(action_close, &QAction::triggered, this, [this, index]() {
      emit close_file_requested(index);
    });

    QAction *action_close_all = new QAction(
        QIcon(":/images/icon_close_all_tabs.svg"),
        "Close All", &menu);
    action_close_all->setShortcut(QKeyCombination{Qt::ControlModifier | Qt::ShiftModifier, Qt::Key_W});
    connect(action_close_all, &QAction::triggered, this, &EditorTabs::close_all_files_requested);

    menu.addAction(action_close);
    menu.addAction(action_close_all);
    menu.exec(tab_bar->mapToGlobal(mouse_position));
  }
}

/**
 * @brief Receives a key press event.
 *
 * Reimplemented to switch tabs with Ctrl+Tab and Ctrl+PageUp or
 * Ctrl+Shift+Tab and Ctrl+PageDown.
 *
 * @param event The event to handle.
 */
void EditorTabs::keyPressEvent(QKeyEvent* event) {

  const bool control = event->modifiers() & Qt::ControlModifier;
  const bool shift = event->modifiers() & Qt::ShiftModifier;
  const int key = event->key();

  if (count() > 1) {
    if ((control && key == Qt::Key_PageDown) ||
        (control && key == Qt::Key_Tab)) {
      setCurrentIndex((currentIndex() + 1) % count());
      return;
    }

    if ((control && key == Qt::Key_PageUp) ||
      (control && shift && key == Qt::Key_Tab)) {
      setCurrentIndex((currentIndex() + count() - 1) % count());
      return;
    }
  }

  QTabWidget::keyPressEvent(event);
}

}
