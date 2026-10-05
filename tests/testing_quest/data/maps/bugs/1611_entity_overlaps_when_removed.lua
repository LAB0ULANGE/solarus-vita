local map = ...
local game = map:get_game()

function map:on_opening_transition_finished()
   local e1 = entity_1
   local e2 = entity_2
   assert(entity_1 ~= nil)
   assert(entity_2 ~= nil)
   assert_equal(e1:exists(), true)
   assert_equal(e2:exists(), true)
   assert_equal(e1:overlaps(e2), true)
   assert_equal(e2:overlaps(e1), true)

   entity_1:remove()

   assert(entity_1 == nil)
   assert(e1 ~= nil)
   assert_equal(e1:exists(), false)
   assert_equal(e1:overlaps(e2), false)
   assert_equal(e2:overlaps(e1), false)

   sol.main.exit()
end
