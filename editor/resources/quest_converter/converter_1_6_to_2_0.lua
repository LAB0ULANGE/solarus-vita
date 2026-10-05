-- This script updates all data files of a solarus 1.6 quest
-- into the format of solarus 2.0.
-- Usage: lua update_quest.lua path/to/your_quest
--   local converter = require("converter_1_6_to_2_0")
--   converter.convert(quest_path)

local converter = {}

local function write_info(message)

  io.write(message, "\n")
  io.flush()
end

function converter.convert(quest_path)

  assert(quest_path)

  write_info("Updating your quest " .. quest_path .. " from Solarus 1.6 to Solarus 2.0.")
  write_info("It is recommended to backup your quest files before.")

  -- Convert the quest properties file quest.dat.
  write_info("  Converting the quest properties file...")
  local quest_properties_converter = require("1_6_to_2_0/quest_properties_converter_1_6")
  quest_properties_converter.convert(quest_path)

  write_info("Update successful!")

end

return converter
