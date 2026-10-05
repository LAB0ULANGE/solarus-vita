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
#include "widgets/image_editor.h"
#include "widgets/image_view.h"
#include <QVBoxLayout>

namespace SolarusEditor {

/**
 * @brief Creates an image editor.
 * @param quest The quest containing the file.
 * @param file_path Path of the file to open.
 * @param parent The parent object or nullptr.
 * @throws EditorException If the file could not be opened.
 */
ImageEditor::ImageEditor(Quest& quest, const QString& file_path, QWidget* parent) :
  Editor(quest, file_path, parent),
  image_view(nullptr) {

  set_icon(QIcon(":/images/icon_image.svg"));
  set_save_supported(false);
  set_zoom_supported(true);

  QVBoxLayout* layout = new QVBoxLayout();
  layout->setContentsMargins(0, 0, 0, 0);
  setLayout(layout);

  image_view = new ImageView(file_path, this);
  layout->addWidget(image_view);

  get_view_settings().set_zoom(2.0);
  image_view->set_view_settings(get_view_settings());
}

}
