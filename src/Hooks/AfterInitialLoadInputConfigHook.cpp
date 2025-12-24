#include "AfterInitialLoadInputConfigHook.hpp"
#include "InputconfigPatcher.hpp"
#include "State.hpp"

bool AfterInitialLoadInputConfigHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"E8 C1 EE FF FF">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("AfterInitialLoadInputConfigHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void AfterInitialLoadInputConfigHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, Hook);
        DEBUG("Hooked AfterInitialLoadInputConfigHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

bool AfterInitialLoadInputConfigHook::IsValid() { return OriginalFunc != nullptr; }

int64_t AfterInitialLoadInputConfigHook::Hook(QWORD* a1)
{
    auto ret = CallOriginal(a1);

    InputconfigPatcher::Patch();

    return ret;
}

int64_t AfterInitialLoadInputConfigHook::CallOriginal(QWORD* a1)
{
    return OriginalFunc(a1);
}
