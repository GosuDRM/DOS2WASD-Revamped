#include <codecvt>
#include <locale>

#include "Settings.hpp"
#include "State.hpp"
#include "ConcatInputconfigPathHook.hpp"

bool ConcatInputconfigPathHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
        "E8 ?? ?? ?? FE 48 8D 4D B0 E8 ?? ?? ?? FF 45 33 C9 48 8D 55 B0 45 33 C0">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("ConcatInputconfigPathHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void ConcatInputconfigPathHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked ConcatInputconfigPathHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

QWORD* ConcatInputconfigPathHook::OverrideFunc(QWORD* Src, char* a2, size_t Size)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    QWORD* return_value = OriginalFunc(Src, a2, Size);

    if (state->inputconfig_path == L"")
    {
        char* inputconfig_path_char = *(char**)return_value;
        std::string inputconfig_path_string{ inputconfig_path_char };
        if (dku::string::icontains(inputconfig_path_string, "Edition/PlayerProfiles/"))
        {
            std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
            std::wstring inputconfig_path_wstring = converter.from_bytes(inputconfig_path_string);
            state->inputconfig_path = inputconfig_path_wstring;

        }
    }

    return return_value;
}
