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
#include "widgets/change_file_info_dialog.h"
#include "widgets/editor_tabs.h"
#include "widgets/gui_tools.h"
#include "widgets/new_resource_element_dialog.h"
#include "widgets/new_element_dialog.h"
#include "widgets/quest_tree_view.h"
#include "audio.h"
#include "editor_exception.h"
#include "editor_style.h"
#include "quest.h"
#include "quest_files_model.h"
#include <QContextMenuEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QUrl>
#include <QTimer>
#include <QHeaderView>

namespace SolarusEditor {

/**
 * @brief Creates a quest tree view.
 * @param parent The parent or nullptr.
 */
QuestTreeView::QuestTreeView(QWidget* parent) :
  QTreeView(parent),
  model(nullptr),
  play_action(nullptr),
  open_action(nullptr),
  rename_action(nullptr),
  delete_action(nullptr),
  read_only(false),
  opening_files_allowed(true) {

  setUniformRowHeights(true);
  setAutoScroll(false);
  setAlternatingRowColors(true);
  setSelectionBehavior(SelectRows);
  setSelectionMode(ExtendedSelection);

  play_action = new QAction(tr("Play"), this);
  play_action->setShortcutContext(Qt::WidgetShortcut);
  connect(play_action, &QAction::triggered,
          this, &QuestTreeView::play_action_triggered);
  addAction(play_action);

  open_action = new QAction(tr("Open"), this);
  open_action->setShortcutContext(Qt::WidgetShortcut);
  connect(open_action, &QAction::triggered,
          this, &QuestTreeView::open_action_triggered);
  addAction(open_action);

  rename_action = new QAction(
        QIcon(":/images/icon_rename.svg"), tr("Rename..."), this);
  rename_action->setShortcut(tr("F2"));
  rename_action->setShortcutContext(Qt::WidgetShortcut);
  connect(rename_action, &QAction::triggered,
          this, &QuestTreeView::rename_action_triggered);
  addAction(rename_action);

  delete_action = new QAction(
        QIcon(":/images/icon_delete.svg"), tr("Delete..."), this);
  delete_action->setShortcut(QKeySequence::Delete);
  delete_action->setShortcutContext(Qt::WidgetShortcut);
  connect(delete_action, &QAction::triggered,
          this, &QuestTreeView::delete_action_triggered);
  addAction(delete_action);

  change_file_info_action = new QAction(
      QIcon(":/images/icon_author.svg"),tr("Author and license..."), this);
  change_file_info_action->setShortcut(tr("F6"));
  change_file_info_action->setShortcutContext(Qt::WidgetShortcut);
  connect(change_file_info_action, &QAction::triggered,
          this, &QuestTreeView::change_file_info_action_triggered);
  addAction(change_file_info_action);

  header()->setSortIndicatorClearable(true);

  // Tell Qlementine to not draw external borders.
  setFrameShadow(QFrame::Shadow::Plain);
}

/**
 * @brief Sets the quest to represent in this tree view.
 * @param quest The quest.
 */
void QuestTreeView::set_quest(Quest& quest) {

  // Clean the old tree.
  setModel(nullptr);
  setSortingEnabled(false);

  if (quest.exists()) {
    // Create a new model.
    model = new QuestFilesModel(quest, this);
    setModel(model);
    setRootIndex(model->get_quest_root_index());

    if (model->hasChildren(rootIndex())) {
      expand(model->index(0, 0, rootIndex()));  // Expand the data directory.
    }

    sortByColumn(0, Qt::AscendingOrder);
    setColumnWidth(QuestFilesModel::FILE_COLUMN, 200);
    setColumnWidth(QuestFilesModel::DESCRIPTION_COLUMN, 100);

    connect(&quest, &Quest::file_renamed,
            this, &QuestTreeView::file_renamed);
    connect(selectionModel(), &QItemSelectionModel::selectionChanged,
            this, [this](const QItemSelection&, const QItemSelection&) {
      emit selected_path_changed(get_selected_path());
    });

    // It is better for performance to enable sorting only after the model is ready.
    setSortingEnabled(true);
  }
}

/**
 * @brief Returns whether the view is in read-only mode.
 * @return @c true if the mode is read-only, @c false if changes can be
 * made to the tileset.
 */
bool QuestTreeView::is_read_only() const {
  return read_only;
}

/**
 * @brief Sets whether the view is in read-only mode.
 * @param read_only @c true to block changes from this view, @c false to allow them.
 */
void QuestTreeView::set_read_only(bool read_only) {

  this->read_only = read_only;
  rename_action->setEnabled(!read_only);
  delete_action->setEnabled(!read_only);
}

/**
 * @brief Returns whether the user can open files from this view.
 * @return @c true if opening files is allowed.
 */
bool QuestTreeView::is_opening_files_allowed() const {
  return this->opening_files_allowed;
}

/**
 * @brief Sets whether the user can open files from this view.
 * @param opening_files_allowed @c true to allow opening files.
 */
void QuestTreeView::set_opening_files_allowed(bool opening_files_allowed) {

  this->opening_files_allowed = opening_files_allowed;
  open_action->setEnabled(opening_files_allowed);
  play_action->setEnabled(opening_files_allowed);
}

/**
 * @brief Returns the selected file path if exactly one item is selected.
 * @return The selected file path or an empty string.
 */
QString QuestTreeView::get_selected_path() const {

  if (selectionModel() == nullptr) {
    return QString();
  }
  QModelIndexList selected_indexes = selectionModel()->selectedRows();
  if (selected_indexes.size() != 1) {
    return QString();
  }
  return model->get_file_path(selected_indexes.first());
}

/**
 * @brief Selects the item corresponding to the specified file path.
 *
 * Clears any previous selection.
 * Also ensures that the item is expanded.
 *
 * @param The file path to select.
 * Nothing is selected if the path does not exist in the tree.
 */
void QuestTreeView::set_selected_path(const QString& path) {

  if (path.isEmpty()) {
    selectionModel()->clear();
    return;
  }

  if (path == QString(EditorTabs::INTERNAL_URL_PROTOCOL) + "welcome") {
    return;
  }
  set_selected_paths(QStringList() << path);
}

/**
 * @brief Adds the specified file path to the selection.
 *
 * Also ensures that the item is expanded.
 *
 * @param The file path to select.
 * Nothing happens if the path does not exist in the tree.
 */
void QuestTreeView::add_selected_path(const QString& path) {

  if (selectionModel() == nullptr) {
    return;
  }
  QModelIndex index = model->get_file_index(path);
  if (!index.isValid()) {
    return;
  }

  selectionModel()->select(index, QItemSelectionModel::Select | QItemSelectionModel::Rows);
  expand(index.parent());
}

/**
 * @brief Returns the selected file paths.
 * @return The paths of all selected items.
 */
QStringList QuestTreeView::get_selected_paths() const {

  if (selectionModel() == nullptr) {
    return QStringList();
  }

  QStringList selected_paths;
  const QModelIndexList& indexes = selectionModel()->selectedRows();
  for (const QModelIndex& index : indexes) {
    selected_paths << model->get_file_path(index);
  }
  return selected_paths;
}

/**
 * @brief Selects the specified paths and clears the previous selection.
 *
 * Also ensures that the items are expanded.
 *
 * @param The file paths to select.
 * An empty list unselects any previous path.
 * Paths that do not exist in the tree are ignored.
 */
void QuestTreeView::set_selected_paths(const QStringList& paths) {

  if (selectionModel() == nullptr) {
    return;
  }
  selectionModel()->clear();
  QItemSelection selection;
  for (const QString& path : paths) {
    QModelIndex index = model->get_file_index(path);
    if (!index.isValid()) {
      // Item to deep in the model for now: try harder
      // to build it.
      expand_to_path(path);
      index = model->get_file_index(path);
    }
    selection.select(index, index);
    expand(index.parent());
  }
  selectionModel()->select(selection, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
}

/**
 * @brief Expands the tree to the selected path.
 * @param path The path to make selected.
 */
void QuestTreeView::expand_to_path(const QString& path) {

  const Quest& quest = model->get_quest();
  const QString& parent_path = QFileInfo(path).path();
  if (parent_path.startsWith(quest.get_data_path())) {
    // Try to expand parents first otherwise the child index
    // might not exist.
    expand_to_path(parent_path);
  }
  const QModelIndex& index = model->get_file_index(path);
  if (index.isValid()) {
    expand(index.parent());
  }
}

/**
 * @brief Receives a double-click event.
 * @param event The event to handle.
 */
void QuestTreeView::mouseDoubleClickEvent(QMouseEvent* event) {

  QModelIndex index = indexAt(event->pos());
  if (index.isValid() &&
      index.column() == QuestFilesModel::FILE_COLUMN &&
      !model->hasChildren(index) &&
      event->button() == Qt::LeftButton) {
    // Left double-click on leaf item: open or play the file.
    default_action_triggered();
    return;
  }

  // Other cases: keep the default double-click behavior
  // (like expanding nodes).
  QTreeView::mouseDoubleClickEvent(event);
}

/**
 * @brief Shows a popup menu with actions related to the selected item.
 * @param event The event to handle.
 */
void QuestTreeView::contextMenuEvent(QContextMenuEvent* event) {

  if (model == nullptr) {
    return;
  }

  Quest& quest = model->get_quest();
  if (!quest.is_valid()) {
    return;
  }

  QStringList paths = get_selected_paths();
  if (paths.isEmpty()) {
    return;
  }
  QMenu* menu = new QMenu(this);
  EditorStyle::setAutoIconColor(menu, EditorStyle::AutoIconColor::ForegroundColor);

  build_context_menu_play(*menu, paths);
  build_context_menu_open(*menu, paths);
  build_context_menu_new(*menu, paths);
  build_context_menu_rename(*menu, paths);
  build_context_menu_delete(*menu, paths);

  if (menu->isEmpty()) {
    delete menu;
  }
  else {
    menu->popup(viewport()->mapToGlobal(event->pos()) + QPoint(1, 1));
  }
}

/**
 * @brief Builds the "New" part of a context menu for a file.
 * @param menu The context menu being created.
 * @param paths Paths whose context menu is requested.
 */
void QuestTreeView::build_context_menu_new(QMenu& menu, const QStringList& paths) {

  if (paths.size() != 1) {
    return;
  }
  QString path = paths.first();

  if (is_read_only()) {
    return;
  }

  if (!menu.isEmpty()) {
    menu.addSeparator();
  }

  Quest& quest = model->get_quest();
  const QuestDatabase& database = quest.get_database();

  ResourceType resource_type;
  QString element_id;
  const bool is_potential_resource_element = quest.is_potential_resource_element(path, resource_type, element_id);
  const bool is_declared_resource_element = is_potential_resource_element && database.exists(resource_type, element_id);
  const bool is_dir = QFileInfo(path).isDir();

  QAction* new_resource_element_action = nullptr;

  if (is_potential_resource_element) {

    if (is_declared_resource_element) {
      // No creation item on an existing resource element.
      return;
    }

    if (quest.is_in_resource_element(path, resource_type, element_id)) {
      // Don't create an element in the tree of an already existing element.
      // (languages have a tree because they are actually directories).
      return;
    }

    // File that looks like a resource element, but that is not declared in
    // the quest: let the user add it.

    QString resource_type_friendly_name = database.get_friendly_name(resource_type);
    QString resource_type_lua_name = database.get_lua_name(resource_type);

    new_resource_element_action = new QAction(
          QIcon(":/images/symbolic_icon_resource_" + resource_type_lua_name + ".svg"),
          tr("Add to quest as %1...").arg(resource_type_friendly_name),
          this);
  }
  else if (quest.is_resource_path(path, resource_type) ||
           (is_dir && quest.is_in_resource_path(path, resource_type))) {
    // Resource directory or subdirectory: let the user create a new element in it.

    QString resource_type_create_friendly_name = database.get_create_friendly_name(resource_type);
    QString resource_type_lua_name = database.get_lua_name(resource_type);

    new_resource_element_action = new QAction(
          QIcon(":/images/symbolic_icon_resource_" + resource_type_lua_name + ".svg"),
          resource_type_create_friendly_name,
          this);
  } else if (quest.is_image(path)) {
    // Image file: let the user create a sprite from it, unless it already exists.
    static const QRegularExpression pngExtension("\\.png$");
    QString sprite_path = path;
    sprite_path.replace(pngExtension, ".dat");
    if (quest.is_potential_resource_element(sprite_path, resource_type, element_id) &&
        resource_type == ResourceType::SPRITE &&
        !quest.exists(sprite_path)) {
      new_resource_element_action = new QAction(
            QIcon(":/images/symbolic_icon_resource_sprite.svg"),
            tr("New sprite from image..."),
            this);
    }
  }

  if (new_resource_element_action != nullptr) {

    connect(new_resource_element_action, &QAction::triggered,
            this, &QuestTreeView::new_element_action_triggered);
    menu.addAction(new_resource_element_action);

    if (is_dir && resource_type == ResourceType::SHADER) {
      QAction* action = new QAction(
            QIcon(":/images/symbolic_icon_shader_code.svg"),
            tr("New GLSL file..."),
            this);
      connect(action, &QAction::triggered,
              this, &QuestTreeView::new_shader_code_file_action_triggered);
      menu.addAction(action);
    }

    menu.addSeparator();
  }

  if (is_dir) {
    // Any directory.

    QAction* action = new QAction(
          QIcon(":/images/icon_add_folder.svg"),
          tr("New folder..."),
          this);
    connect(action, &QAction::triggered,
            this, &QuestTreeView::new_directory_action_triggered);
    menu.addAction(action);

    action = new QAction(
          QIcon(":/images/symbolic_icon_script.svg"),
          tr("New script..."),
          this);
    connect(action, &QAction::triggered,
            this, &QuestTreeView::new_script_action_triggered);
    menu.addAction(action);
  }
}

/**
 * @brief Builds the "Play" part of a context menu for a file.
 * @param menu The context menu being created.
 * @param paths Paths whose context menu is requested.
 */
void QuestTreeView::build_context_menu_play(QMenu& menu, const QStringList& paths) {

  if (paths.size() != 1) {
    return;
  }
  QString path = paths.first();

  const Quest& quest = model->get_quest();

  ResourceType resource_type;
  QString element_id;
  if (quest.is_potential_resource_element(path, resource_type, element_id)) {
    // A resource element.

    switch (resource_type) {

    case ResourceType::MUSIC:
      play_action->setEnabled(quest.exists(path));
      if (Audio::is_playing_music(quest, element_id)) {
        play_action->setIcon(QIcon(":/images/icon_stop.svg"));
        play_action->setText(tr("Stop"));
      }
      else {
        play_action->setIcon(QIcon(":/images/icon_start.svg"));
        play_action->setText(tr("Play"));
      }
      menu.addAction(play_action);
      break;

    case ResourceType::SOUND:
      play_action->setEnabled(quest.exists(path));
      play_action->setIcon(QIcon(":/images/icon_start.svg"));
      play_action->setText(tr("Play"));
      menu.addAction(play_action);

    default:
      break;
    }

  }
}

/**
 * @brief Builds the "Open" part of a context menu for a file.
 * @param menu The context menu being created.
 * @param paths Paths whose context menu is requested.
 */
void QuestTreeView::build_context_menu_open(QMenu& menu, const QStringList& paths) {

  if (paths.size() != 1) {
    return;
  }
  QString path = paths.first();

  if (!is_opening_files_allowed()) {
    return;
  }

  if (!menu.isEmpty()) {
    menu.addSeparator();
  }

  Quest& quest = model->get_quest();
  const QuestDatabase& database = quest.get_database();
  QAction* action = nullptr;
  open_action->setText(tr("Open"));  // Restore the normal Open text and icon.
  open_action->setIcon(QIcon());

  ResourceType resource_type;
  QString element_id;
  if (quest.is_resource_element(path, resource_type, element_id)) {
    // A resource element.

    QString resource_type_lua_name = database.get_lua_name(resource_type);
    open_action->setIcon(
          QIcon(":/images/symbolic_icon_resource_" + resource_type_lua_name + ".svg"));

    switch (resource_type) {

    case ResourceType::MAP:

      // For a map, the user can open the map data file or the map script.
      menu.addAction(open_action);

      action = new QAction(
            QIcon(":/images/symbolic_icon_script.svg"),
            tr("Open Script"),
            this
      );
      connect(action, SIGNAL(triggered()),
              this, SLOT(open_map_script_action_triggered()));
      menu.addAction(action);
      break;

    case ResourceType::LANGUAGE:

      // For a language, the user can open dialogs or strings.
      open_action->setText(tr("Open Dialogs"));
      open_action->setIcon(QIcon(":/images/symbolic_icon_dialogs.svg"));
      menu.addAction(open_action);

      action = new QAction(
            QIcon(":/images/symbolic_icon_strings.svg"),
            tr("Open Strings"),
            this
      );
      connect(action, SIGNAL(triggered()),
              this, SLOT(open_language_strings_action_triggered()));
      menu.addAction(action);

      menu.addSeparator();
      break;

    case ResourceType::TILESET:
    case ResourceType::SPRITE:
    case ResourceType::ITEM:
    case ResourceType::ENEMY:
    case ResourceType::ENTITY:
    case ResourceType::SHADER:
      // Other editable resource types,
      menu.addAction(open_action);
      break;

    case ResourceType::MUSIC:
    case ResourceType::SOUND:
    case ResourceType::FONT:
      // These resource types cannot be edited.
      break;
    }

  }
  else if (quest.is_image(path)) {
    // Open a PNG file.
    open_action->setIcon(QIcon(":/images/symbolic_icon_image.svg"));
    menu.addAction(open_action);
  }
  else if (quest.is_script(path)) {
    // Open a Lua script that is not a resource.
    open_action->setIcon(QIcon(":/images/symbolic_icon_script.svg"));
    menu.addAction(open_action);
  }
  else if (quest.is_shader_code_file(path)) {
    // Open a GLSL file.
    open_action->setIcon(QIcon(":/images/symbolic_icon_shader_code.svg"));
    menu.addAction(open_action);
  }
  else if (quest.is_data_path(path)) {
    // Open quest properties file.
    open_action->setText(tr("Open Properties"));
    open_action->setIcon(QIcon(":/images/icon_tool.svg"));
    menu.addAction(open_action);
  }

  if (quest.is_dir(path)) {
    QAction* explore_action = new QAction(
          QIcon(":/images/icon_external_link.svg"),
          tr("Explore folder"),
          this
    );
    connect(explore_action, &QAction::triggered,
            this, [path]() {
      QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    });
    menu.addAction(explore_action);
    return;
  }

}

/**
 * @brief Builds the "Rename" part of a context menu for a file.
 * @param menu The context menu being created.
 * @param paths Paths whose context menu is requested.
 */
void QuestTreeView::build_context_menu_rename(QMenu& menu, const QStringList& paths) {

  if (paths.isEmpty()) {
    return;
  }

  if (is_read_only()) {
    return;
  }

  if (!menu.isEmpty()) {
    menu.addSeparator();
  }

  Quest& quest = model->get_quest();

  // Rename + Change description.
  const QString& path = paths.first();
  ResourceType resource_type;
  QString element_id;
  if (paths.size() == 1 &&
      !quest.is_resource_path(path, resource_type) &&
      path != quest.get_data_path()) {

    // We don't want to rename the data directory.
    // or the built-in resource directories.
    // All other paths can have a "Rename" menu item.
    menu.addAction(rename_action);

    if (quest.is_resource_element(path, resource_type, element_id)) {
      // Resource element: additionally, allow to change the description.
      QAction* action = new QAction(
          QIcon(":/images/icon_edit.svg"),
          tr("Change description..."), this);
      connect(action, &QAction::triggered,
              this, &QuestTreeView::change_description_action_triggered);
      menu.addAction(action);
    }
  }

  // Allow to change metadata.
  if (paths.size() > 1 ||
      path != quest.get_data_path()) {
    menu.addAction(change_file_info_action);
  }
}

/**
 * @brief Builds the "Delete" part of a context menu for a file.
 * @param menu The context menu being created.
 * @param paths Paths whose context menu is requested.
 */
void QuestTreeView::build_context_menu_delete(QMenu& menu, const QStringList& paths) {

  if (is_read_only()) {
    return;
  }

  if (!can_delete_paths(paths)) {
    return;
  }

  if (!menu.isEmpty()) {
    menu.addSeparator();
  }

  menu.addAction(delete_action);
}

/**
 * @brief Slot called when the user wants to create a new resource element.
 *
 * The type of resource will be the one of the selected element.
 * The id and description of the element will be prompted in a dialog.
 * If the selected element is an existing file, then its id is initially
 * proposed in the dialog.
 */
void QuestTreeView::new_element_action_triggered() {

  const QString path = get_selected_path();
  if (path.isEmpty()) {
    return;
  }

  try {
    Quest& quest = model->get_quest();
    QuestDatabase& database = quest.get_database();
    ResourceType resource_type;
    QString initial_id_value;
    QString initial_description_value;

    if (quest.is_potential_resource_element(path, resource_type, initial_id_value)) {
      if (database.exists(resource_type, initial_id_value)) {
        // The element already exists.
        return;
      }
      QString element_id;
      if (quest.is_in_resource_element(path, resource_type, element_id)) {
        // Already under an existing element.
        return;
      }
      initial_description_value = initial_id_value;
    }
    else if (quest.is_image(path)) {
      // Creating a sprite from an image.
      static const QRegularExpression pngExtension("\\.png$");
      QString sprite_path = path;
      sprite_path.replace(pngExtension, ".dat");
      if (quest.is_potential_resource_element(sprite_path, resource_type, initial_id_value) &&
          resource_type == ResourceType::SPRITE) {
        static const QRegularExpression datExtension("\\.dat$");
        initial_description_value = QFileInfo(sprite_path).fileName().replace(datExtension, "");
      }
    }
    else {
      if (!quest.is_resource_path(path, resource_type) &&
          !quest.is_in_resource_path(path, resource_type)) {
        // We expect a built-in resource directory or a subdirectory or it.
        return;
      }

      // Put the directory clicked as initial id value in the dialog.
      QString resource_path = quest.get_resource_path(resource_type);
      if (path != resource_path) {
        initial_id_value = path.right(path.size() - resource_path.length() - 1) + '/';
      }
      else {
        initial_id_value = "";  // Top-level resource directory.
      }
    }

    // Check if tilesets exist to create a map
    if (resource_type == ResourceType::MAP) {
      QStringList tilesets = database.get_elements(ResourceType::TILESET);
      if (tilesets.count() == 0) {
        GuiTools::error_dialog(tr("No tileset present. Create a tileset before creating a new map."));
        return;
      }
    }

    NewResourceElementDialog dialog(resource_type, parentWidget());
    dialog.set_element_id(initial_id_value);
    dialog.set_element_description(initial_description_value);
    int result = dialog.exec();

    if (result != QDialog::Accepted) {
      return;
    }

    QString element_id = dialog.get_element_id();
    QString description = dialog.get_element_description();
    const QuestDatabase::FileInfo& data_file_info = dialog.get_data_file_info();
    const QuestDatabase::FileInfo& script_file_info = dialog.get_script_file_info();

    quest.create_resource_element(
          resource_type, element_id, description, data_file_info, script_file_info);

    if (quest.is_image(path)) {
      quest.create_sprite_from_image(path, element_id);
    }

    QString created_path = quest.get_resource_element_path(resource_type, element_id);
    if (quest.exists(created_path)) {
      // Select the resource created.
      set_selected_path(created_path);

      // Open it.
      emit open_file_requested(quest, created_path);
    }
  }

  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Slot called when the user wants to create a new directory
 * under the selected directory.
 *
 * The directory name will be prompted to the user.
 */
void QuestTreeView::new_directory_action_triggered() {

  create_new_file("");
}

/**
 * @brief Slot called when the user wants to create a new Lua script
 * under the selected directory.
 *
 * The file name will be prompted to the user.
 */
void QuestTreeView::new_script_action_triggered() {

  create_new_file("lua");
}

/**
 * @brief Slot called when the user wants to create a new Lua script
 * under the selected directory.
 *
 * The file name will be prompted to the user.
 */
void QuestTreeView::new_shader_code_file_action_triggered() {

  create_new_file("glsl");
}

/**
 * @brief Create a new file, such as a script, or a shader code file.
 * @param file_type The type of file to be created.
 * A directory is created when no file type is present.
 */
void QuestTreeView::create_new_file(const QString& file_type) {

  if (is_read_only()) {
    return;
  }

  QString parent_path = get_selected_path();
  if (parent_path.isEmpty()) {
    return;
  }

  try {
    NewElementDialog dialog(file_type, parentWidget());
    int result = dialog.exec();

    if (result != QDialog::Accepted) {
      return;
    }

    QString file_name = dialog.get_element_id();
    bool is_file = !(file_type.isEmpty());

    // Automatically add file type extension if not present.
    if (is_file && !file_name.contains(".")) {
      file_name = file_name + "." + file_type;
    }
    Quest& quest = model->get_quest();
    Quest::check_valid_file_name(file_name);
    QString path = parent_path + "/" + file_name;

    // Create file based on type.
    // A directory is created when no file type is available.
    if (!is_file) {
      quest.create_dir(parent_path, file_name);
    } else if (file_type == "glsl") {
      quest.create_shader_code_file(path);
    } else {
      quest.create_script(path);
    }

    // Add file info to database.
    const QString& relative_path =
        quest.get_path_relative_to_data_path(path);
    QuestDatabase& database = quest.get_database();
    QuestDatabase::FileInfo file_info = dialog.get_file_info();

    database.set_file_info(relative_path, file_info);
    database.save();

    // Select the element created.
    set_selected_path(path);

    // Open element if it's a file.
    if (is_file) {
      emit open_file_requested(quest, path);
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Slot called when the user double-clicks or presses enter on a file.
 */
void QuestTreeView::default_action_triggered() {

  QString path = get_selected_path();
  if (path.isEmpty()) {
    return;
  }

  const Quest& quest = model->get_quest();
  ResourceType resource_type;
  QString element_id;

  if (quest.is_dir(path)) {
    // Double-clicking a directory already expands it,
    // we don't want to trigger the open action here.
    return;
  }

  if (quest.is_potential_resource_element(path, resource_type, element_id)) {

    if (quest.exists(path)) {
      if (resource_type == ResourceType::SOUND || resource_type == ResourceType::MUSIC) {
        play_action_triggered();
        return;
      }
    }
  }
  open_action_triggered();
}

/**
 * @brief Slot called when the user wants to play the selected file.
 */
void QuestTreeView::play_action_triggered() {

  QString path = get_selected_path();
  if (path.isEmpty()) {
    return;
  }

  Quest& quest = model->get_quest();
  ResourceType resource_type;
  QString element_id;
  if (quest.is_potential_resource_element(path, resource_type, element_id)) {
    if (resource_type == ResourceType::SOUND) {
      Audio::play_sound(quest, element_id);
    }
    else if (resource_type == ResourceType::MUSIC) {
      if (Audio::is_playing_music(quest, element_id)) {
        Audio::stop_music(quest);
      }
      else {
        Audio::play_music(quest, element_id);
      }
    }
  }

}

/**
 * @brief Slot called when the user wants to open the selected file.
 */
void QuestTreeView::open_action_triggered() {

  if (!is_opening_files_allowed()) {
    return;
  }

  QString path = get_selected_path();
  if (path.isEmpty()) {
    return;
  }

  emit open_file_requested(model->get_quest(), path);
}

/**
 * @brief Slot called when the user wants to open the script of a map.
 */
void QuestTreeView::open_map_script_action_triggered() {

  if (!is_opening_files_allowed()) {
    return;
  }

  QString path = get_selected_path();
  if (path.isEmpty()) {
    return;
  }

  Quest& quest = model->get_quest();
  ResourceType resource_type;
  QString element_id;
  if (!quest.is_resource_element(path, resource_type, element_id) ||
      resource_type != ResourceType::MAP) {
    return;
  }

  emit open_file_requested(quest, quest.get_map_script_path(element_id));
}

/**
 * @brief Slot called when the user wants to open the strings file of a
 * language.
 */
void QuestTreeView::open_language_strings_action_triggered() {

  if (!is_opening_files_allowed()) {
    return;
  }

  QString path = get_selected_path();
  if (path.isEmpty()) {
    return;
  }

  Quest& quest = model->get_quest();
  ResourceType resource_type;
  QString element_id;
  if (!quest.is_resource_element(path, resource_type, element_id) ||
      resource_type != ResourceType::LANGUAGE) {
    return;
  }

  emit open_file_requested(quest, quest.get_strings_path(element_id));
}

/**
 * @brief Slot called when the user wants to rename the selected file or
 * directory.
 */
void QuestTreeView::rename_action_triggered() {

  if (is_read_only()) {
    return;
  }

  emit rename_file_requested(model->get_quest(), get_selected_path());
}

/**
 * @brief Slot called when a file or directory of the quest was just renamed.
 * @param old_path File name before the change.
 * @param new_path File name after the change.
 */
void QuestTreeView::file_renamed(const QString& old_path,
                                 const QString& new_path) {

  if (get_selected_path() == old_path) {
    // Select the new file instead of the old one.
    set_selected_path(new_path);
  }
}

/**
 * @brief Slot called when the user wants to change the description of the
 * seelcted resource element.
 *
 * The new description will be prompted to the user.
 */
void QuestTreeView::change_description_action_triggered() {

  if (is_read_only()) {
    return;
  }

  QString path = get_selected_path();
  if (path.isEmpty()) {
    return;
  }

  Quest& quest = model->get_quest();
  QuestDatabase& database = quest.get_database();
  ResourceType resource_type;
  QString element_id;
  if (!quest.is_resource_element(path, resource_type, element_id)) {
    // Only resource elements have a description.
    return;
  }

  QString resource_friendly_type_name_for_id = database.get_friendly_name_for_id(resource_type);
  QString old_description = database.get_description(resource_type, element_id);
  bool ok = false;
  QString new_description = QInputDialog::getText(
        this,
        tr("Change description"),
        tr("New description for %1 '%2':").arg(resource_friendly_type_name_for_id, element_id),
        QLineEdit::Normal,
        old_description,
        &ok);

  if (ok) {
    try {
      database.set_description(resource_type, element_id, new_description);
      database.save();
    }
    catch (const EditorException& ex) {
      ex.show_dialog();
    }
  }
}

/**
 * @brief Slot called when the user wants to change metadata of selected files.
 *
 * The new info will be prompted to the user.
 */
void QuestTreeView::change_file_info_action_triggered() {

  if (is_read_only()) {
    return;
  }

  const QStringList& paths = get_selected_paths();
  if (paths.isEmpty()) {
    return;
  }

  Quest& quest = model->get_quest();
  QuestDatabase& database = quest.get_database();

  ChangeFileInfoDialog dialog(parentWidget());
  if (paths.size() == 1) {
    dialog.set_message(tr("File information for '%1'").arg(paths.first()));
  } else {
    dialog.set_message(tr("File information for %1 selected items").arg(paths.count()));
  }

  for (const QString& path : paths) {
    QString path_from_data = quest.get_path_relative_to_data_path(path);
    QuestDatabase::FileInfo file_info = database.get_file_info(path_from_data);
    if (!file_info.is_empty()) {
      // Suggest the first existing value initially,
      // even in case of multiple selection.
      dialog.set_file_info(file_info);
      break;
    }
  }
  int result = dialog.exec();

  if (result != QDialog::Accepted) {
    return;
  }

  QuestDatabase::FileInfo file_info = dialog.get_file_info();
  try {
    for (const QString& path : paths) {
      if (path == quest.get_data_path()) {
        continue;
      }
      QString path_from_data = quest.get_path_relative_to_data_path(path);
      database.set_file_info(path_from_data, file_info);
    }
    database.save();
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Returns whether the given files can be deleted.
 * @param paths The paths to test.
 * @return @c true if deleting these files is allowed.
 */
bool QuestTreeView::can_delete_paths(const QStringList& paths) {

  if (is_read_only()) {
    return false;
  }

  const Quest& quest = model->get_quest();
  for (const QString& path : std::as_const(paths)) {
    if (path == quest.get_data_path()) {
      // We don't want to delete the data directory.
      return false;
    }

    ResourceType resource_type;
    if (quest.is_resource_path(path, resource_type)) {
      // Don't delete resource directories.
      return false;
    }
  }

  return true;
}

/**
 * @brief Slot called when the user wants to delete the selected files or
 * directories.
 *
 * Confirmation will be asked to the user.
 */
void QuestTreeView::delete_action_triggered() {

  if (is_read_only()) {
    return;
  }

  QStringList paths = get_selected_paths();
  if (!can_delete_paths(paths)) {
    return;
  }

  QString question = paths.size() == 1 ?
        tr("Do you really want to delete '%1'?").arg(paths.first()) :
        tr("Do you really want to delete these %1 items?").arg(paths.size());
  QMessageBox::StandardButton answer = QMessageBox::question(
        this, tr("Delete confirmation"), question,
        QMessageBox::Yes | QMessageBox::No);
  if (answer != QMessageBox::Yes) {
    return;
  }

  try {
    Quest& quest = model->get_quest();
    for (const QString& path : std::as_const(paths)) {
      ResourceType resource_type;
      QString element_id;
      if (quest.is_resource_element(path, resource_type, element_id)) {
        // This is a resource element: remove it from the project database file.
        // This will also delete the file from disk,
        // or the full folder if it was a language.
        quest.delete_resource_element(resource_type, element_id);
      }
      else {
        // This is a regular file or directory.
        if (QFileInfo(path).isDir()) {
          quest.delete_dir_recursive(path);
        } else {
          // Not a directory and not a resource.
          quest.delete_file(path);
        }
      }
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

}
