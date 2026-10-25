# Solarus Launcher for PS Vita

**[⬇ Download SolarusLauncher.vpk](https://github.com/LAB0ULANGE/solarus-vita/releases/latest/download/SolarusLauncher.vpk)** · [All releases](https://github.com/LAB0ULANGE/solarus-vita/releases) · [Version française plus bas](#version-française)

A port of the [Solarus](https://www.solarus-games.org) 2.0 engine to the PlayStation Vita, with a built-in quest launcher.
Browse the free games published on solarus-games.org, download them and play them directly on your Vita — no PC needed.

**Current version: 2.1** (title ID `SOLR00002`).

![LiveArea](vpk/sce_sys/livearea/contents/bg.png)

## Screenshots

| Game list | Loading a game | In-game engine menu (Select + L) |
|---|---|---|
| ![Game list](docs/screenshots/launcher.jpg) | ![Loading a game](docs/screenshots/loading.jpg) | ![Engine menu](docs/screenshots/engine-menu.jpg) |

| In game | In game |
|---|---|
| ![In game](docs/screenshots/ingame-1.jpg) | ![In game](docs/screenshots/ingame-2.jpg) |

## Features

- Solarus engine 2.0.3 running natively on the Vita
- Built-in launcher: game list with pictures and descriptions, downloaded from solarus-games.org
- One-tap download and install of quests (stored in `ux0:data/Solarus/quests/`)
- Delete an installed game from the launcher, with or without its saves
- In-game engine menu (**Select + L**): per-game button remapping, saved for each game
- **R**: toggle stretched / centered display
- Pressing the PS button pauses the game; reopening the app resumes it
- Interface in French or English, following the console language

## Installation

1. **[Download SolarusLauncher.vpk](https://github.com/LAB0ULANGE/solarus-vita/releases/latest/download/SolarusLauncher.vpk)**.
2. Copy it to your Vita and install it with VitaShell.
3. Launch **Solarus Launcher**, pick a game and press X to download it.

An internet connection is needed to download games. Already downloaded games can be played offline.

### Updating from Solarus Vita 1.x (Mystery of Solarus DX)

Version 2.0 replaces the first port, [Solarus Vita](https://github.com/LAB0ULANGE/solarus-vita), which only contained Mystery of Solarus DX. It uses the same title ID (`SOLR00002`), so just install it over the old version with VitaShell.

Your Mystery of Solarus DX saves are kept: both versions use the same folder, `ux0:data/.solarus/zsdx/`. Mystery of Solarus DX is no longer included in the package: download it once from the launcher (about 20 MB) and your saves will show up.

## Notes

- Some games are hidden because they are too slow on the Vita for now, or already have a native Vita port (Return of the Hylian SE).
- The `vpk/overrides/` folder contains small Vita-specific adjustments (HUD layout, French translations) applied on top of some quests.

## Building

The port is built with [VitaSDK](https://vitasdk.org) under MSYS2. See [compilation.MSYS2.md](compilation.MSYS2.md) for the toolchain, then build the `solarus-run` target in a `build-vita` folder and package `eboot.bin` with the contents of `vpk/`.

## Credits

- [Solarus](https://gitlab.com/solarus-games/solarus) by Christopho and the Solarus team — the engine and the games.
- PS Vita port and launcher by LAB0ULANGE.

## License

Solarus is free software licensed under the GNU GPL v3 (see [license](license)). This port is distributed under the same license.
The games downloaded by the launcher belong to their respective authors and keep their own licenses.

This project is not affiliated with Nintendo or Sony.

---

## Version française

**[⬇ Télécharger SolarusLauncher.vpk](https://github.com/LAB0ULANGE/solarus-vita/releases/latest/download/SolarusLauncher.vpk)**

Un portage du moteur [Solarus](https://www.solarus-games.org) 2.0 sur PlayStation Vita, avec un lanceur de jeux intégré.
Parcourez les jeux gratuits publiés sur solarus-games.org, téléchargez-les et jouez-y directement sur votre Vita, sans passer par un PC.

**Version actuelle : 2.1** (title ID `SOLR00002`).

### Fonctionnalités

- Moteur Solarus 2.0.3 qui tourne nativement sur la Vita
- Lanceur intégré : liste des jeux avec images et descriptions, récupérée depuis solarus-games.org
- Téléchargement et installation d'un jeu en une touche (stockés dans `ux0:data/Solarus/quests/`)
- Suppression d'un jeu installé depuis le lanceur, avec ou sans ses sauvegardes
- Menu moteur en jeu (**Select + L**) : réaffectation des boutons, mémorisée pour chaque jeu
- **R** : bascule entre affichage étiré et centré
- Le bouton PS met le jeu en pause ; en rouvrant l'appli, on reprend la partie
- Interface en français ou en anglais, selon la langue de la console

### Installation

1. **[Téléchargez SolarusLauncher.vpk](https://github.com/LAB0ULANGE/solarus-vita/releases/latest/download/SolarusLauncher.vpk)**.
2. Copiez-le sur votre Vita et installez-le avec VitaShell.
3. Lancez **Solarus Launcher**, choisissez un jeu et appuyez sur X pour le télécharger.

Une connexion internet est nécessaire pour télécharger les jeux. Les jeux déjà téléchargés se jouent hors ligne.

### Mise à jour depuis Solarus Vita 1.x (Mystery of Solarus DX)

La version 2.0 remplace le premier portage, [Solarus Vita](https://github.com/LAB0ULANGE/solarus-vita), qui ne contenait que Mystery of Solarus DX. Elle utilise le même title ID (`SOLR00002`) : il suffit de l'installer par-dessus l'ancienne version avec VitaShell.

Vos sauvegardes de Mystery of Solarus DX sont conservées : les deux versions utilisent le même dossier, `ux0:data/.solarus/zsdx/`. Mystery of Solarus DX n'est plus inclus dans le paquet : téléchargez-le une fois depuis le lanceur (environ 20 Mo) et vos sauvegardes réapparaîtront.

### Remarques

- Certains jeux sont masqués, car ils sont encore trop lents sur la Vita ou ont déjà un portage Vita natif (Return of the Hylian SE).
- Le dossier `vpk/overrides/` contient de petits ajustements propres à la Vita (disposition du HUD, traductions françaises) appliqués à certains jeux.

### Compilation

Le portage se compile avec [VitaSDK](https://vitasdk.org) sous MSYS2. Voir [compilation.MSYS2.md](compilation.MSYS2.md) pour la chaîne de compilation, puis compiler la cible `solarus-run` dans un dossier `build-vita` et assembler `eboot.bin` avec le contenu de `vpk/`.

### Crédits

- [Solarus](https://gitlab.com/solarus-games/solarus) par Christopho et l'équipe Solarus : le moteur et les jeux.
- Portage PS Vita et lanceur par LAB0ULANGE.

### Licence

Solarus est un logiciel libre sous licence GNU GPL v3 (voir [license](license)). Ce portage est distribué sous la même licence.
Les jeux téléchargés par le lanceur appartiennent à leurs auteurs respectifs et gardent leurs propres licences.

Ce projet n'est affilié ni à Nintendo ni à Sony.
