// SPDX-License-Identifier: GPL-3.0-or-later
#include <Application.h>

#include <solarus/core/Arguments.h>
#include <solarus/core/Debug.h>
#include <solarus/core/MainLoop.h>

#include <SolarusLauncherConfig.h>

#include <string>
#include <iostream>

/**
 * @brief Runs the Solarus Launcher GUI.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return int The exit status.
 */
int runGUI(int argc, char* argv[]) {
  // Run the QApplication event loop.
  solarus::launcher::configureQApplication();
  solarus::launcher::Application app(argc, argv);

  return app.exec();
}

/**
 * @brief Runs the Solarus Launcher as a CLI, i.e. takes a quest file as argument.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return int The exit status.
 */
int runCLI(int argc, char* argv[]) {
  Solarus::Debug::set_abort_on_die(true);
  Solarus::Debug::set_show_popup_on_die(false);

  Solarus::Arguments args;
  for (int i = 0; i < argc; ++i) {
    args.add_argument(QString::fromLocal8Bit(argv[i]).toUtf8().constData());
  }

  Solarus::MainLoop(args).run();
  return EXIT_SUCCESS;
}

/**
 * @brief Prints the help message.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return int The exit status.
 */
int printHelp(int argc, char* argv[]) {
  const auto binary_name = std::string{ (argc > 0) ? argv[0] : solarus::launcher::SOLARUSLAUNCHER_EXECUTABLE_NAME };
  std::cout << "Usage:\n" << binary_name << " [--version] [--help] [<path/to/quest.solarus>]" << std::endl;
  return EXIT_SUCCESS;
}

/**
 * @brief Prints the version of the Solarus Launcher.
 * @return int The exit status.
 */
int printVersion() {
  std::cout << solarus::launcher::SOLARUSLAUNCHER_VERSION << std::endl;
  return EXIT_SUCCESS;
}

/**
 * @brief The main entry point of the Solarus Launcher. It can be run as a GUI or as a CLI.
 * @param argc The number of arguments.
 * @param argv The arguments.
 * @return int The exit status.
 */
int main(int argc, char** argv) {
  if (argc > 1) {
    const auto arg1 = std::string{ argv[1] };
    if (arg1 == "--help" || arg1 == "-h") {
      return printHelp(argc, argv);
    } else if (arg1 == "--version" || arg1 == "-v") {
      return printVersion();
    } else {
      return runCLI(argc, argv);
    }
  } else {
    return runGUI(argc, argv);
  }
}
