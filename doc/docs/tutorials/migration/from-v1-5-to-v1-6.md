# Upgrade Your Quest from Solarus 1.5 to Solarus 1.6

Solarus 1.6 brings a lot new features and customization abilities, including shaders and custom hero states.
Luckily, there is no incompatibility, so the upgrade process from a 1.5 quest to 1.6 is very straightforward.
However, some Lua API functions become deprecated in Solarus 1.6.

## Upgrading Data Files

Data files other than scripts can be upgraded automatically with the editor. Open your quest with Solarus Editor 1.6 and a dialog will let you to perform the upgrade.

## Upgrading Lua Scripts

The Lua scripting API of Solarus 1.6 introduces no incompabilities. You have nothing to do!

### Deprecated Functions

The following functions are now deprecated. You can still use them in Solarus 1.6, but they will show a warning. They might be removed in a future version.

- `sol.video.get_mode()`, `sol.video.set_mode()`, `sol.video.switch_mode()`, `sol.video.get_modes()`, `sol.video.is_mode_supported()` and are now deprecated, you can use [shaders](../../lua-api/shaders.md) instead. Shaders are faster, more powerful and customizable, making built-in video modes obsolete. We provide shaders that perform the same effects as deprecated built-in video modes.
- `sol.video.reset_window_size()` is now deprecated, use [`sol.video.set_window_size()`](../../lua-api/video.md#solvideoset_window_sizewidth-height) instead.
