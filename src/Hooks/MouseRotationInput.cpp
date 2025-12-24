#include "MouseRotationInput.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include <cmath>

bool MouseRotationInput::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"E8 E0 0D 00 00 E9">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("MouseRotationInput #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void MouseRotationInput::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked MouseRotationInput #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

void MouseRotationInput::OverrideFunc(int64_t a1, char a2)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    float val = *(float*)(a1 + 1104);
    if (std::isfinite(val))
    {
        // Get current zoom level (offset 0x41C = 1052)
        float zoom = *(float*)(a1 + 1052);
        
        // Scale rotation inversely with zoom - closer zoom = faster rotation
        // Base of 10.0 is a reasonable reference point for "normal" rotation speed
        float zoom_compensation = 1.0f;
        if (zoom > 0.1f && std::isfinite(zoom))
        {
            zoom_compensation = 10.0f / zoom;
            // Clamp to prevent extreme speeds at very close zoom
            zoom_compensation = std::clamp(zoom_compensation, 1.0f, 2.5f);
        }
        
        *(float*)(a1 + 1104) = val * *(settings->mouse_rotation_speed) * zoom_compensation;
    }

    return OriginalFunc(a1, a2);
}
