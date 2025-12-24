#pragma once

#include "SDL.h"
#include "Settings.hpp"
#include "State.hpp"
#include "VirtualKeyMap.hpp"

#include <string>

// SDL-based input handler for cross-platform compatibility (Windows + Linux Proton)
class SDLInputHandler
{
public:
    // Process an SDL event and trigger mod actions if applicable
    static void ProcessEvent(const SDL_Event* event);

private:
    static inline SDL_Keycode last_keycode = 0;
    static inline Uint8 last_mouse_button = 0;
    static inline SDL_Keymod last_mod_state = KMOD_NONE;
    static inline bool last_was_keydown = false;
    static inline Uint32 last_event_timestamp = 0;  // For deduplication with SDL_PEEKEVENT

    static void HandleInput();
    static bool DidCommandChange(Command command, bool keydown);
    static bool IsModifierDown(SDL_Keymod mod);

    static void AutoRun(State* state);
    static void ToggleMovementMode(State* state);
    static void WalkOrSprint(State* state);
    static void ReloadConfig();
    static void MouseLeftDown(const SDL_Event* event);
    static void MouseRightDown(const SDL_Event* event);
};
