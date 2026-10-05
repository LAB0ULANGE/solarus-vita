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
#include "rectangle.h"

namespace SolarusEditor {

namespace Rectangle {

/**
 * @brief Converts a Solarus rectangle to a Qt rectangle.
 * @param solarus_rect A rectangle for the Solarus library.
 * @return The equivalent QRect.
 */
QRect to_qrect(const Solarus::Rectangle& solarus_rect) {

  return QRect(solarus_rect.get_x(), solarus_rect.get_y(),
               solarus_rect.get_width(), solarus_rect.get_height());
}

/**
 * @brief Converts a Qt rectangle to a Solarus rectangle.
 * @param qt_rect A Qt rectangle.
 * @return The equivalent Solarus rectangle.
 */
Solarus::Rectangle to_solarus_rect(const QRect& qt_rect) {

  return Solarus::Rectangle(
        qt_rect.x(), qt_rect.y(), qt_rect.width(), qt_rect.height());
}

/**
 * @brief Returns the rectangle formed by two opposite corner points.
 * @param p1 A point.
 * @param p2 Another point.
 * @return The corresponding rectangle.
 */
QRect from_two_points(const QPoint& p1, const QPoint& p2) {

  QRect result;
  if (p1.x() < p2.x()) {
    result.setX(p1.x());
    result.setWidth(p2.x() - p1.x());
  }
  else {
    result.setX(p2.x());
    result.setWidth(p1.x() - p2.x());
  }

  if (p1.y() < p2.y()) {
    result.setY(p1.y());
    result.setHeight(p2.y() - p1.y());
  }
  else {
    result.setY(p2.y());
    result.setHeight(p1.y() - p2.y());
  }

  return result;
}

/**
 * @brief Expands a rectangle with the given amount in given directions.
 * @param old_box The rectangle to expand.
 * @param fixed_corner Which corner of the rectangle is the fixed one
 * (the opposite one will get expanded): -1 or +1,-1 or +1
 * @param expansion How much to expand it in both directions, keeping the
 * given fixed corner.
 * @param base_size Minimum size, the fixed corner will keep a rectangle
 * of that size and the rest will be expanded in multiples of that size.
 * @return The corresponding new bounding box.
 */
QRect expand_rect(
    const QRect& old_box,
    const QPoint& fixed_corner,
    const QPoint& expansion,
    const QSize& base_size) {

  QRect new_box = old_box;

  if (fixed_corner.x() == -1) {
    // Left side fixed, right side free.
    int width = old_box.width() + expansion.x();
    if (width > 0) {
      new_box.setWidth(width);
    }
    else {
      new_box.setWidth(-width + 2 * base_size.width());
      new_box.translate(width - base_size.width(), 0);
    }
  }
  else {
    // Right side fixed, left side free.
    int width = old_box.width() - expansion.x();
    if (width > 0) {
      new_box.setWidth(width);
      new_box.translate(expansion.x(), 0);
    }
    else {
      new_box.setWidth(-width + 2 * base_size.width());
      new_box.translate(old_box.width() - base_size.width(), 0);
    }
  }
  if (fixed_corner.y() == -1) {
    // Top side fixed, bottom side free.
    int height = old_box.height() + expansion.y();
    if (height > 0) {
      new_box.setHeight(height);
    }
    else {
      new_box.setHeight(-height + 2 * base_size.height());
      new_box.translate(0, height - base_size.height());
    }
  }
  else {
    // Bottom side fixed, top side free.
    int height = old_box.height() - expansion.y();
    if (height > 0) {
      new_box.setHeight(height);
      new_box.translate(0, expansion.y());
    }
    else {
      new_box.setHeight(-height + 2 * base_size.height());
      new_box.translate(0, old_box.height() - base_size.height());
    }
  }

  return new_box;
}

}

}
