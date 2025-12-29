#include "ZoomHook.hpp"
#include "Addresses/IsInControllerMode.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool ZoomHook::Prepare()
{
    std::array<uintptr_t, 2> address_array = {
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 D9 0F 00 00 E9 82 02 00 00">()),
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 5B 0F 00 00 E9 04">())
    };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("ZoomHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void ZoomHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked ZoomHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void ZoomHook::OverrideFunc(int64_t a1, char a2)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    // In controller mode: zoom only works when left stick is pressed
    // (right stick Y-axis is used for pitch when left stick is not pressed)
    if (IsInControllerMode::Read())
    {
        if (state->controller_left_stick_pressed.load())
        {
            // Left stick is pressed: allow normal zoom behavior
            *(float*)(a1 + 1108) *= *(settings->zoom_speed);
        }
        else
        {
            // Left stick is not pressed: disable zoom (pitch is used instead)
            *(float*)(a1 + 1108) = 0.0f;
        }
    }
    else
    {
        // Mouse/keyboard mode: normal zoom behavior
        *(float*)(a1 + 1108) *= *(settings->zoom_speed);
    }

    return OriginalFunc(a1, a2);
}
