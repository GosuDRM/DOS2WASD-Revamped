#include "HotkeyRotationInput.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool HotkeyRotationInput::Prepare()
{
    std::array<uintptr_t, 3> address_array = {
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 51 0E 00 00 E9">()),
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 38 0E 00 00 E9">()),
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 03 0E 00 00 E9">())
    };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("HotkeyRotationInput #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void HotkeyRotationInput::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked HotkeyRotationInput #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void HotkeyRotationInput::OverrideFunc(int64_t a1, char a2)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    *(float*)(a1 + 1100) *= *(settings->hotkey_rotation_speed);

    return OriginalFunc(a1, a2);
}
