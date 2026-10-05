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
#include "widgets/change_border_set_id_dialog.h"
#include "widgets/gui_tools.h"
#include "widgets/input_dialog_with_check_box.h"
#include "widgets/tileset_editor.h"
#include "widgets/tileset_scene.h"
#include "editor_exception.h"
#include "editor_settings.h"
#include "editor_style.h"
#include "quest.h"
#include "quest_database.h"
#include "refactoring.h"
#include "tileset_model.h"
#include <QGuiApplication>
#include <QColorDialog>
#include <QDebug>
#include <QFile>
#include <QInputDialog>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QTextStream>
#include <QUndoStack>

namespace SolarusEditor {

namespace {

/**
 * @brief Parent class of all undoable commands of the tileset editor.
 */
class TilesetEditorCommand : public QUndoCommand {

public:

  TilesetEditorCommand(TilesetEditor& editor, const QString& text) :
    QUndoCommand(text),
    editor(editor) {
  }

  TilesetEditor& get_editor() const {
    return editor;
  }

  TilesetModel& get_model() const {
    return editor.get_model();
  }

private:

  TilesetEditor& editor;
};

/**
 * @brief Changing the background color of the tileset.
 */
class SetBackgroundCommand : public TilesetEditorCommand {

public:

  SetBackgroundCommand(TilesetEditor& editor, const QColor& color) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Background color")),
    color_before(get_model().get_background_color()),
    color_after(color) {
  }

  virtual void undo() override {
    get_model().set_background_color(color_before);
  }

  virtual void redo() override {
    get_model().set_background_color(color_after);
  }

private:

  QColor color_before;
  QColor color_after;
};

/**
 * @brief Moving a tile pattern.
 */
class SetPatternPositionCommand : public TilesetEditorCommand {

public:

  SetPatternPositionCommand(
      TilesetEditor& editor, int index, const QPoint& position) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Move pattern")),
    index(index),
    position_before(get_model().get_pattern_frame(index).topLeft()),
    position_after(position) {
  }

  virtual void undo() override {

    get_model().set_pattern_position(index, position_before);
    get_model().set_selected_index(index);
  }

  virtual void redo() override {

    get_model().set_pattern_position(index, position_after);
    get_model().set_selected_index(index);
  }

private:

  int index;
  QPoint position_before;
  QPoint position_after;
};

/**
 * @brief Resizing a tile pattern.
 */
class ResizePatternCommand : public TilesetEditorCommand {

public:

  ResizePatternCommand(
      TilesetEditor& editor, int index, const QRect& box) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Resize pattern")),
    index(index),
    box_before(get_model().get_pattern_frame(index)),
    box_after(box) {
  }

  virtual void undo() override {

    get_model().set_pattern_box(index, box_before);
    get_model().set_selected_index(index);
  }

  virtual void redo() override {

    get_model().set_pattern_box(index, box_after);
    get_model().set_selected_index(index);
  }

private:

  int index;
  QRect box_before;
  QRect box_after;
};

/**
 * @brief Moving several tile patterns.
 */
class SetPatternsPositionCommand : public TilesetEditorCommand {

public:

  SetPatternsPositionCommand(
      TilesetEditor& editor, QList<int> indexes, const QPoint& delta) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Move patterns")),
    indexes(indexes),
    delta(delta) {
  }

  virtual void undo() override {

    for (int index : std::as_const(indexes)) {
      QPoint position = get_model().get_pattern_frame(index).topLeft();
      get_model().set_pattern_position(index, position - delta);
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {

    for (int index : std::as_const(indexes)) {
      QPoint position = get_model().get_pattern_frame(index).topLeft();
      get_model().set_pattern_position(index, position + delta);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QPoint delta;
};

/**
 * @brief Changing the ground of tile patterns.
 */
class SetPatternsGroundCommand : public TilesetEditorCommand {

public:

  SetPatternsGroundCommand(
      TilesetEditor& editor, const QList<int>& indexes, Ground ground) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Ground")),
    indexes(indexes),
    ground_after(ground) {

    for (int index : std::as_const(indexes)) {
      grounds_before << get_model().get_pattern_ground(index);
    }
  }

  virtual void undo() override {

    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_ground(index, grounds_before[i]);
      ++i;
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {

    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_ground(index, ground_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<Ground> grounds_before;
  Ground ground_after;
};

/**
 * @brief Changing the default layer of tile patterns.
 */
class SetPatternsDefaultLayerCommand : public TilesetEditorCommand {

public:

  SetPatternsDefaultLayerCommand(TilesetEditor& editor, const QList<int>& indexes, int layer) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Default layer")),
    indexes(indexes),
    layer_after(layer) {

    for (int index : std::as_const(indexes)) {
      layers_before << get_model().get_pattern_default_layer(index);
    }
  }

  virtual void undo() override {

    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_default_layer(index, layers_before[i]);
      ++i;
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {

    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_default_layer(index, layer_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<int> layers_before;
  int layer_after;
};

/**
 * @brief Changing the repeat mode of tile patterns.
 */
class SetPatternsRepeatModeCommand : public TilesetEditorCommand {

public:

  SetPatternsRepeatModeCommand(TilesetEditor& editor, const QList<int>& indexes, PatternRepeatMode repeat_mode) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Repeat mode")),
    indexes(indexes),
    repeat_mode_after(repeat_mode) {

    for (int index : indexes) {
      repeat_modes_before << get_model().get_pattern_repeat_mode(index);
    }
  }

  virtual void undo() override {

    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_repeat_mode(index, repeat_modes_before[i]);
      ++i;
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {

    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_repeat_mode(index, repeat_mode_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<PatternRepeatMode> repeat_modes_before;
  PatternRepeatMode repeat_mode_after;
};

/**
 * @brief Changing the scrolling property of tile patterns.
 */
class SetPatternsScrollingCommand : public TilesetEditorCommand {

public:

  SetPatternsScrollingCommand(
      TilesetEditor& editor, const QList<int>& indexes, PatternScrolling scrolling) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Animation")),
    indexes(indexes),
    scrolling_after(scrolling) {

    for (int index : std::as_const(indexes)) {
      scrollings_before << get_model().get_pattern_scrolling(index);
    }
  }

  virtual void undo() override {

    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_scrolling(index, scrollings_before[i]);
      ++i;
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {

    // TODO don't do anything if one fails.
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_scrolling(index, scrolling_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<PatternScrolling> scrollings_before;
  PatternScrolling scrolling_after;
};

/**
 * @brief Changing the animation property of tile patterns.
 */
class SetPatternsSeparationCommand : public TilesetEditorCommand {

public:

  SetPatternsSeparationCommand(
      TilesetEditor& editor, const QList<int>& indexes, PatternSeparation separation) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Animation separation")),
    indexes(indexes),
    separation_after(separation) {

    for (int index : std::as_const(indexes)) {
      separations_before << get_model().get_pattern_separation(index);
    }
  }

  virtual void undo() override {
    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_separation(index, separations_before[i]);
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_separation(index, separation_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<PatternSeparation> separations_before;
  PatternSeparation separation_after;
};

/**
 * @brief Changing the number of frames of tile patterns.
 */
class SetPatternsNumFramesCommand : public TilesetEditorCommand {

public:

  SetPatternsNumFramesCommand(TilesetEditor& editor, const QList<int>& indexes, int num_frames) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Frame delay")),
    indexes(indexes),
    num_frames_after(num_frames) {

    for (int index : indexes) {
      num_frames_before << get_model().get_pattern_num_frames(index);
    }
  }

  virtual void undo() override {
    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_num_frames(index, num_frames_before[i]);
      ++i;
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_num_frames(index, num_frames_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<int> num_frames_before;
  int num_frames_after;
};

/**
 * @brief Changing the frame delay of tile patterns.
 */
class SetPatternsFrameDelayCommand : public TilesetEditorCommand {

public:

  SetPatternsFrameDelayCommand(TilesetEditor& editor, const QList<int>& indexes, int frame_delay) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Frame delay")),
    indexes(indexes),
    frame_delay_after(frame_delay) {

    for (int index : indexes) {
      frame_delays_before << get_model().get_pattern_frame_delay(index);
    }
  }

  virtual void undo() override {
    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_frame_delay(index, frame_delays_before[i]);
      ++i;
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_frame_delay(index, frame_delay_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<int> frame_delays_before;
  int frame_delay_after;
};

/**
 * @brief Changing the mirror loop property of tile patterns.
 */
class SetPatternsMirrorLoopCommand : public TilesetEditorCommand {

public:

  SetPatternsMirrorLoopCommand(TilesetEditor& editor, const QList<int>& indexes, bool mirror_loop) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Mirror loop")),
    indexes(indexes),
    mirror_loop_after(mirror_loop) {

    for (int index : indexes) {
      mirror_loops_before << get_model().is_pattern_mirror_loop(index);
    }
  }

  virtual void undo() override {
    int i = 0;
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_mirror_loop(index, mirror_loops_before[i]);
      ++i;
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {
    for (int index : std::as_const(indexes)) {
      get_model().set_pattern_mirror_loop(index, mirror_loop_after);
    }
    get_model().set_selected_indexes(indexes);
  }

private:

  QList<int> indexes;
  QList<bool> mirror_loops_before;
  bool mirror_loop_after;
};

/**
 * @brief Creating a tile pattern.
 */
class CreatePatternCommand : public TilesetEditorCommand {

public:

  CreatePatternCommand(TilesetEditor& editor,
                       const QString& pattern_id,
                       const QRect& frame,
                       Ground ground) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Create pattern")),
    index(-1),
    pattern_id(pattern_id),
    frame(frame),
    ground(ground) {
  }

  virtual void undo() override {

    get_model().delete_pattern(index);
  }

  virtual void redo() override {

    index = get_model().create_pattern(pattern_id, frame);
    get_model().set_pattern_ground(index, ground);
    get_model().add_to_selected(index);
  }

private:

  int index;
  QString pattern_id;
  QRect frame;
  Ground ground;
};

/**
 * @brief Duplicate tile patterns.
 */
class DuplicatePatternsCommand : public TilesetEditorCommand {

public:

  DuplicatePatternsCommand(
      TilesetEditor& editor, const QList<int>& indexes, const QPoint& delta) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Duplicate")),
    delta(delta) {
    for (int index : std::as_const(indexes)) {
      ids.append(get_model().index_to_id(index));
    }
  }

  virtual void undo() override {

    for (const QString& new_id : std::as_const(new_ids)) {
      get_model().delete_pattern(get_model().id_to_index(new_id));
    }
  }

  virtual void redo() override {

    get_model().clear_selection();
    new_ids.clear();

    for (const QString& id : std::as_const(ids)) {

      int index = get_model().id_to_index(id);
      QString new_id = get_model().get_unique_pattern_id(id);
      QRect first_frame = get_model().get_pattern_frame(index);
      first_frame.translate(delta);

      int new_index = get_model().create_pattern(new_id, first_frame);
      get_model().set_pattern_scrolling(
        new_index, get_model().get_pattern_scrolling(index));
      get_model().set_pattern_default_layer(
        new_index, get_model().get_pattern_default_layer(index));
      get_model().set_pattern_ground(
        new_index, get_model().get_pattern_ground(index));
      get_model().set_pattern_repeat_mode(
        new_index, get_model().get_pattern_repeat_mode(index));
      get_model().set_pattern_num_frames(
        new_index, get_model().get_pattern_num_frames(index));
      get_model().set_pattern_separation(
        new_index, get_model().get_pattern_separation(index));
      get_model().set_pattern_frame_delay(
        new_index, get_model().get_pattern_frame_delay(index));
      get_model().set_pattern_mirror_loop(
        new_index, get_model().is_pattern_mirror_loop(index));

      new_ids.append(new_id);
      get_model().add_to_selected(new_index);
    }
  }

private:

  QList<QString> ids;
  QList<QString> new_ids;
  QPoint delta;
};

/**
 * @brief Deleting tile patterns.
 */
class DeletePatternsCommand : public TilesetEditorCommand {

public:

  DeletePatternsCommand(TilesetEditor& editor, const QList<int>& indexes) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Delete")) {

    for (int index : indexes) {
      Pattern pattern;
      pattern.id = get_model().index_to_id(index);
      pattern.first_frame = get_model().get_pattern_frame(index);
      pattern.ground = get_model().get_pattern_ground(index);
      pattern.default_layer = get_model().get_pattern_default_layer(index);
      pattern.scrolling = get_model().get_pattern_scrolling(index);
      pattern.num_frames = get_model().get_pattern_num_frames(index);
      pattern.separation = get_model().get_pattern_separation(index);
      pattern.frame_delay = get_model().get_pattern_frame_delay(index);
      pattern.mirror_loop = get_model().is_pattern_mirror_loop(index);
      pattern.repeat_mode = get_model().get_pattern_repeat_mode(index);
      patterns << pattern;
    }
  }

  virtual void undo() override {

    for (const Pattern& pattern : std::as_const(patterns)) {
      int index = get_model().create_pattern(pattern.id, pattern.first_frame);
      get_model().set_pattern_ground(index, pattern.ground);
      get_model().set_pattern_default_layer(index, pattern.default_layer);
      get_model().set_pattern_scrolling(index, pattern.scrolling);
      get_model().set_pattern_num_frames(index, pattern.num_frames);
      get_model().set_pattern_separation(index, pattern.separation);
      get_model().set_pattern_frame_delay(index, pattern.frame_delay);
      get_model().set_pattern_mirror_loop(index, pattern.mirror_loop);
      get_model().set_pattern_repeat_mode(index, pattern.repeat_mode);
    }

    QList<int> indexes;
    for (const Pattern& pattern : std::as_const(patterns)) {
      indexes << get_model().id_to_index(pattern.id);
    }
    get_model().set_selected_indexes(indexes);
  }

  virtual void redo() override {

    QList<int> indexes;
    for (const Pattern& pattern : std::as_const(patterns)) {
      indexes << get_model().id_to_index(pattern.id);
    }
    get_model().delete_patterns(indexes);
  }

private:

  struct Pattern {
    QString id;
    QRect first_frame;
    Ground ground;
    int default_layer = 0;
    PatternScrolling scrolling;
    int num_frames = 0;
    PatternSeparation separation;
    int frame_delay = 0;
    bool mirror_loop = false;
    PatternRepeatMode repeat_mode;
  };

  QList<Pattern> patterns;
};

/**
 * @brief Changing the id of a tile pattern.
 */
class SetPatternIdCommand : public TilesetEditorCommand {

public:

  SetPatternIdCommand(TilesetEditor& editor, int old_index, const QString& new_id) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Pattern id")),
    index_before(old_index),
    index_after(-1),
    id_before(get_model().index_to_id(old_index)),
    id_after(new_id) {
  }

  virtual void undo() override {
    get_model().set_pattern_id(index_after, id_before);
    get_model().set_selected_index(index_before);
  }

  virtual void redo() override {
    index_after = get_model().set_pattern_id(index_before, id_after);
    get_model().set_selected_index(index_after);
  }

private:

  int index_before;
  int index_after;
  QString id_before;
  QString id_after;
};

/**
 * @brief Changing the id of a border set.
 */
class SetBorderSetIdCommand : public TilesetEditorCommand {

public:

  SetBorderSetIdCommand(TilesetEditor& editor, const QString& old_id, const QString& new_id) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Border set id")),
    id_before(old_id),
    id_after(new_id) {
  }

  virtual void undo() override {
    get_model().set_border_set_id(id_after, id_before);
  }

  virtual void redo() override {
    get_model().set_border_set_id(id_before, id_after);
  }

private:

  QString id_before;
  QString id_after;
};

/**
 * @brief Changing the inner property of a border set.
 */
class SetBorderSetInnerCommand : public TilesetEditorCommand {

public:

  SetBorderSetInnerCommand(TilesetEditor& editor, const QString& border_set_id, bool inner) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Border set inner")),
    border_set_id(border_set_id),
    inner_before(get_model().is_border_set_inner(border_set_id)),
    inner_after(inner) {
  }

  virtual void undo() override {
    get_model().set_border_set_inner(border_set_id, inner_before);
  }

  virtual void redo() override {
    get_model().set_border_set_inner(border_set_id, inner_after);
  }

private:

  QString border_set_id;
  bool inner_before;
  bool inner_after;
};

/**
 * @brief Changing the patterns of a border set.
 */
class SetBorderSetPatternsCommand : public TilesetEditorCommand {

public:

  SetBorderSetPatternsCommand(
      TilesetEditor& editor,
      const QString& border_set_id,
      const QStringList& pattern_ids) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Border set patterns")),
    border_set_id(border_set_id),
    pattern_ids_before(get_model().get_border_set_patterns(border_set_id)),
    pattern_ids_after(pattern_ids) {
  }

  virtual void undo() override {
    get_model().set_border_set_patterns(border_set_id, pattern_ids_before);
  }

  virtual void redo() override {
    get_model().set_border_set_patterns(border_set_id, pattern_ids_after);
  }

private:

  QString border_set_id;
  QStringList pattern_ids_before;
  QStringList pattern_ids_after;
};

/**
 * @brief Deleting border sets.
 */
class DeleteBorderSetsCommand : public TilesetEditorCommand {

public:

  DeleteBorderSetsCommand(TilesetEditor& editor, const QStringList& border_set_ids) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Delete contour")) {

    for (const QString& border_set_id : border_set_ids) {
      BorderSet border_set;
      border_set.id = border_set_id;
      border_set.pattern_ids = get_model().get_border_set_patterns(border_set_id);
      border_set.inner = get_model().is_border_set_inner(border_set_id);
      border_sets << border_set;
    }
  }

  virtual void undo() override {

    for (const BorderSet& border_set : std::as_const(border_sets)) {
      get_model().create_border_set(border_set.id);
      get_model().set_border_set_patterns(border_set.id, border_set.pattern_ids);
      get_model().set_border_set_inner(border_set.id, border_set.inner);
    }
  }

  virtual void redo() override {

    for (const BorderSet& border_set : std::as_const(border_sets)) {
      get_model().delete_border_set(border_set.id);
    }
  }

private:

  struct BorderSet {
    QString id;
    QStringList pattern_ids;
    bool inner;
  };

  QList<BorderSet> border_sets;
};

/**
 * @brief Creating a border set.
 */
class CreateBorderSetCommand : public TilesetEditorCommand {

public:

  CreateBorderSetCommand(
      TilesetEditor& editor,
      const QString& border_set_id,
      const QStringList& pattern_ids) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Create contour")),
    border_set_id(border_set_id),
    pattern_ids(pattern_ids) {
  }

  virtual void undo() override {
    get_model().delete_border_set(border_set_id);
  }

  virtual void redo() override {
    get_model().create_border_set(border_set_id);
    get_model().set_border_set_patterns(border_set_id, pattern_ids);
  }

private:

  QString border_set_id;
  QStringList pattern_ids;
};

/**
 * @brief Deleting patterns in border sets.
 */
class DeleteBorderSetPatternsCommand : public TilesetEditorCommand {

public:

  DeleteBorderSetPatternsCommand(TilesetEditor& editor, const QList<QPair<QString, BorderKind>>& patterns) :
    TilesetEditorCommand(editor, TilesetEditor::tr("Delete contour pattern")),
    patterns_deleted(patterns) {

    for (const QPair<QString, BorderKind>& pattern : std::as_const(patterns_deleted)) {
      pattern_ids_before << get_model().get_border_set_pattern(pattern.first, pattern.second);
    }
  }

  virtual void undo() override {

    int i = 0;
    for (const QPair<QString, BorderKind>& pattern : std::as_const(patterns_deleted)) {
      get_model().set_border_set_pattern(pattern.first, pattern.second, pattern_ids_before[i]);
      ++i;
    }
  }

  virtual void redo() override {

    for (const QPair<QString, BorderKind>& pattern : std::as_const(patterns_deleted)) {
      get_model().set_border_set_pattern(pattern.first, pattern.second, "");
    }
  }

private:

  QList<QPair<QString, BorderKind>> patterns_deleted;
  QStringList pattern_ids_before;
};

}  // Anonymous namespace.

/**
 * @brief Creates a tileset editor.
 * @param quest The quest containing the file.
 * @param path Path of the tileset data file to open.
 * @param parent The parent object or nullptr.
 * @throws EditorException If the file could not be opened.
 */
TilesetEditor::TilesetEditor(Quest& quest, const QString& path, QWidget* parent) :
  Editor(quest, path, parent),
  model(nullptr) {

  ui.setupUi(this);

  // Get the tileset.
  ResourceType resource_type;
  QString tileset_id;
  quest.check_exists(path);
  if (!quest.is_resource_element(path, resource_type, tileset_id) ||
      resource_type != ResourceType::TILESET) {
    throw EditorException(tr("File '%1' is not a tileset").arg(path));
  }
  this->tileset_id = tileset_id;

  // Editor properties.
  set_title(tr("Tileset %1").arg(get_file_name_without_extension()));
  set_icon(QIcon(":/images/icon_resource_tileset.svg"));
  set_close_confirm_message(
        tr("Tileset '%1' has been modified. Save changes?").arg(tileset_id));
  set_select_all_supported(true);
  set_zoom_supported(true);
  set_grid_supported(true);

  // Open the file.
  model = new TilesetModel(quest, tileset_id, this);
  model->set_auto_refresh_data_file(false);
  get_undo_stack().setClean();

  // Prepare the gui.
  EditorSettings settings;
  const int side_width = settings.get_value_int(EditorSettings::tileset_side_width);
  ui.splitter->setSizes({ side_width, width() - side_width });
  ui.splitter->setStretchFactor(0, 0);  // Don't expand the left panel
  ui.splitter->setStretchFactor(1, 1);  // but only the tileset view.

  ui.patterns_list_view->set_model(*model);
  ui.border_sets_tree_view->set_tileset(*model);
  ui.tileset_view->set_tileset(model);
  ui.tileset_view->set_view_settings(get_view_settings());

  load_settings();
  update();

  // Make connections.
  connect(&get_database(), &QuestDatabase::element_description_changed,
          this, &TilesetEditor::update_description_to_gui);
  connect(ui.description_field, &QLineEdit::editingFinished,
          this, &TilesetEditor::set_description_from_gui);

  connect(ui.background_field, &ColorChooser::color_changed,
          this, &TilesetEditor::change_background_color);
  connect(model, &TilesetModel::background_color_changed,
          this, &TilesetEditor::update_background_color);

  connect(ui.pattern_id_button, &QToolButton::clicked,
          this, &TilesetEditor::change_selected_pattern_id_requested);
  connect(ui.tileset_view, &TilesetView::change_selected_pattern_id_requested,
          this, &TilesetEditor::change_selected_pattern_id_requested);
  connect(ui.patterns_list_view, &TilePatternsListView::change_selected_pattern_id_requested,
          this, &TilesetEditor::change_selected_pattern_id_requested);
  connect(model, &TilesetModel::pattern_id_changed,
          this, &TilesetEditor::update_pattern_id_field);

  connect(ui.tileset_view, &TilesetView::change_selected_patterns_position_requested,
          this, &TilesetEditor::change_selected_patterns_position_requested);  
  connect(ui.tileset_view, &TilesetView::resize_selected_pattern_requested,
          this, &TilesetEditor::resize_selected_pattern_requested);

  connect(ui.ground_field, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
          this, &TilesetEditor::ground_selector_activated);
  connect(ui.tileset_view, &TilesetView::change_selected_patterns_ground_requested,
          this, &TilesetEditor::change_selected_patterns_ground_requested);
  connect(model, &TilesetModel::pattern_ground_changed,
          this, &TilesetEditor::update_ground_field);

  connect(ui.default_layer_field,static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          this, &TilesetEditor::change_selected_patterns_default_layer_requested);
  connect(ui.tileset_view, &TilesetView::change_selected_patterns_default_layer_requested,
          this, &TilesetEditor::change_selected_patterns_default_layer_requested);
  connect(model, &TilesetModel::pattern_default_layer_changed,
          this, &TilesetEditor::update_default_layer_field);

  connect(ui.repeat_mode_field, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
          this, &TilesetEditor::repeat_mode_selector_activated);
  connect(ui.tileset_view, &TilesetView::change_selected_patterns_repeat_mode_requested,
          this, &TilesetEditor::change_selected_patterns_repeat_mode_requested);
  connect(model, &TilesetModel::pattern_repeat_mode_changed,
          this, &TilesetEditor::update_repeat_mode_field);

  connect(ui.scrolling_field, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
          this, &TilesetEditor::scrolling_selector_activated);
  connect(ui.tileset_view, &TilesetView::change_selected_patterns_scrolling_requested,
          this, &TilesetEditor::change_selected_patterns_scrolling_requested);
  connect(model, &TilesetModel::pattern_scrolling_changed,
          this, &TilesetEditor::update_scrolling_field);

  connect(ui.num_frames_field, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          this, &TilesetEditor::change_selected_patterns_num_frames_requested);
  connect(model, &TilesetModel::pattern_num_frames_changed,
          this, &TilesetEditor::update_num_frames_field);

  connect(ui.frame_delay_field, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
          this, &TilesetEditor::change_selected_patterns_frame_delay_requested);
  connect(model, &TilesetModel::pattern_frame_delay_changed,
          this, &TilesetEditor::update_frame_delay_field);

  connect(ui.mirror_loop_field, &QCheckBox::clicked,
          this, &TilesetEditor::change_selected_patterns_mirror_loop_requested);
  connect(model, &TilesetModel::pattern_mirror_loop_changed,
          this, &TilesetEditor::update_mirror_loop_field);

  connect(ui.animation_separation_field, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
          this, &TilesetEditor::animation_separation_selector_activated);
  connect(model, &TilesetModel::pattern_separation_changed,
          this, &TilesetEditor::update_animation_separation_field);

  connect(ui.tileset_view, &TilesetView::create_pattern_requested,
          this, &TilesetEditor::create_pattern_requested);
  connect(ui.tileset_view, &TilesetView::duplicate_selected_patterns_requested,
          this, &TilesetEditor::duplicate_selected_patterns_requested);
  connect(ui.tileset_view, &TilesetView::create_border_set_requested,
          this, &TilesetEditor::create_border_set_requested);

  connect(ui.patterns_list_view, &TilePatternsListView::delete_selected_patterns_requested,
          this, &TilesetEditor::delete_selected_patterns_requested);
  connect(ui.tileset_view, &TilesetView::delete_selected_patterns_requested,
          this, &TilesetEditor::delete_selected_patterns_requested);

  connect(ui.delete_border_set_button, &QPushButton::clicked,
          this, &TilesetEditor::delete_border_set_selection_requested);
  connect(ui.border_sets_tree_view, &BorderSetTreeView::delete_border_sets_requested,
          this, &TilesetEditor::delete_border_sets_requested);
  connect(ui.border_sets_tree_view, &BorderSetTreeView::delete_border_set_patterns_requested,
          this, &TilesetEditor::delete_border_set_patterns_requested);
  connect(ui.create_border_set_button, &QPushButton::clicked,
          this, [this]() {
    create_border_set_requested({});
  });
  connect(ui.border_sets_tree_view, &BorderSetTreeView::change_border_set_patterns_requested,
          this, &TilesetEditor::change_border_set_patterns_requested);
  connect(&model->get_selection_model(), &QItemSelectionModel::selectionChanged,
          this, &TilesetEditor::update_pattern_view);

  connect(ui.rename_border_set_button, &QPushButton::clicked,
          this, &TilesetEditor::change_selected_border_set_id_requested);
  connect(ui.border_set_id_button, &QToolButton::clicked,
          this, &TilesetEditor::change_selected_border_set_id_requested);
  connect(model, &TilesetModel::border_set_id_changed,
          this, &TilesetEditor::update_border_set_id_field);
  connect(ui.border_set_inner_field, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated),
          this, &TilesetEditor::border_set_inner_selector_activated);
  connect(model, &TilesetModel::border_set_inner_changed,
          this, &TilesetEditor::update_border_set_inner_field);
  connect(ui.border_sets_tree_view->selectionModel(), &QItemSelectionModel::selectionChanged,
          this, &TilesetEditor::update_border_set_view);

  connect(model, &TilesetModel::tileset_data_file_changed,
          this, &TilesetEditor::tileset_data_file_changed);

  connect(ui.splitter, &QSplitter::splitterMoved,
          this, &TilesetEditor::side_panel_resized);

  // Qlementine-related stuff.
  for (auto* widget : std::vector<QWidget*>{
           ui.create_border_set_button,
           ui.delete_border_set_button,
           ui.border_set_id_button,
           ui.rename_border_set_button,
           ui.pattern_id_button,
           ui.border_set_inner_field,
           ui.animation_separation_field,
           ui.patterns_border_sets_tab_widget->tabBar(),
           ui.repeat_mode_field,
       }) {
    EditorStyle::setAutoIconColor(widget, EditorStyle::AutoIconColor::ForegroundColor);
  }
}

/**
 * @brief Returns the tileset model being edited.
 * @return The tileset model.
 */
TilesetModel& TilesetEditor::get_model() {
  return *model;
}

/**
 * @copydoc Editor::save
 */
void TilesetEditor::save() {

  if (model == nullptr) {
    return;
  }
  model->save();
}

/**
 * @brief Called when the tileset data file has changed on disk.
 *
 * Reloads the tileset unless there are unsaved changes.
 */
void TilesetEditor::tileset_data_file_changed() {

  if (model == nullptr) {
    return;
  }
  if (has_unsaved_changes()) {
    return;
  }
  try {
    model->load();
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @copydoc Editor::select_all
 */
void TilesetEditor::select_all() {

  if (ui.tileset_view == nullptr) {
    return;
  }
  ui.tileset_view->select_all();
}

/**
 * @copydoc Editor::unselect_all
 */
void TilesetEditor::unselect_all() {

  if (ui.tileset_view == nullptr) {
    return;
  }
  ui.tileset_view->unselect_all();
}

/**
 * @copydoc Editor::reload_settings
 */
void TilesetEditor::reload_settings() {

  EditorSettings settings;

  get_view_settings().set_grid_style(static_cast<GridStyle>(
    settings.get_value_int(EditorSettings::tileset_grid_style)));
  get_view_settings().set_grid_color(
    settings.get_value_color(EditorSettings::tileset_grid_color));
}

/**
 * @brief Updates everything in the gui.
 */
void TilesetEditor::update() {

  update_tileset_id_field();
  update_description_to_gui();
  update_background_color();
  update_pattern_view();
  update_border_set_view();
}

/**
 * @brief Updates the tileset id displaying.
 */
void TilesetEditor::update_tileset_id_field() {

  ui.tileset_id_field->setText(tileset_id);
}

/**
 * @brief Updates the background color button from the one of the tileset.
 */
void TilesetEditor::update_background_color() {

  ui.background_field->set_color(model->get_background_color());
}

/**
 * @brief Updates the content of the tileset description text edit.
 */
void TilesetEditor::update_description_to_gui() {

  QString description = get_database().get_description(ResourceType::TILESET, tileset_id);
  if (ui.description_field->text() != description) {
    ui.description_field->setText(description);
  }
}

/**
 * @brief Slot called when the user changes the background color.
 */
void TilesetEditor::change_background_color() {

  QColor old_color = model->get_background_color();
  QColor new_color = ui.background_field->get_color();

  if (new_color == old_color) {
    // No change.
    return;
  }

  try_command(new SetBackgroundCommand(*this, new_color));
}

/**
 * @brief Modifies the tileset description in the quest resource list with
 * the new text entered by the user.
 *
 * If the new description is invalid, an error dialog is shown.
 */
void TilesetEditor::set_description_from_gui() {

  QString description = ui.description_field->text();
  if (description == get_database().get_description(ResourceType::TILESET, tileset_id)) {
    return;
  }

  if (description.isEmpty()) {
    GuiTools::error_dialog(tr("Invalid description"));
    update_description_to_gui();
    return;
  }

  const bool was_blocked = blockSignals(true);
  try {
    get_database().set_description(ResourceType::TILESET, tileset_id, description);
    get_database().save();
  }
  catch (const EditorException& ex) {
    ex.print_message();
  }
  update_description_to_gui();
  blockSignals(was_blocked);
}

/**
 * @brief Fills the tile pattern view.
 */
void TilesetEditor::update_pattern_view() {

  update_pattern_id_field();
  update_ground_field();
  update_default_layer_field();
  update_repeat_mode_field();
  update_scrolling_field();
  update_num_frames_field();
  update_animation_separation_field();
  update_frame_delay_field();
  update_mirror_loop_field();

  // If no pattern is selected, disable the tile pattern view.
  ui.pattern_properties_group_box->setEnabled(!model->is_selection_empty());
}

/**
 * @brief Slot called when the user wants to move tile pattern(s).
 */
void TilesetEditor::change_selected_patterns_position_requested(const QPoint& delta) {

  QList<int> indexes = model->get_selected_indexes();
  if (indexes.empty()) {
    // No pattern selected.
    return;
  } else if (indexes.length() == 1) {
    int index = indexes.first();
    QPoint position = model->get_pattern_frame(index).topLeft() + delta;
    try_command(new SetPatternPositionCommand(*this, index, position));
  } else {
    try_command(new SetPatternsPositionCommand(*this, indexes, delta));
  }
}

/**
 * @brief Slot called when the user wants to resize a tile pattern.
 */
void TilesetEditor::resize_selected_pattern_requested(const QRect& box) {

  int index = model->get_selected_index();
  if (index == -1) {
    // No pattern or multiple patterns selected.
    return;
  }
  try_command(new ResizePatternCommand(*this, index, box));
}

/**
 * @brief Updates the pattern id field from the model.
 */
void TilesetEditor::update_pattern_id_field() {

  // Get the id of the selected pattern
  // (an empty string if no pattern is selected or if multiple patterns are).
  QString pattern_id = model->index_to_id(model->get_selected_index());
  ui.pattern_id_value->setText(pattern_id);

  bool enable = !pattern_id.isEmpty();
  ui.pattern_id_label->setEnabled(enable);
  ui.pattern_id_field->setEnabled(enable);
}

/**
 * @brief Slot called when the user wants to change the id of the selected
 * pattern.
 */
void TilesetEditor::change_selected_pattern_id_requested() {

  int old_index = model->get_selected_index();
  if (old_index == -1) {
    // No pattern selected or several patterns selected.
    return;
  }

  QString old_id = model->index_to_id(old_index);
  InputDialogWithCheckBox dialog(
        tr("Rename tile pattern"),
        tr("New pattern id:"),
        tr("Update existing maps using this pattern"),
        old_id,
        this);
  dialog.set_check_box_setting(EditorSettings::tileset_refactoring_change_pattern_id);
  int result = dialog.exec();

  if (result != QDialog::Accepted) {
    return;
  }

  QString new_id = dialog.get_value();
  if (new_id == old_id) {
    // No change.
    return;
  }

  if (!TilesetModel::is_valid_pattern_id(new_id)) {
    GuiTools::error_dialog("Invalid tile pattern id");
    return;
  }

  if (!dialog.is_checked()) {
    // The change is only in the tileset file.
    try_command(new SetPatternIdCommand(*this, old_index, new_id));
  }
  else {
    // Also update references in existing maps
    // (not as an undoable command).
    Refactoring refactoring([this, old_index, old_id, new_id]() {

      // Do the change in the tileset.
      model->set_pattern_id(old_index, new_id);

      // Save the tileset and clear the undo history.
      save();
      get_undo_stack().clear();

      // Update all maps that use this tileset.
      return change_pattern_id_in_maps(old_id, new_id);
    });
    emit refactoring_requested(refactoring);
  }
}

/**
 * @brief Replaces a pattern id by a new value in all maps that use this tileset.
 * @param old_pattern_id The pattern id to change.
 * @param new_pattern_id The new value.
 * @return The list of files that were modified.
 * @throws EditorException In case of error.
 */
QStringList TilesetEditor::change_pattern_id_in_maps(
    const QString& old_pattern_id, const QString& new_pattern_id) {

  QStringList modified_paths;
  const QStringList& map_ids = get_database().get_elements(ResourceType::MAP);
  for (const QString& map_id : map_ids) {
    if (change_pattern_id_in_map(map_id, model->get_tileset_id(), old_pattern_id, new_pattern_id)) {
      modified_paths << get_quest().get_map_data_file_path(map_id);
    }
  }
  return modified_paths;
}

/**
 * @brief Replaces a pattern id by a new value in a map if it uses this tileset.
 * @param map_id Id of the map to update.
 * @param tileset_id Id of the tileset of the pattern being changed.
 * @param old_pattern_id The pattern id to change.
 * @param new_pattern_id The new value.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool TilesetEditor::change_pattern_id_in_map(
    const QString& map_id,
    const QString& tileset_id,
    const QString& old_pattern_id,
    const QString& new_pattern_id) {

  // Load the map.
  QString file_name = get_quest().get_map_data_file_path(map_id);
  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    throw EditorException(tr("Cannot open map file '%1'").arg(file_name));
  }
  QTextStream in(&file);
  in.setEncoding(QStringConverter::Utf8);
  QString old_content = in.readAll();
  file.close();
  Solarus::MapData map;
  if (!map.import_from_buffer(old_content.toStdString(), file_name.toStdString())) {
    throw EditorException(tr("Invalid map file: '%1'").arg(file_name));
  }

  // Change the pattern id.
  for (int layer = map.get_min_layer(); layer <= map.get_max_layer(); ++layer) {
    for (int i = 0; i < map.get_num_entities(layer); ++i) {
      Solarus::EntityData& entity = map.get_entity({ layer, i });
      if (entity.get_type() == EntityType::TILE ||
          entity.get_type() == EntityType::DYNAMIC_TILE) {
        std::string tile_pattern_id = entity.get_specific_property("pattern").string_value;
        std::string tile_tileset_id = entity.get_specific_property("tileset").string_value;
        if (tile_tileset_id.empty()) {
          tile_tileset_id = map.get_tileset_id();
        }
        if (tile_tileset_id == tileset_id.toStdString() &&
            tile_pattern_id == old_pattern_id.toStdString()) {
          entity.set_string("pattern", new_pattern_id.toStdString());
        }
      }
    }
  }

  // Save the map.
  std::string buffer;
  if (!map.export_to_buffer(buffer)) {
    throw EditorException(tr("Failed to export map after changing pattern id: '%1'").arg(file_name));
  }
  QString content = QString::fromStdString(buffer);
  if (content == old_content) {
    // No change.
    return false;
  }
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    throw EditorException(tr("Cannot open map file '%1' for writing").arg(file.fileName()));
  }
  QTextStream out(&file);
  out.setEncoding(QStringConverter::Utf8);
  out << content;
  file.close();
  return true;
}

/**
 * @brief Slot called when the user wants to change a pattern in border set.
 * @param border_set_id Id of the border set to change.
 * @param pattern_ids New pattern id to set.
 */
void TilesetEditor::change_border_set_patterns_requested(
    const QString& border_set_id,
    const QStringList& pattern_ids
) {

  try_command(new SetBorderSetPatternsCommand(*this, border_set_id, pattern_ids));
}

/**
 * @brief Updates the ground selector from the model.
 */
void TilesetEditor::update_ground_field() {

  Ground ground = Ground::EMPTY;
  bool enable = model->is_common_pattern_ground(
        model->get_selected_indexes(), ground);

  ui.ground_label->setEnabled(enable);
  ui.ground_field->setEnabled(enable);

  if (enable) {
    ui.ground_field->set_selected_value(ground);
  }
}

/**
 * @brief Slot called when the user changes the ground in the selector.
 */
void TilesetEditor::ground_selector_activated() {

  if (model->is_selection_empty()) {
    return;
  }

  QList<int> indexes = model->get_selected_indexes();
  Ground new_ground = ui.ground_field->get_selected_value();
  Ground old_common_ground;
  if (model->is_common_pattern_ground(indexes, old_common_ground) &&
      new_ground == old_common_ground) {
    // No change.
    return;
  }

  try_command(new SetPatternsGroundCommand(*this, indexes, new_ground));
}

/**
 * @brief Slot called when the user changes the ground of selected patterns.
 * @param ground The new ground.
 */
void TilesetEditor::change_selected_patterns_ground_requested(Ground ground) {

  if (model->is_selection_empty()) {
    return;
  }

  try_command(new SetPatternsGroundCommand(*this, model->get_selected_indexes(), ground));
}

/**
 * @brief Updates the default layer selector from the model.
 */
void TilesetEditor::update_default_layer_field() {

  int default_layer = 0;
  bool enable = model->is_common_pattern_default_layer(
      model->get_selected_indexes(), default_layer);

  ui.default_layer_label->setEnabled(enable);
  ui.default_layer_field->setEnabled(enable);

  if (enable) {
    const bool was_blocked = ui.default_layer_field->signalsBlocked();
    ui.default_layer_field->blockSignals(true);
    ui.default_layer_field->setValue(default_layer);
    ui.default_layer_field->blockSignals(was_blocked);
  }
}

/**
 * @brief Slot called when the user changes the default layer of selected patterns.
 * @param default_layer The new default layer.
 */
void TilesetEditor::change_selected_patterns_default_layer_requested(int default_layer) {

  if (model->is_selection_empty()) {
    return;
  }

  try_command(new SetPatternsDefaultLayerCommand(*this, model->get_selected_indexes(), default_layer));
}

/**
 * @brief Updates the repeat mode selector from the model.
 */
void TilesetEditor::update_repeat_mode_field() {

  PatternRepeatMode repeat_mode = PatternRepeatMode::ALL;
  bool enable = model->is_common_pattern_repeat_mode(
      model->get_selected_indexes(), repeat_mode);

  ui.repeat_mode_label->setEnabled(enable);
  ui.repeat_mode_field->setEnabled(enable);

  if (enable) {
    ui.repeat_mode_field->set_selected_value(repeat_mode);
  }
}

/**
 * @brief Slot called when the user changes the repeat mode in the selector.
 */
void TilesetEditor::repeat_mode_selector_activated() {

  if (model->is_selection_empty()) {
    return;
  }

  QList<int> indexes = model->get_selected_indexes();
  PatternRepeatMode new_repeat_mode = ui.repeat_mode_field->get_selected_value();
  PatternRepeatMode old_common_repeat_mode;
  if (model->is_common_pattern_repeat_mode(indexes, old_common_repeat_mode) &&
      new_repeat_mode == old_common_repeat_mode) {
    // No change.
    return;
  }

  try_command(new SetPatternsRepeatModeCommand(*this, indexes, new_repeat_mode));
}

/**
 * @brief Slot called when the user changes the repeat mode of selected patterns.
 * @param repeat_mode The new repeat mode.
 */
void TilesetEditor::change_selected_patterns_repeat_mode_requested(PatternRepeatMode repeat_mode) {

  if (model->is_selection_empty()) {
    return;
  }

  try_command(new SetPatternsRepeatModeCommand(*this, model->get_selected_indexes(), repeat_mode));
}

/**
 * @brief Updates the scrolling selector from the model.
 */
void TilesetEditor::update_scrolling_field() {

  PatternScrolling scrolling = PatternScrolling::NONE;
  bool enable = model->is_common_pattern_scrolling(
        model->get_selected_indexes(), scrolling);

  ui.scrolling_label->setEnabled(enable);
  ui.scrolling_field->setEnabled(enable);

  if (enable) {
    ui.scrolling_field->set_selected_value(scrolling);
  }
}

/**
 * @brief Slot called when the user changes the scrolling in the selector.
 */
void TilesetEditor::scrolling_selector_activated() {

  if (model->is_selection_empty()) {
    return;
  }

  QList<int> indexes = model->get_selected_indexes();
  PatternScrolling new_scrolling = ui.scrolling_field->get_selected_value();
  PatternScrolling old_common_scrolling;
  if (model->is_common_pattern_scrolling(indexes, old_common_scrolling) &&
      new_scrolling == old_common_scrolling) {
    // No change.
    return;
  }

  if (!try_command(new SetPatternsScrollingCommand(*this, indexes, new_scrolling))) {
    // In case of failure, restore the selector.
    update_scrolling_field();
  }
}

/**
 * @brief Slot called when the user changes the scrolling of selected patterns.
 * @param scrolling The new scrolling.
 */
void TilesetEditor::change_selected_patterns_scrolling_requested(PatternScrolling scrolling) {

  if (model->is_selection_empty()) {
    return;
  }

  try_command(new SetPatternsScrollingCommand(*this, model->get_selected_indexes(), scrolling));
}

/**
 * @brief Updates the number of frames selector from the model.
 */
void TilesetEditor::update_num_frames_field() {

  int num_frames = 0;
  bool enable = model->is_common_pattern_num_frames(
      model->get_selected_indexes(), num_frames);

  ui.num_frames_label->setEnabled(enable);
  ui.num_frames_field->setEnabled(enable);

  if (enable) {
    const bool was_blocked = ui.num_frames_field->signalsBlocked();
    ui.num_frames_field->blockSignals(true);
    ui.num_frames_field->setValue(num_frames);
    ui.num_frames_field->blockSignals(was_blocked);
  }

  // Enable or disable fields depending on whether
  // this pattern has multiple frames.
  update_animation_separation_field();
  update_frame_delay_field();
  update_mirror_loop_field();
}

/**
 * @brief Slot called when the user changes the number of frames of selected patterns.
 * @param num_frames The new number of frames.
 */
void TilesetEditor::change_selected_patterns_num_frames_requested(int num_frames) {

  if (model->is_selection_empty()) {
    return;
  }

  // TODO check that we don't overlap existing patterns

  try_command(new SetPatternsNumFramesCommand(*this, model->get_selected_indexes(), num_frames));
}

/**
 * @brief Updates the animation separation selector from the model.
 */
void TilesetEditor::update_animation_separation_field() {

  const QList<int>& indexes = model->get_selected_indexes();
  bool multi_frame = model->are_patterns_multi_frame(indexes);

  PatternSeparation separation = PatternSeparation::HORIZONTAL;
  bool enable = multi_frame && model->is_common_pattern_separation(indexes, separation);

  ui.animation_separation_field->setEnabled(enable);

  if (enable) {
    ui.animation_separation_field->set_selected_value(separation);
  }
}

/**
 * @brief Slot called when the user changes the animation separation in the selector.
 */
void TilesetEditor::animation_separation_selector_activated() {

  if (model->is_selection_empty()) {
    return;
  }

  QList<int> indexes = model->get_selected_indexes();
  PatternSeparation new_separation = ui.animation_separation_field->get_selected_value();
  PatternSeparation old_common_separation;
  if (model->is_common_pattern_separation(indexes, old_common_separation) &&
      new_separation == old_common_separation) {
    // No change.
    return;
  }

  change_selected_patterns_separation_requested(new_separation);
}

/**
 * @brief Slot called when the user changes the separation of selected patterns.
 * @param separation The new separation.
 */
void TilesetEditor::change_selected_patterns_separation_requested(PatternSeparation separation) {

  if (model->is_selection_empty()) {
    return;
  }

  // TODO check that we don't overlap existing patterns

  if (!try_command(new SetPatternsSeparationCommand(*this, model->get_selected_indexes(), separation))) {
    // In case of failure, restore the selector.
    update_animation_separation_field();
  }
}

/**
 * @brief Updates the frame delay selector from the model.
 */
void TilesetEditor::update_frame_delay_field() {

  int frame_delay = 0;
  bool enable = model->is_common_pattern_frame_delay(
      model->get_selected_indexes(), frame_delay);

  ui.frame_delay_label->setEnabled(enable);
  ui.frame_delay_field->setEnabled(enable);

  if (enable) {
    const bool was_blocked = ui.frame_delay_field->signalsBlocked();
    ui.frame_delay_field->blockSignals(true);
    ui.frame_delay_field->setValue(frame_delay);
    ui.frame_delay_field->blockSignals(was_blocked);
  }
}

/**
 * @brief Slot called when the user changes the frame delay of selected patterns.
 * @param frame_delay The new frame delay.
 */
void TilesetEditor::change_selected_patterns_frame_delay_requested(int frame_delay) {

  if (model->is_selection_empty()) {
    return;
  }

  try_command(new SetPatternsFrameDelayCommand(*this, model->get_selected_indexes(), frame_delay));
}

/**
 * @brief Updates the mirror loop check box from the model.
 */
void TilesetEditor::update_mirror_loop_field() {

  bool mirror_loop = false;
  bool enable = model->is_common_pattern_mirror_loop(
      model->get_selected_indexes(), mirror_loop);

  ui.mirror_loop_field->setEnabled(enable);

  if (enable) {
    ui.mirror_loop_field->setChecked(mirror_loop);
  }
}

/**
 * @brief Slot called when the user changes the mirror loop property of selected patterns.
 * @param mirror_loop The new mirror loop value.
 */
void TilesetEditor::change_selected_patterns_mirror_loop_requested(int mirror_loop) {

  if (model->is_selection_empty()) {
    return;
  }

  try_command(new SetPatternsMirrorLoopCommand(*this, model->get_selected_indexes(), mirror_loop));
}

/**
 * @brief Slot called when the user wants to create a pattern.
 * @param paterrn_id Id of the pattern to create.
 * @param frame Position of the frame in the tileset image
 * (it will be a single-frame pattern).
 * @param ground Ground to set.
 */
void TilesetEditor::create_pattern_requested(
    const QString& pattern_id, const QRect& frame, Ground ground) {

  try_command(new CreatePatternCommand(*this, pattern_id, frame, ground));
}

/**
 * @brief Slot called when the user wants to duplicate the selected tile patterns.
 * @param delta Translation to apply on duplicate tile patterns.
 */
void TilesetEditor::duplicate_selected_patterns_requested(const QPoint& delta) {

  QList<int> indexes = model->get_selected_indexes();
  if (indexes.empty()) {
    // No pattern selected.
    return;
  }

  try_command(new DuplicatePatternsCommand(*this, indexes, delta));
}

/**
 * @brief Slot called when the user wants to delete the selected patterns.
 */
void TilesetEditor::delete_selected_patterns_requested() {

  QList<int> indexes = model->get_selected_indexes();

  if (indexes.empty()) {
    return;
  }

  QString question_text;
  if (indexes.size() == 1) {
    QString pattern_id = model->index_to_id(indexes.first());
    question_text = tr("Do you really want to delete pattern '%1'?").
        arg(pattern_id);
  }
  else {
    question_text = tr("Do you really want to delete these %1 patterns?").
        arg(indexes.size());
  }

  QMessageBox::StandardButton answer = QMessageBox::question(
        this,
        tr("Delete confirmation"),
        question_text,
        QMessageBox::Yes | QMessageBox::No);

  if (answer != QMessageBox::Yes) {
    return;
  }

  try_command(new DeletePatternsCommand(*this, indexes));
}

/**
 * @brief Fills the border set view.
 *
 * If a border set is selected, its properties are displayed in the tile
 * pattern view.
 * Otherwise, the border set view becomes disabled.
 */
void TilesetEditor::update_border_set_view() {

  update_border_set_id_field();
  update_border_set_inner_field();

  // If no border set is selected, disable the border set view.
  const QString& border_set_id = ui.border_sets_tree_view->get_selected_border_set_id();
  ui.border_set_properties_group_box->setEnabled(!border_set_id.isEmpty());
}

/**
 * @brief Slot called when the user wants to create a border set.
 * @param pattern_ids Patterns to create the border set from (can be empty).
 */
void TilesetEditor::create_border_set_requested(const QStringList& pattern_ids) {

  bool ok = false;
  QString border_set_id = QInputDialog::getText(
        this,
        tr("Border set name"),
        tr("Border set name:"),
        QLineEdit::Normal,
        "",
        &ok);

  if (!ok) {
    return;
  }

  if (try_command(new CreateBorderSetCommand(*this, border_set_id, pattern_ids))) {
    ui.patterns_border_sets_tab_widget->setCurrentWidget(ui.border_sets_view);
  }
}

/**
 * @brief Slot called when the user wants to delete something in the border
 * set editor.
 */
void TilesetEditor::delete_border_set_selection_requested() {

  ui.border_sets_tree_view->delete_border_set_selection_requested();
}

/**
 * @brief Slot called when the user wants to delete border sets.
 */
void TilesetEditor::delete_border_sets_requested(const QStringList& border_set_ids) {

  if (border_set_ids.isEmpty()) {
    return;
  }

  try_command(new DeleteBorderSetsCommand(*this, border_set_ids));
}

/**
 * @brief Slot called when the user wants to delete some patterns in border sets.
 * @param patterns The border patterns to delete and their border sets.
 */
void TilesetEditor::delete_border_set_patterns_requested(const QList<QPair<QString, BorderKind>>& patterns) {

  if (patterns.isEmpty()) {
    return;
  }

  try_command(new DeleteBorderSetPatternsCommand(*this, patterns));
}

/**
 * @brief Updates the border set id field from the model.
 */
void TilesetEditor::update_border_set_id_field() {

  // Get the id of the selected border set
  // (an empty string if no border set selected.
  QString border_set_id = ui.border_sets_tree_view->get_selected_border_set_id();
  ui.border_set_id_value->setText(border_set_id);

  bool enable = !border_set_id.isEmpty();
  ui.border_set_id_label->setEnabled(enable);
  ui.border_set_id_button->setEnabled(enable);
}

/**
 * @brief Slot called when the user wants to change the id of the selected
 * border set.
 */
void TilesetEditor::change_selected_border_set_id_requested() {

  const QString& old_id = ui.border_sets_tree_view->get_selected_border_set_id();
  if (old_id.isEmpty()) {
    // No border set selected.
    return;
  }

  ChangeBorderSetIdDialog dialog(old_id, this);
  int result = dialog.exec();

  if (result != QDialog::Accepted) {
    return;
  }

  QString new_id = dialog.get_border_set_id();
  if (new_id == old_id) {
    // No change.
    return;
  }

  try_command(new SetBorderSetIdCommand(*this, old_id, new_id));
}

/**
 * @brief Updates the border set inner setting from the model.
 */
void TilesetEditor::update_border_set_inner_field() {

  const QString& border_set_id = ui.border_sets_tree_view->get_selected_border_set_id();
  bool inner = false;
  if (!border_set_id.isEmpty()) {
    inner = model->is_border_set_inner(border_set_id);
  }

  ui.border_set_inner_field->setCurrentIndex(inner ? 1 : 0);
}

/**
 * @brief Slot called when the user changes the border set inner setting in the selector.
 */
void TilesetEditor::border_set_inner_selector_activated() {

  const QString& border_set_id = ui.border_sets_tree_view->get_selected_border_set_id();
  if (border_set_id.isEmpty()) {
    return;
  }

  bool old_inner = model->is_border_set_inner(border_set_id);
  bool new_inner = (ui.border_set_inner_field->currentIndex() == 1);

  if (new_inner == old_inner) {
    // No change.
    return;
  }

  try_command(new SetBorderSetInnerCommand(*this, border_set_id, new_inner));
}

/**
 * @brief Loads settings.
 */
void TilesetEditor::load_settings() {

  ViewSettings& view = get_view_settings();
  EditorSettings settings;

  view.set_zoom(settings.get_value_double(EditorSettings::tileset_zoom));
  view.set_grid_visible(
    settings.get_value_bool(EditorSettings::tileset_grid_show_at_opening));
  view.set_grid_size(
    settings.get_value_size(EditorSettings::tileset_grid_size));

  reload_settings();
}

/**
 * @brief Saves the tileset side panel splitter into settings.
 * @param pos the new splitter position
 * @param index the index of splitter that just moved
 */
void TilesetEditor::side_panel_resized(int pos, int index) {
  if (index == 1) {
    EditorSettings settings;
    settings.set_value(EditorSettings::tileset_side_width, pos);
  }
}

}
