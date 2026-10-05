# Choosing a license

## Engine and Quest Files Licensing

Solarus engine is licensed under **GNU GPL v3**. Lots of quests made with the engine have been licensed the same, but it doesn't mean all quests need to be licensed under GPL.

### Engine Licensing

![Solarus is GPL](images/choosing-a-license/solarus-gpl.png)

The engine being **GPL** means that, first and foremost, it is free and open-source, and will forever be. Source code is [available](https://gitlab.com/solarus-games) and you can read it and study how it has been made, download it, modify it, distribute it as it is, or modified versions.

- You can modify the engine as much as you want. Every modification made to the engine, if you decide to make them public, should also be licensed with GPL (or a compatible license), consequently guaranteeing that these modifications are also free and open-source. You can't make a proprietary game engine based on Solarus.

- You can modify the engine for your own purposes, i.e. for private use, as long as you don't distribute it. But if you distribute the engine or a modified version of it, then it must remain open-source. It ensures modifications (improvements, new features, bug fixes, etc.) will always be shared with the community, and everyone will benefit from it.

!!! success "Summary"

    If you make a quest with Solarus and you don't need to modify Solarus, just use it as it is, and everything will be fine.

### Quest Licensing

![Solarus quests might be any license](images/choosing-a-license/quest-any-license.png)

The engine just acts as an interpreter for the quests. Actually, you can license your quest files as **any license**, even _proprietary_. It means you can copyright your code and resources.

However, if you license them as _proprietary_, it means you should not use existing GPL Lua files. If only one file is GPL, all files must also be open-source, and licensed with a GPL license.

Note that _proprietary_ does not mean _closed-source_, here. Currently, Solarus quests cannot be _closed-source_, because the `.solarus` file is just a zip archive that contains all images, musics and scripts.

!!! success "Summary"

    Your quest may be licensed with any license, but be careful with what third-party elements you use in your quest.

## Making a Proprietary Commercial Game with Solarus

![Quest with copyright](images/choosing-a-license/quest-copyright.png)

Let's say you want to make a commercial game with Solarus, i.e. copyright your scripts and resources. Here are the legal requirements:

- You must package the executable `solarus_run` _unmodifed_ (or with modifications made public), along with your quest data file `data.solarus` (or `your-quest-name.solarus`), and all required libs.

- You must provide a [copy of the GPL license](https://www.gnu.org/licenses/gpl-3.0.txt) with the program, so that players are aware of their rights. A text file in the download package, or within an informative Solarus `menu` accessible by the player, are valid ways to do it.

- You must include the Solarus source code, or make the Solarus source code freely available. An hyperlink to [solarus-games.org](https://www.solarus-games.org) is a valid way to do it.

- You may license your quest as _proprietary_, but it currently cannot be _closed-source_. Players will be able to see the quest data file content. You can legally forbid modification if you copyright your quest.

- You may preserve the Solarus logo at the quest start. It's a nice way to say thank you to the community. The logo is licensed under MIT (Lua code) and CC-BY (art, sound).

## Using the Commercial-Friendly Solarus MIT Starter Pack

Coincidentally, Solarus community has initiated a project called [MIT Starter Quest](https://gitlab.com/Splyth/solarus-mit-starter-quest). It contains only permissive-licensed scripts and resources:

- **MIT-licensed Lua scripts** (or more permissive license).
- **CC-BY-licensed resources** (or more permissive license like CC0 a.k.a. _Public Domain_).

It means you can use these scripts and resources for proprietary commercial projects and free non-commercial projects:

- Modifications are allowed, may remain private, and may be distributed under different terms.
- No need to distribute source code.
- You must still preserve the original copyright and license notices.

Some essential scripts like the one that handles multi-events or the dialog box have been re-written and licensed under MIT, so you can use them in a commercial project.
