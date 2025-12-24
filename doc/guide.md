# DOS2WASD Revamped

> **Note:** This is the Revamped fork with additional cross-platform improvements.  
> Original guide from [Ch4nKyy/DOS2WASD](https://github.com/Ch4nKyy/DOS2WASD).

For the complete guide, please read doc/guide.md in the BG3 WASD Movement repo.  
Since both games use the same engine, it will be very similar and we don't want to maintain two docs.  
This document here shall only document the differences between BG3 and DOS2.

---

## Revamped Changes (Linux/Proton Compatibility)

The original mod used Windows-specific low-level hooks (`WH_KEYBOARD_LL`, `WH_MOUSE_LL`) which don't work under Wine/Proton.

**Changes made in Revamped:**

1. **SDL-Based Input Handling** — Replaced Windows hooks with cross-platform SDL event processing
   - New file: `SDLInputHandler.cpp` / `SDLInputHandler.hpp`
   - `InputHook.cpp` is now an empty stub

2. **Event Processing** — Uses `SDL_PeepEvents()` with `SDL_PEEKEVENT` to read events without removing them from the queue, so the game still receives all input

3. **Deduplication** — Added timestamp tracking to avoid processing the same event multiple times per frame

4. **Code Quality** — Zero-initialized SDL events, replaced Windows types (`WORD`) with standard types (`uint16_t`)

---

## Core

In DOS2, the camera struct looks a bit different. Some things that were members of the camera object are now part of the owning manager object I think.

In BG3, the engine automatically centers the character, when you move with WASD.  
In DOS2, this does not happen anymore. You need to set the according flag (manager + 1196).
This usually happens when you press the Center Camera key, but we want it always, so we force this flag inside GetCameraObjectHook.  
If the flag is always true, Center Camera would not center anymore, because it looks if the flag changed. The patch CenterCameraAlwaysJumps fixes that. But who cares about this command.

In BG3, we have the BlockAnalogStickSelection patches that fix an issue with a BG3 controller setting that messes up the movement input.  
In DOS2, this feature does not seem to exist, so we don't need the patch.

In BG3, implementing a slow walk was easily possible by scaling the movement input vectors/values.
In DOS2, it didn't work at first. The engine is doing some input vector calculations and checks that seem a bit overcomplicated or buggy. I don't know, it didn't debug it. However, removing two of those checks makes walking possible (Patches FixWalking1 and FixWalking2).

## Improved Mouselook

Improved mouselook requires SDL_HINT_MOUSE_RELATIVE_MODE_CENTER and some bugfixes from SDL 2.0.28. DOS2 shipped with SDL 2.0.7. These versions are incompatible, since the old version has some exports that are missing in the new one, specifically SDL_JoystickGetDInputDeviceReference(), SDL_JoystickIsXInput() and SDL_JoystickGetXInputPlayerIndex().  
The SDL2 version in the extern directory is a modified version 2.0.28.5 that stubs or redirects these functions so the game does not crash.  
That means we have to build the dll ourself.

In BG3, SDL_PollEvents() is called.  
In DOS2, SDL_PumpEvents() is called instead.

In BG3, we have dedicated SetCursorRotate() and ResetCursorRotate() functions that can be hooked.  
In DOS2, this is happening inline, e.g. in PollEvents(), so we had to create SetOrResetCursorRotateCavehook.

In BG3, SDL_GetWindowGrab() was called and hooked.  
In DOS2, this is not called anymore, but SDL_SetWindowGrab() is, which is also fine.

## Camera Improvements

In BG3, the mod Native Camera Tweaks by Ersh provided camera improvements.  
In DOS2, this mod does not exist and the existing alternatives are rather uncomfortable to use.  
Therefore, I implemented a few features in this mod.

We read the mouse delta in the PollEvents function and use that to calculate and overwrite the pitch.

Many settings can easily be changed by overwriting the settings struct.  
The zoom speed needs a hook to affect scroll wheel zooming.

## Improved Rightclick Rotation

In DOS2, CastOrCancelAbility is now called virtually, so we replaced the CastOrCancelAbilityHook with CastOrCancelAbilityKeydownCavehook and CastOrCancelAbilityKeyupHook.

In DOS2, ContextMenu only fires an keyup event, not a keydown event, so we cannot just stop the time between the keydown and keyup.  
Instead, if ContextMenu and CameraRotate have the same key, just use the timer of CameraRotate. If they don't, don't block ContextMenu and all is good.

## InteractMoveBlocker/Canceller

No noteworthy differences.
