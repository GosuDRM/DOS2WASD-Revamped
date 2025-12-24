#include "FixWarpMouseInWindowCrash.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool FixWarpMouseInWindowCrash::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"FF 15 2E ?? C3 00 E9">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("FixWarpMouseInWindowCrash #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    DEBUG("FixWarpMouseInWindowCrash found: {:X}", AsAddress(addresses[0]));
    return all_found;
}

void FixWarpMouseInWindowCrash::Activate()
{
    if (not all_found)
    {
        return;
    }

    if (!handle)
    {
        return;
    }
    handle->Enable();
}

void FixWarpMouseInWindowCrash::Deactivate()
{
    if (not all_found)
    {
        return;
    }

    if (!handle)
    {
        return;
    }
    handle->Disable();
}

void FixWarpMouseInWindowCrash::Enable()
{
    FixWarpMouseInWindowCrashAsm patch;
    patch.ready();
    handle = dku::Hook::AddASMPatch(addresses[0], { 0, 6 }, &patch);
}
