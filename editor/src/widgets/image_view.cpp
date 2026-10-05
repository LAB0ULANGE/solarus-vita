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
#include "widgets/gui_tools.h"
#include "widgets/image_view.h"
#include "widgets/pan_tool.h"
#include "widgets/zoom_tool.h"
#include "editor_exception.h"
#include "view_settings.h"
#include <QGraphicsScene>
#include <QPixmap>
#include <QScrollBar>

namespace SolarusEditor {

/**
 * @brief Creates an image view.
 * @param Path of the image file to open.
 * @param parent Parent widget or nullptr.
 */
ImageView::ImageView(const QString& file_path, QWidget* parent) :
  QGraphicsView(parent),
  scene(new QGraphicsScene(this)),
  zoom(1.0) {

  QPixmap pixmap(file_path);
  if (pixmap.isNull()) {
    throw EditorException(tr("Failed to load image '%1'").arg(file_path));
  }

  // Install panning and zooming helpers.
  new PanTool(this);
  new ZoomTool(this);

  scene->setBackgroundBrush(palette().base());
  scene->addRect(QRect(QPoint(), pixmap.size()), Qt::NoPen, GuiTools::get_checkered_brush());
  scene->addPixmap(pixmap);
  setScene(scene);
}

/**
 * @brief Sets the view settings for this view.
 *
 * When they change, the view is updated accordingly.
 *
 * @param view_settings The settings to watch.
 */
void ImageView::set_view_settings(ViewSettings& view_settings) {

  this->view_settings = &view_settings;

  connect(&view_settings, &ViewSettings::zoom_changed,
          this, &ImageView::update_zoom);
  update_zoom();

  horizontalScrollBar()->setValue(0);
  verticalScrollBar()->setValue(0);
}

/**
 * @brief Sets the zoom level of the view from the settings.
 *
 * Zooming will be anchored at the mouse position.
 * The zoom value will be clamped between 0.25 and 4.0.
 */
void ImageView::update_zoom() {

  if (view_settings == nullptr) {
    return;
  }

  double zoom = view_settings->get_zoom();
  if (zoom == this->zoom) {
    return;
  }

  setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
  double scale_factor = zoom / this->zoom;
  scale(scale_factor, scale_factor);
  this->zoom = zoom;
}

/**
 * @brief Scales the view by a factor of 2.
 *
 * Zooming will be anchored at the mouse position.
 * The maximum zoom value is 4.0: this function does nothing if you try to
 * zoom more.
 */
void ImageView::zoom_in() {

  if (view_settings == nullptr) {
    return;
  }

  view_settings->set_zoom(view_settings->get_zoom() * 2.0);
}

/**
 * @brief Scales the view by a factor of 0.5.
 *
 * Zooming will be anchored at the mouse position.
 * The maximum zoom value is 0.25: this function does nothing if you try to
 * zoom less.
 */
void ImageView::zoom_out() {

  if (view_settings == nullptr) {
    return;
  }

  view_settings->set_zoom(view_settings->get_zoom() / 2.0);
}

}
