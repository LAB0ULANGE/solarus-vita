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
#ifndef SOLARUSEDITOR_CHANGE_FILE_INFO_DIALOG_H
#define SOLARUSEDITOR_CHANGE_FILE_INFO_DIALOG_H

#include "quest_database.h"
#include <QDialog>

namespace SolarusEditor {

namespace Ui {
class ChangeFileInfoDialog;
}

/**
 * @brief Dialog shown when editing file metadata in the quest.
 *
 * This dialog does not write anything.
 * It only shows input widgets and provides the text entered by the user.
 */
class ChangeFileInfoDialog : public QDialog {
  Q_OBJECT

public:

  explicit ChangeFileInfoDialog(QWidget* parent = nullptr);
  ~ChangeFileInfoDialog();

  QString get_message() const;
  void set_message(const QString& message);
  QuestDatabase::FileInfo get_file_info() const;
  void set_file_info(const QuestDatabase::FileInfo& file_info);

private:

  QScopedPointer<Ui::ChangeFileInfoDialog> ui;     /**< The widgets. */
};

}

#endif
