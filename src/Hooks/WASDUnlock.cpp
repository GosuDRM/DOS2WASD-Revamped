#include "WASDUnlock.hpp"
#include "State.hpp"

struct WASDUnlockPatch : Xbyak::CodeGenerator
{
    WASDUnlockPatch()
    {
        nop();
        nop();
    }
};

void* WASDUnlock::Search(uintptr_t a_base = 0)
{
    return dku::Hook::Assembly::search_pattern<
        "74 E6 E8 9F D6 E6 FF">(a_base);
}

bool WASDUnlock::Prepare()
{
    auto match1 = Search();
    address1 = AsAddress(match1);

    if (not(address1))
    {
        State::GetSingleton()->mod_found_all_addresses = false;
        WARN("WASD unlock not found!");
        return false;
    }

    return true;
}

void WASDUnlock::Enable()
{
    if (not(address1))
    {
        return;
    }

    WASDUnlockPatch patch;

    dku::Hook::WritePatch(address1, &patch);
    DEBUG("WASD unlocked : {:X}", address1);

    return;
}
