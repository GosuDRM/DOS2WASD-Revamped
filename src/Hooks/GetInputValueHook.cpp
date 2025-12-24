#include "GetInputValueHook.hpp"
#include "Addresses/IsInControllerMode.hpp"
#include "GameCommand.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include "Structs/Vector2.hpp"

using enum GameCommand;

bool GetInputValueHook::Prepare()
{
    std::array<uintptr_t, 8> address_array = {
        AsAddress(
            dku::Hook::Assembly::search_pattern<"E8 ?? ?? A5 FF EB 05 48 8D 44 24 60 F3 0F 10">()),
        AsAddress(dku::Hook::Assembly::search_pattern<
            "E8 ?? ?? A5 FF EB 05 48 8D ?? ?? ?? ?? ?? ?? 00">()),
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? A5 FF EB 05 48 8D 44 24 70 F3">()),
        AsAddress(
            dku::Hook::Assembly::search_pattern<"E8 ?? ?? A5 FF EB 05 48 8D 44 24 60 F3 0F 5C">()),
        AsAddress(
            dku::Hook::Assembly::search_pattern<"E8 ?? ?? A5 FF EB 04 48 8D 45 C8 F3 0F 10">()),
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? 5F A5 FF EB 04 48 8D 45 D4 C7">()),
        AsAddress(dku::Hook::Assembly::search_pattern<"E8 ?? ?? A5 FF EB 04 48 8D 45 D4 F3">()),
        AsAddress(
            dku::Hook::Assembly::search_pattern<"E8 ?? ?? A5 FF EB 04 48 8D 45 C8 F3 0F 5C">())
    };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("GetInputValueHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void GetInputValueHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked GetInputValueHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

// GameThread.
// Called 4 times in GetMovementInputVector (every frame)
// and 4 times in GetMovementInputVectorWorld (on input).
// (Not if the player cannot move!)
int64_t GetInputValueHook::OverrideFunc(int64_t player_input_controller_ptr,
    int64_t in_out_value_vec_ptr, DWORD* command_id_ptr, int a4)
{
    in_out_value_vec_ptr =
        OriginalFunc(player_input_controller_ptr, in_out_value_vec_ptr, command_id_ptr, a4);

    if (IsInControllerMode::Read())
    {
        return in_out_value_vec_ptr;
    }

    Vector2* xyz_v = reinterpret_cast<Vector2*>(in_out_value_vec_ptr);

    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();
    int command_id = *(int*)command_id_ptr;
    if (state->autoforward_toggled)
    {
        if (command_id == CharacterMoveForward)
        {
            xyz_v->x = 1.0f;
            xyz_v->y = 1.0f;
        }
    }
    if (*settings->enable_rotate_plus_lmb_is_forward && !state->rotate_keys_include_lmb)
    {
        bool rotate_and_lmb_is_pressed = state->is_mouseleft_pressed && state->IsRotating();
        if (state->last_frame_rotate_and_lmb_was_pressed && !rotate_and_lmb_is_pressed)
        {
            state->autoforward_toggled = false;
        }
        if (rotate_and_lmb_is_pressed)
        {
            if (command_id == CharacterMoveForward)
            {
                xyz_v->x = 1.0f;
                xyz_v->y = 1.0f;
            }
        }
        state->last_frame_rotate_and_lmb_was_pressed = rotate_and_lmb_is_pressed;
    }

    if (state->walking_toggled ^ state->walking_held)
    {
        xyz_v->x *= *settings->walk_speed;
        xyz_v->y *= *settings->walk_speed;
    }
    if (not state->IsCharacterMovementMode())
    {
        xyz_v->x = 0.0f;
        xyz_v->y = 0.0f;
    }
    if (state->frames_to_hold_forward_to_center_camera > 0)
    {
        // Send a move input for a few frames, so the camera moves back to the character.
        // The game has a center camera command, but it sucks, because it always faces north.
        // There is center logic that doesn't do that, e.g. when you press F1, but I didn't find it
        // yet.
        if (command_id == CharacterMoveForward)
        {
            --(state->frames_to_hold_forward_to_center_camera);
            xyz_v->x = 1.0f;
            xyz_v->y = 1.0f;
        }
    }

    if (*Settings::GetSingleton()->enable_improved_mouselook)
    {
        state->player_can_input_movement = true;
    }


    return in_out_value_vec_ptr;
}
