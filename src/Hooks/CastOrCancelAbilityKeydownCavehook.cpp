#include "CastOrCancelAbilityKeydownCavehook.hpp"
#include "GameCommand.hpp"
#include "InputFaker.hpp"
#include "Patches/BlockCancelActionStoppingMovementPatch.hpp"
#include "Settings.hpp"
#include "State.hpp"

using enum GameCommand;

struct CastOrCancelAbilityKeydownProlog : Xbyak::CodeGenerator
{
    CastOrCancelAbilityKeydownProlog()
    {
        push(rax);
        push(rbx);
        push(rcx);
        push(rdx);
        push(r8);
    }
};

struct CastOrCancelAbilityKeydownEpilog : Xbyak::CodeGenerator
{
    CastOrCancelAbilityKeydownEpilog()
    {
        pop(r8);
        pop(rdx);
        pop(rcx);
        pop(rbx);
        pop(rax);
        mov(word [rbx], 0);
    }
};

bool CastOrCancelAbilityKeydownCavehook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
        "FF 90 80 01 00 00 83 BF BC 00 00 00 04 7D 0A C7 87 BC 00 00 00 04 00 00 00 66 C7 03 01 00 "
        "48 8B C3 48 8B 5C 24 30 48 83 C4 20 5F C3">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("CastOrCancelAbilityKeydownCavehook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void CastOrCancelAbilityKeydownCavehook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        CastOrCancelAbilityKeydownProlog prolog;
        prolog.ready();
        CastOrCancelAbilityKeydownEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address + 25, { 0, 5 }, FUNC_INFO(Func), &prolog,
            &epilog, DKUtil::Hook::HookFlag::kSkipNOP);
        handle->Enable();
        DEBUG("Hooked CastOrCancelAbilityKeydownCavehook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void CastOrCancelAbilityKeydownCavehook::Func()
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    state->last_time_cancel_action_pressed2 = SDL_GetTicks();
}
