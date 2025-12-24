#include "HorizontalOffsetZ.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include "Structs/Vector3.hpp"
#include "HorizontalOffsetX.hpp"
#include <cmath>

struct HorizontalOffsetZProlog : Xbyak::CodeGenerator
{
    HorizontalOffsetZProlog()
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
        movss(xmm1, xmm7);  // z
        mov(rcx, rsi);
    }
};

struct HorizontalOffsetZEpilog : Xbyak::CodeGenerator
{
    HorizontalOffsetZEpilog()
    {
        movss(xmm7, xmm0);  // z
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

bool HorizontalOffsetZ::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"F3 0F 58 B8 FC 00 00 00">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("HorizontalOffsetZ #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void HorizontalOffsetZ::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        HorizontalOffsetZProlog prolog;
        prolog.ready();
        HorizontalOffsetZEpilog epilog;
        epilog.ready();
        handle = DKUtil::Hook::AddCaveHook(address, { 0, 8 }, FUNC_INFO(Func), &prolog, &epilog,
            DKUtil::Hook::HookFlag::kRestoreBeforeProlog);
        handle->Enable();
        DEBUG("Hooked HorizontalOffsetZ #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

float HorizontalOffsetZ::Func(int64_t manager, float z)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (HorizontalOffsetX::ShouldNotUseOffsetNow(manager))
    {
        return z;
    }

    float current_zoom = *(float*)(manager + 0x41C);
    float val = (current_zoom - *settings->min_zoom) / 2.0f;
    float scale = std::fmax(0.0f, std::fmin(1.0f, val));

    Vector3* right_vec = reinterpret_cast<Vector3*>(manager + 0x50);
    if (std::isnan(right_vec->z) || std::isinf(right_vec->z))
    {
        return z;
    }
    float result = z + right_vec->z * (*settings->horizontal_offset * scale);
    if (std::isnan(result) || std::isinf(result))
    {
        return z;
    }
    return result;
}
