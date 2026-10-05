local map = ...

function map:on_started()
    local volume = sol.audio.get_music_volume()
    assert(volume == 100)

    sol.audio.set_music_volume(40)
    volume = sol.audio.get_music_volume()
    assert(volume == 40)
end
