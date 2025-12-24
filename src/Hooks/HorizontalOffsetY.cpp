#include "HorizontalOffsetY.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include "Structs/Vector3.hpp"
#include "HorizontalOffsetX.hpp"
#include <cmath>

struct HorizontalOffsetYProlog : Xbyak::CodeGenerator
{
    HorizontalOffsetYProlog()
    {
        push(rax);
        push(rbx);
        push(rcx);
        push(rdx);
        push(r8);
        push(r9);
        push(r10);
        push(r11);
        movss(xmm2, xmm0);  // backup
        movss(xmm3, xmm1);  // backup
        movss(xmm1, xmm9);  // y
        mov(rcx, rsi);
    }
};

struct HorizontalOffsetYEpilog : Xbyak::CodeGenerator
{
    HorizontalOffsetYEpilog()
    {
        movss(xmm9, xmm0);  // y
        movss(xmm0, xmm2);  // restore
        movss(xmm1, xmm3);  // restore
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

bool HorizontalOffsetY::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"F3 44 0F 58 88 F8 00 00 00">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("HorizontalOffsetY #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void HorizontalOffsetY::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        HorizontalOffsetYProlog prolog;
        prolog.ready();
        HorizontalOffsetYEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 9 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kRestoreBeforeProlog);
        handle->Enable();
        DEBUG("Hooked HorizontalOffsetY #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

float HorizontalOffsetY::Func(int64_t manager, float y)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (HorizontalOffsetX::ShouldNotUseOffsetNow(manager))
    {
        return y;
    }

    float current_zoom = *(float*)(manager + 0x41C);
    float val = (current_zoom - *settings->min_zoom) / 2.0f;
    float scale = std::fmax(0.0f, std::fmin(1.0f, val));

    Vector3* right_vec = reinterpret_cast<Vector3*>(manager + 0x50);
    if (std::isnan(right_vec->y) || std::isinf(right_vec->y))
    {
        return y;
    }
    float result = y + right_vec->y * (*settings->horizontal_offset * scale);
    if (std::isnan(result) || std::isinf(result))
    {
        return y;
    }
    return result;
}
