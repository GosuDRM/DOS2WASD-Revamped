#include "Settings.hpp"
#include "State.hpp"
#include "OverrideDialogVerticalOffset2.hpp"

bool OverrideDialogVerticalOffset2::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"0F 57 C0 C3 48 8B ?? ?? ?? ?? ?? 80 B9 C0 04">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("OverrideDialogVerticalOffset2 #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    DEBUG("OverrideDialogVerticalOffset2 found: {:X}", AsAddress(addresses[0]));
    return all_found;
}

void OverrideDialogVerticalOffset2::Activate()
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

void OverrideDialogVerticalOffset2::Enable()
{
    OverrideDialogVerticalOffset2Asm patch;
    patch.ready();
    handle = dku::Hook::AddASMPatch(addresses[0], { 0, 3 }, &patch);
}

void OverrideDialogVerticalOffset2::Deactivate()
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
