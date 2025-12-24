#include "OverrideDialogVerticalOffset.hpp"
#include "Settings.hpp"
#include "State.hpp"

struct OverrideDialogVerticalOffsetProlog : Xbyak::CodeGenerator
{
    OverrideDialogVerticalOffsetProlog()
    {
        push(rax);
        push(rbx);
        push(rcx);
        push(rdx);
        push(r8);
        push(r9);
    }
};

struct OverrideDialogVerticalOffsetEpilog : Xbyak::CodeGenerator
{
    OverrideDialogVerticalOffsetEpilog()
    {
        pop(r9);
        pop(r8);
        pop(rdx);
        pop(rcx);
        pop(rbx);
        pop(rax);
    }
};

bool OverrideDialogVerticalOffset::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"F6 81 58 04 00 00 06">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("OverrideDialogVerticalOffset #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void OverrideDialogVerticalOffset::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OverrideDialogVerticalOffsetProlog prolog;
        prolog.ready();
        OverrideDialogVerticalOffsetEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 7 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kRestoreAfterEpilog);
        handle->Enable();
        DEBUG("Hooked OverrideDialogVerticalOffset #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

float OverrideDialogVerticalOffset::Func()
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    return *settings->dialog_vertical_offset;
}
