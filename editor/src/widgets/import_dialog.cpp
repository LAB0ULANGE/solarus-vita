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
#include "widgets/gui_tools.h"
#include "widgets/import_dialog.h"
#include "audio.h"
#include "editor_exception.h"
#include "editor_settings.h"
#include "editor_style.h"
#include "file_tools.h"
#include <QCheckBox>
#include <QFileDialog>
#include <QTimer>
#include <set>

namespace SolarusEditor {

/**
 * @brief Creates an import dialog.
 * @param destination_quest The destination quest to import files to.
 * It must exist.
 * @param parent Parent object or nullptr.
 */
ImportDialog::ImportDialog(Quest& destination_quest, QWidget* parent) :
  QDialog(parent),
  ui(),
  source_quest(),
  destination_quest(destination_quest),
  memorized_overwrite_file_choice(QMessageBox::Cancel),
  memorized_overwrite_dir_choice(QMessageBox::Cancel) {

  ui.setupUi(this);

  Q_ASSERT(destination_quest.exists());

  EditorStyle::setAutoIconColor(ui.source_quest_browse_button, EditorStyle::AutoIconColor::ForegroundColor);
  EditorStyle::setAutoIconColor(ui.find_missing_button, EditorStyle::AutoIconColor::ForegroundColor);

  ui.source_quest_tree_view->set_read_only(true);
  ui.source_quest_tree_view->set_opening_files_allowed(false);

  ui.destination_quest_field->setText(destination_quest.get_root_path());
  ui.destination_quest_tree_view->set_quest(destination_quest);
  ui.destination_quest_tree_view->set_opening_files_allowed(false);

  ui.missing_files_count_label->clear();
  QPushButton* importButton = ui.button_box->button(QDialogButtonBox::Apply);
  importButton->setText(tr("Import files"));
  importButton->setIcon(QIcon(":/images/icon_next"));
  EditorStyle::setAutoIconColor(importButton, EditorStyle::AutoIconColor::ForegroundColor);

  connect(ui.source_quest_browse_button, &QToolButton::clicked,
          this, &ImportDialog::browse_source_quest);
  connect(&source_quest, &Quest::root_path_changed,
          this, &ImportDialog::source_quest_root_path_changed);
  connect(ui.source_quest_tree_view, &QuestTreeView::selected_path_changed,
          this, &ImportDialog::source_quest_selected_path_changed);
  connect(ui.destination_quest_tree_view, &QuestTreeView::rename_file_requested,
          this, &ImportDialog::destination_quest_rename_file_requested);
  connect(ui.find_missing_button, &QPushButton::clicked,
          this, &ImportDialog::find_missing_button_triggered);
  connect(ui.button_box->button(QDialogButtonBox::Apply), &QPushButton::clicked,
          this, &ImportDialog::import_button_triggered);

  EditorSettings settings;
  QString last_source_quest_path = settings.get_value_string(EditorSettings::import_last_source_quest);
  if (last_source_quest_path != destination_quest.get_root_path()) {
    source_quest.set_root_path(last_source_quest_path);
  }
  if (!source_quest.exists()) {
    browse_source_quest();
  }
}

/**
 * @brief Returns the source quest where to import files from.
 * @return The source quest or an invalid quest if it was not set.
 */
const Quest& ImportDialog::get_source_quest() const {
  return source_quest;
}

/**
 * @brief Returns the destination quest where to import files to.
 * @return The destination quest.
 */
Quest& ImportDialog::get_destination_quest() const {
  return destination_quest;
}

/**
 * @brief Stop audio and then close
 */
void ImportDialog::done(int result) {

  if (Audio::is_playing_music(source_quest)) {
    Audio::stop_music(source_quest);
  }
  QDialog::done(result);
}

/**
 * @brief Lets the user choose the source quest where to import from.
 */
void ImportDialog::browse_source_quest() {

  // Ask the quest path where to import from.
  EditorSettings settings;
  QString initial_value = settings.get_value_string(EditorSettings::import_last_source_quest);
  QString src_quest_path = QFileDialog::getExistingDirectory(
        this,
        tr("Select a quest where to import from"),
        initial_value,
#ifdef SOLARUSEDITOR_NO_NATIVE_DIALOGS
        QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog
#else
        QFileDialog::ShowDirsOnly
#endif
  );

  if (src_quest_path.isEmpty()) {
    // Canceled.
    return;
  }

  if (src_quest_path == destination_quest.get_root_path()) {
    // Same quest.
    GuiTools::warning_dialog(tr("Source and destination quest are the same"));
    return;
  }

  source_quest.set_root_path(src_quest_path);
  if (!source_quest.exists()) {
    GuiTools::error_dialog(
          tr("No source quest was not found in folder '%1'").arg(src_quest_path));
    source_quest.set_root_path("");
  }
}

/**
 * @brief Slot called when the root path of the source quest has changed.
 */
void ImportDialog::source_quest_root_path_changed() {

  ui.source_quest_browse_field->setText(source_quest.get_root_path());
  ui.source_quest_tree_view->set_quest(source_quest);

  update_import_button();
  if (source_quest.exists()) {
    EditorSettings settings;
    settings.set_value(EditorSettings::import_last_source_quest, source_quest.get_root_path());
  }
}

/**
 * @brief Slot called when the selection has changed in the source quest.
 */
void ImportDialog::source_quest_selected_path_changed() {

  update_import_button();
  update_find_missing_button();
  ui.missing_files_count_label->clear();
}

/**
 * @brief Updates the enabled state of the "Find missing" button.
 */
void ImportDialog::update_find_missing_button() {

  ui.find_missing_button->setEnabled(false);
  if (!source_quest.exists()) {
    return;
  }

  QString selected_path = ui.source_quest_tree_view->get_selected_path();
  if (selected_path.isEmpty() ||
      QFileInfo(selected_path).isDir()
  ) {
    ui.find_missing_button->setEnabled(true);
  }
}

/**
 * @brief Slot called when the user clicks the "Find missing" button.
 */
void ImportDialog::find_missing_button_triggered() {

  if (!source_quest.exists()) {
    return;
  }

  QString selected_source_path = ui.source_quest_tree_view->get_selected_path();
  QString initial_source_path = !selected_source_path.isEmpty() ?
        selected_source_path : source_quest.get_data_path();
  QStringList missing_source_paths;
  ui.source_quest_tree_view->expand_to_path(initial_source_path);
  find_source_paths_not_in_destination_quest(initial_source_path, missing_source_paths);

  ui.missing_files_count_label->setText(
        missing_source_paths.isEmpty() ?
          tr("No candidates found") :
          tr("%1 candidates found").arg(ui.source_quest_tree_view->get_selected_paths().size())
  );

  ui.source_quest_tree_view->set_selected_paths(missing_source_paths);
  // Workaround for the buggy selection of rows that were just created when expanding the tree.
  QTimer::singleShot(100, this, [this, missing_source_paths]() {
    ui.source_quest_tree_view->set_selected_paths(missing_source_paths);
  });
}

/**
 * @brief Checks if a path and its children exist in the destination quest.
 *
 * Only the existence of files is checked, not their content.
 *
 * @param source_path The source path to check.
 * If it is a directory, it will be recursively explored.
 * @param[out] missing_source_paths List that will be appended with source
 * paths that have no equivalent in the destination quest.
 */
void ImportDialog::find_source_paths_not_in_destination_quest(
    const QString& source_path,
    QStringList& missing_source_paths
) {

  QFileInfo source_info(source_path);
  if (!source_info.exists()) {
    return;
  }
  if (source_info.isSymLink()) {
    return;
  }

  QString destination_path = source_to_destination_path(source_path);
  if (!QFileInfo::exists(destination_path)) {
    // Found a missing one.
    missing_source_paths << source_path;
  }

  if (source_info.isDir()) {
    // Recursively explore the directory.
    const QStringList& source_children_file_names = QDir(source_path).entryList();
    for (const QString& source_child_file_name : source_children_file_names) {
      if (source_child_file_name == "." ||
          source_child_file_name == ".." ) {
        continue;
      }
      QString source_child_path = QString("%1/%2").arg(source_path, source_child_file_name);
      find_source_paths_not_in_destination_quest(source_child_path, missing_source_paths);
    }
  }
}

/**
 * @brief Updates the enabled state of the import button.
 */
void ImportDialog::update_import_button() {

  QPushButton* import_button = ui.button_box->button(QDialogButtonBox::Apply);
  const int count = static_cast<int>(ui.source_quest_tree_view->selectionModel()->selectedRows().count());

  import_button->setEnabled(count > 0);
  import_button->setText(tr("Import 1 item"));

  if (count > 1) {
    import_button->setText(tr("Import %1 items").arg(count));
  }
}

/**
 * @brief Slot called when the user clicks the "Import" button.
 */
void ImportDialog::import_button_triggered() {

  memorized_overwrite_file_choice = QMessageBox::Cancel;
  memorized_overwrite_dir_choice = QMessageBox::Cancel;
  paths_to_select.clear();
  ui.missing_files_count_label->clear();

  try {
    const QStringList source_paths = ui.source_quest_tree_view->get_selected_paths();
    std::set<QString> source_path_set(source_paths.begin(), source_paths.end());
    if (source_path_set.empty()) {
      return;
    }

    // When a folder is selected, remove its children from the list
    // because they are already imported recursively.
    std::set<QString> unique_source_path_set = source_path_set;
    for (const QString& source_path : source_path_set) {
      QString parent = QFileInfo(source_path).dir().path();
      if (source_path_set.find(parent) != source_path_set.end()) {
        unique_source_path_set.erase(source_path);
      }
    }

    // Show a warning if a lot of files are about to be imported.
    if (source_path_set.size() > 5 && QMessageBox::warning(
          nullptr,
          tr("Import confirmation"),
          tr("%1 items will be imported to your quest.").arg(source_path_set.size()),
          QMessageBox::Ok | QMessageBox::Cancel,
          QMessageBox::Ok
          ) == QMessageBox::Cancel) {
      return;
    }

    bool multiple = unique_source_path_set.size() > 1;
    for (const QString& source_path : unique_source_path_set) {
      if (!import_path(source_path, multiple)) {
        // Cancelled.
        break;
      }
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }

  destination_quest.get_database().save();
  ui.destination_quest_tree_view->setFocus();
  QTimer::singleShot(200, this, SLOT(select_recently_created_paths()));
}

/**
 * @brief Imports the given file or directory into the destination quest.
 * @param source_path Path or the file or directory to import.
 * @param multiple Whether this is part of a multiple import operation.
 * @return @c false if the user cancelled the operation.
 */
bool ImportDialog::import_path(const QString& source_path, bool multiple) {

  QFileInfo source_info(source_path);
  if (source_info.isSymLink()) {
    throw EditorException(tr("Cannot import symbolic link '%1'").arg(source_path));
  }

  if (source_info.isDir()) {
    return import_dir(source_info, multiple);
  }
  return import_file(source_info, multiple);
}

/**
 * @brief Imports the given file into the destination quest.
 * @param source_file File to import.
 * @param multiple Whether this is part of a multiple import operation.
 * @return @c false if the user cancelled the operation.
 */
bool ImportDialog::import_file(const QFileInfo& source_info, bool multiple) {

  const QString& source_path = source_info.filePath();
  if (!source_info.exists()) {
    throw EditorException(QApplication::tr("Source file does not exist: '%1'").arg(source_path));
  }

  if (source_info.isDir()) {
    throw EditorException(QApplication::tr("Source path is a folder: '%1'").arg(source_path));
  }

  if (source_info.isSymLink()) {
    throw EditorException(QApplication::tr("Source file is a symbolic link: '%1'").arg(source_path));
  }

  if (!source_info.isReadable()) {
    throw EditorException(QApplication::tr("Source file cannot be read: '%1'").arg(source_path));
  }

  QString destination_path = source_to_destination_path(source_path);
  QFileInfo destination_info(destination_path);
  if (destination_info.exists()) {
    if (destination_info.isDir()) {
      throw EditorException(tr("Destination path already exists and is a folder: '%1'").arg(destination_path));
    }

    int overwrite_choice = QMessageBox::Cancel;
    if (memorized_overwrite_file_choice != QMessageBox::Cancel) {
      overwrite_choice = memorized_overwrite_file_choice;
    } else {
      bool remember = false;
      overwrite_choice = prompt_overwrite_confirmation(
            tr("Destination already exists"),
            tr("The destination file '%1' already exists.\nDo you want to overwrite it?").arg(destination_path),
            multiple ? tr("Apply this choice for remaining files") : QString(),
            true,
            remember);
      if (remember) {
        memorized_overwrite_file_choice = overwrite_choice;
      }
    }

    switch (overwrite_choice) {

    case QMessageBox::Cancel:
      // Cancel.
      return false;

    case QMessageBox::Ignore:
      // Skip.
      return true;

    case QMessageBox::No:
      // Rename.
      destination_path = destination_quest.get_available_path(destination_path);
      destination_info = QFileInfo(destination_path);
      break;

    case QMessageBox::Yes:
      // Overwrite.
      if (!QFile::remove(destination_path)) {
        throw EditorException(tr("Failed to remove existing file '%1'").arg(destination_path));
      }

    default:
      break;
    }
  }

  // Copy author and license info.
  import_path_meta_information(source_path, destination_path);

  // Ensure that the destination directory exists.
  QString destination_parent_path = destination_info.path();  // Strip the file name part.
  FileTools::create_directories(destination_parent_path);

  if (!QFile::copy(source_path, destination_path)) {
    throw EditorException(tr("Failed to copy file '%1' to '%2'").arg(source_path, destination_path));
  }

  // Handle declared resources.
  import_resource_element(source_path, destination_path);

  paths_to_select << destination_path;
  return true;
}

/**
 * @brief Imports the given directory into the destination quest.
 * @param source_dir The directory to import.
 * @param multiple Whether this is part of a multiple import operation.
 * @return @c false if the user cancelled the operation.
 */
bool ImportDialog::import_dir(const QFileInfo& source_info, bool multiple) {

  const QString& source_path = source_info.filePath();
  if (!source_info.exists()) {
    throw EditorException(QApplication::tr("Source folder does not exist: '%1'").arg(source_path));
  }

  if (!source_info.isDir()) {
    throw EditorException(QApplication::tr("Source path is not a folder: '%1'").arg(source_path));
  }

  if (!source_info.isReadable()) {
    throw EditorException(QApplication::tr("Source folder cannot be read: '%1'").arg(source_path));
  }

  QString destination_path = source_to_destination_path(source_path);
  QFileInfo destination_info(destination_path);
  if (destination_info.exists()) {
    if (!destination_info.isDir()) {
      throw EditorException(tr("Destination path already exists and is not a directory: '%1'").arg(destination_path));
    }

    ui.destination_quest_tree_view->expand_to_path(destination_path);

    int overwrite_choice = QMessageBox::Cancel;
    if (memorized_overwrite_dir_choice != QMessageBox::Cancel) {
      overwrite_choice = memorized_overwrite_dir_choice;
    } else {
      bool remember = false;
      overwrite_choice = prompt_overwrite_confirmation(
            tr("Destination already exists"),
            tr("The destination directory '%1' already exists.\nDo you want to overwrite its content?").arg(destination_path),
            multiple ? tr("Apply this choice for remaining directories") : QString(),
            false,  // No renaming for directories.
            remember);
      if (remember) {
        memorized_overwrite_dir_choice = overwrite_choice;
      }
    }

    switch (overwrite_choice) {

    case QMessageBox::Cancel:
      // Cancel.
      return false;

    case QMessageBox::Ignore:
      // Skip.
      return true;

    default:
      break;
    }
  }

  // Copy author and license info.
  import_path_meta_information(source_path, destination_path);

  // Create the directory itself.
  FileTools::create_directories(destination_path);

  // Check if directory is a declared resource.
  import_resource_element(source_path, destination_path);

  // Copy children.
  const QStringList& source_children_file_names = QDir(source_path).entryList(
        QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot | QDir::Hidden | QDir::System);
  for (const QString& source_child_file_name : source_children_file_names) {
    QString source_child_path = QString("%1/%2").arg(source_path, source_child_file_name);
    if (!import_path(source_child_path, true)) {
      // Cancelled.
      return false;
    }
  }

  paths_to_select << destination_path;
  return true;
}

/**
 * @brief Copies the author and license information from a file to another one.
 * @param source_path File or directory in the source quest.
 * @param destination_path File or directory in the destination quest.
 */
void ImportDialog::import_path_meta_information(
    const QString& source_path, const QString& destination_path) {

  QString source_relative_path = source_quest.get_path_relative_to_data_path(source_path);
  const QuestDatabase& source_database = source_quest.get_database();
  QString destination_relative_path = destination_quest.get_path_relative_to_data_path(destination_path);
  QuestDatabase& destination_database = destination_quest.get_database();
  QuestDatabase::FileInfo source_info = source_database.get_file_info(source_relative_path);
  while (source_info.is_empty()) {
    // Try parent directories.
    if (!source_relative_path.contains('/')) {
      break;
    }
    source_relative_path = source_relative_path.section('/', 0, -2);
    source_info = source_database.get_file_info(source_relative_path);
  }
  destination_database.set_file_info(destination_relative_path, source_info);
}

/**
 * @brief Handles declared resource imported from a source file or directory.
 * @param source_path File or directory in the source quest.
 * @param destination_path File or directory in the destination quest.
 */
void ImportDialog::import_resource_element(
    const QString& source_path, const QString& destination_path) {

  ResourceType resource_type;
  QString element_id;
  if (source_quest.is_resource_element(source_path, resource_type, element_id)) {
    const QString& description = source_quest.get_database().get_description(resource_type, element_id);
    ResourceType destination_resource_type;
    QString destination_element_id;
    if (destination_quest.is_potential_resource_element(destination_path, destination_resource_type, destination_element_id) &&
        destination_resource_type == resource_type) {
      QuestDatabase& destination_database = destination_quest.get_database();
      destination_database.add(destination_resource_type, destination_element_id, description);
    }
  }
}

/**
 * @brief Asks the user what to do when a destination path already exists.
 * @param title Title of the message box to create.
 * @param message The question to show.
 * @param remember_choice_text Text for the "Remember my choice" checkbox.
 * An empty string means no checkbox.
 * @param allow_rename Whether to include a "Rename" button.
 * @param[out] remember_choice
 * @return A button: Cancel, Ignore (skip), No (rename) or Yes (overwrite).
 */
int ImportDialog::prompt_overwrite_confirmation(
    const QString& title,
    const QString& message,
    const QString& remember_choice_text,
    bool allow_rename,
    bool& remember_choice) {

  QMessageBox dialog;
  dialog.setIcon(QMessageBox::Warning);
  dialog.setWindowTitle(title);
  dialog.setText(message);
  dialog.setStandardButtons(QMessageBox::Yes | QMessageBox::Ignore | QMessageBox::Cancel);
  if (allow_rename) {
    dialog.setStandardButtons(dialog.standardButtons() | QMessageBox::No);
    dialog.button(QMessageBox::No)->setText(tr("Rename"));
    dialog.button(QMessageBox::No)->setIcon(QIcon());
  }
  dialog.button(QMessageBox::Yes)->setText(tr("Overwrite"));
  dialog.button(QMessageBox::Yes)->setIcon(QIcon());
  dialog.button(QMessageBox::Ignore)->setText(tr("Skip"));
  QCheckBox* rememberCheckBox = nullptr;
  if (!remember_choice_text.isEmpty()) {
    rememberCheckBox = new QCheckBox(remember_choice_text, nullptr);
    dialog.setCheckBox(rememberCheckBox);
  }
  int answer = dialog.exec();
  if (rememberCheckBox != nullptr) {
    remember_choice = rememberCheckBox->isChecked();
  }
  return answer;
}

/**
 * @brief Returns an equivalent path in the destination quest
 * from a source path.
 * @param source_path Path of a file or directory in the source quest.
 * @return The same path with the source quest replaced by the destination one.
 */
QString ImportDialog::source_to_destination_path(const QString& source_path) {

  QString relative_path = source_path.right(source_path.size() - source_quest.get_root_path().size());
  return destination_quest.get_root_path() + relative_path;
}

/**
 * @brief Selects paths that were just imported in the destination quest.
 */
void ImportDialog::select_recently_created_paths() {

  ui.destination_quest_tree_view->set_selected_paths(paths_to_select);
  for (const QString& path : std::as_const(paths_to_select)) {
    ui.destination_quest_tree_view->expand_to_path(path);
  }
}

}
