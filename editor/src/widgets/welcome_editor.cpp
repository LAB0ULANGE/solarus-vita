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
#include "widgets/welcome_editor.h"
#include "editor_style.h"

#include <QApplication>
#include <QLabel>
#include <QList>
#include <QPixmap>
#include <QPushButton>
#include <QStyle>
#include <QVBoxLayout>

#include <oclero/qlementine/widgets/CommandLinkButton.hpp>
#include <oclero/qlementine/widgets/Label.hpp>

namespace SolarusEditor {

WelcomeEditor::WelcomeEditor(Quest &quest, const QString &file_path,
                             QWidget *parent)
    : Editor(quest, file_path, parent) {
  set_title(tr("Welcome"));
  set_icon(QIcon(":/images/icon_solarus.svg"));
  setup_ui();
  setFocusPolicy(Qt::NoFocus);
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

/**
 * @brief Sets up the welcome editor user interface.
 */
void WelcomeEditor::setup_ui() {
  int h_spacing = 8;
  int v_spacing = 8;
  if (const QStyle *style = this->style()) {
    h_spacing = style->pixelMetric(QStyle::PM_LayoutHorizontalSpacing);
    v_spacing = style->pixelMetric(QStyle::PM_LayoutVerticalSpacing);
  }

  QVBoxLayout *main_layout = new QVBoxLayout(this);
  main_layout->setAlignment(Qt::AlignCenter);
  main_layout->setContentsMargins(0, 0, 0, 0);
  main_layout->setSpacing(0);
  setLayout(main_layout);

  QWidget *container = new QWidget(this);
  {
    container->setFocusPolicy(Qt::NoFocus);
    container->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    QVBoxLayout *container_layout = new QVBoxLayout(container);
    container_layout->setAlignment(Qt::AlignHCenter);
    container_layout->setContentsMargins(v_spacing * 2, h_spacing * 2,
                                         v_spacing * 2, h_spacing * 2);
    container_layout->setSpacing(v_spacing);
    container->setLayout(container_layout);

    ui.app_icon_label = new QLabel(container);
    ui.app_icon_label->setScaledContents(true);
    ui.app_icon_label->setFixedSize(64, 64);
    ui.app_icon_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui.app_icon_label->setAlignment(Qt::AlignCenter);
    ui.app_icon_label->setPixmap(QPixmap(":/app_icon/solarus-editor-1024.png"));

    ui.title_label = new oclero::qlementine::Label(container);
    ui.title_label->setText(tr("Welcome to %1")
                                .arg(QApplication::applicationDisplayName()));
    ui.title_label->setAlignment(Qt::AlignHCenter);
    ui.title_label->setRole(oclero::qlementine::TextRole::H3);

    ui.version_label = new oclero::qlementine::Label(container);
    ui.version_label->setText(QApplication::applicationVersion());
    ui.version_label->setAlignment(Qt::AlignHCenter);
    ui.version_label->setRole(oclero::qlementine::TextRole::Caption);

    ui.new_quest_button = new oclero::qlementine::CommandLinkButton(container);
    ui.new_quest_button->setSizePolicy(QSizePolicy::Expanding,
                                       QSizePolicy::Fixed);
    ui.new_quest_button->setText(tr("Create a New Quest"));
    ui.new_quest_button->setDescription(
        tr("Create a new quest from scratch or from a template."));
    ui.new_quest_button->setIcon(QIcon(":/images/icon_new_24x24.svg"));
    QObject::connect(ui.new_quest_button, &QPushButton::clicked, this,
                     &Editor::new_quest_requested);

    ui.open_quest_button = new oclero::qlementine::CommandLinkButton(container);
    ui.open_quest_button->setSizePolicy(QSizePolicy::Expanding,
                                        QSizePolicy::Fixed);
    ui.open_quest_button->setText(tr("Open an Existing Quest"));
    ui.open_quest_button->setDescription(
        tr("Open an existing quest from your computer."));
    ui.open_quest_button->setIcon(QIcon(":/images/icon_open_24x24.svg"));
    QObject::connect(ui.open_quest_button, &QPushButton::clicked, this,
                     &Editor::open_quest_requested);

    QHBoxLayout *links_layout = new QHBoxLayout();
    {
      links_layout->setAlignment(Qt::AlignHCenter);
      links_layout->setSpacing(8);

      ui.documentation_button =
          new QPushButton(tr("Documentation"), container);
      ui.documentation_button->setSizePolicy(QSizePolicy::Fixed,
                                             QSizePolicy::Fixed);
      ui.documentation_button->setFlat(true);
      ui.documentation_button->setIcon(QIcon(":/images/icon_book.svg"));
      QObject::connect(ui.documentation_button, &QPushButton::clicked, this,
                       &Editor::documentation_requested);

      ui.website_button = new QPushButton(tr("Website"), container);
      ui.website_button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
      ui.website_button->setFlat(true);
      ui.website_button->setIcon(QIcon(":/images/icon_web.svg"));
      QObject::connect(ui.website_button, &QPushButton::clicked, this,
                       &Editor::website_requested);

      links_layout->addWidget(ui.documentation_button);
      links_layout->addWidget(ui.website_button);
    }

    EditorStyle::setAutoIconColor(ui.new_quest_button,
                                  EditorStyle::AutoIconColor::ForegroundColor);
    EditorStyle::setAutoIconColor(ui.open_quest_button,
                                  EditorStyle::AutoIconColor::ForegroundColor);
    EditorStyle::setAutoIconColor(ui.documentation_button,
                                  EditorStyle::AutoIconColor::ForegroundColor);
    EditorStyle::setAutoIconColor(ui.website_button,
                                  EditorStyle::AutoIconColor::ForegroundColor);

    container_layout->addWidget(ui.app_icon_label);
    container_layout->addSpacing(v_spacing);
    container_layout->addWidget(ui.title_label);
    container_layout->addWidget(ui.version_label);
    container_layout->addSpacing(v_spacing * 2);
    container_layout->addWidget(ui.new_quest_button);
    container_layout->addWidget(ui.open_quest_button);
    container_layout->addSpacing(v_spacing * 2);
    container_layout->addLayout(links_layout);

    container_layout->setAlignment(ui.app_icon_label, Qt::AlignHCenter);
  }

  main_layout->addWidget(container);

  QWidget::setTabOrder({
      ui.new_quest_button,
      ui.open_quest_button,
      ui.documentation_button,
      ui.website_button,
  });
}

} // namespace SolarusEditor
