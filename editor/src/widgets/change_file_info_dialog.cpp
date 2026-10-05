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
#include "quest.h"
#include "ui_change_file_info_dialog.h"

namespace SolarusEditor {

/**
 * @brief Creates a file info dialog.
 * @param resource_type The type of resource to create.
 */
ChangeFileInfoDialog::ChangeFileInfoDialog(QWidget* parent) :
  QDialog(parent),
  ui(new Ui::ChangeFileInfoDialog()) {

  ui->setupUi(this);
}

/**
 * @brief Destructor.
 */
ChangeFileInfoDialog::~ChangeFileInfoDialog() {
}

/**
 * @brief Returns the message displayed in the GUI.
 * @return The message.
 */
QString ChangeFileInfoDialog::get_message() const {
  return ui->message_label->text();
}

/**
 * @brief Sets the message displayed in the GUI.
 * @param message The message.
 */
void ChangeFileInfoDialog::set_message(const QString& message) {
  return ui->message_label->setText(message);
}

/**
 * @brief Returns the file info entered by the user.
 */
QuestDatabase::FileInfo ChangeFileInfoDialog::get_file_info() const {

  QuestDatabase::FileInfo file_info;
  file_info.author = ui->author_field->text();
  file_info.license = ui->license_field->text();
  return file_info;
}

/**
 * @brief Sets the file info to display in the GUI.
 */
void ChangeFileInfoDialog::set_file_info(const QuestDatabase::FileInfo& file_info) {

  ui->author_field->setText(file_info.author);
  ui->license_field->setText(file_info.license);
  ui->author_field->selectAll();
}

}
