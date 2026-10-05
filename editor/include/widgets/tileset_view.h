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
#ifndef SOLARUSEDITOR_TILESET_VIEW_H
#define SOLARUSEDITOR_TILESET_VIEW_H

#include "ground_traits.h"
#include "pattern_repeat_mode_traits.h"
#include "pattern_scrolling_traits.h"
#include <QGraphicsView>
#include <QPointer>
#include <QMenu>
#include <memory>

class QAction;

namespace SolarusEditor {

class TilesetModel;
class TilesetScene;
class TilesetViewState;
class ViewSettings;

/**
 * @brief Graphical view of the tileset image, allowing to manage tile patterns.
 */
class TilesetView : public QGraphicsView {
  Q_OBJECT

public:

  explicit TilesetView(QWidget* parent = nullptr);

  TilesetModel* get_tileset();
  TilesetScene* get_scene();
  void set_tileset(TilesetModel* tileset);
  const ViewSettings* get_view_settings() const;
  void set_view_settings(ViewSettings& view_settings);
  double get_zoom() const;
  bool is_read_only() const;
  void set_read_only(bool read_only);
  bool is_multi_selection_enabled() const;
  void set_multi_selection_enabled(bool multi_selection_enabled);
  QRect get_selection_bounding_box() const;
  void create_pattern_with_auto_id(const QRect& frame, Ground ground);

  QList<QGraphicsItem*> get_items_intersecting_areas(
      const QList<QGraphicsRectItem*>& area_items,
      bool ignore_selected) const;
  const TilesetViewState* get_state() const;
  void start_state_idle();
  void start_state_drawing_rectangle(const QPoint& initial_point);
  void start_state_moving_patterns(const QPoint& initial_point);
  void start_state_resizing_pattern();

signals:

  void create_pattern_requested(
      const QString& pattern_id, const QRect& frame, Ground ground);
  void duplicate_selected_patterns_requested(const QPoint& delta);
  void delete_selected_patterns_requested();
  void change_selected_pattern_id_requested();
  void change_selected_patterns_position_requested(const QPoint& delta);
  void resize_selected_pattern_requested(const QRect& box);
  void change_selected_patterns_ground_requested(Ground ground);
  void change_selected_patterns_default_layer_requested(int layer);
  void change_selected_patterns_repeat_mode_requested(PatternRepeatMode repeat_mode);
  void change_selected_patterns_scrolling_requested(PatternScrolling scrolling);
  void create_border_set_requested(const QStringList& pattern_ids);
  void selection_changed_by_user();

public slots:

  void update_zoom();
  void zoom_in();
  void zoom_out();
  void update_grid_visibility();
  void select_all();
  void unselect_all();

protected:

  void paintEvent(QPaintEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;
  void mousePressEvent(QMouseEvent* event) override;
  void mouseReleaseEvent(QMouseEvent* event) override;
  void mouseDoubleClickEvent(QMouseEvent* event) override;
  void mouseMoveEvent(QMouseEvent* event) override;
  void contextMenuEvent(QContextMenuEvent* event) override;

private:

  /**
   * @brief Stores the scrollbar position and zoom of recent tilesets.
   */
  struct ScrollSettings {
    QPoint scroll_bar_position;
    double zoom = 1.0;
  };

  void show_context_menu(const QPoint& where);
  void build_context_menu_ground(QMenu& menu, const QList<int>& indexes);
  void build_context_menu_layer(QMenu& menu, const QList<int>& indexes);
  void build_context_menu_repeat_mode(QMenu& menu, const QList<int>& indexes);
  void build_context_menu_scrolling(QMenu& menu, const QList<int>& indexes);
  void tileset_selection_changed();

  // State of the view.
  void set_state(TilesetViewState* state);

  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragMoveEvent(QDragMoveEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

  void notify_tileset_changed();

  QPointer<TilesetModel> tileset;      /**< The tileset model. */
  std::unique_ptr<TilesetScene> scene; /**< The scene viewed. */
  QPointer<ViewSettings>
      view_settings;                   /**< How the view is displayed. */
  double zoom = 1.0;                   /**< Zoom factor currently applied. */
  TilesetViewState* state = nullptr;   /**< Current state of the view. Owned by this view. */

  QAction* resize_pattern_action = nullptr;      /**< Action of resizing a pattern. */
  QAction* create_border_set_action = nullptr;   /**< Action of creating a border set. */
  QAction* change_pattern_id_action = nullptr;   /**< Action of changing a pattern id. */
  QAction* delete_patterns_action = nullptr;     /**< Action of deleting the selected
                                        * patterns. */
  QList<QAction*>
      set_repeat_mode_actions;         /**< Actions of changing the repeat
                                        * modes of patterns. */
  int last_integer_pattern_id = 0;     /**< Last auto-generated pattern id. */
  bool read_only = false;              /**< Whether the view forbids editing the tileset. */
  bool multi_selection_enabled = true; /**< Whether it is allowed to select multiple patterns. */
  QMap<QString, ScrollSettings>
      recent_scroll_settings;          /**< Scroll bar positions and zoom of recent tilesets. */
};

}

#endif
