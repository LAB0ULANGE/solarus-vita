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
#include "pattern_scrolling_traits.h"
#include <QApplication>

namespace SolarusEditor {

/**
 * @brief Returns all values.
 * @return The existing values.
 */
QList<PatternScrolling> EnumTraits<PatternScrolling>::get_values() {

  return QList<PatternScrolling>()
      << PatternScrolling::NONE
      << PatternScrolling::SELF
      << PatternScrolling::PARALLAX;
}

/**
 * @brief Returns a user-friendly name describing a value.
 * @param value A value.
 * @return The human-readable name of this value in the current language.
 */
QString EnumTraits<PatternScrolling>::get_friendly_name(PatternScrolling value) {

  // Use a switch to ensure we don't forget a value,
  // and also to translate names dynamically.
  switch (value) {

  case PatternScrolling::NONE:
    return QApplication::tr("None", "Tile pattern scrolling");

  case PatternScrolling::SELF:
    return QApplication::tr("Scrolling on itself");

  case PatternScrolling::PARALLAX:
    return QApplication::tr("Parallax scrolling");

  }

  return "";
}

/**
 * @brief Returns an icon representing a value.
 * @param value A value.
 * @return The corresponding icon.
 */
QIcon EnumTraits<PatternScrolling>::get_icon(PatternScrolling /* value */) {
  // No icon for now.
  return QIcon();
}

}
