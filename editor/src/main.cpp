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
#include "widgets/main_window.h"
#include "config.h"
#include "editor_settings.h"
#include "editor_style.h"
#include "version.h"
#include <solarus/core/Arguments.h>
#include <solarus/core/Debug.h>
#include <solarus/core/Game.h>
#include <solarus/core/MainLoop.h>
#include <solarus/core/Savegame.h>
#include <solarus/core/System.h>
#include <solarus/lua/LuaContext.h>
#include <QApplication>
#include <QLibraryInfo>
#include <QStyleFactory>
#include <QTranslator>
#include <QIcon>
#include <QPixmap>
#include <QSysInfo>
#include <array>

// SDLmain is required in some platforms, i.e. Windows, for proper initialization.
// For instance, in Windows, SDLmain encodes argv in main() using UTF-8 by default.
#include <SDL.h>

namespace SolarusEditor {

namespace {

/**
 * @brief Sets the application metadata. Must be done before creating a QApplication.
 */
void setup_application_information() {

  // Enable high DPI scaling for Windows/Linux with fractional scaling factors.
  QGuiApplication::setHighDpiScaleFactorRoundingPolicy(
      Qt::HighDpiScaleFactorRoundingPolicy::Round);

  QApplication::setApplicationName(SOLARUSEDITOR_EXECUTABLE_NAME);
  QApplication::setApplicationDisplayName(SOLARUSEDITOR_APP_DISPLAY_NAME);
  QApplication::setApplicationVersion(SOLARUSEDITOR_VERSION);
  QApplication::setOrganizationName("Solarus Labs");
  QApplication::setOrganizationDomain("solarus-games.org");
  // Set desktop filename so that the QtWayland backend will report the correct AppID
  // based on this and make the launcher icon and startup notification work.
  QGuiApplication::setDesktopFileName(SOLARUSEDITOR_APP_ID);
}


/**
 * @brief Sets the QGuiApplication icon. Must done after creating a QApplication.
 */
void set_application_icon() {

  // The bundle icon is used on macOS/OS X. No need to change it.
  const QString platform = QSysInfo::productType();
  if (platform == QStringLiteral("osx") || platform == QStringLiteral("macos"))
    return;

  static constexpr std::array<QIcon::Mode, 4> modes{
      QIcon::Mode::Normal,
      QIcon::Mode::Disabled,
      QIcon::Mode::Active,
      QIcon::Mode::Selected,
  };
  static constexpr std::array<int, 12> dimensions{ 16, 20, 24, 32, 40, 48, 64, 96, 128, 256, 512, 1024 };
  static constexpr const char* path_base(":/app_icon/solarus-editor");
  static constexpr const char* path_ext("png");

  QIcon app_icon;
  for (const int dimension : dimensions) {
    const QString path = QString("%1-%2.%3").arg(path_base, QString::number(dimension), path_ext);
    const QPixmap pixmap(path);
    if (pixmap.isNull())
      continue;
    for (const QIcon::Mode mode : modes) {
      app_icon.addPixmap(pixmap, mode, QIcon::State::On);
      app_icon.addPixmap(pixmap, mode, QIcon::State::Off);
    }
  }
  QGuiApplication::setWindowIcon(app_icon);
}

/**
 * @brief Runs the quest editor GUI.
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int run_editor_gui(int argc, char* argv[]) {

  QString cmd_quest_path;
  QString cmd_file_path;
  if (argc > 1) {
    // Quest to open initially.
    cmd_quest_path = argv[1];
    if (argc > 2) {
      // File to open in this quest.
      cmd_file_path = argv[2];
    }
  }

  // Set up the application.
  QApplication application(argc, argv);

  // Get current system locale.
  const QLocale locale = QLocale::system();

  // Set up Qt translations.
  QTranslator qt_translator;
  for (const QString& searchPath : std::vector<QString>{
           QApplication::applicationDirPath(),
           QApplication::applicationDirPath() + "/translations",
           QApplication::applicationDirPath() + "/../Resources/translations",
           SOLARUSEDITOR_DATADIR_PATH "/translations"}) {
    if (qt_translator.load(locale, "qtbase", "_", searchPath)) {
      break;
    }
  }
  application.installTranslator(&qt_translator);

  // Set up application translations.
  QTranslator app_translator;
  for (const QString& searchPath : std::vector<QString>{
           QApplication::applicationDirPath(),
           QApplication::applicationDirPath() + "/translations",
           QApplication::applicationDirPath() + "/../Resources/translations",
           SOLARUSEDITOR_DATADIR_PATH "/translations"}) {
    if (app_translator.load(locale, "solarus_editor", "_", searchPath)) {
      break;
    }
  }
  application.installTranslator(&app_translator);

  application.setStyle(new EditorStyle());

  set_application_icon();

  MainWindow window(nullptr);

  // Choose an appropriate initial window size and position.
  window.initialize_geometry_on_screen();

  // Determine the quest and files to open,
  // from the command line or from recent files.
  QString quest_path;
  QStringList file_paths;
  QString active_file_path;

  EditorSettings settings;
  if (!cmd_quest_path.isEmpty()) {
    // Quest specified in the command line.
    quest_path = cmd_quest_path;
    if (!cmd_file_path.isEmpty()) {
      file_paths << cmd_file_path;
    }
  }

  if (settings.get_value_bool(EditorSettings::restore_last_files)) {
    if (cmd_quest_path.isEmpty()) {
      // No quest specified in the commands line: try to restore the last quest.
      const QString& current_quest = settings.get_value_string(EditorSettings::current_quest);
      if (!current_quest.isEmpty()) {
        quest_path = current_quest;
      }
    }
    if (cmd_file_path.isEmpty()) {
      // No file specified in the command line: try to restore last opened files.
      file_paths = settings.get_value_string_list(EditorSettings::last_files);
      active_file_path = settings.get_value_string(EditorSettings::last_file);
    }
  }

  // Open the quest.
  bool open_success = false;
  if (!quest_path.isEmpty()) {
    window.open_quest(quest_path);

    if (window.get_quest().is_valid()) {

      // Open the tabs.
      for (const QString& file_path : std::as_const(file_paths)) {
        window.open_file(window.get_quest(), file_path);
      }
      if (!active_file_path.isEmpty()) {
        // Restore the active tab.
        window.open_file(window.get_quest(), active_file_path);
      }

      open_success = true;
    }
  }

  if (!open_success) {
    window.open_welcome();
  }

  window.show();

  return application.exec();
}

/**
 * @brief Runs a quest or a map for testing purposes.
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int run_quest(int argc, char* argv[]) {

  // Abort the process in case of fatal error.
  Solarus::Debug::set_abort_on_die(true);

  // Run the Solarus main loop.
  const Solarus::Arguments args(argc, argv);
  const std::string& map_id = args.get_argument_value("-map");
  Solarus::MainLoop main_loop(args);
  if (map_id.empty()) {
    // Just run the quest like solarus-run does.
    main_loop.run();
  } else {
    // Run a specific map.
    std::shared_ptr<Solarus::Savegame> savegame = std::make_shared<Solarus::Savegame>(
        main_loop, "save1.dat"
    );
    savegame->initialize();
    savegame->set_string(Solarus::Savegame::KEY_STARTING_MAP, map_id);
    savegame->unset(Solarus::Savegame::KEY_STARTING_POINT);
    Solarus::Game* game = new Solarus::Game(main_loop, savegame);
    main_loop.set_game(game);
    main_loop.run();
  }

  return 0;
}

}  // Anonymous namespace

}  // namespace SolarusEditor

/**
 * @brief Entry point of the quest editor.
 *
 * To run the editor GUI:
 *   solarus-editor [quest_path [file_path]]
 * To directly run a quest (no GUI, similar to solarus-run):
 *   solarus-editor -run quest_path
 * To directly run a map of a quest quest for testing purposes:
 *   solarus-editor -run -map=your_map_id quest_path
 *
 * @param argc Number of arguments of the command line.
 * @param argv Command-line arguments.
 * @return 0 in case of success.
 */
int main(int argc, char* argv[]) {

  SolarusEditor::setup_application_information();

  if (argc > 1 && QString(argv[1]) == "-run") {
    // Quest run mode.
    return SolarusEditor::run_quest(argc, argv);
  } else {
    // Editor GUI mode.
    return SolarusEditor::run_editor_gui(argc, argv);
  }
}
