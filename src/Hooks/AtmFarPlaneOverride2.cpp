#include "AtmFarPlaneOverride2.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool AtmFarPlaneOverride2::Prepare()
{
    std::array<uintptr_t, 2> address_array = {
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? ?? FF 01 BB FC">()),
        AsAddress(dku::Hook::Assembly::search_pattern<
            "E8 ?? ?? ?? FF 01 7B 10 8B 43 10 3B 43 0C 76 70">())
    };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("AtmFarPlaneOverride2 #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void AtmFarPlaneOverride2::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked AtmFarPlaneOverride2 #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void* AtmFarPlaneOverride2::OverrideFunc(void* dest, const void* src, size_t size)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (state->override_atm_far_plane == 1)
    {
        // Means we hit SetNearPlane. Next time will come SetFarPlane.
        state->override_atm_far_plane = 2;
        return OriginalFunc(dest, src, size);
    }
    else if (state->override_atm_far_plane == 2)
    {
        float far_plane = 3000.0f;
        *(float*)dest = far_plane;
        state->override_atm_far_plane = 0;
        return dest;
    }
    else
    {
        return OriginalFunc(dest, src, size);
    }
}
