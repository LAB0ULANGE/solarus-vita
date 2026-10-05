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
#include "widgets/border_set_selector.h"
#include "editor_exception.h"
#include "quest.h"
#include "tileset_model.h"

namespace SolarusEditor {

/**
 * @brief Creates an empty border set selector.
 *
 * Call the setter functions and then build() to fill the selector.
 *
 * @param parent The parent widget or nullptr.
 */
BorderSetSelector::BorderSetSelector(QWidget* parent) :
  QComboBox(parent) {

  setIconSize(QSize(24, 24));
}

/**
 * @brief Returns the id of the tileset where border sets come from.
 * @return The tileset id or an empty string if it is not set yet.
 */
QString BorderSetSelector::get_tileset_id() const {
  return tileset != nullptr ? tileset->get_tileset_id() : QString();
}

/**
 * @brief Sets the tileset where entities should come from.
 * @param quest The quest the tileset belongs to.
 * @param tileset_id Id of the tileset.
 */
void BorderSetSelector::set_tileset_id(Quest& quest, const QString& tileset_id) {

  try {
    tileset = std::make_unique<TilesetModel>(quest, tileset_id);
    connect(tileset.get(), &TilesetModel::tileset_data_file_changed,
            this, &BorderSetSelector::build);
    connect(tileset.get(), &TilesetModel::tileset_image_file_reloaded,
            this, &BorderSetSelector::build);
    build();
  }
  catch (const EditorException& ex) {
    // The tileset file could not be opened: the tileset is probably
    // unset or incorrect.
    Q_UNUSED(ex);
  }
}

/**
 * @brief Returns the border set id in the selected item.
 * @return The selected border set id.
 */
QString BorderSetSelector::get_selected_border_set_id() const {

  return currentData().toString();
}

/**
 * @brief Selects the specified border set.
 * @return Id of the border set to make selected.
 * Nothing happens if such a border set does not exist.
 */
void BorderSetSelector::set_selected_border_set_id(const QString& border_set_id) {

  int index = findData(border_set_id, Qt::UserRole);
  if (index == -1) {
    return;
  }

  setCurrentIndex(index);
}

/**
 * @brief Builds or rebuilds the combobox using the parameters previously set.
 */
void BorderSetSelector::build() {

  QString old_border_set_id = get_selected_border_set_id();
  clear();

  if (tileset == nullptr) {
    return;
  }

  // Add border sets.
  const QStringList& border_set_ids = tileset->get_border_set_ids();
  for (const QString& border_set_id : border_set_ids) {
    addItem(tileset->get_border_set_icon(border_set_id), border_set_id, border_set_id);
  }

  if (!old_border_set_id.isEmpty()) {
    set_selected_border_set_id(old_border_set_id);
  } else {
    if (!border_set_ids.isEmpty()) {
      set_selected_border_set_id(border_set_ids.first());
    }
  }
}

}
