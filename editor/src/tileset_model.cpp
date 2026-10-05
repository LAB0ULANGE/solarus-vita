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
#include "color.h"
#include "editor_exception.h"
#include "quest.h"
#include "rectangle.h"
#include "pattern_scrolling_traits.h"
#include "tileset_model.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QFile>
#include <QFileSystemWatcher>
#include <QIcon>
#include <QTimer>

namespace SolarusEditor {

using TilePatternData = Solarus::TilePatternData;

/**
 * @brief Creates a tileset model.
 * @param quest The quest.
 * @param tileset_id Id of the tileset to manage.
 * @param parent The parent object or nullptr.
 * @throws EditorException If the file could not be opened.
 */
TilesetModel::TilesetModel(
    Quest& quest,
    const QString& tileset_id,
    QObject* parent) :
  QAbstractListModel(parent),
  quest(quest),
  tileset_id(tileset_id),
  data_file_watcher(),
  image_file_watcher(),
  auto_refresh_data_file(true),
  selection_model(this) {

  Q_ASSERT(!tileset_id.isEmpty());

  data_file_watcher.addPath(quest.get_tileset_data_file_path(tileset_id));
  connect(&data_file_watcher, &QFileSystemWatcher::fileChanged,
          this, &TilesetModel::tileset_data_file_changing);

  image_file_watcher.addPath(quest.get_tileset_tiles_image_path(tileset_id));
  connect(&image_file_watcher, &QFileSystemWatcher::fileChanged,
          this, &TilesetModel::tileset_image_file_changing);

  load();
  reload_patterns_image();
}

/**
 * @brief Returns the quest.
 */
const Quest& TilesetModel::get_quest() const {
  return quest;
}

/**
 * @overload Non-const version.
 */
Quest& TilesetModel::get_quest() {
  return quest;
}

/**
 * @brief Returns the id of the tileset managed by this model.
 * @return The tileset id.
 */
QString TilesetModel::get_tileset_id() const {
  return tileset_id;
}

/**
 * @brief Returns whether to automatically refresh after changes on disk.
 * @return @c true if the tileset automatically refreshes.
 */
bool TilesetModel::get_auto_refresh_data_file() {
  return auto_refresh_data_file;
}

/**
 * @brief Sets whether to automatically refresh after changes on disk.
 *
 * Use the tileset_data_file_changed() signal if you want to do something
 * specific instead of automatically refreshing the model.
 *
 * @param auto_refresh_data_file @c true to automatically refresh.
 */
void TilesetModel::set_auto_refresh_data_file(bool auto_refresh_data_file) {
  this->auto_refresh_data_file = auto_refresh_data_file;
}

/**
 * @brief Loads the tileset from its data file.
 */
void TilesetModel::load() {

  QString path = quest.get_tileset_data_file_path(tileset_id);

  beginResetModel();
  if (!tileset.import_from_file(path.toLocal8Bit().toStdString())) {
    throw EditorException(tr("Cannot open tileset data file '%1'").arg(path));
  }

  build_index_map();
  patterns.clear();
  for (const auto& kvp : ids_to_indexes) {
    const QString& pattern_id = kvp.first;
    patterns.append(PatternModel(pattern_id));
  }

  endResetModel();
}

/**
 * @brief Saves the tileset to its data file.
 * @throws EditorException If the file could not be saved.
 */
void TilesetModel::save() const {

  QString path = quest.get_tileset_data_file_path(tileset_id);

  std::string new_data;
  tileset.export_to_buffer(new_data);

  // First read the existing file if it exists,
  // to check if there are actual changes.
  QFile file(path);
  if (file.open(QIODevice::ReadOnly)) {
      QCryptographicHash hasher(QCryptographicHash::Sha1);
      hasher.addData(&file);
      QByteArray old_file_hash = hasher.result();

      hasher.reset();
      hasher.addData(QByteArrayView(new_data.data(), new_data.size()));

      if (hasher.result() == old_file_hash) {
        // The saved version is already up-to-date: nothing to save.
        // Avoid unnecessary refreshes of the tileset.
        return;
      }
      file.close();
  }

  // Now write the file.
  file.open(QIODevice::WriteOnly);
  qint64 bytes = file.write(new_data.data(), new_data.size());
  if (bytes != static_cast<qint64>(new_data.size())) {
    throw EditorException(tr("Cannot save tileset data file '%1'").arg(path));
  }
}

/**
 * @brief Called when the tileset data file is being modified on the filesystem.
 */
void TilesetModel::tileset_data_file_changing() {

  // To avoid any risk of duplicate refreshes.
  QString path = quest.get_tileset_data_file_path(tileset_id);
  data_file_watcher.removePath(path);

  QTimer::singleShot(100, this, [this, path]() {
    if (auto_refresh_data_file) {
      try {
        load();
      }
      catch (const EditorException& ex) {
        qWarning() << tr("Failed to refresh tileset: %1").arg(ex.get_message());
      }
    }

    // Watch the path again because saving the tileset might have removed it
    // from the watcher.
    data_file_watcher.addPath(path);

    emit tileset_data_file_changed();
  });
}

/**
 * @brief Called when the tileset image file is being modified on the filesystem.
 */
void TilesetModel::tileset_image_file_changing() {

  // To avoid any risk of duplicate refreshes.
  QString path = quest.get_tileset_tiles_image_path(tileset_id);
  image_file_watcher.removePath(path);

  QTimer::singleShot(500, this, [this, path]() {

    // Watch the path again because saving the tileset might have removed it
    // from the watcher.
    image_file_watcher.addPath(path);

    reload_patterns_image();
  });
}

/**
 * @brief Returns the tileset's background color.
 * @return The background color.
 */
QColor TilesetModel::get_background_color() const {

  return Color::to_qcolor(tileset.get_background_color());
}

/**
 * @brief Sets the tileset's background color.
 *
 * Emits background_color_changed if there is a change.
 *
 * @param background_color The background color.
 */
void TilesetModel::set_background_color(const QColor& background_color) {

  const Solarus::Color solarus_color = Color::to_solarus_color(background_color);
  if (solarus_color == tileset.get_background_color()) {
    return;
  }
  tileset.set_background_color(solarus_color);
  emit background_color_changed(background_color);
}

/**
 * @brief Returns the number of items in the list.
 *
 * This is the number of patterns of the tileset.
 *
 * @return The number of patterns.
 */
int TilesetModel::rowCount(const QModelIndex& /* parent */) const {
  return tileset.get_num_patterns();
}

/**
 * @brief Returns the datat of an item for a given role.
 * @param index Index of the item to get.
 * @param role The wanted role.
 * @return The data.
 */
QVariant TilesetModel::data(const QModelIndex& index, int role) const {

  switch (role) {

  case Qt::DisplayRole:
    return QVariant();  // No text: only the icon.
    break;

  case Qt::DecorationRole:
    // Show an icon representing the pattern.
    return get_pattern_icon(index.row());
    break;

  default:
    break;
  }

  return QVariant();
}

/**
 * @brief Returns the number of patterns in the tileset.
 * @return The number of patterns.
 */
int TilesetModel::get_num_patterns() const {

  return tileset.get_num_patterns();
}

/**
 * @brief Returns whether there exists a pattern with the specified index.
 * @param index A pattern index.
 * @return @c true if such a pattern exists in the tileset.
 */
bool TilesetModel::pattern_exists(int index) const {

  return index >= 0 && index < patterns.size();
}

/**
 * @brief Returns whether there exists a pattern with the specified id.
 * @param pattern_id A pattern id.
 * @return @c true if such a pattern exists in the tileset.
 */
bool TilesetModel::pattern_exists(const QString& pattern_id) const {

  return id_to_index(pattern_id) != -1;
}

/**
 * @brief Returns the given id, possibly modified to make it unique.
 * @param pattern_id A candidate pattern id.
 * @return The same value if it does not exist yet,
 * or a modified value different from all patterns of this tileset.
 */
QString TilesetModel::get_unique_pattern_id(const QString& pattern_id) const {

  if (!pattern_exists(pattern_id)) {
    return pattern_id;
  }

  // Extract the prefix.
  int counter = 1;
  QString prefix = pattern_id;
  QStringList words = pattern_id.split('_');
  if (words.size() > 1) {
    QString last_word = words.last();
    bool is_int = false;
    counter = last_word.toInt(&is_int);
    if (is_int) {
      words.removeLast();
      prefix = words.join('_');
    }
  }

  // Add the first available integer as suffix.
  QString new_id;
  do {
    ++counter;
    new_id = QString("%1_%2").arg(prefix).arg(counter);
  } while (pattern_exists(new_id));

  return new_id;
}

/**
 * @brief Returns the list index of the specified pattern.
 * @param pattern_id Id of a tile pattern
 * @return The corresponding index in the list.
 * Returns -1 there is no pattern with this id.
 */
int TilesetModel::id_to_index(const QString& pattern_id) const {

  auto it = ids_to_indexes.find(pattern_id);
  if (it == ids_to_indexes.end()) {
    return -1;
  }
  return it->second;
}

/**
 * @brief Returns the string id of the pattern at the specified list index.
 * @param index An index in the list of patterns.
 * @return The corresponding pattern id.
 * Returns an empty string if there is no pattern at this index.
 */
QString TilesetModel::index_to_id(int index) const {

  if (!pattern_exists(index)) {
    return "";
  }

  return patterns.at(index).id;
}

/**
 * @brief Builds or rebuilds the internal mapping that gives indexes from ids.
 *
 * Tile patterns are indexed by string ids, but the model also treats them
 * as a linear list, so we need an additional integer index.
 */
void TilesetModel::build_index_map() {

  // boost::multi_index_container could do that for us, but this feels a bit
  // overkill to add a boost dependency just for this use case.

  ids_to_indexes.clear();

  // This is a bit tricky because we change the order of
  // the map from the Solarus library to use natural order instead.

  const std::map<std::string, TilePatternData>& pattern_map =
      tileset.get_patterns();
  // First, put the string keys to have natural sort.
  for (const auto& kvp : pattern_map) {
    QString pattern_id = QString::fromStdString(kvp.first);
    ids_to_indexes.insert(std::make_pair(pattern_id, 0));
  }

  // Then, we can put the integer value.
  int index = 0;
  for (const auto& kvp : ids_to_indexes) {
    QString pattern_id = kvp.first;
    ids_to_indexes[pattern_id] = index;
    ++index;
  }
}

/**
 * @brief Creates a new pattern in this tileset with default properties.
 *
 * The index of multiple patterns in the pattern list may change, since
 * patterns are sorted alphabetically.
 * Emits rowsAboutToBeInserted(), adds the new pattern
 * and then emits rowsInserted(), as required by QAbstractItemModel.
 *
 * Then, emits pattern_created().
 *
 * The newly created pattern is not initially selected.
 * The existing selection is preserved, though the index of many patterns can
 * change.
 * The selection is cleared before the operations and restored after,
 * updated with the new indexes.
 *
 * @param pattern_id Id of the pattern to create.
 * @param frame Position of the pattern in the tileset image
 * (it will be a single-frame pattern).
 * @return Index of the created pattern.
 * @throws EditorException in case of error.
 */
int TilesetModel::create_pattern(const QString& pattern_id, const QRect& frame) {

  // Make some checks first.
  if (!is_valid_pattern_id(pattern_id)) {
      throw EditorException(tr("Invalid tile pattern id: '%1'").arg(pattern_id));
  }

  if (id_to_index(pattern_id) != -1) {
      throw EditorException(tr("Tile pattern '%1' already exists").arg(pattern_id));
  }

  // Save and clear the selection since a lot of indexes may change.
  const QModelIndexList& old_selected_indexes = selection_model.selection().indexes();
  QStringList old_selection_ids;
  for (const QModelIndex& old_selected_index : old_selected_indexes) {
    old_selection_ids << index_to_id(old_selected_index.row());
  }
  clear_selection();

  // Add the pattern to the tileset file.
  TilePatternData pattern(Rectangle::to_solarus_rect(frame));
  tileset.add_pattern(pattern_id.toStdString(), pattern);

  // Rebuild indexes in the list model (indexes were shifted).
  build_index_map();
  int index = id_to_index(pattern_id);

  // Call beginInsertRows() as requested by QAbstractItemModel.
  beginInsertRows(QModelIndex(), index, index);

  // Update our pattern model list.
  patterns.insert(index, PatternModel(pattern_id));

  // Notify people before restoring the selection, so that they have a
  // chance to know new indexes before receiving selection signals.
  endInsertRows();
  emit pattern_created(index, pattern_id);

  // Restore the selection.
  for (const QString& selected_pattern_id : std::as_const(old_selection_ids)) {
    int new_index = id_to_index(selected_pattern_id);
    add_to_selected(new_index);
  }

  return index;
}

/**
 * @brief Deletes a tile pattern.
 *
 * The index of multiple patterns in the pattern list may change, since
 * patterns are sorted alphabetically.
 * Emits rowsAboutToBeRemoved(), removes the pattern
 * and then emits rowsRemoved(), as required by QAbstractItemModel.
 *
 * Then, emits pattern_deleted().
 *
 * Except for the deleted pattern, the existing selection is preserved,
 * though the index of many patterns can change.
 * The selection is cleared before the operations and restored after,
 * updated with the new indexes.
 *
 * If you have multiple patterns to delete, call delete_patterns()
 * for better performance.
 *
 * @param index Index of the pattern to delete.
 * @throws EditorException in case of error.
 */
void TilesetModel::delete_pattern(int index) {

  QString pattern_id = index_to_id(index);

  // Make some checks first.
  if (pattern_id.isEmpty()) {
      throw EditorException(tr("Invalid tile pattern index: %1").arg(index));
  }

  // Save and clear the selection since a lot of indexes may change.
  const QModelIndexList& old_selected_indexes = selection_model.selection().indexes();
  QStringList old_selection_ids;
  for (const QModelIndex& old_selected_index : old_selected_indexes) {
    old_selection_ids << index_to_id(old_selected_index.row());
  }
  clear_selection();

  // Delete the pattern in the tileset file.
  tileset.remove_pattern(pattern_id.toStdString());

  // Rebuild indexes in the list model (indexes were shifted).
  build_index_map();

  // Call beginRemoveRows() as requested by QAbstractItemModel.
  beginRemoveRows(QModelIndex(), index, index);

  // Update our pattern model list.
  patterns.removeAt(index);

  // Notify people before restoring the selection, so that they have a
  // chance to know new indexes before receiving selection signals.
  endRemoveRows();
  emit pattern_deleted(index, pattern_id);

  // Restore the selection.
  for (const QString& selected_pattern_id : std::as_const(old_selection_ids)) {

    if (selected_pattern_id == pattern_id) {
      // Exclude the deleted one.
      continue;
    }

    int new_index = id_to_index(selected_pattern_id);
    add_to_selected(new_index);
  }
}

/**
 * @brief Deletes some tile patterns.
 *
 * The index of multiple patterns in the pattern list may change, since
 * patterns are sorted alphabetically.
 * For each pattern, emits rowsAboutToBeRemoved(), removes the pattern,
 * emits rowsRemoved() as required by QAbstractItemModel,
 * and then, emits pattern_deleted().
 *
 * Except for the deleted patterns, the existing selection is preserved,
 * though the index of many patterns can change.
 * The selection is cleared before the operations and restored after,
 * updated with the new indexes.
 *
 * @param indexes Indexes of the patterns to delete.
 * @throws EditorException in case of error.
 */
void TilesetModel::delete_patterns(const QList<int>& indexes) {

  // Make a sorted list of patterns with their index and id.
  QList<QPair<int, QString>> sorted_patterns;
  for (int index : indexes) {
    QString pattern_id = index_to_id(index);
    if (pattern_id.isEmpty()) {
        throw EditorException(tr("Invalid tile pattern index: %1").arg(index));
    }
    sorted_patterns.append({ index, pattern_id });
  }
  std::sort(sorted_patterns.begin(), sorted_patterns.end(), [](const QPair<int, QString>& pattern_1, const QPair<int, QString>& pattern_2) {
    return pattern_1.first < pattern_2.first;
  });

  // Save and clear the selection during the whole operation.
  const QModelIndexList old_selected_indexes = selection_model.selection().indexes();
  QStringList old_selection_ids;
  for (const QModelIndex& old_selected_index : old_selected_indexes) {
    old_selection_ids << index_to_id(old_selected_index.row());
  }
  clear_selection();

  // Delete patterns, starting with the last one to avoid to shift any index.
  beginResetModel();
  for (auto it = sorted_patterns.rbegin(); it != sorted_patterns.rend(); ++it) {
    int index = it->first;
    QString pattern_id = it->second;
    tileset.remove_pattern(pattern_id.toStdString());
    patterns.removeAt(index);
  }
  build_index_map();
  endResetModel();

  // Restore the selection.
  for (const QString& selected_pattern_id : std::as_const(old_selection_ids)) {
    int new_index = id_to_index(selected_pattern_id);
    if (new_index == -1) {
      // This one was just deleted.
      continue;
    }
    add_to_selected(new_index);
  }
}

/**
 * @brief Changes the string id of a pattern.
 *
 * The index of multiple patterns in the pattern list may change, since
 * patterns are sorted alphabetically.
 * In this case, emits rowsAboutToBeMoved(), changes the index
 * and then emits rowsMoved(), as required by QAbstractItemModel.
 *
 * Then, emits pattern_id_changed(), no matter if the index has also changed.
 *
 * The selection is preserved, though the index of many patterns can change.
 * The selection is cleared before the operations and restored after,
 * updated with the new indexes.
 *
 * @param index Index of an existing pattern.
 * @param new_id The new id to set.
 * @return The new index of the pattern.
 * @throws EditorException in case of error.
 */
int TilesetModel::set_pattern_id(int index, const QString& new_id) {

  QString old_id = index_to_id(index);
  if (new_id == old_id) {
    // Nothing to do.
    return index;
  }

  // Make some checks first.
  if (old_id.isEmpty()) {
      throw EditorException(tr("Invalid tile pattern index: %1").arg(index));
  }

  if (!is_valid_pattern_id(new_id)) {
      throw EditorException(tr("Invalid tile pattern id: '%1'").arg(new_id));
  }

  if (id_to_index(new_id) != -1) {
      throw EditorException(tr("Tile pattern '%1' already exists").arg(new_id));
  }

  // Save and clear the selection since a lot of indexes may change.
  const QModelIndexList& old_selected_indexes = selection_model.selection().indexes();
  QStringList old_selection_ids;
  for (const QModelIndex& old_selected_index : old_selected_indexes) {
    old_selection_ids << index_to_id(old_selected_index.row());
  }
  clear_selection();

  // Change the id in the tileset file.
  tileset.set_pattern_id(old_id.toStdString(), new_id.toStdString());

  // Change the index in the list model (if the order has changed).
  build_index_map();
  int new_index = id_to_index(new_id);

  // Call beginMoveRows() if the index changes, as requested by
  // QAbstractItemModel.
  if (new_index != index) {
    int above_row = new_index;
    if (new_index > index) {
      ++above_row;
    }
    beginMoveRows(QModelIndex(), index, index,
                  QModelIndex(), above_row);

    // Update our pattern model list.
    patterns.move(index, new_index);
  }

  patterns[new_index].id = new_id;

  // Notify people before restoring the selection, so that they have a
  // chance to know new indexes before receiving selection signals.
  if (new_index != index) {
    endMoveRows();
  }
  emit pattern_id_changed(index, old_id, new_index, new_id);

  // Restore the selection.
  for (QString pattern_id : std::as_const(old_selection_ids)) {
    if (pattern_id == old_id) {
      pattern_id = new_id;
    }
    int new_index = id_to_index(pattern_id);
    add_to_selected(new_index);
  }

  return new_index;
}

/**
 * @brief Returns whether a string is a valid tile pattern id.
 * @param pattern_id The id to check.
 * @return @c true if this id is legal.
 */
bool TilesetModel::is_valid_pattern_id(const QString& pattern_id) {

  if (pattern_id.isEmpty()) {
      return false;
  }

  if (
      pattern_id.contains('\"') ||
      pattern_id.contains('\'') ||
      pattern_id.contains('\\') ||
      pattern_id.contains('\n') ||
      pattern_id.contains('\r')
  ) {
    return false;
  }

  return true;
}

/**
 * @brief Returns whether a pattern is a multi-frame pattern.
 * @param index A pattern index.
 * @return @c true if this is a multi-frame pattern.
 */
bool TilesetModel::is_pattern_multi_frame(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  return tileset.get_pattern(pattern_id)->is_multi_frame();
}

/**
 * @brief Returns whether the given patterns are all multi-frame.
 * @param indexes A list of pattern indexes.
 * @return @c true if all these patterns are multi-frame.
 */
bool TilesetModel::are_patterns_multi_frame(const QList<int>& indexes) const {

  for (int index : indexes) {
    if (!is_pattern_multi_frame(index)) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Returns the number of frames of a pattern.
 * @param index A pattern index.
 * @return The number of frames in the tileset.
 */
int TilesetModel::get_pattern_num_frames(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  return tileset.get_pattern(pattern_id)->get_num_frames();
}

/**
 * @brief Gets the number of frames of the specified patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] num_frames The common number of frames if any.
 * The value is left unchanged if the frame count is not common.
 * @return @c true if all specified patterns have the same number of frames.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_num_frames(const QList<int>& indexes, int& num_frames) const {

  if (indexes.empty()) {
    return false;
  }

  int candidate = get_pattern_num_frames(indexes.first());
  for (int index : indexes) {
    if (get_pattern_num_frames(index) != candidate) {
      return false;
    }
  }

  num_frames = candidate;
  return true;
}

/**
 * @brief Sets the number of frames of a pattern.
 *
 * Emits pattern_num_frames_changed() if there is a change.
 *
 * @param index A pattern index.
 * @return The number of frames to set.
 * @throws EditorException in case of error.
 */
void TilesetModel::set_pattern_num_frames(int index, int num_frames) {

  if (num_frames <= 0) {
    throw EditorException("Invalid number of frames");
  }

  if (num_frames == get_pattern_num_frames(index)) {
    return;
  }

  const std::string& pattern_id = index_to_id(index).toStdString();
  Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);

  Solarus::Rectangle first = pattern.get_frame();
  std::vector<Solarus::Rectangle> frames(num_frames, first);
  PatternSeparation separation = get_pattern_separation(index);
  for (int i = 1; i < num_frames; ++i) {
    if (separation == PatternSeparation::HORIZONTAL) {
      frames[i].add_x(i * first.get_width());
    }
    else {
      frames[i].add_y(i * first.get_height());
    }
  }
  pattern.set_frames(frames);
  patterns[index].set_image_dirty();

  emit pattern_num_frames_changed(index, num_frames);
}

/**
 * @brief Returns the coordinates of a pattern's frame in the tileset image.
 * @param index A pattern index.
 * @return The pattern's frame.
 * If this is a multi-frame pattern, the first frame is returned.
 */
QRect TilesetModel::get_pattern_frame(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  const Solarus::Rectangle& frame = tileset.get_pattern(pattern_id)->get_frame();
  return Rectangle::to_qrect(frame);
}

/**
 * @brief Returns the coordinates of all frames of a pattern in the tileset
 * image.
 * @param index A pattern index.
 * @return The pattern's frames.
 */
QList<QRect> TilesetModel::get_pattern_frames(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  const std::vector<Solarus::Rectangle>& frames =
      tileset.get_pattern(pattern_id)->get_frames();

  QList<QRect> result;
  for (const Solarus::Rectangle& frame : frames) {
    result << Rectangle::to_qrect(frame);
  }
  return result;
}

/**
 * @brief Returns the coordinates of the rectangle containing all frames of a
 * pattern in the tileset image.
 *
 * For single-frame patterns, this gives the same result as get_pattern_frame().
 *
 * @param index A pattern index.
 * @return The bounding box of the pattern's frames.
 */
QRect TilesetModel::get_pattern_frames_bounding_box(int index) const {

  QRect box = get_pattern_frame(index);
  if (!is_pattern_multi_frame(index)) {
    return box;
  }

  int num_frames = get_pattern_num_frames(index);
  if (get_pattern_separation(index) == PatternSeparation::HORIZONTAL) {
    box.setWidth(box.width() * num_frames);
  }
  else {
    box.setHeight(box.height() * num_frames);
  }
  return box;
}

/**
 * @brief Sets the coordinates of the rectangle containing all frames of a
 * pattern in the tileset image.
 *
 * Emits pattern_position_changed() if there is a change.
 *
 * @param index A pattern index.
 * @param position Top-left position of the first frame of the pattern.
 */
void TilesetModel::set_pattern_position(int index, const QPoint& position) {

  if (position == get_pattern_frame(index).topLeft()) {
    // No change.
    return;
  }

  const std::string& pattern_id = index_to_id(index).toStdString();
  std::vector<Solarus::Rectangle> frames = tileset.get_pattern(pattern_id)->get_frames();

  int old_x = frames[0].get_x();
  int old_y = frames[0].get_y();
  for (Solarus::Rectangle& frame : frames) {
    int dx = frame.get_x() - old_x;
    int dy = frame.get_y() - old_y;
    frame.set_xy(position.x() + dx, position.y() + dy);
  }

  tileset.get_pattern(pattern_id)->set_frames(frames);

  // The icon has changed.
  patterns[index].set_image_dirty();

  // Notify people.
  emit pattern_position_changed(index);

  QModelIndex model_index = this->index(index);
  emit dataChanged(model_index, model_index);
}

/**
 * @brief Sets the rectangle containing of a pattern in the tileset image.
 *
 * This makes the pattern single frame if it was not.
 * Emits pattern_position_changed() if there is a change.
 *
 * @param index A pattern index.
 * @param box The new rectangle to set.
 */
void TilesetModel::set_pattern_box(int index, const QRect& box) {

  if (box == get_pattern_frame(index)) {
    // No change.
    return;
  }

  const std::string& pattern_id = index_to_id(index).toStdString();

  tileset.get_pattern(pattern_id)->set_frame(Rectangle::to_solarus_rect(box));

  // The icon has changed.
  patterns[index].set_image_dirty();

  // Notify people.
  emit pattern_position_changed(index);

  QModelIndex model_index = this->index(index);
  emit dataChanged(model_index, model_index);
}

/**
 * @brief Returns the ground of a pattern.
 * @param index A pattern index.
 * @return The pattern's ground.
 */
Ground TilesetModel::get_pattern_ground(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  return tileset.get_pattern(pattern_id)->get_ground();
}

/**
 * @brief Gets the ground of the specified patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] ground The common ground if any.
 * The value is left unchanged if the ground is not common.
 * @return @c true if all specified patterns have the same ground.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_ground(const QList<int>& indexes, Ground& ground) const {

  if (indexes.empty()) {
    return false;
  }

  Ground candidate = get_pattern_ground(indexes.first());
  for (int index : indexes) {
    if (get_pattern_ground(index) != candidate) {
      return false;
    }
  }

  ground = candidate;
  return true;
}

/**
 * @brief Sets the ground of a tile pattern.
 *
 * Emits pattern_ground_changed() if there is a change.
 *
 * @param index A pattern index.
 * @param ground The ground to set.
 */
void TilesetModel::set_pattern_ground(int index, Ground ground) {

  Solarus::TilePatternData& pattern = *tileset.get_pattern(index_to_id(index).toStdString());
  if (ground == pattern.get_ground()) {
    return;
  }
  pattern.set_ground(ground);
  emit pattern_ground_changed(index, ground);
}

/**
 * @brief Returns the default layer of a pattern.
 * @param index A pattern index.
 * @return The pattern's default layer.
 */
int TilesetModel::get_pattern_default_layer(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  return tileset.get_pattern(pattern_id)->get_default_layer();
}

/**
 * @brief Gets the default layer of the specified patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] default_layer The common default layer if any.
 * The value is left unchanged if the layer is not common.
 * @return @c true if all specified patterns have the same default layer.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_default_layer(const QList<int>& indexes, int& default_layer) const {

  if (indexes.empty()) {
    return false;
  }

  int candidate = get_pattern_default_layer(indexes.first());
  for (int index : indexes) {
    if (get_pattern_default_layer(index) != candidate) {
      return false;
    }
  }

  default_layer = candidate;
  return true;
}

/**
 * @brief Sets the default layer of a tile pattern.
 *
 * Emits pattern_default_layer_changed() if there is a change.
 *
 * @param index A pattern index.
 * @param default_layer The default layer to set.
 */
void TilesetModel::set_pattern_default_layer(int index, int default_layer) {

  Solarus::TilePatternData& pattern = *tileset.get_pattern(index_to_id(index).toStdString());
  if (default_layer == pattern.get_default_layer()) {
    return;
  }
  pattern.set_default_layer(default_layer);
  emit pattern_default_layer_changed(index, default_layer);
}

/**
 * @brief Returns the repeat mode of a pattern.
 * @param index A pattern index.
 * @return The pattern's repeat mode.
 */
PatternRepeatMode TilesetModel::get_pattern_repeat_mode(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  return tileset.get_pattern(pattern_id)->get_repeat_mode();
}

/**
 * @brief Gets the repeat mode of the specified patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] repeat_mode The common repeat mode if any.
 * The value is left unchanged if the repeat mode is not common.
 * @return @c true if all specified patterns have the same repeat mode.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_repeat_mode(
    const QList<int>& indexes,
    PatternRepeatMode& repeat_mode) const {

  if (indexes.empty()) {
    return false;
  }

  PatternRepeatMode candidate = get_pattern_repeat_mode(indexes.first());
  for (int index : indexes) {
    if (get_pattern_repeat_mode(index) != candidate) {
      return false;
    }
  }

  repeat_mode = candidate;
  return true;
}

/**
 * @brief Sets the repeat_mode of a tile pattern.
 *
 * Emits pattern_repeat_mode_changed() if there is a change.
 *
 * @param index A pattern index.
 * @param repeat_mode The repeat mode to set.
 */
void TilesetModel::set_pattern_repeat_mode(int index, PatternRepeatMode repeat_mode) {

  Solarus::TilePatternData& pattern = *tileset.get_pattern(index_to_id(index).toStdString());
  if (repeat_mode == pattern.get_repeat_mode()) {
    return;
  }
  pattern.set_repeat_mode(repeat_mode);
  emit pattern_repeat_mode_changed(index, repeat_mode);
}

/**
 * @brief Returns the scrolling property of a pattern.
 * @param index A pattern index.
 * @return The pattern's scrolling.
 */
PatternScrolling TilesetModel::get_pattern_scrolling(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  const Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);

  return pattern.get_scrolling();
}

/**
 * @brief Gets the scrolling property of the specified patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] scrolling The common scrolling if any.
 * The value is left unchanged if the scrolling is not common.
 * @return @c true if all specified patterns have the same scrolling.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_scrolling(const QList<int>& indexes, PatternScrolling& scrolling) const {

  if (indexes.empty()) {
    return false;
  }

  PatternScrolling candidate = get_pattern_scrolling(indexes.first());
  for (int index : indexes) {
    if (get_pattern_scrolling(index) != candidate) {
      return false;
    }
  }

  scrolling = candidate;
  return true;
}

/**
 * @brief Sets the scrolling property of a pattern.
 *
 * Emits pattern_scrolling_changed() if there is a change.
 *
 * @param index A pattern index.
 * @return The scrolling value to set.
 * @throws EditorException in case of error.
 */
void TilesetModel::set_pattern_scrolling(int index, PatternScrolling scrolling) {

  PatternScrolling old_scrolling = get_pattern_scrolling(index);
  if (scrolling == old_scrolling) {
    return;
  }

  const std::string& pattern_id = index_to_id(index).toStdString();
  Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);

  pattern.set_scrolling(scrolling);

  emit pattern_scrolling_changed(index, scrolling);
}

/**
 * @brief Returns the separation of the frames if the pattern is multi-frame.
 * @param index A pattern index.
 * @return The type of separation of the frames.
 * Returns TilePatternSeparation::HORIZONTAL if the pattern is single-frame.
 */
PatternSeparation TilesetModel::get_pattern_separation(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  const Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);

  const std::vector<Solarus::Rectangle>& frames = pattern.get_frames();
  if (frames.size() == 1) {
    return PatternSeparation::HORIZONTAL;
  }

  if (frames[0].get_y() == frames[1].get_y()) {
    return PatternSeparation::HORIZONTAL;
  }
  return PatternSeparation::VERTICAL;
}

/**
 * @brief Gets the separation of the specified patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] separation The common separation if any.
 * The value is left unchanged if the separation is not common.
 * @return @c true if all specified patterns have the same separation.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_separation(const QList<int>& indexes, PatternSeparation& separation) const {

  if (indexes.empty()) {
    return false;
  }

  PatternSeparation candidate = get_pattern_separation(indexes.first());
  for (int index : indexes) {
    if (get_pattern_separation(index) != candidate) {
      return false;
    }
  }

  separation = candidate;
  return true;
}

/**
 * Sets the type of separation of the frames if the tile pattern is multi-frame.
 *
 * Emits pattern_separation_changed() if there is a change.
 *
 * Nothing is done if the tile pattern is single-frame.
 *
 * @param index A pattern index.
 * @param separation The type of separation of the frames.
 * @throws EditorException If the separation is not valid, i.e. if the size of
 * each frame after separation is not divisible by 8.
 */
void TilesetModel::set_pattern_separation(int index, PatternSeparation separation) {

  const std::string& pattern_id = index_to_id(index).toStdString();
  Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);

  if (!pattern.is_multi_frame()) {
    // Nothing to do.
    return;
  }

  PatternSeparation old_separation = get_pattern_separation(index);
  if (separation == old_separation) {
    // No change.
    return;
  }

  int num_frames = pattern.get_num_frames();
  Solarus::Rectangle first = pattern.get_frame();
  std::vector<Solarus::Rectangle> frames(num_frames, first);
  for (int i = 1; i < num_frames; ++i) {
    if (separation == PatternSeparation::HORIZONTAL) {
      frames[i].add_x(i * first.get_width());
    }
    else {
      frames[i].add_y(i * first.get_height());
    }
  }
  pattern.set_frames(frames);

  patterns[index].set_image_dirty();

  emit pattern_separation_changed(index, separation);
}

/**
 * @brief Returns the delay between frames for a multi-frame pattern.
 * @param index A pattern index.
 * @return The frame delay in milliseconds.
 */
int TilesetModel::get_pattern_frame_delay(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  const Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);
  return pattern.get_frame_delay();
}

/**
 * @brief Gets the frame delay of the specified patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] frame_delay The common frame delay if any.
 * The value is left unchanged if the frame delay is not common
 * or if all patterns are not multi-frame.
 * @return @c true if all specified patterns are multi-frame and have the
 * same frame delay.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_frame_delay(const QList<int>& indexes, int& frame_delay) const {

  if (indexes.empty()) {
    return false;
  }

  int candidate = get_pattern_frame_delay(indexes.first());
  for (int index : indexes) {
    if (!is_pattern_multi_frame(index) ||
        get_pattern_frame_delay(index) != candidate) {
      return false;
    }
  }

  frame_delay = candidate;
  return true;
}

/**
 * Sets the frame delay of a multi-tile pattern.
 *
 * Emits pattern_frame_delay_changed() if there is a change.
 *
 * Nothing is done if the tile pattern is single frame.
 *
 * @param index A pattern index.
 * @param frame_delay The frame delay to set in milliseconds.
 * @throws EditorException In case of error.
 */
void TilesetModel::set_pattern_frame_delay(int index, int frame_delay) {

  if (frame_delay <= 0) {
    throw EditorException("Invalid frame delay");
  }

  if (frame_delay == get_pattern_frame_delay(index)) {
    return;
  }

  if (!is_pattern_multi_frame(index)) {
    return;
  }

  const std::string& pattern_id = index_to_id(index).toStdString();
  Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);

  pattern.set_frame_delay(frame_delay);

  emit pattern_frame_delay_changed(index, frame_delay);
}

/**
 * @brief Returns whether a multi-frame patterns does a mirror loop.
 * @param index A pattern index.
 * @return @c true if the animation plays backwards when looping.
 */
bool TilesetModel::is_pattern_mirror_loop(int index) const {

  const std::string& pattern_id = index_to_id(index).toStdString();
  const Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);
  return pattern.is_mirror_loop();
}

/**
 * @brief Gets the mirror loop property of patterns if it is the same.
 * @param[in] indexes A list of pattern indexes.
 * @param[out] mirror_loop The common mirror loop value if any.
 * The value is left unchanged if mirror loop is not common
 * or if all patterns are not multi-frame.
 * @return @c true if all specified patterns are multi-frame and have the
 * same mirror loop value.
 * If the list is empty, @c false is returned.
 */
bool TilesetModel::is_common_pattern_mirror_loop(const QList<int>& indexes, bool& mirror_loop) const {

  if (indexes.empty()) {
    return false;
  }

  bool candidate = is_pattern_mirror_loop(indexes.first());
  for (int index : indexes) {
    if (!is_pattern_multi_frame(index) ||
        is_pattern_mirror_loop(index) != candidate) {
      return false;
    }
  }

  mirror_loop = candidate;
  return true;
}

/**
 * Sets the mirror loop property of a multi-tile pattern.
 *
 * Emits pattern_mirror_loop_changed() if there is a change.
 *
 * Nothing is done if the tile pattern is single-frame.
 *
 * @param index A pattern index.
 * @param mirror_loop Whether to play the animation backwards when looping.
 */
void TilesetModel::set_pattern_mirror_loop(int index, bool mirror_loop) {

  if (mirror_loop == is_pattern_mirror_loop(index)) {
    return;
  }

  if (!is_pattern_multi_frame(index)) {
    return;
  }

  const std::string& pattern_id = index_to_id(index).toStdString();
  Solarus::TilePatternData& pattern = *tileset.get_pattern(pattern_id);

  pattern.set_mirror_loop(mirror_loop);

  emit pattern_mirror_loop_changed(index, mirror_loop);
}

/**
 * @brief Returns an image representing the specified pattern.
 *
 * The image has the size of the pattern.
 * If the pattern is multi-frame, the image of the first frame is returned.
 *
 * @param index Index of a tile pattern.
 * @return The corresponding image.
 * Returns a null pixmap if the tileset image is not loaded.
 */
QPixmap TilesetModel::get_pattern_image(int index) const {

  if (!pattern_exists(index)) {
    // No such pattern.
    return QPixmap();
  }

  if (patterns_image.isNull()) {
    // No tileset image.
    return QPixmap();
  }

  const PatternModel& pattern = patterns.at(index);
  if (!pattern.image.isNull()) {
    // Image already created.
    return pattern.image;
  }

  // Lazily create the image.
  QRect frame = get_pattern_frame(index);
  QImage image = patterns_image.copy(frame);

  pattern.image = QPixmap::fromImage(image);
  return pattern.image;
}

/**
 * @brief Returns an image representing the specified pattern.
 *
 * If the pattern is multi-frame, the image returned contains all frames.
 *
 * @param index Index of a tile pattern.
 * @return The corresponding image with all frames.
 * Returns a null pixmap if the tileset image is not loaded.
 */
QPixmap TilesetModel::get_pattern_image_all_frames(int index) const {

  if (!pattern_exists(index)) {
    // No such pattern.
    return QPixmap();
  }

  if (!is_pattern_multi_frame(index)) {
    // Single frame pattern.
    return get_pattern_image(index);
  }

  if (patterns_image.isNull()) {
    // No tileset image.
    return QPixmap();
  }

  const PatternModel& pattern = patterns.at(index);
  if (!pattern.image_all_frames.isNull()) {
    // Image already created.
    return pattern.image_all_frames;
  }

  // Lazily create the image.
  QRect frame = get_pattern_frames_bounding_box(index);
  QImage image_all_frames = patterns_image.copy(frame);

  pattern.image_all_frames = QPixmap::fromImage(image_all_frames);
  return pattern.image_all_frames;
}

/**
 * @brief Returns a 32x32 icon representing the specified pattern.
 * @param index Index of a tile pattern.
 * @return The corresponding icon.
 * Returns a null pixmap if the tileset image is not loaded.
 */
QPixmap TilesetModel::get_pattern_icon(int index) const {

  QPixmap pixmap = get_pattern_image(index);

  if (pixmap.isNull()) {
    // No image available.
    return QPixmap();
  }

  const PatternModel& pattern = patterns.at(index);
  if (!pattern.icon.isNull()) {
    // Icon already created.
    return pattern.icon;
  }

  // Lazily create the icon.
  QImage image = pixmap.toImage();
  // Make sure we have an alpha channel.
  image = image.convertToFormat(QImage::Format_RGBA8888_Premultiplied);

  if (image.height() <= 16 && image.width() <= 16) {
    image = image.scaledToHeight(image.height() * 2);
  }
  else if (image.height() > 32) {
    image = image.scaledToHeight(32);
  }

  // Center the pattern in a 32x32 pixmap.
  int dx = (32 - image.width()) / 2;
  int dy = (32 - image.height()) / 2;
  image = image.copy(-dx, -dy, 32, 32);

  pattern.icon = QPixmap::fromImage(image);
  return pattern.icon;
}

/**
 * @brief Returns the PNG image of all tile patterns.
 * @return The patterns image.
 * Returns a null image if the tileset image is not loaded.
 */
QImage TilesetModel::get_patterns_image() const {
  return patterns_image;
}

/**
 * @brief Loads the tileset image from its PNG file.
 */
void TilesetModel::reload_patterns_image() {

  QString path = quest.get_tileset_tiles_image_path(tileset_id);
  patterns_image = QImage(path);

  for (PatternModel& pattern : patterns) {
    pattern.set_image_dirty();
  }

  emit tileset_image_file_reloaded();
}

/**
 * @brief Returns the selection model of the tileset.
 * @return The selection info.
 */
QItemSelectionModel& TilesetModel::get_selection_model() {
  return selection_model;
}

/**
 * @brief Returns whether no patterns are selected.
 * @return @c true if the selection is empty.
 */
bool TilesetModel::is_selection_empty() const {

  return selection_model.selection().isEmpty();
}

/**
 * @brief Returns the number of selected patterns.
 * @return The number of selected pattern.
 */
int TilesetModel::get_selection_count() const {

  return static_cast<int>(selection_model.selectedIndexes().count());
}

/**
 * @brief Returns the index of the selected pattern.
 * @return The selected pattern index.
 * Returns -1 if no pattern is selected or if multiple patterns are selected.
 */
int TilesetModel::get_selected_index() const {

  QModelIndexList selected_indexes = selection_model.selectedIndexes();
  if (selected_indexes.size() != 1) {
    return -1;
  }
  return selected_indexes.first().row();
}

/**
 * @brief Returns the id of the selected pattern.
 * @return The selected pattern id.
 * Returns an empty string if no pattern is selected or if multiple patterns
 * are selected.
 */
QString TilesetModel::get_selected_id() const {
  return index_to_id(get_selected_index());
}

/**
 * @brief Returns all selected pattern indexes.
 * @return The selected pattern indexes.
 */
QList<int> TilesetModel::get_selected_indexes() const {

  QList<int> result;
  const QModelIndexList& selected_indexes = selection_model.selectedIndexes();
  for (const QModelIndex& index : selected_indexes) {
    result << index.row();
  }
  return result;
}

/**
 * @brief Returns all selected pattern ids.
 * @return The selected pattern ids.
 */
QStringList TilesetModel::get_selected_ids() const {

  QStringList result;
  const QModelIndexList& selected_indexes = selection_model.selectedIndexes();
  for (const QModelIndex& index : selected_indexes) {
    result << index_to_id(index.row());
  }
  return result;
}

/**
 * @brief Selects a pattern and deselects all others.
 * @param index The index to select.
 */
void TilesetModel::set_selected_index(int index) {

  set_selected_indexes({ index });
}

/**
 * @brief Selects the specified patterns and deselects others.
 * @param indexes The indexes to select.
 */
void TilesetModel::set_selected_indexes(const QList<int>& indexes) {

  const QModelIndexList& current_selection = selection_model.selectedIndexes();

  QItemSelection itemSelection;
  for (int index : indexes) {
    QModelIndex model_index = this->index(index);
    itemSelection.select(model_index, model_index);
  }
  QModelIndexList selection = itemSelection.indexes();

  QSet<QModelIndex> selection_set;
  for (const QModelIndex& index : std::as_const(selection)) {
    selection_set.insert(index);
  }
  QSet<QModelIndex> current_selection_set;
  for (const QModelIndex& index : current_selection) {
    current_selection_set.insert(index);
  }

  if (selection_set == current_selection_set) {
    // No change.
    return;
  }

  selection_model.select(itemSelection, QItemSelectionModel::ClearAndSelect);
}

/**
 * @brief Selects a pattern and lets the rest of the selection unchanged.
 * @param index The index to select.
 */
void TilesetModel::add_to_selected(int index) {

  add_to_selected(QList<int>({ index }));
}

/**
 * @brief Selects the specified patterns and lets the rest of the selection
 * unchanged.
 * @param indexes The indexes to select.
 */
void TilesetModel::add_to_selected(const QList<int>& indexes) {

  QItemSelection selection;
  for (int index : indexes) {
    QModelIndex model_index = this->index(index);
    selection.select(model_index, model_index);
  }

  selection_model.select(selection, QItemSelectionModel::Select);
}

/**
 * @brief Returns whether a pattern is selected.
 * @param index A pattern index.
 * @return @c true if this pattern is selected.
 */
bool TilesetModel::is_selected(int index) const {

  return selection_model.isSelected(this->index(index));
}

/**
 * @brief Changes the selection state of an item.
 * @param index Index of the pattern to toggle.
 */
void TilesetModel::toggle_selected(int index) {

  selection_model.select(this->index(index), QItemSelectionModel::Toggle);
}

/**
 * @brief Selects all patterns of the tileset.
 */
void TilesetModel::select_all() {

  QItemSelection selection;
  QModelIndex first_index = this->index(0);
  QModelIndex last_index = this->index(get_num_patterns() - 1);
  selection.select(first_index, last_index);
  selection_model.select(selection, QItemSelectionModel::Select);
}

/**
 * @brief Deselects all selected items.
 */
void TilesetModel::clear_selection() {

  selection_model.clear();
}

/**
 * @brief Returns the number of border sets in this tileset.
 * @return The number of border sets.
 */
int TilesetModel::get_num_border_sets() const {

  return static_cast<int>(tileset.get_border_sets().size());
}

/**
 * @brief Returns the ids of all border sets in this tileset.
 * @return The border set ids in alphabetical order.
 */
QStringList TilesetModel::get_border_set_ids() const {

  const std::map<std::string, Solarus::BorderSet>& border_sets = tileset.get_border_sets();
  QStringList border_set_ids;
  for (const auto& kvp : border_sets) {
    border_set_ids << QString::fromStdString(kvp.first);
  }
  return border_set_ids;
}

/**
 * @brief Returns whether a border set exists with the given id.
 * @param border_set_id A border set id.
 * @return @c true if the tileset contains such a border set.
 */
bool TilesetModel::border_set_exists(const QString& border_set_id) const {

  return tileset.border_set_exists(border_set_id.toStdString());
}

/**
 * @brief Returns whether all patterns of a border set exist in the tileset.
 * @param border_set_id A border set id.
 * @return @c true if the patterns of this border set all exist.
 */
bool TilesetModel::border_set_patterns_exist(const QString& border_set_id) const {

  if (!border_set_exists(border_set_id)) {
    return false;
  }

  for (int i = 0; i < 12; ++i) {
    const QString& pattern_id = get_border_set_pattern(border_set_id, static_cast<BorderKind>(i));
    if (!pattern_id.isEmpty() && !pattern_exists(pattern_id)) {
      return false;
    }
  }
  return true;
}

/**
 * @brief Creates an empty border set with the given id.
 *
 * Emits border_set_created().
 *
 * @param border_set_id A border set id.
 * @throws EditorException in case of error.
 */
void TilesetModel::create_border_set(const QString& border_set_id) {

  if (border_set_exists(border_set_id)) {
    throw EditorException(tr("Contour already exists: '%1'").arg(border_set_id));
  }

  if (!is_valid_border_set_id(border_set_id)) {
      throw EditorException(tr("Invalid contour id: '%1'").arg(border_set_id));
  }

  bool success = tileset.add_border_set(border_set_id.toStdString(), Solarus::BorderSet());

  if (!success) {
    throw EditorException(tr("Failed to create contour '%1'").arg(border_set_id));
  }

  emit border_set_created(border_set_id);
}

/**
 * @brief Deletes a border set.
 *
 * Emits border_set_deleted().
 *
 * @param border_set_id Id of the border set to delete.
 * @throws EditorException in case of error.
 */
void TilesetModel::delete_border_set(const QString& border_set_id) {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  bool success = tileset.remove_border_set(border_set_id.toStdString());

  if (!success) {
    throw EditorException(tr("Failed to delete contour '%1'").arg(border_set_id));
  }

  emit border_set_deleted(border_set_id);
}

/**
 * @brief Renames a border set.
 *
 * Emits border_set_id_changed().
 *
 * @param old_id Id of an existing border set.
 * @param new_id New id to set.
 * @throws EditorException in case of error.
 */
void TilesetModel::set_border_set_id(const QString& old_id, const QString& new_id) {

  if (!border_set_exists(old_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(old_id));
  }

  if (!is_valid_border_set_id(new_id)) {
      throw EditorException(tr("Invalid contour id: '%1'").arg(new_id));
  }

  if (border_set_exists(new_id)) {
    throw EditorException(tr("Contour id already in use: '%1'").arg(new_id));
  }

  bool success = tileset.set_border_set_id(old_id.toStdString(), new_id.toStdString());

  if (!success) {
    throw EditorException(tr("Failed to rename contour '%1'").arg(old_id));
  }

  emit border_set_id_changed(old_id, new_id);
}

/**
 * @brief Returns the pattern id of a border for the given border set.
 * @param border_set_id A border set id.
 * @param border_kind The kind of border to get in this border set.
 * @return The pattern id of this border,
 * or an empty string if no pattern is set for this border kind.
 * @throws EditorException in case of error.
 */
QString TilesetModel::get_border_set_pattern(const QString& border_set_id, BorderKind border_kind) const {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  const Solarus::BorderSet& border_set = tileset.get_border_set(border_set_id.toStdString());
  return QString::fromStdString(border_set.get_pattern(border_kind));
}

/**
 * @brief Sets a pattern to a border for the given border set.
 *
 * Emits border_set_pattern_changed() if there is a change.
 *
 * @param border_set_id A border set id.
 * @param border_kind The kind of border to set in this border set.
 * @param pattern_id The pattern id to set for this border,
 * or an empty string to unset it.
 * @throws EditorException in case of error.
 */
void TilesetModel::set_border_set_pattern(const QString& border_set_id, BorderKind border_kind, const QString& pattern_id) {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  if (pattern_id == get_border_set_pattern(border_set_id, border_kind)) {
    // No change.
    return;
  }

  Solarus::BorderSet& border_set = tileset.get_border_set(border_set_id.toStdString());
  border_set.set_pattern(border_kind, pattern_id.toStdString());

  emit border_set_pattern_changed(border_set_id, border_kind, pattern_id);
}

/**
 * @brief Returns whether a pattern is defined for a border in the given border set.
 * @param border_set_id A border set id.
 * @param border_kind The kind of border to get in this border set.
 * @return @c true if a pattern is defined for this border.
 * @throws EditorException in case of error.
 */
bool TilesetModel::has_border_set_pattern(const QString& border_set_id, BorderKind border_kind) const {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  return tileset.get_border_set(border_set_id.toStdString()).has_pattern(border_kind);
}

/**
 * @brief Returns the list of border patterns for the given border set.
 * @param border_set_id A border set id.
 * @return The pattern ids in the order of the BorderKind enum.
 * @throws EditorException in case of error.
 */
QStringList TilesetModel::get_border_set_patterns(const QString& border_set_id) const {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  const Solarus::BorderSet& border_set = tileset.get_border_set(border_set_id.toStdString());
  QStringList patterns;
  for (int i = 0; i < 12; ++i) {
    BorderKind border_kind = static_cast<BorderKind>(i);
    patterns << QString::fromStdString(border_set.get_pattern(border_kind));
  }

  return patterns;
}

/**
 * @brief Sets the list of border patterns for the given border set.
 *
 * Emits border_set_pattern_changed() for each pattern that changes.
 *
 * @param border_set_id A border set id.
 * @param patterns The pattern ids in the order of the BorderKind enum.
 * @throws EditorException in case of error.
 */
void TilesetModel::set_border_set_patterns(
    const QString& border_set_id,
    const QStringList& patterns
) {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  for (int i = 0; i < 12; ++i) {
    BorderKind border_kind = static_cast<BorderKind>(i);
    if (i >= patterns.size()) {
      return;
    }
    set_border_set_pattern(border_set_id, border_kind, patterns[i]);
  }
}

/**
 * @brief Returns whether a border set generates tiles inside or outside the selection.
 * @param border_set_id A border set id.
 * @return @c true if this is an inner border set.
 * @throws EditorException in case of error.
 */
bool TilesetModel::is_border_set_inner(const QString& border_set_id) const {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  return tileset.get_border_set(border_set_id.toStdString()).is_inner();
}

/**
 * @brief Sets whether a border set generates tiles inside or outside the selection.
 *
 * Emits border_set_inner_changed() if there is a change.
 *
 * @param border_set_id A border set id.
 * @param inner @c true to make this border set an inner one.
 * @throws EditorException in case of error.
 */
void TilesetModel::set_border_set_inner(const QString& border_set_id, bool inner) {

  if (!border_set_exists(border_set_id)) {
    throw EditorException(tr("No such contour: '%1'").arg(border_set_id));
  }

  if (inner == is_border_set_inner(border_set_id)) {
    // No change.
    return;
  }

  tileset.get_border_set(border_set_id.toStdString()).set_inner(inner);

  emit border_set_inner_changed(border_set_id, inner);
}

/**
 * @brief Returns whether a string is a valid border set id.
 * @param border_set_id The id to check.
 * @return @c true if this id is legal.
 */
bool TilesetModel::is_valid_border_set_id(const QString& border_set_id) {

  if (border_set_id.isEmpty()) {
      return false;
  }

  if (
      border_set_id.contains('\"') ||
      border_set_id.contains('\'') ||
      border_set_id.contains('\\') ||
      border_set_id.contains('\n') ||
      border_set_id.contains('\r')
  ) {
    return false;
  }

  return true;
}

/**
 * @brief Returns an image representing the specified border set.
 *
 * The image has the size of the pattern.
 *
 * @param border_set_id A border set id.
 * @return The corresponding image.
 * Returns a null pixmap if the tileset image is not loaded
 * or if there is no such border set.
 */
QPixmap TilesetModel::get_border_set_image(const QString& border_set_id) const {

  if (!border_set_exists(border_set_id)) {
    return QPixmap();
  }

  QString pattern_id = get_border_set_pattern(border_set_id, BorderKind::TOP);
  return get_pattern_image(id_to_index(pattern_id));
}

/**
 * @brief Returns a 32x32 icon representing the specified border set.
 * @param border_set_id A border set id.
 * @return The corresponding icon.
 * Returns a null pixmap if the tileset image is not loaded
 * or if there is no such border set.
 */
QPixmap TilesetModel::get_border_set_icon(const QString& border_set_id) const {

  if (!border_set_exists(border_set_id)) {
    return QPixmap();
  }

  QString pattern_id = get_border_set_pattern(border_set_id, BorderKind::TOP);
  return get_pattern_icon(id_to_index(pattern_id));
}

}
