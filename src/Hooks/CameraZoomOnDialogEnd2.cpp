#include "CameraZoomOnDialogEnd2.hpp"
#include "Settings.hpp"
#include "State.hpp"

struct CameraZoomOnDialogEnd2Prolog : Xbyak::CodeGenerator
{
    CameraZoomOnDialogEnd2Prolog()
    {
        push(rax);
        push(rbx);
        push(rcx);
        push(rdx);
        push(r8);
        push(r9);
    }
};

struct CameraZoomOnDialogEnd2Epilog : Xbyak::CodeGenerator
{
    CameraZoomOnDialogEnd2Epilog()
    {
        pop(r9);
        pop(r8);
        pop(rdx);
        pop(rcx);
        pop(rbx);
        pop(rax);
    }
};

bool CameraZoomOnDialogEnd2::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
        "F3 0F 11 89 1C 04 00 00 48 83 C4 28 C3 CC">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("CameraZoomOnDialogEnd2 #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void CameraZoomOnDialogEnd2::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        CameraZoomOnDialogEnd2Prolog prolog;
        prolog.ready();
        CameraZoomOnDialogEnd2Epilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 8 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kSkipNOP);
        handle->Enable();
        DEBUG("Hooked CameraZoomOnDialogEnd2 #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void CameraZoomOnDialogEnd2::Func(int64_t a1)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (state->zoom_before_dialog > 0.0f)
    {
        *(float*)(a1 + 0x41C) = state->zoom_before_dialog;
        state->zoom_before_dialog = -1.0f;
    }
}
