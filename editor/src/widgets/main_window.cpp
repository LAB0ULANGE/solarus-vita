/*
 * Copyright (C) 2014-2021 Christopho, Solarus - http://www.solarus-games.org
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
#include "entities/entity_traits.h"
#include "widgets/change_resource_id_dialog.h"
#include "widgets/editor.h"
#include "widgets/enum_menus.h"
#include "widgets/external_script_dialog.h"
#include "widgets/gui_tools.h"
#include "widgets/import_dialog.h"
#include "widgets/input_dialog_with_check_box.h"
#include "widgets/main_window.h"
#include "widgets/new_quest_dialog.h"
#include "widgets/package_dialog.h"
#include "widgets/pair_spin_box.h"
#include "audio.h"
#include "config.h"
#include "editor_style.h"
#include "file_tools.h"
#include "map_model.h"
#include "new_quest_builder.h"
#include "obsolete_editor_exception.h"
#include "obsolete_quest_exception.h"
#include "quest.h"
#include "refactoring.h"
#include "version.h"
#include "quest_runner.h"
#include <QActionGroup>
#include <QCloseEvent>
#include <QDebug>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QSplitter>
#include <QToolButton>
#include <QUndoGroup>
#include <QStandardPaths>

#include <oclero/qlementine/widgets/AboutDialog.hpp>

namespace SolarusEditor {

using EntityType = Solarus::EntityType;

/**
 * @brief Creates a main window.
 * @param parent The parent widget or nullptr.
 */
MainWindow::MainWindow(QWidget* parent) :
  QMainWindow(parent),
  quest_runner(),
  recent_quests_menu(nullptr),
  zoom_menu(nullptr),
  zoom_button(nullptr),
  zoom_actions(),
  show_layers_menu(nullptr),
  show_layers_button(nullptr),
  show_layers_action(nullptr),
  show_layers_subactions(),
  lock_layers_menu(nullptr),
  lock_layers_subactions(),
  show_entities_menu(nullptr),
  show_entities_button(nullptr),
  show_entities_subactions(),
  common_actions(),
  settings_dialog(this) {

  // Set up widgets.
  ui.setupUi(this);

  // Title.
  update_title();

  // Get layout settings
  EditorSettings settings;

  // Quest tree splitter.
  const bool quest_tree_visible = settings.get_value_bool(EditorSettings::quest_tree_visible);
  const int quest_tree_width = quest_tree_visible
    ? settings.get_value_int(EditorSettings::quest_tree_width)
    : 0;
  ui.quest_tree_splitter->setSizes({ quest_tree_width, width() - quest_tree_width });
  ui.quest_tree_splitter->setStretchFactor(0, 0);  // Don't expand the left panel
  ui.quest_tree_splitter->setStretchFactor(1, 1);  // but only the map view.
  ui.quest_tree_view->setVisible(quest_tree_visible);

  // Console splitter.
  const int console_height = 140; //settings.get_value_int(EditorSettings::console_height);
  ui.console_splitter->setSizes({ height() - console_height, console_height });
  ui.console_widget->setVisible(false);
  ui.console_widget->set_quest_runner(quest_runner);

  // Menu and toolbar actions.
  recent_quests_menu = new QMenu(tr("Recent quests"));
  update_recent_quests_menu();
  const QList<QAction*> actions = ui.menu_quest->actions();
  ui.menu_quest->insertMenu(actions[3], recent_quests_menu);
  ui.action_import->setEnabled(false);
  ui.action_package_quest->setEnabled(false);
  ui.action_open_quest_properties->setEnabled(false);

  QUndoGroup& undo_group = ui.tab_widget->get_undo_group();
  QAction* undo_action = undo_group.createUndoAction(this);
  undo_action->setIcon(QIcon(":/images/icon_undo.svg"));
  QAction* redo_action = undo_group.createRedoAction(this);
  redo_action->setIcon(QIcon(":/images/icon_redo.svg"));
  ui.menu_edit->insertAction(ui.action_cut, undo_action);
  ui.menu_edit->insertAction(ui.action_cut, redo_action);
  ui.menu_edit->insertSeparator(ui.action_cut);
  ui.tool_bar->insertAction(ui.action_run_quest, undo_action);
  ui.tool_bar->insertAction(ui.action_run_quest, redo_action);
  ui.tool_bar->insertSeparator(ui.action_run_quest);
  addAction(ui.action_run_quest);
  ui.action_run_quest->setEnabled(false);
  addAction(ui.action_run_map);
  ui.action_run_map->setEnabled(false);
  update_music_actions();

  zoom_button = new QToolButton();
  zoom_button->setIcon(QIcon(":/images/icon_zoom.svg"));
  zoom_button->setToolTip(tr("Zoom"));
  zoom_menu = create_zoom_menu();
  zoom_button->setMenu(zoom_menu);
  zoom_button->setPopupMode(QToolButton::InstantPopup);
  ui.tool_bar->insertWidget(ui.action_show_grid, zoom_button);
  ui.menu_view->insertMenu(ui.action_show_grid, zoom_menu);

  grid_size = new PairSpinBox();
  grid_size->config("x", 8, 99999, 8);
  grid_size->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed));

  ui.tool_bar->insertWidget(ui.action_show_layer_0, grid_size);
  ui.tool_bar->insertSeparator(ui.action_show_layer_0);

  ui.action_show_quest_files->setChecked(quest_tree_visible);
  ui.action_show_layer_0->setShortcutContext(Qt::WidgetShortcut);
  ui.action_show_layer_1->setShortcutContext(Qt::WidgetShortcut);
  ui.action_show_layer_2->setShortcutContext(Qt::WidgetShortcut);
  show_layers_button = new QToolButton();
  show_layers_button->setIcon(QIcon(":/images/icon_layer_more.svg"));
  show_layers_button->setToolTip(show_layers_button->text());
  show_layers_menu = new QMenu(tr("Show/hide more layers"));
  show_layers_button->setMenu(show_layers_menu);
  show_layers_button->setPopupMode(QToolButton::InstantPopup);
  show_layers_action = ui.tool_bar->insertWidget(ui.action_show_traversables, show_layers_button);
  lock_layers_menu = new QMenu(tr("Lock/unlock layers"));
  update_grouping_actions();
  ui.tool_bar->insertSeparator(ui.action_show_traversables);
  ui.menu_view->insertMenu(ui.action_show_traversables, show_layers_menu);
  ui.menu_view->insertMenu(ui.action_show_traversables, lock_layers_menu);
  ui.menu_view->insertSeparator(ui.action_show_traversables);

  show_entities_button = new QToolButton();
  show_entities_button->setIcon(QIcon(":/images/icon_glasses.svg"));
  show_entities_button->setToolTip(tr("Show/hide entity types"));
  show_entities_menu = create_show_entities_menu();
  show_entities_button->setMenu(show_entities_menu);
  show_entities_button->setPopupMode(QToolButton::InstantPopup);
  ui.tool_bar->insertWidget(ui.action_export_to_image, show_entities_button);
  ui.menu_view->addMenu(show_entities_menu);

  common_actions["cut"] = ui.action_cut;
  common_actions["copy"] = ui.action_copy;
  common_actions["paste"] = ui.action_paste;
  common_actions["undo"] = undo_action;
  common_actions["redo"] = redo_action;
  common_actions["group"] = ui.action_group;
  common_actions["ungroup"] = ui.action_ungroup;

  // Set standard keyboard shortcuts.
  ui.action_new_quest->setShortcut(QKeySequence::New);
  ui.action_close->setShortcut(QKeySequence::Close);
  ui.action_save->setShortcut(QKeySequence::Save);
  ui.action_exit->setShortcut(QKeySequence::Quit);
  undo_action->setShortcut(QKeySequence::Undo);
  redo_action->setShortcut(QKeySequence::Redo);
  ui.action_cut->setShortcut(QKeySequence::Cut);
  ui.action_copy->setShortcut(QKeySequence::Copy);
  ui.action_paste->setShortcut(QKeySequence::Paste);
  ui.action_select_all->setShortcut(QKeySequence::SelectAll);
  ui.action_unselect_all->setShortcut(QKeySequence::Deselect);
  ui.action_find->setShortcut(QKeySequence::Find);

  // Workaround for broken window shortcuts with appmenu-qt on Ubuntu.
  addAction(ui.action_new_quest);
  addAction(ui.action_load_quest);
  addAction(ui.action_close_quest);
  addAction(ui.action_exit);
  addAction(ui.action_close);
  addAction(ui.action_save);
  addAction(ui.action_import);
  addAction(ui.action_cut);
  addAction(ui.action_copy);
  addAction(ui.action_paste);
  addAction(ui.action_select_all);
  addAction(ui.action_unselect_all);
  addAction(ui.action_find);
  addAction(ui.action_save_all);
  addAction(ui.action_close_all);
  addAction(ui.action_open_quest_properties);
  addAction(ui.action_package_quest);
  addAction(ui.action_stop_music);
  addAction(ui.action_pause_music);
  addAction(ui.action_show_quest_files);
  addAction(ui.action_show_console);
  addAction(ui.action_show_grid);
  addAction(ui.action_show_layer_0);
  addAction(ui.action_show_layer_1);
  addAction(ui.action_show_layer_2);
  addAction(ui.action_export_to_image);
  addAction(ui.action_settings);
  addAction(ui.action_doc);
  addAction(ui.action_website);
  addAction(ui.action_about);

  // Connect children.
  connect(ui.quest_tree_view, &QuestTreeView::open_file_requested,
          ui.tab_widget, &EditorTabs::open_file_requested);
  connect(ui.quest_tree_view, &QuestTreeView::rename_file_requested,
          this, &MainWindow::rename_file_requested);
  connect(ui.quest_tree_view, &QuestTreeView::selected_path_changed,
          this, &MainWindow::selected_path_changed);

  connect(ui.tab_widget, &EditorTabs::currentChanged,
          this, &MainWindow::current_editor_changed);
  connect(ui.tab_widget, &EditorTabs::can_cut_changed,
          ui.action_cut, &QAction::setEnabled);
  connect(ui.tab_widget, &EditorTabs::can_copy_changed,
          ui.action_copy, &QAction::setEnabled);
  connect(ui.tab_widget, &EditorTabs::can_paste_changed,
          ui.action_paste, &QAction::setEnabled);
  connect(ui.tab_widget, &EditorTabs::can_group_ungroup_changed,
          this, &MainWindow::update_grouping_actions);
  connect(ui.tab_widget, &EditorTabs::refactoring_requested,
          this, &MainWindow::refactoring_requested);
  connect(ui.tab_widget, &EditorTabs::clear_console,
          ui.console_widget, &SolarusEditor::Console::clear);
  connect(ui.tab_widget, &EditorTabs::log_message_to_console,
          this, &MainWindow::log_message_to_console);
  connect(ui.tab_widget, &EditorTabs::run_map_requested,
          this, &MainWindow::run_quest);
  connect(ui.tab_widget, &EditorTabs::new_quest_requested,
          ui.action_new_quest, &QAction::trigger);
  connect(ui.tab_widget, &EditorTabs::open_quest_requested,
          ui.action_load_quest, &QAction::trigger);
  connect(ui.tab_widget, &EditorTabs::documentation_requested,
          ui.action_doc, &QAction::trigger);
  connect(ui.tab_widget, &EditorTabs::website_requested,
          ui.action_website, &QAction::trigger);

  connect(grid_size, &PairSpinBox::value_changed,
          this, &MainWindow::change_grid_size);

  connect(&quest_runner, &SolarusEditor::QuestRunner::running,
          this, &MainWindow::quest_running);
  connect(&quest_runner, &SolarusEditor::QuestRunner::finished,
          this, &MainWindow::quest_finished);

  connect(&quest, &Quest::current_music_changed,
          this, &MainWindow::current_music_changed);

  connect(&settings_dialog, &SettingsDialog::settings_changed,
          this, &MainWindow::reload_settings);

  connect(ui.quest_tree_splitter, &QSplitter::splitterMoved,
          this, &MainWindow::quest_tree_resized);

  // No editor initially.
  current_editor_changed(-1);

  // Check that we can find the assets directory.
  FileTools::initialize_assets();
  if (FileTools::get_assets_path().isEmpty()) {
    GuiTools::warning_dialog(tr("Could not locate the assets directory.\n"
                              "Some features like creating a new quest will not be available.\n"
                              "Please make sure that Solarus Editor is correctly installed."));
  }

  // Exceptions for automatic icon coloring.
  for (QWidget* widget : std::vector<QWidget*>{
           ui.tool_bar,
           ui.menu_bar,
           ui.menu_tools,
           ui.menuAudio,
           ui.menu_help,
           ui.menu_edit,
           ui.menu_run,
           ui.menu_view,
           ui.menu_quest,
       }) {
    EditorStyle::setAutoIconColor(widget, EditorStyle::AutoIconColor::ForegroundColor);
  }

  // Prevent Qt's default toolbar context menu that allows to hide the QToolBar.
  // It should NOT be able to hide.
  ui.tool_bar->installEventFilter(this);
}

/**
 * @brief Destructor of main window.
 */
MainWindow::~MainWindow() {

}

/**
 * @brief Returns the editor of the current tab, if any.
 * @return The current editor or nullptr.
 */
Editor* MainWindow::get_current_editor() {

  return ui.tab_widget->get_editor();
}

/**
 * @brief Builds or rebuilds the recent quests menu.
 *
 * Disables it if there is no recent quest.
 */
void MainWindow::update_recent_quests_menu() {

  if (recent_quests_menu == nullptr) {
    return;
  }

  // Get the recent quest list.
  EditorSettings settings;
  const QStringList& last_quests = settings.get_value_string_list(EditorSettings::last_quests);

  // Clear previous actions.
  const QList<QAction*> old_actions = recent_quests_menu->actions();
  for (QAction* action : old_actions) {
    // Let enough time to properly delete FlashActionHelper.
    action->deleteLater();
  }
  recent_quests_menu->clear();

  // Disable if there is no recent quest.
  recent_quests_menu->setEnabled(!last_quests.isEmpty());

  // Create new actions.
  for (const QString& quest_path : last_quests) {

    QAction* action = new QAction(quest_path, this);
    connect(action, &QAction::triggered, this, [this, quest_path]() {

      // Close the previous quest and open the new one.
      if (confirm_before_closing()) {
        close_quest();
        open_quest(quest_path);
      }

      ui.menu_quest->close();
    });

    recent_quests_menu->addAction(action);
  }
}

/**
 * @brief Creates a menu with zoom actions.
 * @return The created menu. It has no parent initially.
 */
QMenu* MainWindow::create_zoom_menu() {

  QMenu* zoom_menu = new QMenu(tr("Zoom"));
  std::vector<std::pair<QString, double>> zooms = {
    { tr("25 %"), 0.25 },
    { tr("50 %"), 0.5 },
    { tr("100 %"), 1.0 },
    { tr("200 %"), 2.0 },
    { tr("400 %"), 4.0 }
  };
  QActionGroup* action_group = new QActionGroup(zoom_menu);
  for (const std::pair<QString, double>& zoom : zooms) {
    QAction* action = new QAction(zoom.first, action_group);
    zoom_actions[zoom.second] = action;
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, [this, zoom]() {
      Editor* editor = get_current_editor();
      if (editor != nullptr) {
        editor->get_view_settings().set_zoom(zoom.second);
      }
    });
    zoom_menu->addAction(action);
  }

  return zoom_menu;
}

/**
 * @brief Updates the show layers menu to match the current range of layers.
 */
void MainWindow::update_show_layers_menu() {

  if (show_layers_menu == nullptr) {
    return;
  }

  // Clear previous actions.
  const QList<QAction*>& actions = show_layers_menu->actions();
  for (QAction* action : actions) {
    delete action;
  }
  show_layers_menu->clear();

  // Add special actions Show all and Hide all.
  QAction* show_all_action = new QAction(tr("Show all layers"), this);
  show_layers_subactions["action_show_all"] = show_all_action;
  show_layers_menu->addAction(show_all_action);
  connect(show_all_action, &QAction::triggered, this, [this]() {
    Editor* editor = get_current_editor();
    if (editor != nullptr) {
      editor->get_view_settings().show_all_layers();
    }
  });

  QAction* hide_all_action = new QAction(tr("Hide all layers"), this);
  show_layers_subactions["action_hide_all"] = hide_all_action;
  show_layers_menu->addAction(hide_all_action);
  connect(hide_all_action, &QAction::triggered, this, [this]() {
    Editor* editor = get_current_editor();
    if (editor != nullptr) {
      editor->get_view_settings().hide_all_layers();
    }
  });

  show_layers_menu->addSeparator();

  // Add an action for each layer.
  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    int min_layer = 0;
    int max_layer = 0;
    editor->get_layers_supported(min_layer, max_layer);
    for (int i = min_layer; i <= max_layer; ++i) {
      QAction* action = new QAction(tr("Show layer %1").arg(i), this);
      if (i >= 0 && i < 3) {
        // Layers 0, 1 and 2 have an icon.
        QString file_name = QString(":/images/icon_layer_%1.svg").arg(i);
        action->setIcon(QIcon(file_name));
      }
      // Layers -4 to 5 have a shortcut.
      if (i >= -4 && i <= 5) {
        const int digit = (i >= 0) ? i : (10 + i);
        action->setShortcut(QString::number(digit));
        addAction(action);
      }
      action->setCheckable(true);
      action->setChecked(true);
      show_layers_menu->addAction(action);
      connect(action, &QAction::triggered, this, [this, action, i]() {
        Editor* editor = get_current_editor();
        if (editor != nullptr) {
          const bool visible = action->isChecked();
          editor->get_view_settings().set_layer_visible(i, visible);
        }
      });
    }
  }

  // set_menu_bar_menu_icons_visibility(show_layers_menu, !menuBar()->isNativeMenuBar());
}

/**
 * @brief Updates the lock layers menu to match the current range of layers.
 */
void MainWindow::update_lock_layers_menu() {

  if (lock_layers_menu == nullptr) {
    return;
  }

  // Clear previous actions.
  const QList<QAction*>& actions = lock_layers_menu->actions();
  for (QAction* action : actions) {
    delete action;
  }
  lock_layers_menu->clear();

  // TODO add an "Unlock all layers" action.

  // Add an action for each layer.
  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    int min_layer = 0;
    int max_layer = 0;
    editor->get_layers_supported(min_layer, max_layer);
    for (int i = min_layer; i <= max_layer; ++i) {
      QAction* action = new QAction(tr("Lock layer %1").arg(i), this);
      // TODO add an icon.
      if (i >= -4 && i <= 5) {
        const int digit = (i >= 0) ? i : (10 + i);
        // Layers -4 to 5 have a shortcut.
        action->setShortcut(tr("Ctrl+%1").arg(QString::number(digit)));
        addAction(action);
      }
      action->setCheckable(true);
      action->setChecked(false);
      lock_layers_menu->addAction(action);
      connect(action, &QAction::triggered, this, [this, action, i]() {
        Editor* editor = get_current_editor();
        if (editor != nullptr) {
          const bool locked = action->isChecked();
          editor->get_view_settings().set_layer_locked(i, locked);
        }
      });
    }
  }

  // set_menu_bar_menu_icons_visibility(lock_layers_menu, !menuBar()->isNativeMenuBar());
}

/**
 * @brief Creates a menu with actions to show or hide each entity type.
 * @return The created menu. It has no parent initially.
 */
QMenu* MainWindow::create_show_entities_menu() {

  QMenu* menu = new QMenu(tr("Show/hide entity types"));

  // Add show entity types actions to the menu.
  const QList<QAction*>& entity_actions = EnumMenus<EntityType>::create_actions(
        *menu,
        EnumMenuCheckableOption::CHECKABLE,
        [this](EntityType type) {
    Editor* editor = get_current_editor();
    if (editor != nullptr) {
      QString type_name = EntityTraits::get_lua_name(type);
      const bool visible = show_entities_subactions[type_name]->isChecked();
      editor->get_view_settings().set_entity_type_visible(type, visible);
    }
  });

  for (QAction* action : entity_actions) {
    EntityType type = static_cast<EntityType>(action->data().toInt());
    if (!EntityTraits::can_be_stored_in_map_file(type)) {
      // Only show the ones that can exist in map files.
      menu->removeAction(action);
      continue;
    }
    const QString& shortcut = EntityTraits::get_show_hide_shortcut(type);
    action->setShortcut(shortcut);
    QString type_name = EntityTraits::get_lua_name(type);
    show_entities_subactions[type_name] = action;
  }

  // Add special actions Show all and Hide all.
  QAction* show_all_action = new QAction(
      tr("Show all entities"), this);
  show_entities_subactions["action_show_all"] = show_all_action;
  menu->insertAction(entity_actions.first(), show_all_action);
  connect(show_all_action, &QAction::triggered, this, [this]() {
    Editor* editor = get_current_editor();
    if (editor != nullptr) {
      editor->get_view_settings().show_all_entity_types();
    }
  });

  QAction* hide_all_action = new QAction(
      tr("Hide all entities"), this);
  show_entities_subactions["action_hide_all"] = hide_all_action;
  menu->insertAction(entity_actions.first(), hide_all_action);
  connect(hide_all_action, &QAction::triggered, this, [this]() {
    Editor* editor = get_current_editor();
    if (editor != nullptr) {
      editor->get_view_settings().hide_all_entity_types();
    }
  });

  menu->insertSeparator(entity_actions.first());

  return menu;
}

/**
 * @brief Sets an appropriate size and centers the window on the screen having
 * the mouse.
 */
void MainWindow::initialize_geometry_on_screen() {

  // Get the screen where the cursor is currently located.
  QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
  if (screen == nullptr) {
    // Fallback to the primary screen if no screen is found.
    screen = QGuiApplication::primaryScreen();
  }

  QRect screen_geometry = screen->geometry();


  // Choose a comfortable initial size depending on the screen resolution.
  // The ui is designed to work well with a window size of 1280x680 and above.
  int width = 1270;
  int height = 680;
  if (screen_geometry.width() >= 1920) {
    width = 1500;
  }
  if (screen_geometry.height() >= 1024) {
    height = 980;
  }
  setGeometry(0, 0, qMin(width, screen_geometry.width()), qMin(height, screen_geometry.height()));

  // And center the window on the screen where the mouse is currently.
  int x = screen_geometry.width() / 2 - frameGeometry().width() / 2 + screen_geometry.left() - 2;
  int y = screen_geometry.height() / 2 - frameGeometry().height() / 2 + screen_geometry.top() - 10;

  move(qMax(0, x), qMax(0, y));
}

/**
 * @brief Returns the current quest open in the window.
 * @return The current quest, or an invalid quest if no quest is open.
 */
Quest& MainWindow::get_quest() {
  return quest;
}

/**
 * @brief Closes the current quest if any and without confirmation.
 */
void MainWindow::close_quest() {

  ui.tab_widget->close_without_confirmation();

  if (quest.exists()) {
    disconnect(&quest, &Quest::file_renamed,
               ui.tab_widget, &EditorTabs::file_renamed);
    disconnect(&quest, &Quest::file_deleted,
               ui.tab_widget, &EditorTabs::file_deleted);
  }

  quest.set_root_path("");
  update_title();
  ui.action_import->setEnabled(false);
  ui.action_package_quest->setEnabled(false);
  ui.action_open_quest_properties->setEnabled(false);
  ui.action_run_quest->setEnabled(false);
  ui.action_run_map->setEnabled(false);
  ui.quest_tree_view->set_quest(quest);
  ui.console_widget->clear();

  EditorSettings settings;
  settings.set_value(EditorSettings::current_quest, "");
}

/**
 * @brief Opens a quest.
 *
 * Shows an error dialog if the quest could not be opened.
 *
 * @param quest_path Path of the quest to open.
 * @return @c true if the quest was successfully opened.
 */
bool MainWindow::open_quest(const QString& quest_path) {

  bool success = false;

  try {
    // Load the requested quest.
    quest.set_root_path(quest_path);

    if (!quest.exists()) {
      throw EditorException(tr("No quest was found in directory\n'%1'").arg(quest_path));
    }
    quest.check_version();

    // Make sure all resource directories exist.
    for (ResourceType resource_type : Solarus::EnumInfo<ResourceType>::enums()) {
      if (QFileInfo(quest.get_data_path()).isWritable()) {
        quest.create_dir_if_not_exists(quest.get_resource_path(resource_type));
      }
    }

    connect(&quest, &Quest::file_renamed,
            ui.tab_widget, &EditorTabs::file_renamed);
    connect(&quest, &Quest::file_deleted,
            ui.tab_widget, &EditorTabs::file_deleted);

    ui.action_import->setEnabled(true);
    ui.action_package_quest->setEnabled(true);
    ui.action_open_quest_properties->setEnabled(true);
    ui.action_run_quest->setEnabled(true);

    add_quest_to_recent_list();
    EditorSettings settings;
    settings.set_value(EditorSettings::current_quest, quest_path);

    success = true;
  }
  catch (const ObsoleteEditorException& ex) {
    ex.show_dialog();
  }
  catch (const ObsoleteQuestException& ex) {
    // Quest data files are obsolete: upgrade them and try again.
    if (confirm_upgrade_quest(ex.get_quest_format())) {
      try {
        upgrade_quest();
        // Reload the quest after upgrade.
        quest.set_root_path("");
        quest.set_root_path(quest_path);
        quest.check_version();
        ui.action_import->setEnabled(true);
        ui.action_package_quest->setEnabled(true);
        ui.action_open_quest_properties->setEnabled(true);
        ui.action_run_quest->setEnabled(true);
        success = true;
      }
      catch (const EditorException& ex) {
        // Upgrade failed.
        ex.show_dialog();
      }
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }

  if (!success) {
    quest.set_root_path("");
  }

  update_title();
  ui.quest_tree_view->set_quest(quest);

  return success;
}

/**
 * @brief Check if the user wants to upgrade an obsolete quest.
 * @param old_format The quest format to upgrade from.
 * @return @c true if the quest should be upgraded, @c false otherwise.
 */
bool MainWindow::confirm_upgrade_quest(const QString& old_format) {
  QMessageBox dialog(
      QMessageBox::Question,
      tr("Obsolete quest"),
      tr("The format of this quest (%1) is outdated.\n"
         "Your data files will be automatically updated to Solarus %2.\n"
         "Would you like to update this quest?").
         arg(old_format, SOLARUS_VERSION_WITHOUT_PATCH),
      QMessageBox::Ok | QMessageBox::Cancel,
      this);
  dialog.setDefaultButton(QMessageBox::Ok);
  dialog.button(QMessageBox::Ok)->setText(tr("Update"));

  int result = dialog.exec();

  return (QMessageBox::Ok == result);
}

/**
 * @brief Attempts to upgrade the quest to the latest version and shows the
 * result in a dialog.
 */
void MainWindow::upgrade_quest() {

  // First backup the files.
  QString quest_version = quest.get_properties().get_solarus_version_without_patch();
  QString root_path = quest.get_root_path();
  QString backup_dir_name = "data." + quest_version + ".bak";
  QString backup_path = root_path + "/" + backup_dir_name;

  FileTools::delete_recursive(backup_path);  // Remove any previous backup.
  FileTools::copy_recursive(quest.get_data_path(), backup_path);

  // Upgrade data files.
  ExternalScriptDialog dialog(
        tr("Upgrading quest data files"),
        ":/quest_converter/update_quest",
        root_path);

  dialog.exec();
  bool upgrade_success = dialog.is_successful();

  if (!upgrade_success) {
    // The upgrade failed.
    // Restore the backuped version.
    QDir root_dir(root_path);
    FileTools::delete_recursive(root_path + "/data.err");
    root_dir.rename("data", "data.err");
    FileTools::delete_recursive(root_path + "/data");
    root_dir.rename(backup_dir_name, "data");

    throw EditorException(
          tr("An error occurred while upgrading the quest.\n"
             "Your quest was kept unchanged in format %1.").arg(quest_version));
  }
}

/**
 * @brief Adds the quest path to the list of recent quests.
 *
 * Moves it to the beginning of the list if it is already presents.
 * Keeps the number of recent quests in the list limited.
 */
void MainWindow::add_quest_to_recent_list() {

  EditorSettings settings;
  QStringList last_quests = settings.get_value_string_list(EditorSettings::last_quests);
  QString quest_path = get_quest().get_root_path();

  if (!last_quests.isEmpty() && last_quests.first() == quest_path) {
    // Nothing to do.
    return;
  }

  // Remove if already present.
  if (last_quests.contains(quest_path)) {
    last_quests.removeAll(quest_path);
  }

  // Add to the beginning of the list.
  last_quests.prepend(quest_path);

  // Keep the list limited to 10 quests.
  constexpr int max = 10;
  while (last_quests.size() > max) {
    last_quests.removeLast();
  }

  settings.set_value(EditorSettings::last_quests, last_quests);

  // Rebuild the recent quest menu.
  update_recent_quests_menu();
}

/**
 * @brief Slot called when the user triggers the "New quest" action.
 */
void MainWindow::on_action_new_quest_triggered() {

  if (FileTools::get_assets_path().isEmpty()) {
    GuiTools::error_dialog(tr("Could not find the assets directory.\nMake sure that Solarus Editor is properly installed."));
    return;
  }

  // Check unsaved files but don't close them yet
  // in case the user cancels.
  if (!confirm_before_closing()) {
    return;
  }

  // Decide the default directory to create the quest in.
  QString default_path = quest.get_root_path();
  QDir default_dir(default_path);
  if (!default_path.isEmpty() && default_dir.cdUp()) {
    default_path = default_dir.absolutePath();
  } else {
    EditorSettings settings;
    default_path = settings.get_value_string(EditorSettings::working_directory);
  }

  // Fallback to the user's Documents folder.
  if (default_path.isEmpty()) {
    default_path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  }

  // Open the new quest dialog and then get its results.
  NewQuestDialog new_quest_dialog(default_path, this);
  if (QDialog::Rejected == new_quest_dialog.exec()) {
    return;
  }

  close_quest();

  try {
    // Create the quest directory and its contents.
    const NewQuestBuilder::NewQuestConfig& config = new_quest_dialog.get_new_quest_config();
    NewQuestBuilder::create_initial_quest_files(config);
    if (open_quest(config.quest_path)) {
      // Open the quest properties editor initially.
      open_file(quest, quest.get_data_path());

      // Save the working directory.
      const QString quest_dir = QFileInfo(config.quest_path).absoluteDir().absolutePath();
      EditorSettings settings;
      settings.set_value(EditorSettings::working_directory, quest_dir);
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Slot called when the user triggers the "Load quest" action.
 */
void MainWindow::on_action_load_quest_triggered() {

  // Check unsaved files but don't close them yet
  // in case the user cancels.
  if (!confirm_before_closing()) {
    return;
  }

  // Ask the quest path.
  EditorSettings settings;
  QString quest_path = QFileDialog::getExistingDirectory(
        this,
        tr("Select quest directory"),
        settings.get_value_string(EditorSettings::working_directory),
#ifdef SOLARUSEDITOR_NO_NATIVE_DIALOGS
        {QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog}
#else
        {QFileDialog::ShowDirsOnly}
#endif
  );

  if (quest_path.isEmpty()) {
    // Canceled.
    return;
  }

  close_quest();
  open_quest(quest_path);
}

/**
 * @brief Slot called when the user triggers the "Close quest" action.
 */
void MainWindow::on_action_close_quest_triggered() {

  // Check unsaved files.
  if (!confirm_before_closing()) {
    return;
  }

  close_quest();
}

/**
 * @brief Slot called when the user triggers the "Re-open closed tab" action.
 */
void MainWindow::on_action_reopen_closed_tab_triggered() {

  ui.tab_widget->reopen_last_closed_editor(get_quest());
}

/**
 * @brief Slot called when the user triggers the "Save" action.
 */
void MainWindow::on_action_save_triggered() {

  int index = ui.tab_widget->currentIndex();
  if (index == -1) {
    return;
  }
  ui.tab_widget->save_file_requested(index);
}

/**
 * @brief Slot called when the user triggers the "Save all" action.
 */
void MainWindow::on_action_save_all_triggered() {

  ui.tab_widget->save_all_files_requested();
}

/**
 * @brief Slot called when the user triggers the "Close" action.
 */
void MainWindow::on_action_close_triggered() {

  int index = ui.tab_widget->currentIndex();
  if (index == -1) {
    return;
  }

  ui.tab_widget->close_file_requested(index);
}

/**
 * @brief Slot called when the user triggers the "Close all" action.
 */
void MainWindow::on_action_close_all_triggered() {

  ui.tab_widget->close_all_files_requested();
}

/**
 * @brief Slot called when the user triggers the "Import" action.
 */
void MainWindow::on_action_import_triggered() {

  Quest& quest = get_quest();
  if (!quest.exists()) {
    // No valid quest is currently open.
    return;
  }

  ImportDialog import_dialog(quest);

  // If the user wants to rename something from the dialog's quest tree,
  // we need to handle it from here in order to check open files
  // and perform refactoring if necessary.
  connect(&import_dialog, &ImportDialog::destination_quest_rename_file_requested,
          this, &MainWindow::rename_file_requested);

  import_dialog.exec();
}

/**
 * @brief Slot called when the user triggers the "Quest properties" action.
 */
void MainWindow::on_action_open_quest_properties_triggered() {

  ui.tab_widget->open_quest_properties_editor(quest);
}

/**
 * @brief Slot called when user triggers the "Package Quest" action.
 */
void MainWindow::on_action_package_quest_triggered() {

  PackageDialog package(quest, this);
  package.exec();
}

/**
 * @brief Slot called when the user triggers the "Exit" action.
 */
void MainWindow::on_action_exit_triggered() {

  if (confirm_before_closing()) {
    ui.tab_widget->save_open_files_list();
    ui.tab_widget->close_without_confirmation();
    QApplication::exit(0);
  }
}

/**
 * @brief Slot called when the user triggers the "Cut" action.
 */
void MainWindow::on_action_cut_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->cut();
  }
}

/**
 * @brief Slot called when the user triggers the "Copy" action.
 */
void MainWindow::on_action_copy_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->copy();
  }
}

/**
 * @brief Slot called when the user triggers the "Paste" action.
 */
void MainWindow::on_action_paste_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->paste();
  }
}

/**
 * @brief Called when the user triggers the "Group" action.
 */
void MainWindow::on_action_group_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->group();
  }
}

/**
 * @brief Called when the user triggers the "Ungroup" action.
 */
void MainWindow::on_action_ungroup_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->ungroup();
  }
}

/**
 * @brief Slot called when the user triggers the "Select all" action.
 */
void MainWindow::on_action_select_all_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->select_all();
  }
}

/**
 * @brief Slot called when the user triggers the "Unselect all" action.
 */
void MainWindow::on_action_unselect_all_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->unselect_all();
  }
}

/**
 * @brief Slot called when the user triggers the "Find" action.
 */
void MainWindow::on_action_find_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->find();
  }
}

/**
 * @brief Slot called when the user triggers the "Run quest" action.
 */
void MainWindow::on_action_run_quest_triggered() {
  run_quest("");
}

/**
 * @brief Slot called when the user triggers the "Run current map" action.
 */
void MainWindow::on_action_run_map_triggered() {
  Editor* editor = get_current_editor();
  if (editor != nullptr) {
    editor->run_map();
  }
}

/**
 * @brief Runs the quest, possibly on a specific map.
 * @param map_id A map to run, or an empty string to run the quest normally.
 */
void MainWindow::run_quest(const QString& map_id) {

  if (!quest_runner.is_started()) {

    if (ui.tab_widget->has_unsaved_files()) {

      EditorSettings settings;
      const QString& save_files = settings.get_value_string(EditorSettings::save_files_before_running);

      QMessageBox::StandardButton answer = QMessageBox::Yes;
      if (save_files == "yes") {
        answer = QMessageBox::Yes;
      }
      else if (save_files == "no") {
        answer = QMessageBox::No;
      }
      else {
        answer = QMessageBox::warning(
              this,
              tr("Files are modified"),
              tr("Do you want to save modifications before running the quest?"),
              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
              QMessageBox::Yes
              );
      }
      if (answer == QMessageBox::Cancel) {
        return;
      }

      if (answer == QMessageBox::Yes) {
        ui.tab_widget->save_all_files_requested();
      }
    }

    quest_runner.start(quest.get_root_path(), map_id);

    // Automatically show the console when the quest starts.
    set_console_visible(true);
    ui.action_show_console->setChecked(true);
  }
  else {
    quest_runner.stop();
  }
  update_run_quest();
}

/**
 * @brief Slot called when the user triggers the "Play music"
 * or "Stop music" action.
 */
void MainWindow::on_action_stop_music_triggered() {

  Quest& quest = get_quest();
  if (Audio::is_playing_music(quest)) {
    // A music is playing: stop it.
    Audio::stop_music(quest);
  }
  else {
    // No music is playing: play the selected one if any.
    const QString& selected_path = ui.quest_tree_view->get_selected_path();
    ResourceType resource_type;
    QString element_id;
    if (!selected_path.isEmpty() &&
        get_quest().is_potential_resource_element(selected_path, resource_type, element_id) &&
        resource_type == ResourceType::MUSIC &&
        get_quest().exists(selected_path)) {
      Audio::play_music(quest, element_id);
    }
  }
}

/**
 * @brief Slot called when the user triggers the "Show grid" action.
 */
void MainWindow::on_action_show_grid_triggered() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  editor->get_view_settings().set_grid_visible(ui.action_show_grid->isChecked());
}

/**
 * @brief Slot called when the user triggers the "Show quest files" action.
 */
void MainWindow::on_action_show_quest_files_triggered() {

  // Show or hide the quest file tree
  const bool show_quest_files = ui.action_show_quest_files->isChecked();
  set_quest_files_visible(show_quest_files);
}

/**
 * @brief Slot called when the user triggers the "Show console" action.
 */
void MainWindow::on_action_show_console_triggered() {

  // Show or hide the console.
  const bool show_console = ui.action_show_console->isChecked();
  set_console_visible(show_console);
}

/**
 * @brief Returns whether the quest file tree is shown.
 * @return @c true if the quest file tree is visible.
 */
bool MainWindow::is_quest_files_visible() const {

  return ui.quest_tree_view->isVisible();
}

/**
 * @brief Shows or hide the quest file tree.
 * @param quest_files_visible @c true to show the file tree.
 */
void MainWindow::set_quest_files_visible(bool visible) {

  EditorSettings settings;
  const int tree_width = settings.get_value_int(EditorSettings::quest_tree_width);

  if (!visible) {
    ui.quest_tree_splitter->setSizes({ 0, width() });
  } else {
    ui.quest_tree_splitter->setSizes({ tree_width, width() - tree_width });
  }

  ui.quest_tree_view->setVisible(visible);
  settings.set_value(EditorSettings::quest_tree_visible, visible);
}

/**
 * @brief Returns whether the execution console is shown.
 * @return @c true if the execution console is visible.
 */
bool MainWindow::is_console_visible() const {

  return ui.console_widget->isVisible();
}

/**
 * @brief Shows or hide the execution console.
 * @param console_visible @c true to show the console.
 */
void MainWindow::set_console_visible(bool console_visible) {

  if (!console_visible &&
      ui.console_widget->height() < 16) {
    // Hiding a very small console: make sure it gets a decent size
    // when restored later.
    const int console_height = 100;
    ui.console_splitter->setSizes({ height() - console_height, console_height });
  }

  ui.console_widget->setVisible(console_visible);
}

/**
 * @brief Adds a message to the console and shows it.
 * @param log_level Log level of the message.
 * @param message The message to log.
 */
void MainWindow::log_message_to_console(const QString& log_level, const QString& message) {

  set_console_visible(true);
  ui.console_widget->add_message(log_level, message);
}

/**
 * @brief Slot called when the user changes the grid size.
 */
void MainWindow::change_grid_size() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  editor->get_view_settings().set_grid_size(grid_size->get_size());
}

/**
 * @brief Slot called when the user triggers the "Show low layer" action.
 */
void MainWindow::on_action_show_layer_0_triggered() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  editor->get_view_settings().set_layer_visible(0, ui.action_show_layer_0->isChecked());
}

/**
 * @brief Slot called when the user triggers the "Show intermediate layer" action.
 */
void MainWindow::on_action_show_layer_1_triggered() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  editor->get_view_settings().set_layer_visible(1, ui.action_show_layer_1->isChecked());
}

/**
 * @brief Slot called when the user triggers the "Show high layer" action.
 */
void MainWindow::on_action_show_layer_2_triggered() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  editor->get_view_settings().set_layer_visible(2, ui.action_show_layer_2->isChecked());
}

/**
 * @brief Slot called when the user triggers the "Show traversable entities" action.
 */
void MainWindow::on_action_show_traversables_triggered() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  editor->get_view_settings().set_traversables_visible(ui.action_show_traversables->isChecked());
}

/**
 * @brief Slot called when the user triggers the "Show obstacle entities" action.
 */
void MainWindow::on_action_show_obstacles_triggered() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  editor->get_view_settings().set_obstacles_visible(ui.action_show_obstacles->isChecked());
}

/**
 * @brief Slot called when the user triggers the "Export to image" action.
 */
void MainWindow::on_action_export_to_image_triggered() {

  Editor* editor = get_current_editor();
  if (editor != nullptr && editor->is_export_to_image_supported()) {
    editor->export_to_image();
  }
}
/**
 * @brief Slot called when the user triggers the "Settings" action.
 */
void MainWindow::on_action_settings_triggered() {

  settings_dialog.exec();
}

/**
 * @brief Slot called when the user triggers the "Website" action.
 */
void MainWindow::on_action_website_triggered() {

  QDesktopServices::openUrl(QUrl("http://www.solarus-games.org/"));
}

/**
 * @brief Slot called when the user triggers the "Welcome" action.
 */
void MainWindow::on_action_welcome_triggered() {

  ui.tab_widget->open_welcome_editor_requested(get_quest());
}

/**
 * @brief Slot called when the user triggers the "Website" action.
 */
void MainWindow::on_action_about_triggered() {

  auto* dialog = new oclero::qlementine::AboutDialog(qApp->activeWindow());
  dialog->setIcon(QIcon(":/app_icon/solarus-editor-1024.png"));
  dialog->setWindowTitle(QApplication::translate("SolarusEditor::AboutDialog",
     "About %0").arg(QApplication::applicationDisplayName()));
  dialog->setWebsiteUrl(SOLARUSEDITOR_WEBSITE);
  dialog->setDescription(QApplication::translate("SolarusEditor::AboutDialog",
     "Integrated development environment for Solarus, a free and open-source ARPG 2D game engine."));
  dialog->setLicense("GPL-3.0 and CC-BY-SA-4.0");
  dialog->setCopyright(QString("%1 %2").arg(SOLARUSEDITOR_COPYRIGHT, tr("All rights reserved.")));

  EditorStyle::setAutoIconColor(dialog, EditorStyle::AutoIconColor::ForegroundColor);

  for (const auto& [tooltip, url, icon] : {
         std::make_tuple("X", "https://x.com/solarusgames", ":/images/icon_x.svg"),
         std::make_tuple("Mastodon", "https://mastodon.gamedev.place/@solarus", ":/images/icon_mastodon.svg"),
         std::make_tuple("YouTube", "https://www.youtube.com/c/ChristophoZS", ":/images/icon_youtube.svg"),
         std::make_tuple("GitLab", "https://gitlab.com/solarus-games", ":/images/icon_gitlab.svg"),
       }) {
    QIcon q_icon(icon);
    dialog->addSocialMediaLink(tooltip, url, q_icon);
  }
  dialog->show();
}

/**
 * @brief Helper that offers to go online for documentation.
 */
static void offer_online_docs(MainWindow *parent) {

  QMessageBox::StandardButton answer = QMessageBox::question(
      parent,
      MainWindow::tr("Local documentation not found"),
      MainWindow::tr(
          "The local copy of Solarus Documentation could not be found. "
          "Would you like to try going online to find the documentation?"),
      QMessageBox::Ok | QMessageBox::Cancel,
      QMessageBox::Ok
  );

  if (QMessageBox::Ok == answer) {
    QDesktopServices::openUrl(QUrl("https://docs.solarus-games.org"));
  }
}

/**
 * @brief Slot called when the user triggers the "Documentation" action.
 */
void MainWindow::on_action_doc_triggered() {

  const QString& assets_path = FileTools::get_assets_path();
  const QString& doc_path = assets_path + "/doc/index.html";
  if (!assets_path.isEmpty() && QFile::exists(doc_path)) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(doc_path));
  } else {
    offer_online_docs(this);
  }
}

/**
 * @brief Slot called when the current editor changes.
 * @param index Index of the new current editor, or -1 if no editor is active.
 */
void MainWindow::current_editor_changed(int index) {

  Q_UNUSED(index);

  Editor* editor = get_current_editor();
  const bool has_editor = editor != nullptr;

  // Set up toolbar buttons for this editor.
  ui.action_cut->setEnabled(has_editor);
  ui.action_copy->setEnabled(has_editor);
  ui.action_paste->setEnabled(has_editor);
  ui.action_reopen_closed_tab->setEnabled(!ui.tab_widget->get_last_closed_file().isEmpty());
  ui.action_close->setEnabled(has_editor);
  ui.action_close_all->setEnabled(has_editor);
  ui.action_save->setEnabled(has_editor);
  ui.action_save_all->setEnabled(has_editor);

  const bool export_to_image_supported = has_editor && editor->is_export_to_image_supported();
  ui.action_export_to_image->setEnabled(export_to_image_supported);

  const bool save_supported = has_editor && editor->is_save_supported();
  ui.action_save->setEnabled(save_supported);

  const bool select_all_supported = has_editor && editor->is_select_all_supported();
  ui.action_select_all->setEnabled(select_all_supported);

  update_run_quest();

  const bool find_supported = has_editor && editor->is_find_supported();
  ui.action_find->setEnabled(find_supported);

  const bool zoom_supported = has_editor && editor->is_zoom_supported();
  zoom_menu->setEnabled(zoom_supported);
  zoom_button->setEnabled(zoom_supported);

  const bool grid_supported = has_editor && editor->is_grid_supported();
  ui.action_show_grid->setEnabled(grid_supported);
  if (!grid_supported) {
    ui.action_show_grid->setChecked(false);
  }
  grid_size->setEnabled(ui.action_show_grid->isChecked());

  update_layer_range();

  const bool traversables_visibility_supported =
      has_editor && editor->is_traversables_visibility_supported();
  ui.action_show_traversables->setEnabled(traversables_visibility_supported);
  if (!traversables_visibility_supported) {
    ui.action_show_traversables->setChecked(false);
  }
  const bool obstacles_visibility_supported =
      has_editor && editor->is_obstacles_visibility_supported();
  ui.action_show_obstacles->setEnabled(obstacles_visibility_supported);
  if (!obstacles_visibility_supported) {
    ui.action_show_obstacles->setChecked(false);
  }

  bool entity_type_visibility_supported =
      has_editor && editor->is_entity_type_visibility_supported();
  show_entities_menu->setEnabled(entity_type_visibility_supported);
  show_entities_button->setEnabled(entity_type_visibility_supported);

  if (has_editor) {
    ViewSettings& view_settings = editor->get_view_settings();

    connect(&view_settings, &ViewSettings::zoom_changed,
            this, &MainWindow::update_zoom);
    update_zoom();

    connect(&view_settings, &ViewSettings::grid_visibility_changed,
            this, &MainWindow::update_grid_visibility);
    update_grid_visibility();
    connect(&view_settings, &ViewSettings::grid_size_changed,
            this, &MainWindow::update_grid_size);
    update_grid_size();

    connect(&view_settings, &ViewSettings::layer_range_changed,
            this, &MainWindow::update_layer_range);
    connect(&view_settings, &ViewSettings::layer_visibility_changed,
            this, &MainWindow::update_layer_visibility);
    connect(&view_settings, &ViewSettings::layer_locking_changed,
            this, &MainWindow::update_layer_locking);
    update_layers_visibility();
    update_layers_locking();

    connect(&view_settings, &ViewSettings::traversables_visibility_changed,
            this, &MainWindow::update_traversables_visibility);
    update_traversables_visibility();
    connect(&view_settings, &ViewSettings::obstacles_visibility_changed,
            this, &MainWindow::update_obstacles_visibility);
    update_obstacles_visibility();
    connect(&view_settings, &ViewSettings::entity_type_visibility_changed,
            this, &MainWindow::update_entity_type_visibility);
    update_entity_types_visibility();

    editor->set_common_actions(common_actions);

    ui.quest_tree_view->set_selected_path(editor->get_file_path());
  }
}

/**
 * @brief Slot called when the zoom of the current editor changes.
 */
void MainWindow::update_zoom() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  double zoom = editor->get_view_settings().get_zoom();

  if (zoom_actions.contains(zoom)) {
    zoom_actions[zoom]->setChecked(true);
  }
}

/**
 * @brief Slot called when the grid of the current editor was just shown or hidden.
 */
void MainWindow::update_grid_visibility() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  bool visible = editor->get_view_settings().is_grid_visible();

  ui.action_show_grid->setChecked(visible);
  grid_size->setEnabled(visible);
}

/**
 * @brief Slot called when the grid size of the current editor has just changed.
 */
void MainWindow::update_grid_size() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  grid_size->set_size(editor->get_view_settings().get_grid_size());
}

/**
 * @brief Updates the number of layer visibility buttons.
 *
 * This function is called when the range of layers of the current editor changes.
 */
void MainWindow::update_layer_range() {

  Editor* editor = get_current_editor();
  const bool has_editor = editor != nullptr;

  int min_layer = 0;
  int max_layer = -1;

  if (has_editor) {
    editor->get_layers_supported(min_layer, max_layer);
    ViewSettings& view_settings = editor->get_view_settings();
    view_settings.set_layer_range(min_layer, max_layer);
  }

  ui.action_show_layer_0->setVisible(max_layer >= 0);
  ui.action_show_layer_1->setVisible(max_layer >= 1);
  ui.action_show_layer_2->setVisible(max_layer >= 2);
  show_layers_action->setVisible(min_layer < 0 || max_layer >= 3);
  show_layers_menu->setEnabled(min_layer < 0 || max_layer >= 3);
  update_show_layers_menu();
  update_lock_layers_menu();
}

/**
 * @brief Slot called when a layer of the current editor was just locked or unlocked.
 * @param layer The layer whose locking state has just changed.
 */
void MainWindow::update_layer_locking(int layer) {

  const Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }
  const ViewSettings& view_settings = editor->get_view_settings();
  const bool locked = view_settings.is_layer_locked(layer);

  int min_layer = 0;
  int max_layer = 0;
  editor->get_layers_supported(min_layer, max_layer);

  // Update the show layer menu.
  const int index = layer - min_layer;
  QAction* action = lock_layers_menu->actions().value(index);
  if (action == nullptr) {
    qCritical() << "Missing lock layer action for layer " << layer << ": " << index;
    return;
  }
  action->setChecked(locked);
}

/**
 * @brief Slot called when the locking state of all layers should be updated to the GUI.
 */
void MainWindow::update_layers_locking() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  int min_layer = 0;
  int max_layer = 0;
  editor->get_layers_supported(min_layer, max_layer);

  const ViewSettings& view_settings = editor->get_view_settings();

  const QList<QAction*>& actions = lock_layers_menu->actions();
  for (int i = min_layer; i <= max_layer; ++i) {
    QAction* action = actions.value(i - min_layer);
    if (action != nullptr) {
      action->setChecked(view_settings.is_layer_locked(i));
    }
  }
}

/**
 * @brief Slot called when a layer of the current editor was just shown or hidden.
 * @param layer The layer whose visibility has just changed.
 */
void MainWindow::update_layer_visibility(int layer) {

  const Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }
  const ViewSettings& view_settings = editor->get_view_settings();
  const bool visible = view_settings.is_layer_visible(layer);

  int min_layer = 0;
  int max_layer = 0;
  editor->get_layers_supported(min_layer, max_layer);

  switch (layer) {

  case 0:
    ui.action_show_layer_0->setChecked(visible);
    break;

  case 1:
    ui.action_show_layer_1->setChecked(visible);
    break;

  case 2:
    ui.action_show_layer_2->setChecked(visible);
    break;

  default:
    break;
  }

  // Update the show layer menu.
  const int index = layer + 3 - min_layer;  // Skip "Show all", "Hide all" and the separator.
  QAction* action = show_layers_menu->actions().value(index);
  if (action == nullptr) {
    qCritical() << "Missing show layer action for layer " << layer << ": " << index;
    return;
  }
  action->setChecked(visible);
}

/**
 * @brief Slot called when the visibility of all layers should be updated to the GUI.
 */
void MainWindow::update_layers_visibility() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  int min_layer = 0;
  int max_layer = 0;
  editor->get_layers_supported(min_layer, max_layer);

  const ViewSettings& view_settings = editor->get_view_settings();
  ui.action_show_layer_0->setChecked(view_settings.is_layer_visible(0));
  ui.action_show_layer_1->setChecked(max_layer >= 1 && view_settings.is_layer_visible(1));
  ui.action_show_layer_2->setChecked(max_layer >= 2 && view_settings.is_layer_visible(2));

  const QList<QAction*>& actions = show_layers_menu->actions();
  for (int i = min_layer; i <= max_layer; ++i) {
    QAction* action = actions.value(i - min_layer);
    if (action != nullptr) {
      action->setChecked(view_settings.is_layer_visible(i));
    }
  }
}

/**
 * @brief Slot called when the traversables of the current editor were just
 * shown or hidden.
 */
void MainWindow::update_traversables_visibility() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  bool visible = editor->get_view_settings().are_traversables_visible();

  ui.action_show_traversables->setChecked(visible);
}

/**
 * @brief Slot called when the obstacles of the current editor were just
 * shown or hidden.
 */
void MainWindow::update_obstacles_visibility() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  bool visible = editor->get_view_settings().are_obstacles_visible();

  ui.action_show_obstacles->setChecked(visible);
}

/**
 * @brief Slot called when a entity type of the current editor was just shown or hidden.
 * @param entity_type The entity type whose visibility has just changed.
 */
void MainWindow::update_entity_type_visibility(EntityType entity_type) {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }
  ViewSettings& view_settings = editor->get_view_settings();
  bool visible = view_settings.is_entity_type_visible(entity_type);

  QString type_name = EntityTraits::get_lua_name(entity_type);
  const auto& it = show_entities_subactions.find(type_name);
  if (it == show_entities_subactions.end() || it.value() == nullptr) {
    // This type of entity is not present in the menu.
    // This might be a type that cannot be used in the editor but only by the engine.
    return;
  }

  QAction* action = it.value();
  action->setChecked(visible);
}

/**
 * @brief Slot called when the visibility of all entity types should be updated to the GUI.
 */
void MainWindow::update_entity_types_visibility() {

  Editor* editor = get_current_editor();
  if (editor == nullptr) {
    return;
  }

  ViewSettings& view_settings = editor->get_view_settings();
  for (QAction* action : std::as_const(show_entities_subactions)) {
    if (action == nullptr) {
      qCritical() << tr("Missing show entity type action");
      return;
    }
    if (action->data().isValid()) {  // Skip special actions.
      EntityType entity_type = static_cast<EntityType>(action->data().toInt());
      action->setChecked(view_settings.is_entity_type_visible(entity_type));
    }
  }
}

/**
 * @brief Updates the state of the grouping actions.
 */
void MainWindow::update_grouping_actions() {

  Editor* editor = get_current_editor();
  if (editor == nullptr || !editor->is_grouping_supported()) {
    ui.action_group->setVisible(false);
    ui.action_ungroup->setVisible(false);
    return;
  }

  ui.action_group->setVisible(true);
  ui.action_ungroup->setVisible(true);
  ui.action_group->setEnabled(editor->can_group());
  ui.action_ungroup->setEnabled(editor->can_ungroup());
}

/**
 * @brief Updates the run quest and run map actions.
 */
void MainWindow::update_run_quest() {

  if (quest_runner.is_started()) {
    ui.action_run_quest->setIcon(QIcon(":/images/icon_stop.svg"));
    ui.action_run_quest->setToolTip(tr("Stop quest"));
    ui.action_run_map->setEnabled(false);
  } else {
    ui.action_run_quest->setIcon(QIcon(":/images/icon_start.svg"));
    ui.action_run_quest->setToolTip(tr("Run quest"));
    const Editor* editor = get_current_editor();
    ui.action_run_map->setEnabled(editor != nullptr && editor->is_run_map_supported());
  }
}

/**
 * @brief Slot called when the quest execution begins.
 */
void MainWindow::quest_running() {

  // Update the run quest action.
  update_run_quest();
}

/**
 * @brief Slot called when the quest execution is finished.
 */
void MainWindow::quest_finished() {

  // Update the run quest action.
  update_run_quest();
}

/**
 * @brief Slot called when a music is started or stopped.
 * @param music_id Id of the music currently playing
 * or an empty string.
 */
void MainWindow::current_music_changed(const QString& music_id) {

  Q_UNUSED(music_id);
  update_music_actions();
}

/**
 * @brief Sets ups the play/stop and pause/continue actions.
 *
 * This depends on the current music and on the selection in the quest tree.
 */
void MainWindow::update_music_actions() {

  const Quest& quest = get_quest();
  const QString& music_id = Audio::get_current_music_id(quest);
  if (!music_id.isEmpty()) {
    // A music is being played.
    ui.action_stop_music->setEnabled(true);
    ui.action_stop_music->setText(tr("Stop music"));
    ui.action_stop_music->setIcon(QIcon(":/images/icon_stop_music.svg"));

    ui.action_pause_music->setEnabled(true);
    ui.action_pause_music->setText(tr("Pause music"));
    ui.action_pause_music->setIcon(QIcon(":/images/icon_pause_music.svg"));
  }
  else {
    ui.action_stop_music->setText(tr("Play selected music"));
    ui.action_stop_music->setIcon(QIcon(":/images/icon_start_music.svg"));
    ui.action_pause_music->setEnabled(false);
    ui.action_pause_music->setText(tr("Pause music"));
    const QString& selected_path = ui.quest_tree_view->get_selected_path();
    ResourceType resource_type;
    QString element_id;
    if (!selected_path.isEmpty() &&
        quest.is_potential_resource_element(selected_path, resource_type, element_id) &&
        resource_type == ResourceType::MUSIC &&
        quest.exists(selected_path)) {
      // A music is selected: allow to play it.
      ui.action_stop_music->setEnabled(true);
    }
    else {
      // No music is selected.
      ui.action_stop_music->setEnabled(false);
    }
  }
}

/**
 * @brief Reloads settings.
 */
void MainWindow::reload_settings() {

  ui.tab_widget->reload_settings();
  ui.console_widget->reload_settings();
}

/**
 * @brief Updates the title of the window from the current quest.
 */
void MainWindow::update_title() {

  setWindowTitle(quest.get_name());
}

/**
 * @brief Opens a file of a quest if it exists.
 * @param quest A quest.
 * @param path The file to open.
 */
void MainWindow::open_file(Quest& quest, const QString& path) {

  ui.tab_widget->open_file_requested(quest, path);
}

/**
 * @brief Opens the welcome page.
 */
void MainWindow::open_welcome() {
  ui.tab_widget->open_welcome_editor_requested(quest);
}

/**
 * @brief Receives a window close event.
 * @param event The event to handle.
 */
void MainWindow::closeEvent(QCloseEvent* event) {

  if (confirm_before_closing()) {
    ui.tab_widget->save_open_files_list();
    ui.tab_widget->close_without_confirmation();
    event->accept();
  }
  else {
    event->ignore();
  }
}

/**
 * @brief
 * @param target
 * @param evt
 * @return
 */
bool MainWindow::eventFilter(QObject* target, QEvent* evt) {

  if (target == ui.tool_bar) {
    const QEvent::Type event_type = evt->type();

    // Prevent right-click.
    if (event_type == QEvent::Type::ContextMenu) {
      return true;
    }
  }

  return QMainWindow::eventFilter(target, evt);
}

/**
 * @brief Function called when the user wants to exit the program.
 *
 * The user can save files if necessary.
 *
 * @return @c false to cancel the closing operation.
 */
bool MainWindow::confirm_before_closing() {

  return ui.tab_widget->confirm_before_closing();
}

/**
 * @brief Slot called when the selection changes in the quest tree view.
 * @param path The new selected path or an empty string.
 */
void MainWindow::selected_path_changed(const QString& path) {

  Q_UNUSED(path);
  update_music_actions();
}

/**
 * @brief Slot called when the user wants to rename a file.
 *
 * The new file name will be prompted to the user.
 * It may or may not be a resource element file.
 *
 * @param quest The quest that holds this file.
 * @param path Path of the file to rename.
 */
void MainWindow::rename_file_requested(Quest& quest, const QString& path) {

  if (path.isEmpty()) {
    return;
  }

  if (path == quest.get_data_path()) {
    // We don't want to rename the data directory.
    return;
  }

  int editor_index = ui.tab_widget->find_editor(path);
  if (editor_index != -1) {
    // Don't rename a file that has unsaved changes.
    const Editor* editor = ui.tab_widget->get_editor(editor_index);
    if (editor != nullptr && editor->has_unsaved_changes()) {
      QMessageBox::warning(this,
                           tr("File modified"),
                           tr("This file is open and has unsaved changes.\nPlease save it or close it before renaming."));
      ui.tab_widget->show_editor(path);
      return;
    }
  }

  ResourceType resource_type;
  if (quest.is_resource_path(path, resource_type)) {
    // Don't rename built-in resource directories.
    return;
  }

  try {
    QString element_id;
    if (quest.is_resource_element(path, resource_type, element_id)) {
      // Change the filename (and therefore the id) of a resource element.

      ChangeResourceIdDialog dialog(quest, resource_type, element_id);
      int result = dialog.exec();
      if (result != QDialog::Accepted) {
        return;
      }
      const QString& new_element_id = dialog.get_element_id();
      if (new_element_id == element_id) {
        return;
      }
      if (!dialog.get_update_references()) {
        // Regular renaming.
        quest.rename_resource_element(resource_type, element_id, new_element_id);
      }
      else {
        // Refactoring.
        if (resource_type == ResourceType::MAP) {
          // Update teletransporters leading to this map.
          refactor_map_id(element_id, new_element_id);
        }
        else if (resource_type == ResourceType::TILESET) {
          // Update maps using this tileset.
          refactor_tileset_id(element_id, new_element_id);
        }
        else if (resource_type == ResourceType::MUSIC) {
          // Update maps using this music.
          refactor_music_id(element_id, new_element_id);
        }
        else if (resource_type == ResourceType::ENEMY) {
          // Update maps using this enemy model.
          refactor_enemy_id(element_id, new_element_id);
        }
        else if (resource_type == ResourceType::ENTITY) {
          // Update maps using this custom entity model.
          refactor_custom_entity_id(element_id, new_element_id);
        }
      }
    }
    else {
      // Rename a regular file or directory.
      bool ok = false;
      QFileInfo info(path);
      QString file_name = info.fileName();

      if (quest.is_image(path) && path.startsWith(quest.get_resource_path(ResourceType::SPRITE))) {
        // Rename a PNG file in the sprites directory.
        QString path_from_sprites = quest.get_path_relative_to_sprites_path(path);
        InputDialogWithCheckBox dialog(
              tr("Rename file"),
              tr("New name for file '%1':").arg(file_name),
              tr("Update existing sprites using this image"),
              path_from_sprites,
              this
        );
        int result = dialog.exec();
        if (result != QDialog::Accepted) {
          return;
        }
        const QString& new_path_from_sprites = dialog.get_value();
        if (new_path_from_sprites == path_from_sprites) {
          return;
        }
        Quest::check_valid_file_name(new_path_from_sprites);
        QString new_path = quest.get_sprite_image_path(new_path_from_sprites);
        if (!dialog.is_checked()) {
          // Regular renaming.
          quest.rename_file(path, new_path);
        } else {
          // Refactoring.
          refactor_image_file(path, new_path);
        }
      } else {
        QString new_file_name = QInputDialog::getText(
              this,
              tr("Rename file"),
              tr("New name for file '%1':").arg(file_name),
              QLineEdit::Normal,
              file_name,
              &ok);

        if (ok && new_file_name != file_name) {

          Quest::check_valid_file_name(file_name);
          QString new_path = QFileInfo(path).path() + '/' + new_file_name;
          if (!info.isDir()) {
            quest.rename_file(path, new_path);
          } else {
            quest.rename_dir(path, new_path);
          }
        }
      }
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }

}

/**
 * @brief Slot called when the user wants to perform some refactoring.
 *
 * Makes sure that all files are saved first, performs the refactoring
 * and closes the files that were modified during the operation.
 *
 * @param refactoring The refactoring action to perform.
 */
void MainWindow::refactoring_requested(const Refactoring& refactoring) {

  try {

    // Make sure that all open files are saved before doing the refactoring.
    const QSet<QString> files_unsaved_allowed = refactoring.get_files_unsaved_allowed();
    if (ui.tab_widget->has_unsaved_files_other_than(files_unsaved_allowed)) {

      QMessageBox::StandardButton answer = QMessageBox::question(
            nullptr,
            tr("Unsaved changes"),
            tr("All files must be saved before this operation.\nDo you want to save them now?"),
            QMessageBox::SaveAll | QMessageBox::Cancel,
            QMessageBox::SaveAll
      );

      switch (answer) {

      case QMessageBox::SaveAll:
        if (!ui.tab_widget->save_all_files_requested()) {
          return;
        }
        break;

      case QMessageBox::Cancel:
      case QMessageBox::Escape:
        // Cancel the refactoring.
        return;

      default:
        return;
      }
    }

    // Do the work.
    QStringList modified_paths = refactoring.execute();

    // See if some of the impacted files was open.
    for (const QString& path : std::as_const(modified_paths)) {
      int editor_index = ui.tab_widget->find_editor(path);
      if (editor_index != -1) {
        // Reload the file.
        ui.tab_widget->reload_file_requested(editor_index);
      }
    }
  }
  catch (const EditorException& ex) {
    ex.show_dialog();
  }
}

/**
 * @brief Changes the id of a map and updates teletransporters leading to it.
 * @param map_id_before Current map id.
 * @param map_id_after New map id.
 */
void MainWindow::refactor_map_id(const QString& map_id_before, const QString& map_id_after) {

  Refactoring refactoring([=]() {

    // Change the id.
    quest.rename_resource_element(ResourceType::MAP, map_id_before, map_id_after);

    // Update teletransporters in all maps.
    QStringList modified_paths;
    const QStringList& map_ids = quest.get_database().get_elements(ResourceType::MAP);
    for (const QString& map_id : map_ids) {
      if (update_destination_map_in_map(map_id, map_id_before, map_id_after)) {
        modified_paths << quest.get_map_data_file_path(map_id);
      }
    }
    return modified_paths;
  });

  refactoring_requested(refactoring);
}

/**
 * @brief Updates existing teletransporters in a map when a map id was changed.
 * @param map_id Id of the map to update.
 * @param map_id_before Id of the map that has changed.
 * @param map_id_after New id of the changed map.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool MainWindow::update_destination_map_in_map(
    const QString& map_id,
    const QString& map_id_before,
    const QString& map_id_after
) {
  // We don't load the entire map with all its entities for performance.
  // Instead, we just find and replace the appropriate text in the map
  // data file.

  QString path = get_quest().get_map_data_file_path(map_id);
  if (!QFile(path).exists()) {
    return false;
  }

  QString pattern = QString("\n  destination_map = \"?%1\"?,\n").arg(
        QRegularExpression::escape(map_id_before));

  QString replacement = QString("\n  destination_map = \"%1\",\n").arg(map_id_after);

  return FileTools::replace_in_file(path, QRegularExpression(pattern), replacement);
}

/**
 * @brief Changes the id of a tileset and updates maps using it.
 * @param tileset_id_before Current tileset id.
 * @param tileset_id_after New tileset id.
 */
void MainWindow::refactor_tileset_id(const QString& tileset_id_before, const QString& tileset_id_after) {

  Refactoring refactoring([=]() {

    // Change the id.
    quest.rename_resource_element(ResourceType::TILESET, tileset_id_before, tileset_id_after);

    // Update all maps.
    QStringList modified_paths;
    const QStringList& map_ids = quest.get_database().get_elements(ResourceType::MAP);
    for (const QString& map_id : map_ids) {
      if (update_tileset_in_map(map_id, tileset_id_before, tileset_id_after)) {
        modified_paths << quest.get_map_data_file_path(map_id);
      }
    }
    return modified_paths;
  });

  refactoring_requested(refactoring);
}

/**
 * @brief Updates the tileset id in a map file.
 * @param map_id Id of the map to update.
 * @param tileset_id_before Id of the tileset that has changed.
 * @param tileset_id_after New id of the changed tileset.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool MainWindow::update_tileset_in_map(
    const QString& map_id,
    const QString& tileset_id_before,
    const QString& tileset_id_after
) {
  // We don't load the entire map with all its entities for performance.
  // Instead, we just find and replace the appropriate text in the map
  // data file.

  QString path = get_quest().get_map_data_file_path(map_id);
  if (!QFile(path).exists()) {
    return false;
  }

  QString pattern = QString("\n  tileset = \"?%1\"?,\n").arg(
        QRegularExpression::escape(tileset_id_before));
  QString replacement = QString("\n  tileset = \"%1\",\n").arg(tileset_id_after);
  const bool replace_all = false;  // Don't replace it in tile entities.
  return FileTools::replace_in_file(path, QRegularExpression(pattern), replacement, replace_all);
}

/**
 * @brief Changes the id of a music and updates maps using it.
 * @param music_id_before Current music id.
 * @param music_id_after New music id.
 */
void MainWindow::refactor_music_id(const QString& music_id_before, const QString& music_id_after) {

  Refactoring refactoring([=]() {

    // Change the id.
    quest.rename_resource_element(ResourceType::MUSIC, music_id_before, music_id_after);

    // Update all maps.
    QStringList modified_paths;
    const QStringList& map_ids = quest.get_database().get_elements(ResourceType::MAP);
    for (const QString& map_id : map_ids) {
      if (update_music_in_map(map_id, music_id_before, music_id_after)) {
        modified_paths << quest.get_map_data_file_path(map_id);
      }
    }
    return modified_paths;
  });

  refactoring_requested(refactoring);
}

/**
 * @brief Updates the music id in a map file.
 * @param map_id Id of the map to update.
 * @param music_id_before Id of the music that has changed.
 * @param music_id_after New id of the changed music.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool MainWindow::update_music_in_map(
    const QString& map_id,
    const QString& music_id_before,
    const QString& music_id_after
) {
  // We don't load the entire map with all its entities for performance.
  // Instead, we just find and replace the appropriate text in the map
  // data file.

  QString path = get_quest().get_map_data_file_path(map_id);
  if (!QFile(path).exists()) {
    return false;
  }

  QString pattern = QString("\n  music = \"?%1\"?,\n").arg(
        QRegularExpression::escape(music_id_before));

  QString replacement = QString("\n  music = \"%1\",\n").arg(music_id_after);

  return FileTools::replace_in_file(path, QRegularExpression(pattern), replacement);
}

/**
 * @brief Changes the id of an enemy breed and updates enemies having it.
 * @param enemy_id_before Current enemy breed.
 * @param enemy_id_after New enemy breed.
 */
void MainWindow::refactor_enemy_id(const QString& enemy_id_before, const QString& enemy_id_after) {

  Refactoring refactoring([=]() {

    // Change the id.
    quest.rename_resource_element(ResourceType::ENEMY, enemy_id_before, enemy_id_after);

    // Update enemies in all maps.
    QStringList modified_paths;
    const QStringList& map_ids = quest.get_database().get_elements(ResourceType::MAP);
    for (const QString& map_id : map_ids) {
      if (update_enemy_breed_in_map(map_id, enemy_id_before, enemy_id_after)) {
        modified_paths << quest.get_map_data_file_path(map_id);
      }
    }
    return modified_paths;
  });

  refactoring_requested(refactoring);
}

/**
 * @brief Updates existing enemies in a map when an enemy breed id was changed.
 * @param map_id Id of the map to update.
 * @param enemy_id_before Id of the enemy breed that has changed.
 * @param enemy_id_after New id of the enemy breed.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool MainWindow::update_enemy_breed_in_map(
    const QString& map_id,
    const QString& enemy_id_before,
    const QString& enemy_id_after
) {
  // We don't load the entire map with all its entities for performance.
  // Instead, we just find and replace the appropriate text in the map
  // data file.

  QString path = get_quest().get_map_data_file_path(map_id);
  if (!QFile(path).exists()) {
    return false;
  }

  QString pattern = QString("\n  breed = \"?%1\"?,\n").arg(
        QRegularExpression::escape(enemy_id_before));

  QString replacement = QString("\n  breed = \"%1\",\n").arg(enemy_id_after);

  return FileTools::replace_in_file(path, QRegularExpression(pattern), replacement);
}

/**
 * @brief Changes the id of an custom entity model and updates enemies having it.
 * @param entity_id_before Current custom entity model.
 * @param entity_id_after New custom entity model.
 */
void MainWindow::refactor_custom_entity_id(const QString& entity_id_before, const QString& entity_id_after) {

  Refactoring refactoring([=]() {

    // Change the id.
    quest.rename_resource_element(ResourceType::ENTITY, entity_id_before, entity_id_after);

    // Update enemies in all maps.
    QStringList modified_paths;
    const QStringList& map_ids = quest.get_database().get_elements(ResourceType::MAP);
    for (const QString& map_id : map_ids) {
      if (update_custom_entity_model_in_map(map_id, entity_id_before, entity_id_after)) {
        modified_paths << quest.get_map_data_file_path(map_id);
      }
    }
    return modified_paths;
  });

  refactoring_requested(refactoring);
}

/**
 * @brief Updates existing enemies in a map when an custom entity model id was changed.
 * @param map_id Id of the map to update.
 * @param entity_id_before Id of the custom entity model that has changed.
 * @param entity_id_after New id of the custom entity model.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool MainWindow::update_custom_entity_model_in_map(
    const QString& map_id,
    const QString& entity_id_before,
    const QString& entity_id_after
) {
  // We don't load the entire map with all its entities for performance.
  // Instead, we just find and replace the appropriate text in the map
  // data file.

  QString path = get_quest().get_map_data_file_path(map_id);
  if (!QFile(path).exists()) {
    return false;
  }

  QString pattern = QString("\n  model = \"?%1\"?,\n").arg(
        QRegularExpression::escape(entity_id_before));

  QString replacement = QString("\n  model = \"%1\",\n").arg(entity_id_after);

  return FileTools::replace_in_file(path, QRegularExpression(pattern), replacement);
}

/**
 * @brief Renames a PNG file and updates sprites referencing it.
 * @param image_path_before Path of the PNG file to rename.
 * @param image_path_after New path to set.
 */
void MainWindow::refactor_image_file(
    const QString& image_path_before,
    const QString& image_path_after) {

  if (image_path_after == image_path_before) {
    return;
  }

  Refactoring refactoring([this, image_path_before, image_path_after]() {

    QString relative_image_path_before = quest.get_path_relative_to_sprites_path(image_path_before);
    QString relative_image_path_after = quest.get_path_relative_to_sprites_path(image_path_after);

    if (relative_image_path_before.isEmpty() ||
        relative_image_path_after.isEmpty()) {
      return QStringList();
    }

    // Do the renaming.
    quest.rename_file(image_path_before, image_path_after);

    // Update source images in all sprites.
    QStringList modified_paths;
    const QStringList& sprite_ids = quest.get_database().get_elements(ResourceType::SPRITE);
    for (const QString& sprite_id : sprite_ids) {
      if (update_image_in_sprite(sprite_id, relative_image_path_before, relative_image_path_after)) {
        modified_paths << quest.get_sprite_path(sprite_id);
      }
    }
    return modified_paths;
  });

  refactoring_requested(refactoring);
}

/**
 * @brief Updates existing sprites after a PNG file was moved.
 * @param sprite_id Id of the sprite to update.
 * @param image_before Path of the PNG file that was renamed, relative to the sprites directory.
 * @param image_after New path after renaming, relative to the sprites directory.
 * @return @c true if there was a change.
 * @throws EditorException In case of error.
 */
bool MainWindow::update_image_in_sprite(
    const QString& sprite_id,
    const QString& image_before,
    const QString& image_after
) {
  QString path = get_quest().get_sprite_path(sprite_id);
  if (!QFile(path).exists()) {
    return false;
  }

  QString pattern = QString("\n  src_image = \"%1\",\n").arg(
        QRegularExpression::escape(image_before));

  QString replacement = QString("\n  src_image = \"%1\",\n").arg(image_after);

  return FileTools::replace_in_file(path, QRegularExpression(pattern), replacement);
}

/**
 * @brief Saves the quest tree width in settings
 * @param pos the position of the splitter
 * @param index of the element in the splitter
 */
void MainWindow::quest_tree_resized(int pos, int index) {
  if (index == 1) {
    EditorSettings settings;
    settings.set_value(EditorSettings::quest_tree_width, pos);
  }
}

}
