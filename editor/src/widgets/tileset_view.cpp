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
#include "widgets/enum_menus.h"
#include "widgets/gui_tools.h"
#include "widgets/pan_tool.h"
#include "widgets/tileset_scene.h"
#include "widgets/tileset_view.h"
#include "widgets/zoom_tool.h"
#include "ground_traits.h"
#include "point.h"
#include "rectangle.h"
#include "tileset_model.h"
#include "view_settings.h"
#include "editor_style.h"
#include <QAction>
#include <QApplication>
#include <QDrag>
#include <QGraphicsItem>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QScrollBar>
#include <QtMath>

namespace SolarusEditor {

/**
 * @brief Indicates what the user is currently doing on the tileset view.
 */
class TilesetViewState : public QObject {
    Q_OBJECT

public:
    explicit TilesetViewState(TilesetView& view);
    virtual ~TilesetViewState() = default;

    const TilesetView& get_view() const;
    TilesetView& get_view();
    const TilesetScene& get_scene() const;
    TilesetScene& get_scene();
    const TilesetModel& get_tileset() const;
    TilesetModel& get_tileset();

    virtual void start();
    virtual void stop();

    virtual void mouse_pressed(const QMouseEvent& event);
    virtual void mouse_released(const QMouseEvent& event);
    virtual void mouse_moved(const QMouseEvent& event);
    virtual void drag_enter(QDragEnterEvent& event);
    virtual void drag_move(QDragMoveEvent& event);
    virtual void drop(QDropEvent& event);
    virtual void context_menu_requested(const QPoint& where);

private:
    TilesetView& view;
};

/**
 * @brief State of the tileset view corresponding to the user doing nothing special.
 *
 * They can select or unselect patterns.
 */
class IdleState : public TilesetViewState {
    Q_OBJECT

public:
  explicit IdleState(TilesetView& view);

  void mouse_pressed(const QMouseEvent& event) override;
};

/**
 * @brief Drawing a rectangle for a selection or a new pattern.
 */
class DrawingRectangleState : public TilesetViewState {
    Q_OBJECT

public:
  DrawingRectangleState(TilesetView& view, const QPoint& initial_point);

  void start() override;
  void stop() override;

  void mouse_moved(const QMouseEvent& event) override;
  void mouse_released(const QMouseEvent& event) override;

private:
  void empty_rectangle_clicked(const QMouseEvent& event);
  void valid_rectangle_clicked(const QMouseEvent& event);

  QPoint initial_point;                     /**< Point where the drawing started, in scene coordinates. */
  QPoint current_point;                     /**< Point where the dragging is currently, in scene coordinates. */
  QGraphicsRectItem* current_area_item;     /**< Graphic item of the rectangle the user is drawing
                                             * (belongs to the scene). */
  QList<QGraphicsItem*> initial_selection;  /**< Items that were selected if Ctrl or Shift was pressed */
};

/**
 * @brief State of the tileset view of moving the selected patterns.
 */
class MovingPatternsState : public TilesetViewState {
    Q_OBJECT

public:
  MovingPatternsState(TilesetView& view, const QPoint& initial_point);

  void start() override;
  void stop() override;

  void mouse_released(const QMouseEvent& event) override;
  void drag_enter(QDragEnterEvent& event) override;
  void drag_move(QDragMoveEvent& event) override;
  void drop(QDropEvent& event) override;

private:
  void clear_current_areas();
  void apply_move();

  QPoint initial_point;      /**< Point where the dragging started, in scene coordinates. */
  QPoint last_point;         /**< Point where the mouse was last time it moved, in scene coordinates. */
  QList<QGraphicsRectItem*>
      current_area_items;    /**< Graphic items of the rectangles of target positions. */
};

/**
 * @brief Changing the size of the selected pattern.
 */
class ResizingPatternState : public TilesetViewState {
    Q_OBJECT

public:
  ResizingPatternState(TilesetView& view);

  void start() override;
  void stop() override;

  void mouse_moved(const QMouseEvent& event) override;
  void mouse_pressed(const QMouseEvent& event) override;

private:
  void compute_fixed_corner();
  void apply_resize();

  QGraphicsRectItem* current_area_item =
      nullptr;                  /**< Graphic item of the rectangle the user is drawing
                                 * (belongs to the scene). */
  QPoint fixed_corner;          /**< Which corner of the rectangle is fixed (+-1, +-1).
                                 * The opposite one follows the mouse. */
  QRect old_box;                /**< Position of the pattern before the change. */
  QRect current_box;            /**< Modified position of the pattern. */
};

/**
 * @brief Creates a tileset view.
 * @param parent The parent widget or nullptr.
 */
TilesetView::TilesetView(QWidget* parent) :
  QGraphicsView(parent) {

  setAcceptDrops(true);
  setAlignment(Qt::AlignTop | Qt::AlignLeft);

  resize_pattern_action = new QAction(
      QIcon(":/images/icon_resize_all.svg"),
      tr("Resize"),
      this
  );
  resize_pattern_action->setShortcut(tr("R"));
  resize_pattern_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(resize_pattern_action, &QAction::triggered,
          this, &TilesetView::start_state_resizing_pattern);
  addAction(resize_pattern_action);

  create_border_set_action = new QAction(
      QIcon(":/images/icon_border_set.svg"),
      tr("Create contour..."),
      this
  );
  create_border_set_action->setShortcut(tr("Ctrl+B"));
  create_border_set_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(create_border_set_action, &QAction::triggered,
          this, [this]() {
    QStringList pattern_ids = get_tileset()->get_selected_ids();
    pattern_ids.sort();
    emit create_border_set_requested(pattern_ids);
  });
  addAction(create_border_set_action);

  change_pattern_id_action = new QAction(
      QIcon(":/images/icon_edit.svg"), tr("Change id..."), this);
  change_pattern_id_action->setShortcut(tr("F2"));
  change_pattern_id_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(change_pattern_id_action, &QAction::triggered,
          this, &TilesetView::change_selected_pattern_id_requested);
  addAction(change_pattern_id_action);

  delete_patterns_action = new QAction(
      QIcon(":/images/icon_delete.svg"), tr("Delete..."), this);
  delete_patterns_action->setShortcut(QKeySequence::Delete);
  delete_patterns_action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  connect(delete_patterns_action, &QAction::triggered,
          this, &TilesetView::delete_selected_patterns_requested);
  addAction(delete_patterns_action);

  set_repeat_mode_actions = EnumMenus<PatternRepeatMode>::create_actions(
        *this,
        EnumMenuCheckableOption::CHECKABLE_EXCLUSIVE,
        [this](PatternRepeatMode repeat_mode) {
    emit change_selected_patterns_repeat_mode_requested(repeat_mode);
  });
  // TODO add shortcut support to EnumMenus
  set_repeat_mode_actions[static_cast<int>(PatternRepeatMode::ALL)]->setShortcut(tr("A"));
  set_repeat_mode_actions[static_cast<int>(PatternRepeatMode::HORIZONTAL)]->setShortcut(tr("H"));
  set_repeat_mode_actions[static_cast<int>(PatternRepeatMode::VERTICAL)]->setShortcut(tr("V"));
  set_repeat_mode_actions[static_cast<int>(PatternRepeatMode::NONE)]->setShortcut(tr("N"));
  for (QAction* action : std::as_const(set_repeat_mode_actions)) {
    action->setShortcutContext(Qt::WidgetWithChildrenShortcut);
  }

  ViewSettings* view_settings = new ViewSettings(this);
  set_view_settings(*view_settings);
}

/**
 * @brief Returns the tileset represented in this view.
 * @return The tileset, or nullptr if there is currently no tileset.
 */
TilesetModel* TilesetView::get_tileset() {
  return this->tileset;
}

/**
 * @brief Sets the tileset to represent in this view.
 * @param model The tileset model, or nullptr to remove any tileset.
 */
void TilesetView::set_tileset(TilesetModel* tileset) {

  if (this->tileset != nullptr) {
    double zoom = 1.0;
    if (view_settings != nullptr) {
      zoom = view_settings->get_zoom();
    }
    QPoint scroll_bar_position(horizontalScrollBar()->value(), verticalScrollBar()->value());
    recent_scroll_settings[this->tileset->get_tileset_id()] = ScrollSettings{
        scroll_bar_position, zoom
    };

    disconnect(this->tileset, nullptr,
               this, nullptr);
    this->tileset = nullptr;
    this->scene = nullptr;
  }

  this->tileset = tileset;

  if (tileset != nullptr) {
    // Create the scene from the model.
    scene = std::make_unique<TilesetScene>(*tileset, this);
    setScene(scene.get());  // The view does not take ownership of the scene.

    if (tileset->get_patterns_image().isNull()) {
      return;
    }

    // Restore the previous zoom and scrollbar positions.
    const ScrollSettings& scroll_settings = recent_scroll_settings.value(
          tileset->get_tileset_id(),
          ScrollSettings{QPoint(), 2.0}
    );
    if (view_settings != nullptr) {
      view_settings->set_zoom(scroll_settings.zoom);
    }
    horizontalScrollBar()->setValue(0);  // To force an actual change (refresh bug).
    horizontalScrollBar()->setValue(10);
    horizontalScrollBar()->setValue(scroll_settings.scroll_bar_position.x());

    verticalScrollBar()->setValue(0);
    verticalScrollBar()->setValue(10);
    verticalScrollBar()->setValue(scroll_settings.scroll_bar_position.y());

    // Install panning and zooming helpers.
    new PanTool(this);
    new ZoomTool(this);

    // Start the state mechanism.
    start_state_idle();

    connect(tileset, &TilesetModel::modelAboutToBeReset,
            this, &TilesetView::start_state_idle);
    connect(tileset, &TilesetModel::modelReset,
            this, &TilesetView::notify_tileset_changed);
    connect(tileset, &TilesetModel::tileset_image_file_reloaded,
            this, qOverload<>(&TilesetView::update));
    connect(&tileset->get_selection_model(), &QItemSelectionModel::selectionChanged,
            this, &TilesetView::tileset_selection_changed);
    tileset_selection_changed();
  }
}

/**
 * @brief Called when the tileset model has changed.
 */
void TilesetView::notify_tileset_changed() {
  start_state_idle();
}

/**
 * @brief Returns the tileset scene represented in this view.
 * @return The scene or nullptr if no tileset was set.
 */
TilesetScene* TilesetView::get_scene() {
  return scene.get();
}

/**
 * @brief Returns the view settings for this view.
 * @return The view settings or nullptr if they are not set yet.
 */
const ViewSettings* TilesetView::get_view_settings() const {
  return view_settings;
}

/**
 * @brief Sets the view settings for this view.
 *
 * When they change, the view is updated accordingly.
 *
 * @param view_settings The settings to watch.
 */
void TilesetView::set_view_settings(ViewSettings& view_settings) {

  this->view_settings = &view_settings;

  connect(&view_settings, &ViewSettings::zoom_changed,
          this, &TilesetView::update_zoom);
  update_zoom();

  connect(this->view_settings, &ViewSettings::grid_visibility_changed,
          this, &TilesetView::update_grid_visibility);
  connect(this->view_settings, &ViewSettings::grid_size_changed,
          this, &TilesetView::update_grid_visibility);
  connect(this->view_settings, &ViewSettings::grid_style_changed,
          this, &TilesetView::update_grid_visibility);
  connect(this->view_settings, &ViewSettings::grid_color_changed,
          this, &TilesetView::update_grid_visibility);
  update_grid_visibility();

  horizontalScrollBar()->setValue(0);
  verticalScrollBar()->setValue(0);
}

/**
 * @brief Returns the current zoom factor.
 * @return The zoom factor.
 */
double TilesetView::get_zoom() const {
  return zoom;
}

/**
 * @brief Returns whether the view is in read-only mode.
 * @return @c true if the mode is read-only, @c false if changes can be
 * made to the tileset.
 */
bool TilesetView::is_read_only() const {
  return read_only;
}

/**
 * @brief Sets whether the view is in read-only mode.
 * @param read_only @c true to block changes from this view, @c false to allow them.
 */
void TilesetView::set_read_only(bool read_only) {
  this->read_only = read_only;

  if (read_only) {
    resize_pattern_action->setEnabled(false);
    create_border_set_action->setEnabled(false);
    delete_patterns_action->setEnabled(false);
    change_pattern_id_action->setEnabled(false);
  }
}

/**
 * @brief Returns whether multiple selection is allowed.
 * @return @c true if the user can select multiple patterns.
 */
bool TilesetView::is_multi_selection_enabled() const {
  return multi_selection_enabled;
}

/**
 * @brief Sets whether multiple selection is allowed.
 * @return param multi_selection_enabled @c true to allow multiple selection.
 */
void TilesetView::set_multi_selection_enabled(bool multi_selection_enabled) {
  this->multi_selection_enabled = multi_selection_enabled;
}

/**
 * @brief Requests the creation of a pattern with an automatically generated id.
 * @param frame Rectangle of the pattern to create.
 * @param ground Ground of the pattern to create.
 */
void TilesetView::create_pattern_with_auto_id(const QRect& frame, Ground ground) {

  if (tileset == nullptr) {
    return;
  }

  QString pattern_id;
  do {
    ++last_integer_pattern_id;
    pattern_id = QString::number(last_integer_pattern_id);
  } while (tileset->id_to_index(pattern_id) != -1);

  emit create_pattern_requested(pattern_id, frame, ground);
}

/**
 * @brief Sets the zoom level of the view from the settings.
 *
 * Zooming will be anchored at the mouse position.
 * The zoom value will be clamped between 0.25 and 4.0.
 */
void TilesetView::update_zoom() {

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
void TilesetView::zoom_in() {

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
void TilesetView::zoom_out() {

  if (view_settings == nullptr) {
    return;
  }

  view_settings->set_zoom(view_settings->get_zoom() / 2.0);
}

/**
 * @brief Shows or hides the grid according to the view settings.
 */
void TilesetView::update_grid_visibility() {

  if (view_settings == nullptr) {
    return;
  }

  if (view_settings->is_grid_visible()) {
    // Necessary to correctly show the grid when scrolling,
    // because it is part of the foreground, not of graphics items.
    setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
  }
  else {
    // Faster.
    setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
  }

  if (scene != nullptr) {
    // The foreground has changed.
    scene->invalidate();
  }
}

/**
 * @brief Selects all patterns.
 */
void TilesetView::select_all() {

  if (scene == nullptr) {
    return;
  }

  if (!multi_selection_enabled &&
      tileset->get_num_patterns() > 1) {
    return;
  }

  scene->select_all();
}

/**
 * @brief Unselects all patterns.
 */
void TilesetView::unselect_all() {

  if (scene == nullptr) {
    return;
  }

  scene->unselect_all();
}

/**
 * @brief Draws the tileset view.
 * @param event The paint event.
 */
void TilesetView::paintEvent(QPaintEvent* event) {

  QGraphicsView::paintEvent(event);

  if (view_settings == nullptr || !view_settings->is_grid_visible()) {
    return;
  }

  QSize grid = view_settings->get_grid_size();
  QRect rect = event->rect();
  rect.setTopLeft(mapFromScene(0, 0));

  // Draw the grid.
  QPainter painter(viewport());
  GuiTools::draw_grid(
    painter, rect, grid * zoom, view_settings->get_grid_color(),
    view_settings->get_grid_style());
}

/**
 * @brief Receives a key press event.
 * @param event The event to handle.
 */
void TilesetView::keyPressEvent(QKeyEvent* event) {

  if (event->key() == Qt::Key_Escape && !is_read_only()) {
    start_state_idle();
    return;
  }
  QGraphicsView::keyPressEvent(event);
}

/**
 * @brief Receives a mouse press event.
 * @param event The event to handle.
 */
void TilesetView::mousePressEvent(QMouseEvent* event) {

  if (tileset == nullptr ||
        get_scene() == nullptr ||
        state == nullptr) {
    return;
  }

  if (!(QApplication::mouseButtons() & event->button())) {
    // The button that triggered this event is no longer pressed.
    // This is possible if pressing the button already triggered something
    // else like a modal dialog.
    return;
  }

  state->mouse_pressed(*event);

  // Don't forward the event to the parent because it would select the item
  // clicked. We only do this explicitly from specific states.
}

/**
 * @brief Receives a mouse release event.
 * @param event The event to handle.
 */
void TilesetView::mouseReleaseEvent(QMouseEvent* event) {

  if (tileset != nullptr &&
        get_scene() != nullptr &&
        state != nullptr) {
    state->mouse_released(*event);
  }

  QGraphicsView::mouseReleaseEvent(event);
}

/**
 * @brief Receives a mouse double click event.
 * @param event The event to handle.
 */
void TilesetView::mouseDoubleClickEvent(QMouseEvent* event) {

  // Nothing special but we don't want the behavior from the parent class.
  Q_UNUSED(event);
}

/**
 * @brief Receives a mouse move event.
 * @param event The event to handle.
 */
void TilesetView::mouseMoveEvent(QMouseEvent* event) {

  if (tileset != nullptr &&
        get_scene() != nullptr &&
        state != nullptr) {
    state->mouse_moved(*event);
  }

  // The parent class tracks mouse movements for internal needs
  // such as anchoring the viewport to the mouse when zooming.
  QGraphicsView::mouseMoveEvent(event);
}

/**
 * @brief Receives a context menu event.
 * @param event The event to handle.
 */
void TilesetView::contextMenuEvent(QContextMenuEvent* event) {

  if (scene == nullptr) {
    return;
  }

  QPoint where;
  if (event->pos() != QPoint(0, 0)) {
    where = event->pos();
  }
  else {
    QList<QGraphicsItem*> selected_items = scene->selectedItems();
    where = mapFromScene(selected_items.first()->pos() + QPoint(8, 8));
  }

  show_context_menu(where);
}

/**
 * @brief Shows a context menu with actions relative to the selected patterns.
 *
 * Does nothing if the view is in read-only mode.
 *
 * @param where Where to show the menu, in view coordinates.
 */
void TilesetView::show_context_menu(const QPoint& where) {

  if (tileset == nullptr) {
    return;
  }

  if (is_read_only()) {
    return;
  }

  QList<int> selected_indexes = tileset->get_selected_indexes();
  if (selected_indexes.empty()) {
    return;
  }

  QMenu* menu = new QMenu(this);
  EditorStyle::setAutoIconColor(menu, EditorStyle::AutoIconColor::ForegroundColor);

  // Resize.
  menu->addAction(resize_pattern_action);

  // Ground.
  QMenu* ground_menu = new QMenu(tr("Ground"), this);
  build_context_menu_ground(*ground_menu, selected_indexes);
  connect(ground_menu, &QMenu::aboutToHide, menu, &QMenu::close);
  menu->addMenu(ground_menu);

  // Default layer.
  QMenu* layer_menu = new QMenu(tr("Default layer"), this);
  build_context_menu_layer(*layer_menu, selected_indexes);
  connect(layer_menu, &QMenu::aboutToHide, menu, &QMenu::close);
  menu->addMenu(layer_menu);

  // Repeat mode.
  QMenu* repeat_mode_menu = new QMenu(tr("Repeatable"), this);
  build_context_menu_repeat_mode(*repeat_mode_menu, selected_indexes);
  connect(repeat_mode_menu, &QMenu::aboutToHide, menu, &QMenu::close);
  menu->addMenu(repeat_mode_menu);

  // Animation.
  QMenu* scrolling_menu = new QMenu(tr("Scrolling"), this);
  build_context_menu_scrolling(*scrolling_menu, selected_indexes);
  connect(scrolling_menu, &QMenu::aboutToHide, menu, &QMenu::close);
  menu->addMenu(scrolling_menu);

  // Border set.
  if (selected_indexes.size() <= 12) {
    menu->addAction(create_border_set_action);
  }

  // Change pattern id.
  menu->addSeparator();
  menu->addAction(change_pattern_id_action);

  // Delete patterns.
  menu->addSeparator();
  menu->addAction(delete_patterns_action);

  // Create the menu at 1,1 to avoid the cursor being already in the first item.
  menu->popup(viewport()->mapToGlobal(where) + QPoint(1, 1));
}

/**
 * @brief Builds the ground part of a context menu for patterns.
 * @param menu The menu to fill.
 * @param indexes Patterns to build a context menu for.
 */
void TilesetView::build_context_menu_ground(
    QMenu& menu, const QList<int>& indexes) {

  if (indexes.empty()) {
    return;
  }

  // See if the ground is common.
  Ground ground;
  bool common = tileset->is_common_pattern_ground(indexes, ground);

  // Add ground actions to the menu.
  QList<QAction*> ground_actions = EnumMenus<Ground>::create_actions(
        menu,
        EnumMenuCheckableOption::CHECKABLE_EXCLUSIVE,
        [this](Ground ground) {
    emit change_selected_patterns_ground_requested(ground);
  });

  if (common) {
    int ground_index = static_cast<int>(ground);
    QAction* checked_action = ground_actions[ground_index];
    checked_action->setChecked(true);
    // Add a checkmark (there is none when there is already an icon).
    checked_action->setText("\u2714 " + checked_action->text());
  }
}

/**
 * @brief Builds the default layer part of a context menu for patterns.
 * @param menu The menu to fill.
 * @param indexes Patterns to build a context menu for.
 */
void TilesetView::build_context_menu_layer(
    QMenu& menu, const QList<int>& indexes) {

  if (indexes.empty()) {
    return;
  }

  // See if the default layer is common.
  int common_layer = 0;
  bool common = tileset->is_common_pattern_default_layer(indexes, common_layer);

  // Add 3 layer actions to the menu.
  // (If more layers are necessary, the user can still use the spinbox
  // in the patterns properties view.)
  for (int i = 0; i < 3; ++i) {
    const QIcon icon = i >= 0 && i <= 3 ? QIcon(QString(":/images/icon_layer_%1.svg").arg(i)) : QIcon{};
    QAction* action = new QAction(icon, tr("Layer %1").arg(i), &menu);
    action->setCheckable(true);
    menu.addAction(action);
    connect(action, &QAction::triggered, this, [this, i]() {
      emit change_selected_patterns_default_layer_requested(i);
    });

    if (common && i == common_layer) {
      action->setChecked(true);
    }
  }

  EditorStyle::setAutoIconColor(&menu, EditorStyle::AutoIconColor::TextColor);
}

/**
 * @brief Builds the repeat mode part of a context menu for patterns.
 * @param menu The menu to fill.
 * @param indexes Patterns to build a context menu for.
 */
void TilesetView::build_context_menu_repeat_mode(
    QMenu& menu, const QList<int>& indexes) {

  if (indexes.empty()) {
    return;
  }

  // See if the repeat mode is common.
  PatternRepeatMode repeat_mode = PatternRepeatMode::ALL;
  bool common = tileset->is_common_pattern_repeat_mode(indexes, repeat_mode);

  menu.addActions(set_repeat_mode_actions);

  if (common) {
    int repeat_mode_index = static_cast<int>(repeat_mode);
    QAction* checked_action = set_repeat_mode_actions[repeat_mode_index];
    checked_action->setChecked(true);
  }

  EditorStyle::setAutoIconColor(&menu, EditorStyle::AutoIconColor::TextColor);
}

/**
 * @brief Builds the scrolling property part of a context menu for patterns.
 * @param menu The menu to fill.
 * @param indexes Patterns to build a context menu for.
 */
void TilesetView::build_context_menu_scrolling(
    QMenu& menu, const QList<int>& indexes) {

  if (indexes.empty()) {
    return;
  }

  // See if the scrolling is common.
  PatternScrolling scrolling;
  bool common = tileset->is_common_pattern_scrolling(indexes, scrolling);

  // Add actions to the menu.
  QList<QAction*> scrolling_actions = EnumMenus<PatternScrolling>::create_actions(
        menu,
        EnumMenuCheckableOption::CHECKABLE_EXCLUSIVE,
        [this](PatternScrolling animation) {
    emit change_selected_patterns_scrolling_requested(animation);
  });

  if (common) {
    int scrolling_index = static_cast<int>(scrolling);
    QAction* checked_action = scrolling_actions[scrolling_index];
    checked_action->setChecked(true);
  }
}

/**
 * @brief Called when the selection has changed.
 */
void TilesetView::tileset_selection_changed() {

  if (tileset == nullptr) {
    return;
  }

  if (!is_read_only()) {
    const int selection_count = tileset->get_selection_count();
    const int pattern_index = tileset->get_selected_index();
    if (selection_count == 1) {
      resize_pattern_action->setEnabled(tileset->get_pattern_num_frames(pattern_index) == 1);
      change_pattern_id_action->setEnabled(true);
    } else {
      resize_pattern_action->setEnabled(false);
      change_pattern_id_action->setEnabled(false);
    }

    if (selection_count == 0) {
      create_border_set_action->setEnabled(false);
      delete_patterns_action->setEnabled(false);
    } else {
      create_border_set_action->setEnabled(true);
      delete_patterns_action->setEnabled(true);
    }
  }
}

/**
 * @brief Returns the current state of the view.
 * @return The current state.
 */
const TilesetViewState* TilesetView::get_state() const {
  return state;
}

/**
 * @brief Changes the state of the view.
 *
 * The previous state if any will be destroyed in the next event process.
 *
 * @param state The new state.
 */
void TilesetView::set_state(TilesetViewState* state) {

  if (this->state != nullptr) {
    this->state->stop();
    this->state->deleteLater();
  }

  this->state = state;

  if (this->state != nullptr) {
    this->state->start();
  }
}

/**
 * @brief Moves to the normal state of the tileset view.
 */
void TilesetView::start_state_idle() {

  set_state(new IdleState(*this));
}

/**
 * @brief Moves to the state of drawing a rectangle for a selection or a
 * new pattern.
 * @param initial_point Where the user starts drawing the rectangle,
 * in view coordinates.
 */
void TilesetView::start_state_drawing_rectangle(const QPoint& initial_point) {

  set_state(new DrawingRectangleState(*this, initial_point));
}

/**
 * @brief Moves to the state of moving the selected patterns.
 * @param initial_point Where the user starts dragging the patterns,
 * in view coordinates.
 */
void TilesetView::start_state_moving_patterns(const QPoint& initial_point) {

  if (tileset->is_selection_empty()) {
    return;
  }

  set_state(new MovingPatternsState(*this, initial_point));
}

/**
 * @brief Moves to the state of resizing the selected pattern.
 */
void TilesetView::start_state_resizing_pattern() {

  if (tileset->get_selected_index() == -1) {
    return;
  }

  set_state(new ResizingPatternState(*this));
}

void TilesetView::dragEnterEvent(QDragEnterEvent* event) {

  if (tileset == nullptr || get_scene() == nullptr) {
    return;
  }
  state->drag_enter(*event);
}

void TilesetView::dragMoveEvent(QDragMoveEvent* event) {

  if (tileset == nullptr || get_scene() == nullptr) {
    return;
  }
  state->drag_move(*event);
}

void TilesetView::dragLeaveEvent(QDragLeaveEvent* event) {

  Q_UNUSED(event);
}

void TilesetView::dropEvent(QDropEvent* event) {

  if (tileset == nullptr || get_scene() == nullptr) {
    return;
  }
  state->drop(*event);
}

/**
 * @brief Returns all items that intersect the given areas.
 * @param @c area_items Items to check.
 * @param ignore_selected @c true if the selection should be ignored.
 * @return The items that intersect @c area_items.
 */
QList<QGraphicsItem*> TilesetView::get_items_intersecting_areas(
    const QList<QGraphicsRectItem*>& area_items,
    bool ignore_selected) const {

  QList<QGraphicsItem*> items;

  for (QGraphicsRectItem* item : area_items) {
    QRect area = item->rect().toRect().adjusted(1, 1, -1, -1);
    items.append(scene->items(area, Qt::IntersectsItemBoundingRect));
    items.removeAll(item);  // Ignore the drawn rectangle itself.
  }

  // Ignore selected items.
  if (ignore_selected) {
    const QList<QGraphicsItem*> selected_items = scene->selectedItems();
    for (QGraphicsItem* item : selected_items) {
      items.removeAll(item);
    }
  }

  return items;
}

/**
 * @brief Returns the bounding box corresponding to all selected tile patterns.
 * @return The bounding box of all the selected tile patterns.
 */
QRect TilesetView::get_selection_bounding_box() const {

  QRect bounding_box;
  const QList<int> selected_indexes = tileset->get_selected_indexes();
  for (int index : selected_indexes) {
    bounding_box = bounding_box.united(
      tileset->get_pattern_frames_bounding_box(index));
  }
  return bounding_box;
}

/**
 * @brief Creates a state.
 * @param view The tileset view to manage.
 */
TilesetViewState::TilesetViewState(TilesetView& view) :
  QObject(&view),
  view(view) {

}

/**
 * @brief Returns the tileset view managed by this state.
 * @return The tileset view.
 */
const TilesetView& TilesetViewState::get_view() const {
  return view;
}

/**
 * @overload
 *
 * Non-const version.
 */
TilesetView& TilesetViewState::get_view() {
  return view;
}

/**
 * @brief Returns the tileset scene managed by this state.
 * @return The tileset scene.
 */
const TilesetScene& TilesetViewState::get_scene() const {
  return *view.get_scene();
}

/**
 * @overload
 *
 * Non-const version.
 */
TilesetScene& TilesetViewState::get_scene() {
  return *view.get_scene();
}

/**
 * @brief Returns the tileset model represented in the view.
 * @return The tileset model.
 */
const TilesetModel& TilesetViewState::get_tileset() const {
  return *view.get_tileset();
}

/**
 * @overload
 *
 * Non-const version.
 */
TilesetModel& TilesetViewState::get_tileset() {
  return *view.get_tileset();
}

/**
 * @brief Called when entering this state.
 *
 * Subclasses can reimplement this function to initialize data.
 */
void TilesetViewState::start() {
}

/**
 * @brief Called when leaving this state.
 *
 * Subclasses can reimplement this function to clean data.
 */
void TilesetViewState::stop() {
}

/**
 * @brief Called when the mouse is pressed in the map view during this state.
 *
 * Subclasses can reimplement this function to define what happens.
 *
 * @param event The event to handle.
 */
void TilesetViewState::mouse_pressed(const QMouseEvent& event) {
  Q_UNUSED(event);
}

/**
 * @brief Called when the mouse is released in the tileset view during this state.
 *
 * Subclasses can reimplement this function to define what happens.
 *
 * @param event The event to handle.
 */
void TilesetViewState::mouse_released(const QMouseEvent& event) {
  Q_UNUSED(event);
}

/**
 * @brief Called when the mouse is moved in the tileset view during this state.
 *
 * Subclasses can reimplement this function to define what happens.
 *
 * @param event The event to handle.
 */
void TilesetViewState::mouse_moved(const QMouseEvent& event) {
  Q_UNUSED(event);
}

/**
 * @brief Called when a drag enter event happens in the tileset view
 * during this state.
 * @param event The event to handle.
 */
void TilesetViewState::drag_enter(QDragEnterEvent& event) {
  Q_UNUSED(event);
}

/**
 * @brief Called when a drag move event happens in the tileset view
 * during this state.
 * @param event The event to handle.
 */
void TilesetViewState::drag_move(QDragMoveEvent& event) {
  Q_UNUSED(event);
}

/**
 * @brief Called when a drag enter event happens in the tileset view
 * during this state.
 * @param event The event to handle.
 */
void TilesetViewState::drop(QDropEvent& event) {
  Q_UNUSED(event);
}

/**
 * @brief Called when a context menu is requested in the tileset view during
 * this state.
 *
 * Subclasses can reimplement this function to show a context menu.
 *
 * @param where Where to show the context menu, in global coordinates.
 */
void TilesetViewState::context_menu_requested(const QPoint& where) {
  Q_UNUSED(where);
}

/**
 * @brief Constructor.
 * @param view The tileset view to manage.
 */
IdleState::IdleState(TilesetView& view):
  TilesetViewState(view) {

}

/**
 * @copydoc TilesetView::State::mouse_pressed
 */
void IdleState::mouse_pressed(const QMouseEvent& event) {

  QList<QGraphicsItem*> items_under_mouse = get_view().items(
        QRect(event.pos(), QSize(1, 1)),
        Qt::IntersectsItemBoundingRect  // Pick transparent items too.
  );
  QGraphicsItem* item = items_under_mouse.empty() ? nullptr : items_under_mouse.first();

  const bool control_or_shift = (event.modifiers() & (Qt::ControlModifier | Qt::ShiftModifier));

  bool keep_selected = false;
  if (control_or_shift && get_view().is_multi_selection_enabled()) {
    // If ctrl or shift is pressed, keep the existing selection.
    keep_selected = true;
  }
  else if (item != nullptr && item->isSelected()) {
    // When clicking an already selected item, keep the existing selection too.
    keep_selected = true;
  }

  if (!keep_selected) {
    get_scene().clearSelection();
  }

  if (event.button() == Qt::LeftButton) {
    if (item != nullptr &&
        item->isSelected() &&
        !get_tileset().is_selection_empty() &&
        !control_or_shift &&
        !get_view().is_read_only()) {
      // Clicking on an already selected item: allow to move it.
      get_view().start_state_moving_patterns(event.pos());
    }
    else {
      if (get_view().is_multi_selection_enabled()) {
        // Don't select the item yet, initialize a selection rectangle.
        get_view().start_state_drawing_rectangle(event.pos());
      }
      else {
        // No multiple selection is allowed: don't draw a selection rectangle,
        // directly consider this as a click.
        if (item != nullptr) {
          // An item was clicked.
          if (control_or_shift) {
            // Toggle the selected state of the item.

            item->setSelected(!item->isSelected());
            emit get_view().selection_changed_by_user();
          } else {
            // Select the clicked item.
            item->setSelected(true);
            emit get_view().selection_changed_by_user();
          }
        }
      }
    }
  }
  else {
    if (item != nullptr && !item->isSelected()) {
      // Select the right-clicked item.
      item->setSelected(true);
      emit get_view().selection_changed_by_user();
    }
  }
}

/**
 * @brief Constructor.
 * @param view The tileset view to manage.
 * @param initial_point Point where the drawing started, in view coordinates.
 */
DrawingRectangleState::DrawingRectangleState(TilesetView& view, const QPoint& initial_point):
  TilesetViewState(view),
  initial_point(view.mapToScene(initial_point).toPoint() / 8 * 8),
  current_point(this->initial_point),
  current_area_item(nullptr),
  initial_selection() {

}

/**
 * @copydoc MapView::State::start
 */
void DrawingRectangleState::start() {

  current_area_item = new QGraphicsRectItem();
  current_area_item->setPen(QPen(Qt::yellow));
  get_scene().addItem(current_area_item);
  initial_selection = get_scene().selectedItems();
}

/**
 * @copydoc MapView::State::stop
 */
void DrawingRectangleState::stop() {

  get_scene().removeItem(current_area_item);
  delete current_area_item;
  current_area_item = nullptr;
}

void DrawingRectangleState::mouse_released(const QMouseEvent& event) {

  if (current_area_item == nullptr) {
    return;
  }

  // If the rectangle is empty, consider it was a click and not a drag.
  if (current_area_item->rect().isEmpty()) {
    empty_rectangle_clicked(event);
  } else {
    valid_rectangle_clicked(event);
  }
  get_view().start_state_idle();
}

/**
 * @brief Called when the user releases the mouse after no rectangle was drawn.
 * @param event The mouse event to handle.
 */
void DrawingRectangleState::empty_rectangle_clicked(const QMouseEvent& event) {

  if (event.button() != Qt::LeftButton && event.button() != Qt::RightButton) {
    return;
  }

  // Left or right button: possibly change the selection.
  QList<QGraphicsItem*> items_under_mouse = get_view().items(
        QRect(event.pos(), QSize(1, 1)),
        Qt::IntersectsItemBoundingRect  // Pick transparent items too.
  );
  QGraphicsItem* item = items_under_mouse.empty() ? nullptr : items_under_mouse.first();

  const bool control_or_shift = (event.modifiers() & (Qt::ControlModifier | Qt::ShiftModifier));

  bool keep_selected = false;
  if (control_or_shift) {
    // If ctrl or shift is pressed, keep the existing selection.
    keep_selected = true;
  }
  else if (item != nullptr && item->isSelected()) {
    // When clicking an already selected item, keep the existing selection too.
    keep_selected = true;
  }

  if (!keep_selected) {
    bool selection_was_empty = get_tileset().is_selection_empty();
    get_scene().clearSelection();

    if (item == nullptr && selection_was_empty) {
      // The user clicked outside any item, to unselect everything.
      emit get_view().selection_changed_by_user();
    }
  }

  if (item == nullptr || event.button() != Qt::LeftButton) {
    return;
  }

  // Clicked an item.
  if (control_or_shift) {
    // Left-clicking an item while pressing control or shift: toggle it.
    item->setSelected(!item->isSelected());
    emit get_view().selection_changed_by_user();
  }
  else {
    if (!item->isSelected()) {
      // Select the item.
      item->setSelected(true);
      emit get_view().selection_changed_by_user();
    }
  }
}

/**
 * @brief Called when the user releases the mouse after a non-empty rectangle was drawn.
 * @param event The mouse event to handle.
 */
void DrawingRectangleState::valid_rectangle_clicked(const QMouseEvent& /* event */) {

  QRect rectangle = current_area_item->rect().toRect();
  if (!rectangle.isEmpty() &&
      get_view().sceneRect().contains(rectangle) &&
      get_view().get_items_intersecting_areas({current_area_item}, true).isEmpty() &&
      get_tileset().is_selection_empty() &&
      !get_view().is_read_only()) {

    // Context menu to create a pattern.
    QMenu menu;
    EnumMenus<Ground>::create_actions(
          menu, EnumMenuCheckableOption::NON_CHECKABLE, [this, rectangle, &menu](Ground ground) {
      get_view().create_pattern_with_auto_id(rectangle, ground);
      menu.close();
    });

    // Put most actions in a submenu to make the context menu smaller.
    QMenu sub_menu(TilesetView::tr("New pattern (more options)"));
    const QList<QAction*> actions = menu.actions();
    for (QAction* action : actions) {
      Ground ground = static_cast<Ground>(action->data().toInt());
      if (ground == Ground::TRAVERSABLE ||
          ground == Ground::WALL) {
        action->setText(TilesetView::tr("New pattern (%1)").arg(GroundTraits::get_friendly_name(ground)));
      }
      else {
        menu.removeAction(action);
        sub_menu.addAction(action);
      }
    }
    menu.addMenu(&sub_menu);

    menu.addSeparator();
    menu.addAction(TilesetView::tr("Cancel"));
    menu.exec(get_view().cursor().pos() + QPoint(1, 1));
  }
}

void DrawingRectangleState::mouse_moved(const QMouseEvent& event) {

  // Compute the selected area.
  QPoint previous_point = current_point;
  current_point = get_view().mapToScene(event.pos()).toPoint() / 8 * 8;

  if (current_point == previous_point) {
    return;
  }

  QRect area = Rectangle::from_two_points(initial_point, current_point);
  if (current_area_item->rect().toRect() == area) {
    // No change.
    return;
  }
  current_area_item->setRect(area);

  // Select items strictly in the rectangle.
  get_scene().clearSelection();
  QPainterPath path;
  path.addRect(QRect(area.topLeft() - QPoint(1, 1),
                     area.size() + QSize(2, 2)));
  get_scene().setSelectionArea(path, Qt::ReplaceSelection, Qt::ContainsItemBoundingRect);

  // Re-select items that were already selected if Ctrl or Shift was pressed.
  for (QGraphicsItem* item : std::as_const(initial_selection)) {
    item->setSelected(true);
  }

  emit get_view().selection_changed_by_user();
}

MovingPatternsState::MovingPatternsState(TilesetView& view, const QPoint& initial_point):
  TilesetViewState(view),
  initial_point(Point::floor_8(get_view().mapToScene(initial_point))),
  last_point(this->initial_point) {

}

void MovingPatternsState::start() {

  const QList<int>& selected_indexes = get_tileset().get_selected_indexes();
  for (int index : selected_indexes) {
    const QRect& box = get_tileset().get_pattern_frames_bounding_box(index);
    QGraphicsRectItem *item = new QGraphicsRectItem(box);
    item->setPen(QPen(Qt::yellow));
    get_scene().addItem(item);
    current_area_items.append(item);
  }

  TilesetView& view = get_view();
  const QRect& pattern_frame = get_tileset().get_pattern_frame(selected_indexes.first());
  const QPoint& hot_spot = view.mapFromScene(initial_point) - view.mapFromScene(pattern_frame.topLeft());
  QPixmap drag_pixmap = get_tileset().get_pattern_image(selected_indexes.first());

  const ViewSettings* view_settings = view.get_view_settings();
  if (view_settings != nullptr) {
    double zoom = view_settings->get_zoom();
    drag_pixmap = drag_pixmap.scaled(pattern_frame.size() * zoom);
  }

  // TODO make a pixmap of all selected patterns.
  QDrag* drag = new QDrag(&get_view());
  drag->setPixmap(drag_pixmap);
  drag->setHotSpot(hot_spot);

  QStringList pattern_ids;
  for (int index : selected_indexes) {
    pattern_ids << get_tileset().index_to_id(index);
  }
  std::sort(pattern_ids.begin(), pattern_ids.end());
  QString text_data = pattern_ids.join("\n");

  QMimeData* data = new QMimeData();
  data->setText(text_data);

  drag->setMimeData(data);
  drag->exec(Qt::MoveAction | Qt::CopyAction);  // Blocking call during the drag operation.
  if (view.get_state() == this) {
    view.start_state_idle();
  }
}

void MovingPatternsState::stop() {

  clear_current_areas();
}

/**
 * @brief Deletes any rectangle created by this state.
 */
void MovingPatternsState::clear_current_areas() {

  for (QGraphicsRectItem* item : std::as_const(current_area_items)) {
    get_scene().removeItem(item);
    delete item;
  }
  current_area_items.clear();
}

/**
 * @brief Applies the pattern move action according to the mouse position.
 */
void MovingPatternsState::apply_move() {

  QPoint delta = last_point - initial_point;
  QRect box = get_view().get_selection_bounding_box();
  box.translate(delta);
  if (!box.isEmpty() &&
      get_view().sceneRect().contains(box) &&
      get_view().get_items_intersecting_areas(current_area_items, true).isEmpty() &&
      !get_tileset().is_selection_empty() &&
      !get_view().is_read_only() &&
      last_point != initial_point) {

    // Context menu to move the patterns.
    TilesetView* view = &get_view();
    QMenu menu;
    QAction* move_pattern_action = new QAction(TilesetView::tr("Move here"), view);
    view->connect(move_pattern_action, &QAction::triggered, view, [view, delta]() {
      emit view->change_selected_patterns_position_requested(delta);
    });
    menu.addAction(move_pattern_action);
    QAction* duplicate_pattern_action = new QAction(
        QIcon(":/images/icon_copy.svg"), TilesetView::tr("Duplicate here"), view);
    duplicate_pattern_action->setEnabled(
      view->get_items_intersecting_areas(current_area_items, false).isEmpty());
    view->connect(duplicate_pattern_action, &QAction::triggered, view, [view, delta]() {
      emit view->duplicate_selected_patterns_requested(delta);
    });
    menu.addAction(duplicate_pattern_action);
    menu.addSeparator();
    menu.addAction(TilesetView::tr("Cancel"));
    menu.exec(get_view().cursor().pos() + QPoint(1, 1));
  }

  get_view().start_state_idle();
}

void MovingPatternsState::mouse_released(const QMouseEvent& /* event */) {
  apply_move();
}

void MovingPatternsState::drag_enter(QDragEnterEvent& event) {

  if (event.mimeData()->hasFormat("text/plain")) {
    event.acceptProposedAction();
  }
}

void MovingPatternsState::drag_move(QDragMoveEvent& event) {

  last_point = Point::floor_8(
      get_view().mapToScene(event.position().toPoint()));
  QPoint delta = last_point - initial_point;

  clear_current_areas();

  bool valid_move = true;
  const QList<int>& selected_indexes = get_tileset().get_selected_indexes();
  const QList<QGraphicsItem*> selected_items = get_scene().selectedItems();
  for (int index : selected_indexes) {

    QRect area = get_tileset().get_pattern_frames_bounding_box(index);
    area.translate(delta);
    QGraphicsRectItem* item = new QGraphicsRectItem(area);

    // Check overlapping existing patterns.
    QList<QGraphicsItem*> overlapping_item_list = get_scene().items(
        area.adjusted(1, 1, -1, -1), Qt::IntersectsItemBoundingRect);
    QSet<QGraphicsItem*> overlapping_items;
    for (QGraphicsItem* overlapping_item : std::as_const(overlapping_item_list)) {
        overlapping_items.insert(overlapping_item);
    }

    // Filter out the patterns that are being moved,
    // that is, allow the destination to overlap the source.
    for (QGraphicsItem* selected_item : selected_items) {
      overlapping_items.remove(selected_item);
    }

    if (!area.isEmpty() &&
        get_view().sceneRect().contains(area) &&
        overlapping_items.isEmpty() &&
        !get_view().is_read_only()) {
      item->setPen(QPen(Qt::yellow));
    } else {
      item->setPen(QPen(Qt::red));
      item->setZValue(1);
      valid_move = false;
    }

    // Let the drag cursor show if the move is legal.
    event.setAccepted(valid_move);

    get_scene().addItem(item);
    current_area_items.append(item);
  }
}

void MovingPatternsState::drop(QDropEvent& event) {

  if (event.dropAction() == Qt::CopyAction) {
    event.acceptProposedAction();
  }
  else if (event.dropAction() == Qt::MoveAction) {
    event.acceptProposedAction();
  }
  apply_move();
}

/**
 * @brief Constructor.
 */
ResizingPatternState::ResizingPatternState(TilesetView& view):
  TilesetViewState(view),
  current_area_item(nullptr) {

}

/**
 * @copydoc TilesetView::State::start
 */
void ResizingPatternState::start() {

  const int selected_index = get_tileset().get_selected_index();
  old_box = get_tileset().get_pattern_frames_bounding_box(selected_index);
  current_area_item = new QGraphicsRectItem(old_box);
  current_area_item->setPen(QPen(Qt::yellow));
  get_scene().addItem(current_area_item);

  compute_fixed_corner();
}

/**
 * @copydoc TilesetView::State::stop
 */
void ResizingPatternState::stop() {

  get_scene().removeItem(current_area_item);
  delete current_area_item;
  current_area_item = nullptr;
}

/**
 * Determines which corner of the pattern should be fixed
 * and which one should follow the mouse.
 */
void ResizingPatternState::compute_fixed_corner() {

  // Decide the resizing directions depending
  // on the mouse position relative to the pattern.
  const int selected_index = get_tileset().get_selected_index();
  if (selected_index == -1) {
    return;
  }

  TilesetView& view = get_view();

  const QPoint mouse_position_in_view = view.mapFromGlobal(QCursor::pos());
  const QPoint mouse_position = view.mapToScene(mouse_position_in_view).toPoint();

  const QPointF& pattern_center = current_area_item->rect().center();
  fixed_corner.setX(mouse_position.x() >= pattern_center.x() ? -1 : 1);
  fixed_corner.setY(mouse_position.y() >= pattern_center.y() ? -1 : 1);
}

/**
 * @copydoc TilesetView::State::mouse_moved
 */
void ResizingPatternState::mouse_moved(const QMouseEvent& event) {

  QPointF tileset_point_f = get_view().mapToScene(event.pos());
  QPoint current_point(qFloor(tileset_point_f.x()), qFloor(tileset_point_f.y()));

  QPoint free_corner = old_box.topLeft();
  if (fixed_corner.x() == -1) {
    // The left side is fixed, the right side moves.
    free_corner.rx() += old_box.width() - 8;
  }
  if (fixed_corner.y() == -1) {
    // The top side is fixed, the bottom side moves.
    free_corner.ry() += old_box.height() - 8;
  }
  const QSize base_size(8, 8);
  const QPoint expansion = Point::floor(current_point - free_corner, base_size);
  current_box = Rectangle::expand_rect(old_box, fixed_corner, expansion, base_size);

  current_area_item->setRect(current_box);

  const bool valid = get_view().get_items_intersecting_areas({ current_area_item }, true).isEmpty();
  current_area_item->setPen(valid ? QPen(Qt::yellow) : QPen(Qt::red));
}

/**
 * @copydoc TilesetView::State::mouse_pressed
 */
void ResizingPatternState::mouse_pressed(const QMouseEvent& /* event */) {
  apply_resize();
}

/**
 * @brief Applies the new size to the selected pattern.
 */
void ResizingPatternState::apply_resize() {

  if (current_area_item != nullptr &&
      !current_box.isEmpty() &&
      get_view().get_items_intersecting_areas({ current_area_item }, true).isEmpty() &&
      get_tileset().get_selection_count() == 1 &&
      !get_view().is_read_only() &&
      current_box != old_box) {
    emit get_view().resize_selected_pattern_requested(current_box);
  }
  get_view().start_state_idle();
}

}

#include "tileset_view.moc"
