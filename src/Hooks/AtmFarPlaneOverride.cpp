#include "AtmFarPlaneOverride.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool AtmFarPlaneOverride::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"E8 96 B7 01 00 48 8D">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("AtmFarPlaneOverride #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void AtmFarPlaneOverride::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked AtmFarPlaneOverride #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

// This is called on game launch, when the engine sets the near and far plane of atmospheres.
// The actual set happens in AtmFarPlaneOverride 2.
char AtmFarPlaneOverride::OverrideFunc(int64_t a1, int64_t a2)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (*settings->improve_sky)
    {
        state->override_atm_far_plane = 1;
    }

    return OriginalFunc(a1, a2);
}
