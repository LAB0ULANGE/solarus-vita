local map = ...

function map:on_started()
  local success, sprite = pcall(function()
    sol.sprite.create("imaginary/bad-id")
  end)
  assert_equal(success, false, "Sprite creation should have failed")

  sprite = sol.sprite.create("hero/tunic1")
  assert_equal(sol.main.get_type(sprite), "sprite",
               "Sprite should be of type 'sprite'.")

  sol.main.exit()
end
