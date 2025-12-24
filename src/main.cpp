#include "Addresses/IsInControllerMode.hpp"
#include "Addresses/LoadInputConfig.hpp"
#include "Addresses/SettingsPtr.hpp"
#include "Hooks/AfterChangingKeybindInMenuHook.hpp"
#include "Hooks/AfterInitialLoadInputConfigHook.hpp"
#include "Hooks/AtmFarPlaneOverride.hpp"
#include "Hooks/AtmFarPlaneOverride2.hpp"
#include "Hooks/CallSpecificCommandFunctionPre2Cavehook.hpp"
#include "Hooks/CameraZoomOnDialogEnd.hpp"
#include "Hooks/CameraZoomOnDialogEnd2.hpp"
#include "Hooks/CameraZoomOnDialogStart.hpp"
#include "Hooks/CastOrCancelAbilityKeydownCavehook.hpp"
#include "Hooks/CastOrCancelAbilityKeyupHook.hpp"
#include "Hooks/CheckCommandInputsHook.hpp"
#include "Hooks/CheckContextMenuOrCancelActionHook.hpp"
#include "Hooks/ConcatInputconfigPathHook.hpp"
#include "Hooks/DecideMoveUpdaterHook.hpp"
#include "Hooks/GetCameraObjectHook.hpp"
#include "Hooks/GetInputValueHook.hpp"
#include "Hooks/HorizontalOffsetX.hpp"
#include "Hooks/HorizontalOffsetY.hpp"
#include "Hooks/HorizontalOffsetZ.hpp"
#include "Hooks/HotkeyRotationInput.hpp"
#include "Hooks/InsideUpdateInteractMoveCavehook.hpp"
#include "Hooks/MouseDeltaCavehook.hpp"
#include "Hooks/MouseRotationInput.hpp"
#include "Hooks/OverrideDialogVerticalOffset.hpp"
#include "Hooks/PitchHook.hpp"
#include "Hooks/PollEventHook.hpp"
#include "Hooks/SDL_SetWindowGrabHook.hpp"
#include "Hooks/SetOrResetCursorRotateCavehook.hpp"
#include "Hooks/SetVirtualCursorPosHook.hpp"
#include "Hooks/WASDUnlock.hpp"
#include "Hooks/ZoomHook.hpp"
#include "InputHook.hpp"
#include "MessageBox.hpp"
#include "Patches/BlockCancelActionStoppingMovementPatch.hpp"
#include "Patches/BlockHoldInteractMovePatch.hpp"
#include "Patches/BlockInteractMovePatch.hpp"
#include "Patches/CenterCameraAlwaysJumps.hpp"
#include "Patches/FixWalking1.hpp"
#include "Patches/FixWalking2.hpp"
#include "Patches/FixWarpMouseInWindowCrash.hpp"
#include "Patches/FollowOnChararacterChange.hpp"
#include "Patches/OverrideDialogVerticalOffset2.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include "VersionInfo.hpp"

BOOL APIENTRY DllMain(HMODULE a_hModule, DWORD a_ul_reason_for_call, LPVOID a_lpReserved)
{
    if (a_ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
#ifndef NDEBUG
        while (!IsDebuggerPresent())
        {
            Sleep(100);
        }
#endif
        dku::Logger::Init(Plugin::NAME, std::to_string(Plugin::Version));
        dku::Hook::Trampoline::AllocTrampoline(1 << 11);

        VersionInfo::Print(a_hModule);

        SDL_version linked;
        SDL_GetVersion(&linked);
        if (linked.major != 2 || linked.minor != 28)
        {
            FATAL(
                "SDL2.dll version mismatch. This usually means that you must re-install the mod, "
                "or at least the SDL2.dll!");
        }
        // With SDL 2.24 and spam-clicking Rotate I could bug the cursor so that it would
        // warp to the center of the screen.
        // Using SDL 2.28 and this Hint fixes this issue.
        // Also, the original DOS2 SDL2.7 does not provide SDL_HINT_MOUSE_RELATIVE_MODE_CENTER,
        // which is required for the cursor to stay in place during rotation.
        SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_CENTER, "0");

        auto* settings = Settings::GetSingleton();
        settings->Load();

        std::string errors;

        bool wasd_unlock = WASDUnlock::Prepare();
        bool movement_input = GetInputValueHook::Prepare();
        bool load_input_config = LoadInputConfig::Prepare();
        bool after_changing_keybind_in_menu_hook = AfterChangingKeybindInMenuHook::Prepare();
        bool after_initial_load_inputconfig_hook = AfterInitialLoadInputConfigHook::Prepare();
        bool concat_inputconfig_path_hook = ConcatInputconfigPathHook::Prepare();
        bool center_cam_always_jumps = CenterCameraAlwaysJumps::Prepare();
        bool cam_obj = GetCameraObjectHook::Prepare();
        bool fix_walking1 = FixWalking1::Prepare();
        bool fix_walking2 = FixWalking2::Prepare();
        bool fix_warpmouse = FixWarpMouseInWindowCrash::Prepare();
        bool is_controller = IsInControllerMode::Prepare();
        bool follow_on_change = FollowOnChararacterChange::Prepare();
        bool settings_ptr = SettingsPtr::Prepare();
        if (wasd_unlock && load_input_config && after_initial_load_inputconfig_hook &&
            concat_inputconfig_path_hook && movement_input && after_changing_keybind_in_menu_hook &&
            cam_obj && center_cam_always_jumps && fix_walking1 && fix_walking2 && fix_warpmouse &&
            is_controller && follow_on_change && settings_ptr)
        {
            InputHook::Enable(a_hModule);  // throws on error
            WASDUnlock::Enable();
            GetInputValueHook::Enable();
            AfterChangingKeybindInMenuHook::Enable();
            AfterInitialLoadInputConfigHook::Enable();
            ConcatInputconfigPathHook::Enable();
            CenterCameraAlwaysJumps::Enable();
            CenterCameraAlwaysJumps::Activate();
            GetCameraObjectHook::Enable();
            FixWalking1::Enable();
            FixWalking1::Activate();
            FixWalking2::Enable();
            FixWalking2::Activate();
            FixWarpMouseInWindowCrash::Enable();
            FixWarpMouseInWindowCrash::Activate();
            FollowOnChararacterChange::Enable();
            FollowOnChararacterChange::Activate();

            // needed for both improved mouselook AND interact move canceller
            bool check_command_inputs_hook = CheckCommandInputsHook::Prepare();

            bool set_virtual_cursor_pos_hook = SetVirtualCursorPosHook::Prepare();
            bool set_window_grab_hook = SDL_SetWindowGrabHook::Prepare();
            bool set_cursor_rotate_hook = SetOrResetCursorRotateCavehook::Prepare();
            bool check_context_menu_or_cancel_action_hook =
                CheckContextMenuOrCancelActionHook::Prepare();
            bool cast_or_cancel_keyup = CastOrCancelAbilityKeyupHook::Prepare();
            bool cast_or_cancel_keydown = CastOrCancelAbilityKeydownCavehook::Prepare();
            bool poll_event_hook = PollEventHook::Prepare();

            // TODO ToggleMouselook
            // bool windows_gain_focus_hook = WindowGainFocusHook::Prepare();

            if (set_virtual_cursor_pos_hook && set_window_grab_hook &&
                check_context_menu_or_cancel_action_hook && check_command_inputs_hook &&
                cast_or_cancel_keyup && cast_or_cancel_keydown && set_cursor_rotate_hook)
            {
                SetVirtualCursorPosHook::Enable();
                SDL_SetWindowGrabHook::Enable();
                SetOrResetCursorRotateCavehook::Enable();
                CheckContextMenuOrCancelActionHook::Enable();
                CastOrCancelAbilityKeyupHook::Enable();
                CastOrCancelAbilityKeydownCavehook::Enable();
                PollEventHook::Enable();
                CheckCommandInputsHook::Enable();
            }
            else
            {
                errors.append("Improved Mouselook could not be enabled.\n");
            }

            bool decide_move_updater_hook = DecideMoveUpdaterHook::Prepare();
            bool inside_update_interact_move_hook = InsideUpdateInteractMoveCavehook::Prepare();
            bool call_specific_command_function_pre2_hook =
                CallSpecificCommandFunctionPre2Cavehook::Prepare();
            bool block_interact_move_patch = BlockInteractMovePatch::Prepare();
            bool block_hold_interact_move_patch = BlockHoldInteractMovePatch::Prepare();
            bool block_cancel_stopping_movement_patch =
                BlockCancelActionStoppingMovementPatch::Prepare();
            if (decide_move_updater_hook && inside_update_interact_move_hook &&
                call_specific_command_function_pre2_hook && block_hold_interact_move_patch &&
                block_cancel_stopping_movement_patch && block_interact_move_patch)
            {
                DecideMoveUpdaterHook::Enable();
                InsideUpdateInteractMoveCavehook::Enable();
                BlockCancelActionStoppingMovementPatch::Enable();
                BlockInteractMovePatch::Enable();
                BlockHoldInteractMovePatch::Enable();
                CallSpecificCommandFunctionPre2Cavehook::Enable();

                // The blocker patches are initialized, when, in GetCameraObjectHook, the init
                // condition is met and SetMovementModeToggled is called.
            }
            else
            {
                errors.append("InteractMoveBlocker could not be enabled.\n");
            }

            bool mouse_delta = MouseDeltaCavehook::Prepare();
            bool pitch = PitchHook::Prepare();
            bool zoom = ZoomHook::Prepare();
            bool farplane1 = AtmFarPlaneOverride::Prepare();
            bool farplane2 = AtmFarPlaneOverride2::Prepare();
            bool mouse_rot_input = MouseRotationInput::Prepare();
            bool hotkey_rot_input = HotkeyRotationInput::Prepare();
            bool horizontal_offset_x = HorizontalOffsetX::Prepare();
            bool horizontal_offset_y = HorizontalOffsetY::Prepare();
            bool horizontal_offset_z = HorizontalOffsetZ::Prepare();
            bool zoom_on_dialog_end = CameraZoomOnDialogEnd::Prepare();
            bool zoom_on_dialog_end2 = CameraZoomOnDialogEnd2::Prepare();
            bool zoom_on_dialog_start = CameraZoomOnDialogStart::Prepare();
            bool dialog_vert_offset1 = OverrideDialogVerticalOffset::Prepare();
            bool dialog_vert_offset2 = OverrideDialogVerticalOffset2::Prepare();
            if (mouse_delta && pitch && zoom && farplane1 && farplane2 && mouse_rot_input &&
                hotkey_rot_input && horizontal_offset_x && horizontal_offset_y &&
                horizontal_offset_z && zoom_on_dialog_end && zoom_on_dialog_end2 &&
                zoom_on_dialog_start && dialog_vert_offset1 && dialog_vert_offset2)
            {
                MouseDeltaCavehook::Enable();
                PitchHook::Enable();
                ZoomHook::Enable();
                AtmFarPlaneOverride::Enable();
                AtmFarPlaneOverride2::Enable();
                MouseRotationInput::Enable();
                HotkeyRotationInput::Enable();
                HorizontalOffsetX::Enable();
                HorizontalOffsetY::Enable();
                HorizontalOffsetZ::Enable();
                CameraZoomOnDialogEnd::Enable();
                CameraZoomOnDialogEnd2::Enable();
                CameraZoomOnDialogStart::Enable();
                OverrideDialogVerticalOffset::Enable();
                OverrideDialogVerticalOffset2::Enable();
                OverrideDialogVerticalOffset2::Activate();
            }
            else
            {
                errors.append("Camera Improvements could not be enabled.\n");
            }
        }
        else
        {
            errors.append(
                "Character movement could not be enabled at all! Mod will be inactive.\n");
        }

        if (!State::GetSingleton()->mod_found_all_addresses)
        {
            errors.append("Your game version is not completely compatible with the mod version!\n");
        }

        if (!errors.empty())
        {
            WARN(errors);
            MessageBox::Show(errors);
        }
        else
        {
            INFO("DOS2WASD Revamped: Loaded successfully.");
        }
    }

    return TRUE;
}
