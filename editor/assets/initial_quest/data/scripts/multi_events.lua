-- This script allows to register multiple functions as Solarus events.
--
-- Usage:
--
-- Just require() this script and then all Solarus types
-- will have a register_event() method that adds an event callback.
--
-- Example:
--
-- local multi_events = require("scripts/multi_events")
--
-- -- Register two callbacks for the game:on_started() event:
-- game:register_event("on_started", my_function)
-- game:register_event("on_started", another_function)
--
-- -- It even works on metatables!
-- game_meta:register_event("on_started", my_function)
-- game_meta:register_event("on_started", another_function)
--
-- The good old way of defining an event still works
-- (but you cannot mix both approaches on the same object):
-- function game:on_started()
--   -- Some code.
-- end
--
-- Limitations:
--
-- Menus are regular Lua tables and not a proper Solarus type.
-- They can also support multiple events, but to do so you first have
-- to enable the feature explicitly like this:
-- multi_events:enable(my_menu)
-- Note that sol.main does not have this constraint even if it is also
-- a regular Lua table.

local multi_events = {}

-- get actual '_events' field of the object
local function get_events(object) return sol.main.rawget(object,"_events") or {} end

local function register_event(object, event_name, callback, first)
  local events = get_events(object)
  if not events[event_name] then
    --initial setup for first registered event
    local unregistered_callback = sol.main.rawget(object,event_name)
    if not unregistered_callback then
      --function to lookup if event callback exists in metatable then call it
      object[event_name] = function(...)
        local mt = getmetatable(object) or {}
        local mt_proto = mt.__index
        if type(mt_proto)~="table" then mt_proto = mt end
        local mt_callback = mt_proto[event_name]
        if mt_callback then return mt_callback(...) end
      end
    end
  end

  --create new callback for newly registered event
  object._events = nil --temporarily remove events to allow modification
  events[event_name] = true --set event as registered
  local previous_callbacks = object[event_name] or function() end
  if first then
    object[event_name] = function(...)
      return callback(...) or previous_callbacks(...)
    end
  else
    object[event_name] = function(...)
      return previous_callbacks(...) or callback(...)
    end
  end
  object._events = events
end

-- Adds the multi event register_event() feature to an object
-- (userdata, userdata metatable or table).
function multi_events:enable(object)
  object.register_event = register_event

  local old_newindex = object.__newindex or rawset
  function object.__newindex(t,k,v)
    local events = get_events(t)
    if events and events[k] then
      error(string.format("overriding '%s', a event previously registered with 'register_event'",k))
    else
      old_newindex(t,k,v)
    end
  end
end

local types = {
  "game",
  "map",
  "item",
  "surface",
  "text_surface",
  "sprite",
  "timer",
  "movement",
  "straight_movement",
  "target_movement",
  "random_movement",
  "path_movement",
  "random_path_movement",
  "path_finding_movement",
  "circle_movement",
  "jump_movement",
  "pixel_movement",
  "hero",
  "dynamic_tile",
  "teletransporter",
  "destination",
  "pickable",
  "destructible",
  "carried_object",
  "chest",
  "shop_treasure",
  "enemy",
  "npc",
  "block",
  "jumper",
  "switch",
  "sensor",
  "separator",
  "wall",
  "crystal",
  "crystal_block",
  "stream",
  "door",
  "stairs",
  "bomb",
  "explosion",
  "fire",
  "arrow",
  "hookshot",
  "boomerang",
  "camera",
  "custom_entity",
  "state"
}

-- Add the register_event function to all userdata types.
for _, type in ipairs(types) do

  local meta = sol.main.get_metatable(type)
  assert(meta ~= nil)
  multi_events:enable(meta)
end

-- Also add it to sol.main (which is a regular table).
multi_events:enable(sol.main)

return multi_events
