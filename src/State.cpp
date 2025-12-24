#include "State.hpp"
#include "GameCommand.hpp"
#include "InputFaker.hpp"
#include "Patches/BlockCancelActionStoppingMovementPatch.hpp"
#include "Patches/BlockHoldInteractMovePatch.hpp"
#include "Patches/BlockInteractMovePatch.hpp"
#include "SDL.h"
#include "Settings.hpp"

using enum GameCommand;

void State::SetIsRotating(bool in_value)
{
    if (rotate_keys.size() > 0)
    {
        set_is_rotating_was_faked = true;
        if (in_value)
        {
            InputFaker::SendKey(rotate_keys[0], SDL_PRESSED);
        }
        else
        {
            InputFaker::SendKey(rotate_keys[0], SDL_RELEASED);
        }
    }
}

void State::SetInternalIsRotating(bool in_value) { is_rotating = in_value; }

bool State::IsRotating() { return is_rotating.load(); }

void State::HideCursor(bool in_value)
{
    const std::lock_guard<std::mutex> lock(hide_cursor_mutex);
    should_hide_cursor = in_value;
}

bool State::ShouldHideCursor() { return should_hide_cursor.load(); }

void State::EnableInteractMoveBlocker(bool enabled)
{
    auto* settings = Settings::GetSingleton();

    if (enabled && *settings->block_interact_move)
    {
        BlockInteractMovePatch::Activate();
        BlockHoldInteractMovePatch::Activate();
    }
    else
    {
        BlockInteractMovePatch::Deactivate();
        BlockHoldInteractMovePatch::Deactivate();
    }
}

/* During Interact move, don't block CancelAction from cancelling movement, or you cannot block it
at all!
But during wasd move, block it, so movement is smoother and Context Menu opens faster after moving.
*/
void State::SetCurrentlyInteractMoving(bool in_value)
{
    if (in_value)
    {
        if (!currently_interact_moving)
        {
            BlockCancelActionStoppingMovementPatch::Deactivate();
        }
    }
    else
    {
        if (currently_interact_moving)
        {
            BlockCancelActionStoppingMovementPatch::Activate();
        }
    }
    currently_interact_moving = in_value;
}

bool State::IsCurrentlyInteractMoving() { return currently_interact_moving.load(); }

bool State::ContextMenuHasSameKeyAsRotate()
{
    for (auto keycombo : context_menu_keys)
    {
        auto split = dku::string::split(keycombo, ":"sv);
        if (split.size() < 2)
        {
            continue;
        }
        if (dku::string::iequals(split[0], "c"))
        {
            continue;
        }
        if (dku::string::iequals(split[1], "unknown"))
        {
            continue;
        }
        if (dku::string::iequals(split[1], "INVALID"))
        {
            continue;
        }

        if (std::find(rotate_keys.begin(), rotate_keys.end(), keycombo) != rotate_keys.end())
        {
            return true;
        }
    }
    return false;
}

void State::SetMovementModeToggled(bool in_value)
{
    movement_mode_toggled = in_value;

    UpdateMovementMode();
}

void State::SetMovementModeHeld(bool in_value)
{
    movement_mode_held = in_value;

    UpdateMovementMode();
}

// If true, also center camera
void State::UpdateMovementMode()
{
    auto* settings = Settings::GetSingleton();

    bool is_character_movement = IsCharacterMovementMode();

    if (is_character_movement)
    {
        order_force_stop = true;
        // set it to double the value as in BG3WASD, because it is decreased twice per frame
        frames_to_hold_forward_to_center_camera = 20;
    }

    EnableInteractMoveBlocker(is_character_movement);

    // TODO ToggleMouselook
    // if (*settings->toggle_movement_toggles_mouselook)
    // {
    //     SetIsRotating(is_character_movement);
    // }
}

bool State::IsCharacterMovementMode() { return movement_mode_toggled.load() ^ movement_mode_held.load(); }

bool State::IsMovementModeToggled() { return movement_mode_toggled.load(); }
