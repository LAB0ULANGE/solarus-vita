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
#include "widgets/input_dialog_with_check_box.h"
#include "editor_settings.h"
#include "ui_input_dialog_with_check_box.h"

namespace SolarusEditor {

/**
 * @brief Creates a input dialog with a check box.
 * @param title Title of the dialog.
 * @param message Text to show above the field.
 * @param check_box_text Text of the check box.
 * @param initial_value Initial value of the field.
 * @param parent Parent object or nullptr.
 */
InputDialogWithCheckBox::InputDialogWithCheckBox(
    const QString& title,
    const QString& message,
    const QString& check_box_text,
    const QString& initial_value,
    QWidget* parent) :
  QDialog(parent),
  ui(new Ui::InputDialogWithCheckBox()),
  check_box_setting() {

  ui->setupUi(this);

  setWindowTitle(title);
  ui->value_label->setText(message);
  ui->check_box->setText(check_box_text);
  set_value(initial_value);
  ui->value_field->selectAll();
}

/**
 * @brief Destructor.
 */
InputDialogWithCheckBox::~InputDialogWithCheckBox() {
}

/**
 * @brief Returns the value entered by the user.
 * @return The value.
 */
QString InputDialogWithCheckBox::get_value() const {

  return ui->value_field->text();
}

/**
 * @brief Sets the value displayed in the text edit.
 * @param value The value to set.
 */
void InputDialogWithCheckBox::set_value(const QString& value) {

  ui->value_field->setText(value);
}

/**
 * @brief Returns whether the checkbox is checked.
 * @return @c true if the checkbox is checked.
 */
bool InputDialogWithCheckBox::is_checked() const {

  return ui->check_box->isChecked();
}

/**
 * @brief Sets whether the checkbox is checked.
 * @return @c true if the check the option.
 */
void InputDialogWithCheckBox::set_checked(bool checked) {

  ui->check_box->setChecked(checked);
}

/**
 * @brief Returns the setting name that saves the check box state, if any.
 * @return The setting name or an empty string.
 */
QString InputDialogWithCheckBox::get_check_box_setting() const {
  return check_box_setting;
}

/**
 * @brief Sets the setting name that saves the check box state.
 *
 * Modifies the check box state to match the setting if it exists.
 *
 * @param check_box_setting The setting name or an empty string.
 */
void InputDialogWithCheckBox::set_check_box_setting(const QString& check_box_setting) {

  this->check_box_setting = check_box_setting;
  if (!check_box_setting.isEmpty()) {
    EditorSettings settings;
    set_checked(settings.get_value_bool(check_box_setting));
  }
}

/**
 * @brief Closes the dialog unless the user tries to set invalid data.
 * @param result Result code of the dialog.
 */
void InputDialogWithCheckBox::done(int result) {

  if (result == QDialog::Accepted) {

    if (get_value().isEmpty()) {
      GuiTools::error_dialog("Empty pattern id");
      return;
    }

    if (!check_box_setting.isEmpty()) {
      EditorSettings settings;
      settings.set_value(check_box_setting, is_checked());
    }
  }

  QDialog::done(result);
}

}
