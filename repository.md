# Repository Management

This repository is managed using the
[Gitflow workflow](https://nvie.com/posts/a-successful-git-branching-model/).

In summary:

* The `dev` branch is the development branch for the next minor release.
* The `vX.Y` branches are the development branches for all `X.Y.*` patch releases.
* The `release-X.Y.Z` branches are short-term branches that finalize `X.Y.Z` releases.
* The `vX.Y.Z` TAGS are for releases `X.Y.Z`.
* The `master` branch has all releases.

In addition:

* The `release-X.Y.Z` branches are merged to `master` and the merge commit in `master` is
  the one that receives the `vX.Y.Z` tag.
* The `release-X.Y.Z` branches are also merged back to the `vX.Y` branches, which in turn are
  also merged back to the `dev` branch from time to time.

> [!important]
> Release version numbers DO NOT strictly follow [Semantic Versioning](https://semver.org/).
> It is recommended to refer to the official
> [migration guides](https://docs.solarus-games.org/tutorials/migration/introduction/)
> between versions.
