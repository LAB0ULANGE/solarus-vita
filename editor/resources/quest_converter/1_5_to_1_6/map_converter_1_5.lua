-- Solarus 1.5 to 1.6.
-- Changes in the map syntax:
-- - The maximum_moves property of blocks is now deprecated,
--   the new and better optional property max_moves should be used instead.

local converter = {}

function converter.convert(quest_path, map_id, default_font_id)

  local input_file_name = quest_path .. "/data/maps/" .. map_id .. ".dat"
  local input_file, error_message = io.open(input_file_name, "r")
  if input_file == nil then
    io.write("Cannot open old map file for reading: ", error_message, "\n")
    io.flush()
    return
  end

  local text = input_file:read("*a")  -- Read the whole file.

  -- Replace maximum_moves by max_moves for blocks.
  text = text:gsub("\n  maximum_moves = [012],", {
      ["\n  maximum_moves = 0,"] = "\n  max_moves = 0,",
      ["\n  maximum_moves = 1,"] = "\n  max_moves = 1,",
      ["\n  maximum_moves = 2,"] = "",  -- no value means unlimited now.
  })

  input_file:close()

  local output_file_name = input_file_name
  local output_file, error_message = io.open(output_file_name, "w")
  if output_file == nil then
    error("Cannot open new map file for writing: " .. error_message)
  end

  output_file:write(text)
  output_file:close()
end

return converter
