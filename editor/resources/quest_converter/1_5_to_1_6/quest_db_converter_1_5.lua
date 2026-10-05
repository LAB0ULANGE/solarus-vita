-- This module reads a quest resource list file with the format of Solarus 1.5
-- (project_db.dat).

local converter = {}

local resource_type_names = {
  "map",
  "tileset",
  "music",
  "sprite",
  "sound",
  "item",
  "enemy",
  "language",
  "entity",
  "font",
  "shader",
}

local function load_quest_db(quest_path)

  local resources = {}

  -- Create a table for each type of resource.
  for _, resource_type_name in ipairs(resource_type_names) do
    local resource = {}
    resources[resource_type_name] = resource
  end

  -- Set up the environment with a function for each type
  -- of resource and a file function.
  local env = {}
  setmetatable(env, {
    __index = function(table, key)
      for _, resource_type_name in ipairs(resource_type_names) do
        if key == resource_type_name then
          return function(resource_element)
            local resource = resources[resource_type_name]
            resource[#resource + 1] = resource_element
          end
        end
      end

      if key == "file" then
        return function() end
      end
    end
  })

  local chunk = loadfile(quest_path .. "/data/project_db.dat")
  setfenv(chunk, env)
  chunk()

  return resources
end

function converter.convert(quest_path, fonts)

  -- Read project_db.dat.
  local resources = load_quest_db(quest_path)

  -- No change, just return the info.
  return resources
end

return converter
