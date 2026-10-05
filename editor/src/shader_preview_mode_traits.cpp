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
#include "shader_preview_mode_traits.h"
#include <QApplication>

namespace SolarusEditor {

/**
 * @brief Returns all values.
 * @return The existing values.
 */
QList<ShaderPreviewMode> EnumTraits<ShaderPreviewMode>::get_values() {

  return {
    ShaderPreviewMode::SIDE_BY_SIDE,
    ShaderPreviewMode::INPUT,
    ShaderPreviewMode::OUTPUT,
    ShaderPreviewMode::SWIPE,
  };
}

/**
 * @brief Returns a user-friendly name describing a value.
 * @param value A value.
 * @return The human-readable name of this value in the current language.
 */
QString EnumTraits<ShaderPreviewMode>::get_friendly_name(ShaderPreviewMode value) {

  switch (value) {

  case ShaderPreviewMode::SIDE_BY_SIDE:
    return QApplication::tr("Side by side");

  case ShaderPreviewMode::INPUT:
    return QApplication::tr("Input texture");

  case ShaderPreviewMode::OUTPUT:
    return QApplication::tr("Output texture");

  case ShaderPreviewMode::SWIPE:
    return QApplication::tr("Swipe");

  }

  return QString();
}

/**
 * @brief Returns an icon representing a value.
 * @param value A value.
 * @return The corresponding icon.
 */
QIcon EnumTraits<ShaderPreviewMode>::get_icon(ShaderPreviewMode value) {

  Q_UNUSED(value);
  return QIcon();
}

}
