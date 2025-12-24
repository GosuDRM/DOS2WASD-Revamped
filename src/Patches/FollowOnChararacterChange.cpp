#include "FollowOnChararacterChange.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool FollowOnChararacterChange::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"38 05 ?? ?? ?? 01 0F 85">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("FollowOnChararacterChange #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    DEBUG("FollowOnChararacterChange found: {:X}", AsAddress(addresses[0]));
    return all_found;
}

void FollowOnChararacterChange::Activate()
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

void FollowOnChararacterChange::Deactivate()
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

void FollowOnChararacterChange::Enable()
{
    FollowOnChararacterChangeAsm patch;
    patch.ready();
    handle = dku::Hook::AddASMPatch(addresses[0], { 0, 6 }, &patch);
}
