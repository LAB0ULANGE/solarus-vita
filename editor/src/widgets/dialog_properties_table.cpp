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
#include "widgets/dialog_properties_table.h"
#include "dialogs_model.h"
#include "editor_style.h"
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <cmath>

namespace SolarusEditor {

/**
 * @brief Creates an empty dialogs tree view.
 * @param parent The parent object or nullptr.
 */
DialogPropertiesTable::DialogPropertiesTable(QWidget* parent) :
  QTreeWidget(parent),
  model(nullptr) {

  create_action = new QAction(
        QIcon(":/images/icon_add.svg"), tr("New property..."), this);
  connect(create_action, &QAction::triggered,
          this, &DialogPropertiesTable::create_property_requested);
  addAction(create_action);

  set_key_action = new QAction(
        QIcon(":/images/icon_rename.svg"), tr("Change key..."), this);
  set_key_action->setShortcut(tr("F2"));
  set_key_action->setShortcutContext(Qt::WidgetShortcut);
  connect(set_key_action, &QAction::triggered,
          this, &DialogPropertiesTable::set_property_key_requested);
  addAction(set_key_action);

  delete_action = new QAction(
        QIcon(":/images/icon_delete.svg"), tr("Delete..."), this);
  delete_action->setShortcut(QKeySequence::Delete);
  delete_action->setShortcutContext(Qt::WidgetShortcut);
  connect(delete_action, &QAction::triggered,
          this, &DialogPropertiesTable::delete_property_requested);
  addAction(delete_action);

  set_action = new QAction(
        QIcon(":/images/icon_paste.svg"), tr("Set from translation..."), this);
  connect(set_action, &QAction::triggered,
          this, &DialogPropertiesTable::set_from_translation_requested);
  addAction(set_action);

  connect(this, &QTreeWidget::itemDoubleClicked,
          this, &DialogPropertiesTable::on_item_double_clicked);
  connect(this, &QTreeWidget::itemChanged,
          this, &DialogPropertiesTable::on_item_changed);
}

/**
 * @brief Shows a popup menu with actions related to the selected item.
 * @param event The event to handle.
 */
void DialogPropertiesTable::contextMenuEvent(QContextMenuEvent *event) {

  if (model == nullptr) {
    return;
  }

  QMenu* menu = new QMenu(this);
  EditorStyle::setAutoIconColor(menu, EditorStyle::AutoIconColor::ForegroundColor);

  menu->addAction(create_action);

  QString key = get_selected_property();
  if (model->dialog_property_exists(dialog_id, key)) {
    menu->addSeparator();
    menu->addAction(set_key_action);
    menu->addSeparator();
    menu->addAction(delete_action);
  } else if (!key.isEmpty()) {
    menu->addSeparator();
    menu->addAction(set_action);
  }

  menu->popup(viewport()->mapToGlobal(event->pos()) + QPoint(1, 1));
}

/**
 * @brief Sets the dialogs to represent in this view.
 * @param model The dialogs model or nullptr.
 */
void DialogPropertiesTable::set_model(DialogsModel* model) {

  if (this->model != nullptr) {
    this->model->disconnect(this);
    this->model = nullptr;
  }

  this->model = model;

  if (this->model != nullptr) {
    connect(&model->get_selection_model(),
            &QItemSelectionModel::selectionChanged,
            this, &DialogPropertiesTable::update);
    connect(model, &DialogsModel::dialog_property_created,
            this, &DialogPropertiesTable::dialog_property_created);
    connect(model, &DialogsModel::dialog_property_deleted,
            this, &DialogPropertiesTable::dialog_property_deleted);
    connect(model, &DialogsModel::dialog_property_changed,
            this, &DialogPropertiesTable::dialog_property_value_changed);
  }

  update();
}

/**
 * @brief Returns the current selected property key.
 * @return The key of the current selected property or an empty string if
 * the selection is empty.
 */
QString DialogPropertiesTable::get_selected_property() const {

  QTreeWidgetItem* item = currentItem();
  if (item == nullptr) {
    return "";
  }
  return item->data(KEY_COLUMN, Qt::DisplayRole).toString();
}

/**
 * @brief Changes the current selected property.
 * @param key The key of the property to select.
 */
void DialogPropertiesTable::set_selected_property(const QString& key) {

  if (!items.contains(key)) {
    return;
  }
  setCurrentItem(items[key]);
}

/**
 * @brief Slot called when the selected dialog changed.
 */
void DialogPropertiesTable::update() {

  clear_table();

  dialog_id = model->get_selected_id();
  QMap<QString, QString> properties = model->get_dialog_properties(dialog_id);
  const QList<QString>& keys = properties.keys();
  for (const QString& key : keys) {
    dialog_property_created(dialog_id, key, properties[key]);
  }

  if (!model->translated_dialog_exists(dialog_id)) {
    setColumnHidden(TRANSLATION_COLUMN, true);
    return;
  }

  if (isColumnHidden(TRANSLATION_COLUMN)) {

    setColumnHidden(TRANSLATION_COLUMN, false);
    // Resize value and translation columns.
    int col_width =
      std::floor((viewport()->width() - columnWidth(KEY_COLUMN)) / 2.0);
    setColumnWidth(VALUE_COLUMN, col_width);
    setColumnWidth(TRANSLATION_COLUMN, col_width);
  }

  properties = model->get_translated_dialog_properties(dialog_id);
  const QList<QString>& translated_keys = properties.keys();
  for (const QString& key : translated_keys) {
    add_translation_property(key, properties[key]);
  }
}

/**
 * @brief Slot called when a new property was created.
 */
void DialogPropertiesTable::dialog_property_created(
    const QString &id, const QString &key, const QString &value) {

  if (id != dialog_id) {
    return;
  }

  QTreeWidgetItem* item;
  if (items.contains(key)) {
    item = items[key];
  } else {
    item = new QTreeWidgetItem();
    item->setData(KEY_COLUMN, Qt::DisplayRole, key);
    items.insert(key, item);
    insertTopLevelItem(topLevelItemCount(), item);
  }

  item->setIcon(0, QIcon(":/images/icon_property.svg"));
  item->setData(VALUE_COLUMN, Qt::DisplayRole, value);
}

/**
 * @brief Slot called when a property was deleted.
 */
void DialogPropertiesTable::dialog_property_deleted(
    const QString& id, const QString& key) {

  if (id != dialog_id || !items.contains(key)) {
    return;
  }
  QTreeWidgetItem* item = items[key];

  if (item->data(TRANSLATION_COLUMN, Qt::DisplayRole).toString().isEmpty()) {

    takeTopLevelItem(indexOfTopLevelItem(item));
    items.remove(key);
    delete item;
  }
  else {
    item->setIcon(0, QIcon(":/images/icon_property_missing.svg"));
    item->setData(VALUE_COLUMN, Qt::DisplayRole, "");
  }
}

/**
 * @brief Slot called when a property value has changed.
 */
void DialogPropertiesTable::dialog_property_value_changed(
    const QString &id, const QString &key, const QString &new_value) {

  if (id != dialog_id || !items.contains(key)) {
    return;
  }
  items[key]->setData(VALUE_COLUMN, Qt::DisplayRole, new_value);
}

/**
 * @brief Slot called when the user double click on the table.
 */
void DialogPropertiesTable::on_item_double_clicked(
    QTreeWidgetItem *item, int column) {

  Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
  if (column == VALUE_COLUMN) {
    flags |= Qt::ItemIsEditable;
  }

  item->setFlags(flags);
}

/**
 * @brief Slot called when the user change the value of a property.
 */
void DialogPropertiesTable::on_item_changed(QTreeWidgetItem *item, int column) {

  if (column != VALUE_COLUMN) {
    return;
  }

  QString key = item->data(KEY_COLUMN, Qt::DisplayRole).toString();
  QString value = item->data(VALUE_COLUMN, Qt::DisplayRole).toString();

  if (value == model->get_dialog_property(dialog_id, key)) {
    return;
  }

  emit set_property_value_requested(key, value);
}

/**
 * @brief Clears all the table.
 */
void DialogPropertiesTable::clear_table() {

  const QList<QString>& keys = items.keys();
  for (const QString& key : keys) {
    takeTopLevelItem(indexOfTopLevelItem(items[key]));
    delete items[key];
    items.remove(key);
  }
}

/**
 * @brief Adds a translation property in the table.
 * @param key The key of the translated property to add.
 * @param value The value of the translated property.
 */
void DialogPropertiesTable::add_translation_property(
    const QString& key, const QString& value) {

  if (items.contains(key)) {
    items[key]->setData(TRANSLATION_COLUMN, Qt::DisplayRole, value);
  } else {
    QTreeWidgetItem* item = new QTreeWidgetItem();
    item->setIcon(0, QIcon(":/images/icon_property_missing.svg"));
    item->setData(KEY_COLUMN, Qt::DisplayRole, key);
    item->setData(TRANSLATION_COLUMN, Qt::DisplayRole, value);
    items.insert(key, item);
    insertTopLevelItem(topLevelItemCount(), item);
  }
}

}
