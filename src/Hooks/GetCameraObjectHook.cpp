#include "GetCameraObjectHook.hpp"
#include "Addresses/IsInControllerMode.hpp"
#include "Addresses/SettingsPtr.hpp"
#include "InputFaker.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include <algorithm>
#include <cmath>

bool GetCameraObjectHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"E8 F7 A7 FF FF">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("GetCameraObjectHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void GetCameraObjectHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked GetCameraObjectHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

int64_t GetCameraObjectHook::OverrideFunc(int64_t manager, int64_t* in_out)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (state->should_reload_camera_settings)
    {
        int64_t settings_base = SettingsPtr::Read();
        *(float*)(settings_base + 0xC40) = *(settings->min_zoom);
        *(float*)(settings_base + 0xC44) = *(settings->max_zoom);
        *(float*)(settings_base + 0xC48) = *(settings->max_zoom);
        *(float*)(settings_base + 0xC4C) = *(settings->min_zoom);
        *(float*)(settings_base + 0xC64) = *(settings->vertical_offset);
        *(float*)(settings_base + 0xC6C) = *(settings->camera_movespeed);
        *(float*)(settings_base + 0xC74) = *(settings->fov);
        if (*settings->unlock_pitch)
        {
            state->pitch += (*(settings->min_pitch) + *(settings->max_pitch)) / 2;
        }
        else
        {
            *(float*)(settings_base + 0xCCC) = *(settings->min_pitch);
            *(float*)(settings_base + 0xCE4) = *(settings->min_pitch);  // combat
            *(float*)(settings_base + 0xCC0) = *(settings->max_pitch);
            *(float*)(settings_base + 0xCD8) = *(settings->max_pitch);  // combat
        }

        state->should_reload_camera_settings = false;
    }

    if (!IsInControllerMode::Read())
    {
        *(int32_t*)(manager + 1196) = 1;  // follow flag
    }

    if (manager && state->IsCharacterMovementMode())
    {
        *(float*)(manager + 1092) = 0.0f;
        *(float*)(manager + 1096) = 0.0f;
    }

    bool new_combat_state = (*reinterpret_cast<bool*>(manager + 1112) & 1) != 0;
    if (!state->combat_state_initiliazed || new_combat_state != state->old_combat_state)
    {
        if (*settings->enable_auto_toggling_movement_mode)
        {
            state->SetMovementModeToggled(!new_combat_state);
        }
        if (new_combat_state == false && *settings->walk_after_combat)
        {
            state->walking_toggled = true;
        }
        state->old_combat_state = new_combat_state;
        state->combat_state_initiliazed = true;
        state->last_time_combat_state_changed = SDL_GetTicks();
    }

    // Pitch updates are handled in PitchHook.cpp to avoid duplicate processing and stuttering

    return OriginalFunc(manager, in_out);
}
