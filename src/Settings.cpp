#pragma comment(lib, "version.lib")

#include "Settings.hpp"
#include "Addresses/LoadInputConfig.hpp"
#include "InputconfigPatcher.hpp"
#include "State.hpp"

using enum Command;

void Settings::Load() noexcept
{
    if (first_time_loaded)
    {
        config.Bind(toggle_walkspeed, "key:insert");
        config.Bind(toggle_movement_mode, "key:capslock");
        config.Bind(hold_movement_mode, "");
        config.Bind(toggle_autoforward, "shift+key:w");
        config.Bind(hold_walkspeed, "");
        config.Bind(reload_config, "key:f11");

        config.Bind<0.0, 1.0>(walk_speed, 0.3);
        config.Bind(walking_is_default, FALSE);
        config.Bind(walk_after_combat, FALSE);

        config.Bind(enable_auto_toggling_movement_mode, TRUE);

        config.Bind(enable_improved_mouselook, TRUE);
        config.Bind(enable_rotate_plus_lmb_is_forward, TRUE);
        config.Bind(rotate_threshold, 200);

        config.Bind(block_interact_move, FALSE);

        config.Bind<0.0, 100.0>(mouse_rotation_speed, 1.0);
        config.Bind<0.0, 100.0>(hotkey_rotation_speed, 1.5);
        config.Bind(unlock_pitch, TRUE);
        config.Bind<-10.0, 10.0>(pitch_speed, 1.0);
        config.Bind<0.0, 10.0>(vertical_offset, 0.95);
        config.Bind<-10.0, 10.0>(horizontal_offset, 0.3);
        config.Bind<0.01, 10.0>(min_zoom, 2.0);
        config.Bind<0.01, 100.0>(max_zoom, 20.0);
        config.Bind<-1.0, 100.0>(dialog_zoom, 7.0);
        config.Bind<0.0, 10.0>(dialog_vertical_offset, 0.2);
        config.Bind<0.0, 10.0>(zoom_speed, 1.0);
        config.Bind<0.0, 10.0>(camera_movespeed, 1.25);
        config.Bind<0.0, 90.0>(fov, 45.0);
        config.Bind<-1.0, 1.0>(min_pitch, 0.31);
        config.Bind<0.0, 1.0>(max_pitch, 1.0);
        config.Bind(improve_sky, TRUE);

        // Controller support
        config.Bind<0.0, 10.0>(controller_pitch_speed, 1.0);
    }

    config.Load();



    auto* state = State::GetSingleton();

    if (first_time_loaded)
    {
        InitState();
    }
    else  // reloaded
    {
        // during first load, this is called in AfterInitialLoadInputConfigHook
        InputconfigPatcher::Patch();
    }
    state->EnableInteractMoveBlocker(state->IsCharacterMovementMode());

    state->should_reload_camera_settings = true;

    first_time_loaded = false;

}

void Settings::InitState()
{
    auto* state = State::GetSingleton();

    state->walking_toggled = walking_is_default;

    // Flag invalid to react later.
    state->cursor_position_to_restore.x = -1;
}

std::vector<std::string> Settings::GetBoundKeycombos(const std::string& setting)
{
    std::vector<std::string> result = dku::string::split(setting, ","sv);
    return result;
}
