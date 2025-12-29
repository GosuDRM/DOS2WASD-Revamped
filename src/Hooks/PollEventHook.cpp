#include "PollEventHook.hpp"
#include "InputFaker.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include "SetVirtualCursorPosHook.hpp"
#include "SDLInputHandler.hpp"
#include <cmath>

bool PollEventHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(
        dku::Hook::Assembly::search_pattern<"FF 15 ?? ?? C3 00 49 8D 4F">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("PollEventHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void PollEventHook::Enable()
{
    if (not all_found)
    {
        WARN("PollEventHook: NOT enabled - hook address not found!");
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<6>(address, OverrideFunc);

        ++i;
    }
}

// Actual hiding happens in SetVirtualCursorPosHook.
// Sending a fake mouse input guarantees that it is called.
void PollEventHook::HideVirtualCursor(bool in_value)
{
    auto* state = State::GetSingleton();
    state->should_hide_virtual_cursor = in_value;
    state->rotate_start_time = 0;
    InputFaker::SendMouseMotion(0, 0);
}

// Called in MainThread, every frame
int64_t PollEventHook::OverrideFunc()
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    // Process SDL events for mod hotkeys (cross-platform, works on Windows and Linux/Proton)
    // Call SDL_PumpEvents to gather pending input information from devices
    // Then use SDL_PEEKEVENT to peek at events without removing them from the queue,
    // so the game can still process all keyboard/mouse events normally.
    SDL_PumpEvents();  // Pump events into the queue first!
    
    SDL_Event events[16];
    int num_key_events = SDL_PeepEvents(events, 16, SDL_PEEKEVENT, SDL_KEYDOWN, SDL_KEYUP);

    for (int i = 0; i < num_key_events; ++i)
    {
        SDLInputHandler::ProcessEvent(&events[i]);
    }

    int num_mouse_events = SDL_PeepEvents(events, 16, SDL_PEEKEVENT, SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
    for (int i = 0; i < num_mouse_events; ++i)
    {
        SDLInputHandler::ProcessEvent(&events[i]);
    }

    // Process controller events for pitch control
    // Controller right stick Y-axis controls pitch (instead of zoom in vanilla)
    // Holding left stick pressed enables original zoom behavior
    int num_controller_axis_events = SDL_PeepEvents(events, 16, SDL_PEEKEVENT, SDL_CONTROLLERAXISMOTION, SDL_CONTROLLERAXISMOTION);
    for (int i = 0; i < num_controller_axis_events; ++i)
    {
        if (events[i].caxis.axis == SDL_CONTROLLER_AXIS_RIGHTY)
        {
            // Normalize axis value from [-32768, 32767] to [-1.0, 1.0]
            float normalized = events[i].caxis.value / 32767.0f;
            // Apply deadzone
            if (std::abs(normalized) < 0.15f)
            {
                normalized = 0.0f;
            }
            state->controller_right_stick_y = normalized;
        }
    }

    int num_controller_button_events = SDL_PeepEvents(events, 16, SDL_PEEKEVENT, SDL_CONTROLLERBUTTONDOWN, SDL_CONTROLLERBUTTONUP);
    for (int i = 0; i < num_controller_button_events; ++i)
    {
        if (events[i].cbutton.button == SDL_CONTROLLER_BUTTON_LEFTSTICK)
        {
            state->controller_left_stick_pressed = (events[i].type == SDL_CONTROLLERBUTTONDOWN);
        }
    }

    if (*settings->enable_improved_mouselook)
    {
        const std::lock_guard<std::mutex> lock(state->hide_cursor_mutex);
        if (state->ShouldHideCursor())
        {
            if (!state->cursor_hidden_last_frame)
            {
                SDL_SetRelativeMouseMode(SDL_TRUE);
                state->rotate_start_time = SDL_GetTicks();
            }
            if (state->rotate_start_time != 0 && SDL_GetTicks() - state->rotate_start_time >
                                                     *Settings::GetSingleton()->rotate_threshold)
            {
                HideVirtualCursor(true);
            }
        }
        else
        {
            if (state->cursor_hidden_last_frame)
            {
                POINT p = state->cursor_position_to_restore;
                SDL_WarpMouseInWindow(state->sdl_window, (int)p.x, (int)p.y);
                SDL_SetRelativeMouseMode(SDL_FALSE);
                HideVirtualCursor(false);
            }
        }
        state->cursor_hidden_last_frame = state->ShouldHideCursor();
    }

    return OriginalFunc();
}

