/*
 * Copyright (C) 2020-2021 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUSEDITOR_NEW_QUEST_DIALOG_H
#define SOLARUSEDITOR_NEW_QUEST_DIALOG_H
#include "new_quest_builder.h"
#include "ui_new_quest_dialog.h"
#include <QDialog>

namespace SolarusEditor {

class NewQuestDialogTitlePage;
class NewQuestDialogDirectoryPage;
class NewQuestDialogContentsPage;

/**
 * @brief A dialog used to create a new quest in the editor.
 */
class NewQuestDialog : public QDialog {
  Q_OBJECT

public:

  explicit NewQuestDialog(
    const QString& directory,
    QWidget* parent = nullptr);

  const NewQuestBuilder::NewQuestConfig& get_new_quest_config() const;

private:

  void update_page_buttons();

  void initialize_from_config(int page_index);
  bool next_button_enabled(int page_index) const;

  void on_quest_title_changed();
  void on_quest_path_changed();
  void on_contents_mode_changed();
  void update_next_button();
  void update_config(int page_index);

  void on_browse_button_clicked();
  bool confirm_non_empty_dir() const;

  Ui::NewQuestDialog ui;
  NewQuestBuilder::NewQuestConfig config;
  const QString start_directory;
};

} // namespace SolarusEditor

#endif
