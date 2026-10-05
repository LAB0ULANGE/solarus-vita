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
#include <QWidget>

namespace SolarusEditor {

/**
 * @brief New quest dialog page that asks for the directory path of the quest.
 */
class NewQuestDialogDirectoryPage : public QWidget {
  Q_OBJECT

public:

  explicit NewQuestDialogDirectoryPage(QWidget* parent = nullptr);

  void init_from_config(const NewQuestBuilder::NewQuestConfig& config);

private:

void update_page();

signals:

  void changed(const QString& directoryPath);

private:

  Ui::NewQuestDialogDirectoryPage ui;

};

} // namespace SolarusEditor