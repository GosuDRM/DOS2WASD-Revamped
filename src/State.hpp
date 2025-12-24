#pragma once

#include "SDL.h"
#include <atomic>
#include <mutex>
#include <vector>

class State : public DKUtil::model::Singleton<State>
{
public:
    bool mod_found_all_addresses = true;

    // core
    std::atomic<bool> autoforward_toggled = false;
    std::atomic<bool> walking_toggled = false;
    std::atomic<bool> walking_held = false;
    std::atomic<int> frames_to_hold_forward_to_center_camera = 0;
    // Thread safety: These vectors are populated once during initialization and
    // are read-only during gameplay. No synchronization needed for read access.
    std::vector<std::string> character_forward_keys;
    std::vector<std::string> character_backward_keys;
    std::vector<std::string> context_menu_keys;
    std::wstring inputconfig_path = L"";

    bool ContextMenuHasSameKeyAsRotate();

    void SetMovementModeToggled(bool in_value);
    void SetMovementModeHeld(bool in_value);
    void UpdateMovementMode();
    bool IsCharacterMovementMode();
    bool IsMovementModeToggled();

    // auto toggle movement
    bool last_dying_character_is_player = false;
    std::string combat_end_character_name;
    bool old_combat_state = false;
    bool combat_state_initiliazed = false;
    uint32_t last_time_combat_state_changed = 0;

    // mouselook
    std::atomic<bool> mouselook_toggled = false;
    // Thread safety: Populated once at init, read-only during gameplay.
    std::vector<std::string> rotate_keys;
    std::atomic<bool> rotate_keys_include_lmb = false;
    SDL_Window* sdl_window = 0;
    bool set_is_rotating_was_faked = false;
    POINT cursor_position_to_restore;
    std::atomic<bool> is_mouseleft_pressed = false;
    std::atomic<uint32_t> rotate_start_time = 0;
    // Almost the same as rotate_start_time, but counted in a different thread, so whatever.
    std::atomic<uint32_t> last_time_rotate_pressed = 0;
    std::atomic<uint32_t> last_time_cancel_action_pressed = 0;
    std::atomic<uint32_t> last_time_cancel_action_pressed2 = 0;
    std::atomic<bool> rotation_validated = false;
    std::atomic<bool> player_can_input_movement = false;
    bool player_could_input_movement_last_frame = false;
    bool last_frame_rotate_and_lmb_was_pressed = false;
    bool cursor_hidden_last_frame = false;
    std::mutex hide_cursor_mutex;
    bool virtual_cursor_hidden_last_frame = false;
    bool should_hide_virtual_cursor = false;

    void SetIsRotating(bool in_value);
    void SetInternalIsRotating(bool in_value);
    bool IsRotating();
    void HideCursor(bool in_value);
    bool ShouldHideCursor();

    // interactmove disabler
    bool order_force_stop = false;
    bool is_force_stop = false;

    void EnableInteractMoveBlocker(bool enabled);
    void SetCurrentlyInteractMoving(bool in_value);
    bool IsCurrentlyInteractMoving();

    // camera
    std::atomic<int> dx = 0;
    std::atomic<int> dy = 0;
    std::atomic<float> pitch = 0.6f;
    bool should_reload_camera_settings = true;
    int override_atm_far_plane = 0;
    float zoom_before_dialog = -1.0f;

private:
    //core
    std::atomic<bool> movement_mode_toggled = true;
    std::atomic<bool> movement_mode_held = false;

    // mouselook
    std::atomic<bool> is_rotating = false;

    std::atomic<bool> should_hide_cursor = false;

    // InteractMoveBlocker
    std::atomic<bool> currently_interact_moving = false;
};
