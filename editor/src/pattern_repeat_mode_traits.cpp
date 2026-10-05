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
#include "pattern_repeat_mode_traits.h"
#include <QApplication>

namespace SolarusEditor {

namespace {
  const QList<PatternRepeatMode> names = QList<PatternRepeatMode>()
    << PatternRepeatMode::ALL
    << PatternRepeatMode::HORIZONTAL
    << PatternRepeatMode::VERTICAL
    << PatternRepeatMode::NONE;
}

/**
 * @brief Returns all values.
 * @return The existing values.
 */
QList<PatternRepeatMode> EnumTraits<PatternRepeatMode>::get_values() {
  return names;
}

/**
 * @brief Returns a user-friendly name describing a value.
 * @param value A value.
 * @return The human-readable name of this value in the current language.
 */
QString EnumTraits<PatternRepeatMode>::get_friendly_name(PatternRepeatMode value) {

  // Use a switch to ensure we don't forget a value,
  // and also to translate names dynamically.
  switch (value) {

  case PatternRepeatMode::ALL:
    return QApplication::tr("In both directions");

  case PatternRepeatMode::HORIZONTAL:
    return QApplication::tr("Horizontally");

  case PatternRepeatMode::VERTICAL:
    return QApplication::tr("Vertically");

  case PatternRepeatMode::NONE:
    return QApplication::tr("Non repeatable");

  }

  return "";
}

/**
 * @brief Returns an icon representing a value.
 * @param value A value.
 * @return The corresponding icon.
 */
QIcon EnumTraits<PatternRepeatMode>::get_icon(PatternRepeatMode value) {
  return QIcon(":/images/icon_resize_" + get_lua_name(value) + ".svg");
}

/**
 * @brief Returns the Lua name of a value.
 * @param value A ground value.
 * @return The corresponding Lua name.
 */
QString EnumTraits<PatternRepeatMode>::get_lua_name(PatternRepeatMode value) {
  return QString::fromStdString(Solarus::enum_to_name(value));
}

/**
 * @brief Returns a value given its Lua name.
 * @param name A name.
 * @return The corresponding value.
 */
PatternRepeatMode EnumTraits<PatternRepeatMode>::get_by_lua_name(
    const QString& name) {
  return Solarus::name_to_enum<PatternRepeatMode>(name.toStdString());
}

}
