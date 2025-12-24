#include "FixWalking1.hpp"
#include "Settings.hpp"
#include "State.hpp"

bool FixWalking1::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"0F 2F C1 76 0A F3 0F 10 05 ?? ?? ?? 00">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("FixWalking1 #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    DEBUG("FixWalking1 found: {:X}", AsAddress(addresses[0]));
    return all_found;
}

void FixWalking1::Activate()
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

void FixWalking1::Deactivate()
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

void FixWalking1::Enable()
{
    FixWalking1Asm patch;
    patch.ready();
    handle = dku::Hook::AddASMPatch(addresses[0], { 0, 3 }, &patch);
}
