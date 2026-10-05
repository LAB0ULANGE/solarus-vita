/*
 * Copyright (C) 2014-2022 Christopho, Solarus - http://www.solarus-games.org
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
#include "widgets/new_element_dialog.h"
#include "editor_settings.h"
#include "quest.h"

namespace SolarusEditor {

/**
 * @brief Creates a dialog for a new file or directory.
 * @param file_type Extension of the file to be created.
 * An empty string means a directory.
 * @param parent parent The parent widget or nullptr.
 */
NewElementDialog::NewElementDialog(
    QString file_type, QWidget* parent) :
  QDialog(parent),
  file_type(file_type) {

  ui.setupUi(this);

  // Set the type of resource to show.
  QString title;
  QString id_text;
  EditorSettings settings;

  // Fill in retained author/license.
  bool is_file = !get_file_type().isEmpty();

  ui.author_line_edit->setText(settings.get_value_string(EditorSettings::last_author));
  if (is_file) {
    ui.license_line_edit->setText(settings.get_value_string(EditorSettings::last_license_script));
  }

  // Check if new element created is a script by file type.
  // If not, it's a directory.
  if (get_file_type() == "glsl") {
    title = tr("New GLSL file");
    id_text = tr("GLSL file name:");
  } else if (get_file_type() == "lua") {
    title = tr("New Lua script");
    id_text = tr("Script name:");
  } else {
    title = tr("New folder");
    id_text = tr("Folder name:");
  }

  ui.id_label->setText(id_text);
  setWindowTitle(title);
}

/**
 * @brief The script or file type to be created.
 * @return The type of file.
 */
QString NewElementDialog::get_file_type() const {
  return file_type;
}

/**
 * @brief Returns element id given by the user.
 * @return The element id.
 */
QString NewElementDialog::get_element_id() const {

  return ui.id_line_edit->text();
}

/**
 * @brief Sets the value displayed in the text id_line_edit.
 * @param value The value to set.
 */
void NewElementDialog::set_element_id(const QString& value) {

  ui.id_line_edit->setText(value);
}

/**
 * @brief Returns author and license information of the file given by the user.
 * @return A FileInfo copy.
 */
QuestDatabase::FileInfo NewElementDialog::get_file_info() const {

  return QuestDatabase::FileInfo{
    ui.author_line_edit->text(),
    ui.license_line_edit->text(),
  };
}

/**
 * @brief Closes the dialog unless the user tries to put in invalid data.
 * @param result Dialog result code.
 */
void NewElementDialog::done(int result) {

  if (result == QDialog::Accepted) {

    EditorSettings settings;
    bool is_file = !get_file_type().isEmpty();

    // Store previous author/license when dialog finished.
    settings.set_value(EditorSettings::last_author, ui.author_line_edit->text());
    if (is_file) {
      settings.set_value(EditorSettings::last_license_script, ui.license_line_edit->text());
    }

    if (get_element_id().isEmpty()) {
      if (get_file_type() == "glsl") {
        GuiTools::error_dialog("Empty GLSL file name");
      } else if (get_file_type() == "lua") {
        GuiTools::error_dialog("Empty script name");
      } else {
        GuiTools::error_dialog("Empty folder name");
      }
      return;
    }

    if (!Quest::is_valid_file_name(get_element_id())) {
      if (is_file) {
        GuiTools::error_dialog("Invalid file name");
      } else {
        GuiTools::error_dialog("Invalid folder name");
      }
      return;
    }
  }

  QDialog::done(result);
}

}
