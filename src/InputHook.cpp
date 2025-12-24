#include "InputHook.hpp"

// Windows low-level hooks (WH_KEYBOARD_LL, WH_MOUSE_LL) are not supported on Linux/Proton.
// Input is now handled via SDL events in PollEventHook using SDLInputHandler.
// This function is intentionally left empty for cross-platform compatibility.
void InputHook::Enable(HMODULE a_hModule)
{
    (void)a_hModule;  // Suppress unused parameter warning
}
