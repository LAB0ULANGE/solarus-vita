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
#ifndef SOLARUSEDITOR_NEW_ELEMENT_DIALOG_H
#define SOLARUSEDITOR_NEW_ELEMENT_DIALOG_H

#include "quest_database.h"
#include "ui_new_element_dialog.h"
#include <QDialog>

namespace SolarusEditor {

/**
 * @brief Dialog shown when creating a new script or folder in the quest.
 *
 * This dialog does not create any script nor a folder.
 * It only shows input widgets and provides the text entered by the user.
 */
class NewElementDialog : public QDialog {
  Q_OBJECT

public:

  explicit NewElementDialog(QString file_type, QWidget* parent = nullptr);

  QString get_file_type() const;
  QString get_element_id() const;
  void set_element_id(const QString& value);
  QuestDatabase::FileInfo get_file_info() const;

public slots:

  void done(int result) override;

private:

  Ui::NewElementDialog ui;              /**< The widgets. */
  QString file_type;                    /**< File type of new script. */

};

}

#endif
