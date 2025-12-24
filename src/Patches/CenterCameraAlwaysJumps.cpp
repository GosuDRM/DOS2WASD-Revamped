#include "CenterCameraAlwaysJumps.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool CenterCameraAlwaysJumps::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
        "83 BB AC 04 00 00 01 48 8B 10 75 0C 48 8B 83 A0 04 00 00 48 39 10 74 13 45 33 C9 C6 44 24 "
        "20 03 45 33 C0 48 8B CB E8 81 10 00 00">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("CenterCameraAlwaysJumps #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    DEBUG("CenterCameraAlwaysJumps found: {:X}", AsAddress(addresses[0]));
    return all_found;
}

void CenterCameraAlwaysJumps::Activate()
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

void CenterCameraAlwaysJumps::Deactivate()
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

void CenterCameraAlwaysJumps::Enable()
{
    CenterCameraAlwaysJumpsAsm patch;
    patch.ready();
    handle = dku::Hook::AddASMPatch(addresses[0], { 0, 7 }, &patch);
}
