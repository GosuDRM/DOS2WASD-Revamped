# Changelog

All notable changes to DOS2WASD Revamped will be documented in this file.

## [3.0.1] - 2024-12-29

### ✨ New Features
- **Controller Support**
  - Right stick up/down now controls camera pitch instead of zoom
  - Hold left stick button + right stick for original zoom functionality
  - Added `ControllerPitchSpeed` setting in `DOS2WASD.toml`
  - Process SDL controller axis and button events in PollEventHook
  - Modified PitchHook and ZoomHook to handle controller input
  - Added state tracking for `controller_left_stick_pressed` and `controller_right_stick_y`

## [3.0.0] - 2024-12-24

### 🚀 Major Changes
- **Linux/Proton Compatibility** — Full support for Steam Play (Proton) on Linux
- **SDL-Based Input** — Replaced Windows-only low-level hooks with cross-platform SDL event handling

### ✨ Improvements
- Uses `SDL_PeepEvents()` with `SDL_PEEKEVENT` to read input without consuming events
- Added timestamp-based deduplication to prevent double-processing
- Zero-initialized SDL events for memory safety
- Replaced Windows types (`WORD`) with standard types (`uint16_t`)
- Added thread safety documentation for shared state

### 🐛 Bug Fixes
- Fixed Caps Lock toggle not working (events were being removed from SDL queue)
- Fixed potential undefined behavior from uninitialized SDL event structs

### 🧹 Code Quality
- Removed Windows-specific input hooks (`WH_KEYBOARD_LL`, `WH_MOUSE_LL`)
- `InputHook.cpp` is now a stub — all input handled via `SDLInputHandler`
- Cleaned up repository (removed duplicate CMake files and junk)

### ⚡ Performance
- Changed all string parameters to `const std::string&` to avoid copies
- Changed vector parameters to `const std::vector&` to avoid allocations
- Fixed range-for loops to use `const auto&` instead of copying
- **VirtualKeyMap.hpp/.cpp** — All functions now use `const std::string&` and `const std::vector&`
- **InputFaker.hpp/.cpp** — All functions now use `const std::string&`
- **Settings.hpp/.cpp** — `GetBoundKeycombos` uses `const std::string&`
- **MessageBox.hpp/.cpp** — `Show` and `Show_Impl` use `const std::string&`

### 📦 Build System
- Updated `build-release.ps1` to package all DLLs into a single zip file
- Includes: `DOS2WASD.dll`, `SDL2.dll`, `SDL2_original.dll`, `bink2w64.dll`, `bink2w64_original.dll`, `DOS2WASD.toml`

