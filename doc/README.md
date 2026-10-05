# Solarus Documentation

Source code for the **documentation** of the [Solarus game engine](https://www.solarus-games.org/), built with [Material for MkDocs](https://squidfunk.github.io/mkdocs-material/).

## Contribute

If you want to contribute to the lua API reference, please read the [contribution guide](CONTRIBUTING.md) first.
Note that this guide is currently in progress and may not be complete, so feel free to improve it.

## Setup

First, install `mkdocs` and plugins:

```bash
pip install -r requirements.txt
```

To run the website locally, go to the website directory, then type:

```bash
mkdocs serve
```

## Build

To build the website to the `/public` directory, ready to be deployed:

```bash
rm -rf ./public
mkdocs build --site-dir public
```

## Thanks

Thanks to [S2009](https://s2009.fr) and [PenTaist](https://pentaist.fr) who helped carrying out the migration of the documentation from Doxygen to MkDocs.
