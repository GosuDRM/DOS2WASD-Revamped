#include "HorizontalOffsetX.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include "Structs/Vector3.hpp"
#include <cmath>
#include "Structs/Vector3.hpp"
#include <cmath>

struct HorizontalOffsetXProlog : Xbyak::CodeGenerator
{
    HorizontalOffsetXProlog()
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
        movss(xmm1, xmm8);  // x
        mov(rcx, rsi);
    }
};

struct HorizontalOffsetXEpilog : Xbyak::CodeGenerator
{
    HorizontalOffsetXEpilog()
    {
        movss(xmm8, xmm0);  // x
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

bool HorizontalOffsetX::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"F3 44 0F 58 80 F4 00 00 00">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("HorizontalOffsetX #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

bool HorizontalOffsetX::ShouldNotUseOffsetNow(int64_t manager)
{
    if ((*(char*)(manager + 1112) & 6) != 0)
    {
        return true;
    }
    if (*(char*)(manager + 1216) != 0)
    {
        return true;
    }
    return false;
}

void HorizontalOffsetX::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        HorizontalOffsetXProlog prolog;
        prolog.ready();
        HorizontalOffsetXEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 9 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kRestoreBeforeProlog);
        handle->Enable();
        DEBUG("Hooked HorizontalOffsetX #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

float HorizontalOffsetX::Func(int64_t manager, float x)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (HorizontalOffsetX::ShouldNotUseOffsetNow(manager))
    {
        return x;
    }

    float current_zoom = *(float*)(manager + 0x41C);
    float val = (current_zoom - *settings->min_zoom) / 2.0f;
    float scale = std::fmax(0.0f, std::fmin(1.0f, val));

    Vector3* right_vec = reinterpret_cast<Vector3*>(manager + 0x50);
    if (std::isnan(right_vec->x) || std::isinf(right_vec->x))
    {
        return x;
    }
    float result = x + right_vec->x * (*settings->horizontal_offset * scale);
    if (std::isnan(result) || std::isinf(result))
    {
        return x;
    }
    return result;
}
