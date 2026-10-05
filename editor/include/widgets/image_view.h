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
#ifndef SOLARUSEDITOR_IMAGE_VIEW_H
#define SOLARUSEDITOR_IMAGE_VIEW_H

#include <QGraphicsView>
#include <QImage>
#include <QPointer>

namespace SolarusEditor {

class ViewSettings;

/**
 * @brief A widget that displays an image file.
 */
class ImageView: public QGraphicsView {
  Q_OBJECT

public:

  explicit ImageView(const QString& file_path, QWidget* parent = nullptr);
  void set_view_settings(ViewSettings& view_settings);

public slots:
  void zoom_in();
  void zoom_out();

private:

  void update_zoom();

  QGraphicsScene* scene;               /**< The image scene. */
  QPointer<ViewSettings>
      view_settings;                   /**< How the view is displayed. */
  double zoom;                         /**< Zoom factor currently applied. */

};

}

#endif
