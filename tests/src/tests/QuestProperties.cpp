/*
 * Copyright (C) 2006-2022 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#include "solarus/core/QuestProperties.h"
#include "solarus/core/QuestFiles.h"
#include "tools/TestEnvironment.h"
#include <iostream>

using namespace Solarus;

namespace {

/**
 * \brief Checks reading and writing a quest.dat file.
 */
void check_quest_properties(TestEnvironment& /* env */) {

  QuestProperties quest_properties;
  bool success = false;

  // Import the file.
  const std::string file_name = "quest.dat";
  const std::string imported_buffer = QuestFiles::data_file_read(file_name);
  success = quest_properties.import_from_buffer(imported_buffer, file_name);
  TestEnvironment::verify(success, "Quest properties import failed");

  // Export the file.
  std::string exported_buffer;
  success = quest_properties.export_to_buffer(exported_buffer);
  TestEnvironment::verify(success, "Quest properties export failed");

  // Check that the file is identical after import/export.
  if (exported_buffer != imported_buffer) {
    std::cerr << "Quest properties differ from the original ones after export." << std::endl
        << "*** Original Quest properties:" << std::endl << imported_buffer << std::endl
        << "*** Exported Quest properties:" << std::endl << exported_buffer << std::endl;
    Debug::die("Quest properties: exported file differs from the original one");
  }
}

}

/**
 * \brief Tests reading and writing quest.dat files.
 */
int main(int argc, char** argv) {

  TestEnvironment env(argc, argv);

  check_quest_properties(env);

  return 0;
}
