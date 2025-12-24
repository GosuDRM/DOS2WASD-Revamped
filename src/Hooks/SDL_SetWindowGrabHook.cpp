#include "SDL_SetWindowGrabHook.hpp"
#include "State.hpp"
#include "SDL.h"

bool SDL_SetWindowGrabHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
        "FF 15 ?? ?? C3 00 48 8B 5C 24 40 48 8B 6C 24 48 48 8B 74 24 50 48 83 C4 30 5F C3 CC CC CC "
        "CC CC CC CC CC 48">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("SDL_SetWindowGrabHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void SDL_SetWindowGrabHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<6>(address, OverrideFunc);
        DEBUG("Hooked SDL_SetWindowGrabHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

// Called in MainThread, every frame that has a mouse motion event
void SDL_SetWindowGrabHook::OverrideFunc(int64_t window, bool grabbed)
{
    State::GetSingleton()->sdl_window = reinterpret_cast<SDL_Window*>(window);

    OriginalFunc(window, grabbed);
}
