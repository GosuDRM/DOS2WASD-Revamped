#include "MouseDeltaCavehook.hpp"
#include "Settings.hpp"
#include "State.hpp"

struct MouseDeltaProlog : Xbyak::CodeGenerator
{
    MouseDeltaProlog()
    {
        push(rax);
        push(rbx);
        push(rcx);
        push(rdx);
        push(r8);
        push(r9);
        push(r10);
        push(r11);
        mov(rcx, ptr[rbp + 0x0000009C]);
        mov(rdx, ptr[rbp + 0x000000A0]);
    }
};

struct MouseDeltaEpilog : Xbyak::CodeGenerator
{
    MouseDeltaEpilog()
    {
        pop(r11);
        pop(r10);
        pop(r9);
        pop(r8);
        pop(rdx);
        pop(rcx);
        pop(rbx);
        pop(rax);
    }
};

bool MouseDeltaCavehook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"8B BD A0 00 00 00 0F 5B C0">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("MouseDeltaCavehook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void MouseDeltaCavehook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        MouseDeltaProlog prolog;
        prolog.ready();
        MouseDeltaEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 6 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kRestoreBeforeProlog);
        handle->Enable();
        DEBUG("Hooked MouseDeltaCavehook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void MouseDeltaCavehook::Func(int dx, int dy)
{
    auto* state = State::GetSingleton();

    if (dx > 500) dx = 500;
    if (dx < -500) dx = -500;
    if (dy > 500) dy = 500;
    if (dy < -500) dy = -500;

    state->dx = dx;
    state->dy = dy;
}
