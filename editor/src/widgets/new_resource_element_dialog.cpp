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
#include "widgets/new_resource_element_dialog.h"
#include "editor_settings.h"
#include "quest.h"
#include <QPushButton>

namespace SolarusEditor {

/**
 * @brief Creates a dialog for a new resource element.
 * @param resource_type The type of resource to create.
 * @param parent parent The parent widget or nullptr.
 */
NewResourceElementDialog::NewResourceElementDialog(
    ResourceType resource_type, QWidget* parent) :
  QDialog(parent),
  resource_type(resource_type) {

  ui.setupUi(this);

  // Determine the text to show.
  // A switch is the best way to ensure at compile time that we don't
  // forget resource types, and to also handle translations correctly.
  QString title;
  QString id_text;
  bool supports_default_script = false;
  bool supports_data_file_license = false;
  bool supports_script_file_license = false;
  switch (resource_type) {

  case ResourceType::MAP:
    title = tr("New map");
    id_text = tr("Map id (filename):");
    supports_data_file_license = true;
    supports_script_file_license = true;
    supports_default_script = true;
    break;

  case ResourceType::TILESET:
    title = tr("New tileset");
    id_text = tr("Tileset id (filename):");
    supports_data_file_license = true;
    break;

  case ResourceType::SPRITE:
    title = tr("New sprite");
    id_text = tr("Sprite id (filename):");
    supports_data_file_license = true;
    break;

  case ResourceType::MUSIC:
    title = tr("New music");
    id_text = tr("Music id (filename):");
    supports_data_file_license = true;
    break;

  case ResourceType::SOUND:
    title = tr("New sound");
    id_text = tr("Sound id (filename):");
    supports_data_file_license = true;
    break;

  case ResourceType::ITEM:
    title = tr("New item");
    id_text = tr("Item id (filename):");
    supports_script_file_license = true;
    supports_default_script = true;
    break;

  case ResourceType::ENEMY:
    title = tr("New enemy");
    id_text = tr("Enemy id (filename):");
    supports_script_file_license = true;
    supports_default_script = true;
    break;

  case ResourceType::ENTITY:
    title = tr("New custom entity");
    id_text = tr("Custom entity id (filename):");
    supports_script_file_license = true;
    supports_default_script = true;
    break;

  case ResourceType::LANGUAGE:
    title = tr("New language");
    id_text = tr("Language id (filename):");
    supports_data_file_license = true;
    break;

  case ResourceType::FONT:
    title = tr("New font");
    id_text = tr("Font id (filename):");
    supports_data_file_license = true;
    break;

  case ResourceType::SHADER:
    title = tr("New shader");
    id_text = tr("Shader id (filename):");
    supports_data_file_license = true;
    break;
  }

  EditorSettings settings;
  ui.author_line_edit->setText(settings.get_value_string(EditorSettings::last_author));
  if (supports_data_file_license) {
    ui.data_file_license_line_edit->setText(settings.get_value_string(EditorSettings::last_license_data));
  } else {
    delete ui.data_file_licence_label;
    ui.data_file_licence_label = nullptr;
    delete ui.data_file_license_line_edit;
    ui.data_file_license_line_edit = nullptr;
    if (supports_script_file_license) {
      ui.script_license_label->setText(tr("License"));
    }
  }
  if (supports_script_file_license) {
    ui.script_license_line_edit->setText(settings.get_value_string(EditorSettings::last_license_script));
  } else {
    delete ui.script_license_label;
    ui.script_license_label = nullptr;
    delete ui.script_license_line_edit;
    ui.script_license_line_edit = nullptr;
    if (supports_data_file_license) {
      ui.data_file_licence_label->setText(tr("License"));
    }
  }

  if (supports_default_script) {
    EditorSettings settings;
    ui.create_with_default_code_check_box->setChecked(
        settings.get_value_bool(EditorSettings::create_scripts_with_default_code));
  } else {
    delete ui.create_with_default_code_check_box;
    ui.create_with_default_code_check_box = nullptr;
  }

  ui.id_label->setText(id_text);
  setWindowTitle(title);
  adjustSize();

  connect(ui.id_line_edit, &QLineEdit::textChanged,
          this, &NewResourceElementDialog::element_id_changed);
  element_id_changed();
}

/**
 * @brief Returns the type of resource to be created.
 * @return The type of resource.
 */
ResourceType NewResourceElementDialog::get_resource_type() const {
  return resource_type;
}

/**
 * @brief Returns resource element id entered by the user.
 * @return The resource element id.
 */
QString NewResourceElementDialog::get_element_id() const {

  return ui.id_line_edit->text();
}

/**
 * @brief Sets the value displayed in the resource element id text edit.
 * @param value The value to set.
 */
void NewResourceElementDialog::set_element_id(const QString& value) {

  ui.id_line_edit->setText(value);
}

/**
 * @brief Called when the element id entered by the user changes.
 */
void NewResourceElementDialog::element_id_changed() {

  const bool validId = Quest::is_valid_file_name(ui.id_line_edit->text());
  ui.button_box->button(QDialogButtonBox::Ok)->setEnabled(validId);
}

/**
 * @brief Returns resource element description entered by the user.
 * @return The description.
 */
QString NewResourceElementDialog::get_element_description() const {

  return ui.description_line_edit->text();
}

/**
 * @brief Sets the value id displayed in the description text edit.
 * @param value The value to set.
 */
void NewResourceElementDialog::set_element_description(const QString& value) {

  ui.description_line_edit->setText(value);

  // Select and focus the description if the id looks correct.
  if (!ui.id_line_edit->text().isEmpty() &&
      !ui.id_line_edit->text().endsWith('/')) {
    ui.description_line_edit->selectAll();
    ui.description_line_edit->setFocus();
  }
}

/**
 * @brief Returns metadata entered by the user for data files.
 * @return Data files metadata.
 */
QuestDatabase::FileInfo NewResourceElementDialog::get_data_file_info() const {

  return QuestDatabase::FileInfo{
      ui.author_line_edit->text(),
      ui.data_file_license_line_edit != nullptr ? ui.data_file_license_line_edit->text() : QString()
  };
}

/**
 * @brief Returns metadata entered by the user for script files.
 * @return Script files metadata.
 */
QuestDatabase::FileInfo NewResourceElementDialog::get_script_file_info() const {

  return QuestDatabase::FileInfo{
      ui.author_line_edit->text(),
      ui.script_license_line_edit != nullptr ? ui.script_license_line_edit->text() : QString()
  };
}

/**
 * @brief Closes the dialog unless the user tries to set invalid data.
 * @param result Result code of the dialog.
 */
void NewResourceElementDialog::done(int result) {

  if (result == QDialog::Accepted) {

    EditorSettings settings;
    if (ui.create_with_default_code_check_box != nullptr) {
      settings.set_value(EditorSettings::create_scripts_with_default_code,
                         ui.create_with_default_code_check_box->isChecked());
    }
    settings.set_value(EditorSettings::last_author,
                       ui.author_line_edit->text());
    if (ui.data_file_license_line_edit != nullptr) {
      settings.set_value(EditorSettings::last_license_data,
                         ui.data_file_license_line_edit->text());
    }
    if (ui.script_license_line_edit != nullptr) {
      settings.set_value(EditorSettings::last_license_script,
                         ui.script_license_line_edit->text());
    }

    if (get_element_id().isEmpty()) {
      GuiTools::error_dialog("Empty resource id");
      return;
    }

    if (!Quest::is_valid_file_name(get_element_id())) {
      GuiTools::error_dialog("Invalid resource id");
      return;
    }
  }

  QDialog::done(result);
}

}
