#include "Settings.hpp"
#include "State.hpp"
#include "SetOrResetCursorRotateCavehook.hpp"

struct SetOrResetCursorRotateProlog : Xbyak::CodeGenerator
{
    SetOrResetCursorRotateProlog()
    {
        push(rax);
        push(rbx);
        push(rcx);
        push(rdx);
        push(r8);
        mov(rcx, rax);
    }
};

struct SetOrResetCursorRotateEpilog : Xbyak::CodeGenerator
{
    SetOrResetCursorRotateEpilog()
    {
        pop(r8);
        pop(rdx);
        pop(rcx);
        pop(rbx);
        pop(rax);
        and_(al, 1);
    }
};

bool SetOrResetCursorRotateCavehook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"41 B8 2D 00 00 00 B9 30">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("SetOrResetCursorRotateCavehook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void SetOrResetCursorRotateCavehook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        SetOrResetCursorRotateProlog prolog;
        prolog.ready();
        SetOrResetCursorRotateEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 6 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kRestoreAfterEpilog);
        handle->Enable();
        DEBUG("Hooked SetOrResetCursorRotateCavehook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void SetOrResetCursorRotateCavehook::Func(int64_t set)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (*Settings::GetSingleton()->enable_improved_mouselook)
    {
        if (set)
        {
            state->last_time_rotate_pressed = SDL_GetTicks();
            state->rotation_validated = true;

            state->SetInternalIsRotating(true);

            int x, y;
            SDL_GetMouseState(&x, &y);
            state->cursor_position_to_restore.x = x;
            state->cursor_position_to_restore.y = y;
            state->HideCursor(true);
        }
        else
        {
            if (!state->set_is_rotating_was_faked)
            {
                state->mouselook_toggled = false;
            }
            state->set_is_rotating_was_faked = false;

            //  Skip on game launch / first load.
            if (state->cursor_position_to_restore.x != -1)
            {
                state->SetInternalIsRotating(false);
                state->HideCursor(false);
            }
        }
    }
}
