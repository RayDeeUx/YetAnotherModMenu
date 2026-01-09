# YetAnotherModMenu Changelog
## v1.0.7
- Someone had the bright idea to make levels use a different set of logic when restarting from Practice Mode.
  - Now we all have to suffer the consequences.
## v1.0.6
- Added support for applying changes to trails from the `Icon Kit In Pause Menu` in (almost) real-time. No more re-entering the level to see your new trail!
- Fix a pretty nasty crash when changing death effects from using the `Icon Kit In Pause Menu` feature without [Texture Loader](mod:geode.texture-loader) loaded.
- Fix a pretty nasty bug that sends all people back to the main menu from using `Icon Kit In Pause Menu`.
## v1.0.5
- Refactor `Icon Kit In Pause Menu` to solve touch priority issues.
  - Also add warning to its description about probably being kicked out of a level if opening the Soggy Mod screen from an Icon Kit screen opened from the pause menu, along with other warnings.
  - Currently this implementation could stop you from using the Escape key to fully exit a level, but seeing as the point of opening the icon kit from the pause menu was to change icons without exiting a level, let's consider it as a feature(TM) for now.
- Disabled the keybind in the level editor.
  - To restore this previous behavior, don't bother. Simply curl up in a fetal position and contemplate your life decisions instead.
## v1.0.4
- Added settings related to FiL/tH (F\*r\* \*n th\* H\*l\*) for iOS devices.
  - This emergency update was made possible by hiimjasmine00 and slideglide.
## v1.0.3
- Added `Smooth Node Pulsing` (enabled by default to keep current behavior) for the 1% of people who prefer a different pulsing method.
- Added `Non-Smooth Node Pulsing Factor` (set to `3.6` by default per suggestion of Vinster, the setting's primary target audience) for those who want to mess with the pulsing even further.
- Revised a few setting names related to coin pathfinding.
- Revised a few setting descriptions to take advantage of the new markdown support.
## v1.0.2
- Experimental iOS support.
## v1.0.1
- Renamed due to political concerns.
## v1.0.0
- Initial release (on GitHub).
