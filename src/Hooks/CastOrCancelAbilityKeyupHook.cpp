#include "Settings.hpp"
#include "State.hpp"
#include "CastOrCancelAbilityKeyupHook.hpp"

bool CastOrCancelAbilityKeyupHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"E8 3B CC FA FF">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("CastOrCancelAbilityKeyupHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void CastOrCancelAbilityKeyupHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked CastOrCancelAbilityKeyupHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

int64_t CastOrCancelAbilityKeyupHook::OverrideFunc(QWORD* a1, uint8_t a2)
{
    auto* state = State::GetSingleton();

    if (*Settings::GetSingleton()->enable_improved_mouselook)
    {
        uint32_t time_now = SDL_GetTicks();
        uint32_t time_diff_millis = time_now - state->last_time_cancel_action_pressed2;
        if (time_diff_millis > *Settings::GetSingleton()->rotate_threshold)
        {
            return 0;
        }
    }
    return OriginalFunc(a1, a2);
}
