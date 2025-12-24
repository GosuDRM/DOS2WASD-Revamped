#include "CameraZoomOnDialogStart.hpp"
#include "Settings.hpp"
#include "State.hpp"

struct CameraZoomOnDialogStartProlog : Xbyak::CodeGenerator
{
    CameraZoomOnDialogStartProlog()
    {
        push(rax);
        push(rbx);
        push(rcx);
        push(rdx);
        push(r8);
        push(r9);
        mov(rcx, rdi);
    }
};

struct CameraZoomOnDialogStartEpilog : Xbyak::CodeGenerator
{
    CameraZoomOnDialogStartEpilog()
    {
        pop(r9);
        pop(r8);
        pop(rdx);
        pop(rcx);
        pop(rbx);
        pop(rax);
    }
};

bool CameraZoomOnDialogStart::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"F3 0F 11 BF 1C 04 00 00">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("CameraZoomOnDialogStart #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void CameraZoomOnDialogStart::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        CameraZoomOnDialogStartProlog prolog;
        prolog.ready();
        CameraZoomOnDialogStartEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 8 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kSkipNOP);
        handle->Enable();
        DEBUG("Hooked CameraZoomOnDialogStart #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void CameraZoomOnDialogStart::Func(int64_t a1)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    state->zoom_before_dialog = *(float*)(a1 + 0x41C);
    // player chose not to change zoom level on dialog...
    if (*settings->dialog_zoom - (-1.0f) < 0.0001f)
    {
        // ... but if the zoom is too close, it will not really work for looking at NPCs.
        if (state->zoom_before_dialog < 4.0f)
        {
            *(float*)(a1 + 0x41C) = 4.0f;
        }
        return;
    }
    *(float*)(a1 + 0x41C) = *settings->dialog_zoom;
}
