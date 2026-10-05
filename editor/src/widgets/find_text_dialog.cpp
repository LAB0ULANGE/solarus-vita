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
#include "widgets/find_text_dialog.h"
#include "editor_settings.h"
#include <QPushButton>
#include <QTextDocument>

namespace SolarusEditor {

/**
 * @brief Creates a find and replace text dialog.
 * @param parent The parent object or nullptr.
 */
FindTextDialog::FindTextDialog(QWidget* parent) :
  QDialog(parent),
  ui() {

  ui.setupUi(this);

  EditorSettings settings;

  ui.find_field->setText(settings.get_value_string(EditorSettings::last_text_searched));
  ui.find_field->selectAll();

  QPushButton* find_previous_button = new QPushButton(tr("Find Previous"), this);
  ui.button_box->addButton(find_previous_button, QDialogButtonBox::ApplyRole);

  QPushButton* find_next_button = new QPushButton(tr("Find Next"), this);
  ui.button_box->addButton(find_next_button, QDialogButtonBox::ApplyRole);

  find_next_button->setDefault(true);

  QPushButton* replace_button = new QPushButton(tr("Replace"), this);
  ui.button_box->addButton(replace_button, QDialogButtonBox::ApplyRole);

  connect(find_previous_button, &QPushButton::pressed, this, [this]() {
    emit find_text_requested(ui.find_field->text(), QTextDocument::FindBackward);
  });

  connect(find_next_button, &QPushButton::pressed, this, [this]() {
    emit find_text_requested(ui.find_field->text());
  });

  connect(replace_button, &QPushButton::pressed, this, [this]() {
    emit replace_text_requested(ui.find_field->text(), ui.replace_field->text());
  });
}

}
