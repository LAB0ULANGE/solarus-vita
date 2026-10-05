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
#include "entities/custom_entity.h"

namespace SolarusEditor {

/**
 * @brief Constructor.
 * @param map The map containing the entity.
 * @param index Index of the entity in the map.
 */
CustomEntity::CustomEntity(MapModel& map, const EntityIndex& index) :
  EntityModel(map, index, EntityType::CUSTOM) {

  set_base_size(QSize(8, 8));
  set_resizable(true);

  set_num_directions(4);

  DrawShapeInfo info;
  info.enabled = true;
  info.background_color = QColor(224, 108, 72);
  info.between_border_color = QColor(184, 96, 96);
  info.pixmap = QPixmap(":/images/entity_custom_resizable.png");
  set_draw_shape_info(info);
}

/**
 * @copydoc EntityModel::notify_field_changed
 */
void CustomEntity::notify_field_changed(const QString& key, const QVariant& value) {

  EntityModel::notify_field_changed(key, value);

  if (key == "tiled") {
    DrawSpriteInfo sprite_info = get_draw_sprite_info();
    sprite_info.tiled = value.toBool();
    set_draw_sprite_info(sprite_info);
  }
}

/**
 * @copydoc EntityModel::is_size_valid
 */
bool CustomEntity::is_size_valid(const QSize& size) const {
  // Allow sizes that are not multiples of the base size
  return size.width() > 0 && size.height() > 0;
}


}
