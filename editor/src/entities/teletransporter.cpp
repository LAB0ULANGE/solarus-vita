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
#include "entities/teletransporter.h"
#include "map_model.h"

namespace SolarusEditor {

/**
 * @brief Constructor.
 * @param map The map containing the entity.
 * @param index Index of the entity in the map.
 */
Teletransporter::Teletransporter(MapModel& map, const EntityIndex& index) :
  EntityModel(map, index, EntityType::TELETRANSPORTER) {

  set_resizable(true);
  set_base_size({ 8, 8 });

  DrawShapeInfo info;
  info.enabled = true;
  info.background_color = QColor(240, 200, 80);
  info.between_border_color = QColor(240, 215, 142);
  info.pixmap = QPixmap(":/images/entity_teletransporter_resizable.png");
  set_draw_shape_info(info);
}

/**
 * @copydoc EntityModel::set_initial_values
 */
void Teletransporter::set_initial_values() {

  EntityModel::set_initial_values();

  // Initially propose to stay on the same map.
  set_field("destination_map", get_map().get_map_id());
  set_size({ 16, 16 });
}

/**
 * @copydoc EntityModel::notify_being_added
 */
void Teletransporter::notify_being_added() {

  EntityModel::notify_being_added();

  // The default values guess we want a transition for a door.
  // If the teletransporter is touching the edge of the map and it outside,
  // instead guess a scrolling transition.
  if (get_xy().x() + get_width() == 0 ||
      get_xy().x() == get_map().get_size().width() ||
      get_xy().y() + get_height() == 0 ||
      get_xy().y() == get_map().get_size().height()) {
    set_field("transition", "scrolling");
    set_field("destination", "_side");
  }
}

}
