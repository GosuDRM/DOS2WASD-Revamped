#include "SettingsPtr.hpp"
#include "State.hpp"

bool SettingsPtr::Prepare()
{
    uintptr_t address = AsAddress(
        dku::Hook::Assembly::search_pattern<"48 8B 15 ?? ?? ?? ?? 48 8D 82 1C 0D 00 00 75 07">());

    if (!address)
    {
        State::GetSingleton()->mod_found_all_addresses = false;
        WARN("SettingsPtr not found.");
        return false;
    }

    int32_t offset = *reinterpret_cast<int32_t*>(address + 3);
    settings_ptr = reinterpret_cast<int64_t*>(address + 7 + offset);
    DEBUG("SettingsPtr found {:X}", AsAddress(settings_ptr))
    return true;
}

int64_t SettingsPtr::Read()
{
    if (settings_ptr == 0)
    {
        return false;
    }

    return *settings_ptr;
}
