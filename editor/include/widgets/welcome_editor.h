/*
 * Copyright (C) 2014-2025 Christopho, Solarus - http://www.solarus-games.org
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
#ifndef SOLARUSEDITOR_HOMEPAGE_WIDGET_H
#define SOLARUSEDITOR_HOMEPAGE_WIDGET_H

#include "widgets/editor.h"

class QPushButton;
class QLabel;

namespace oclero {
namespace qlementine {
class CommandLinkButton;
class Label;
} // namespace qlementine
} // namespace oclero

namespace SolarusEditor {
/**
 * \brief A widget displayed in the main window when there is no quest opened.
 */
class WelcomeEditor : public Editor {
  Q_OBJECT

public:
  WelcomeEditor(Quest &quest, const QString &file_path,
                QWidget *parent = nullptr);

private:
  void setup_ui();

  struct {
    QLabel *app_icon_label{nullptr};
    oclero::qlementine::Label *title_label{nullptr};
    oclero::qlementine::Label *version_label{nullptr};
    oclero::qlementine::CommandLinkButton *new_quest_button{nullptr};
    oclero::qlementine::CommandLinkButton *open_quest_button{nullptr};
    QPushButton *documentation_button{nullptr};
    QPushButton *website_button{nullptr};
  } ui;
};

} // namespace SolarusEditor

#endif
