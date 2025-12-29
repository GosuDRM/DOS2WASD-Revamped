#include "PitchHook.hpp"
#include "Addresses/IsInControllerMode.hpp"
#include "Addresses/SettingsPtr.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include <algorithm>
#include <cmath>

bool PitchHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"E8 7A 26 00 00 F3 44 0F">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("PitchHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void PitchHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked PitchHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

int64_t PitchHook::OverrideFunc(int64_t a1, int64_t a2, int64_t a3)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (*settings->unlock_pitch)
    {
        bool is_controller_mode = IsInControllerMode::Read();
        
        // In controller mode: only adjust pitch if left stick is NOT pressed
        // (when left stick is pressed, we want original zoom behavior)
        bool should_adjust_pitch = false;
        float pitch_delta = 0.0f;
        
        if (is_controller_mode)
        {
            // Controller mode: use right stick Y for pitch when left stick is not pressed
            if (!state->controller_left_stick_pressed.load())
            {
                float controller_y = state->controller_right_stick_y.load();
                if (controller_y != 0.0f)
                {
                    should_adjust_pitch = true;
                    // Controller input is continuous, apply per-frame (negated for intuitive direction)
                    pitch_delta = -controller_y * 0.02f * *settings->controller_pitch_speed;
                }
            }
        }
        else
        {
            // Mouse mode: use mouse delta for pitch when rotating
            bool is_rotating = *(bool*)(a1 + 1113) & 1;
            if (is_rotating)
            {
                should_adjust_pitch = true;
                pitch_delta = (float)state->dy.load() * 0.0025f * *settings->pitch_speed *
                              *settings->mouse_rotation_speed;
                state->dy = 0;
            }
        }
        
        if (should_adjust_pitch)
        {
            float current_pitch = state->pitch.load();
            if (std::isnan(current_pitch) || !std::isfinite(current_pitch))
            {
                current_pitch = 0.6f;
            }
            
            current_pitch += pitch_delta;
            
            float min = *(settings->min_pitch);
            float max = *(settings->max_pitch);
            current_pitch = std::clamp(current_pitch, min, max);

            if (std::isnan(current_pitch) || !std::isfinite(current_pitch))
            {
                 current_pitch = 0.6f;
            }
            state->pitch = current_pitch;
            *(float*)(SettingsPtr::Read() + 0xCCC) = current_pitch;
            *(float*)(SettingsPtr::Read() + 0xCE4) = current_pitch;  // combat
            *(float*)(SettingsPtr::Read() + 0xCC0) = current_pitch;
            *(float*)(SettingsPtr::Read() + 0xCD8) = current_pitch;  // combat
        }
    }

    return OriginalFunc(a1, a2, a3);
}
