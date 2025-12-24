#include "SDLInputHandler.hpp"

using enum Command;

void SDLInputHandler::ProcessEvent(const SDL_Event* event)
{
    if (!event)
    {
        return;
    }

    // Deduplicate: skip events we've already processed (since we use SDL_PEEKEVENT)
    Uint32 event_timestamp = 0;
    switch (event->type)
    {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
        event_timestamp = event->key.timestamp;
        break;
    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        event_timestamp = event->button.timestamp;
        break;
    default:
        return;
    }

    if (event_timestamp <= last_event_timestamp)
    {
        return;  // Already processed this event
    }
    last_event_timestamp = event_timestamp;

    switch (event->type)
    {
    case SDL_KEYDOWN:
        if (event->key.repeat == 0)  // Ignore key repeats
        {
            last_keycode = event->key.keysym.sym;
            last_mod_state = (SDL_Keymod)event->key.keysym.mod;
            last_mouse_button = 0;
            last_was_keydown = true;
            HandleInput();
        }
        break;

    case SDL_KEYUP:
        last_keycode = event->key.keysym.sym;
        last_mod_state = (SDL_Keymod)event->key.keysym.mod;
        last_mouse_button = 0;
        last_was_keydown = false;
        HandleInput();
        break;

    case SDL_MOUSEBUTTONDOWN:
        MouseLeftDown(event);
        MouseRightDown(event);
        last_keycode = 0;
        last_mod_state = SDL_GetModState();
        last_mouse_button = event->button.button;
        last_was_keydown = true;
        HandleInput();
        break;

    case SDL_MOUSEBUTTONUP:
        MouseLeftDown(event);
        last_keycode = 0;
        last_mod_state = SDL_GetModState();
        last_mouse_button = event->button.button;
        last_was_keydown = false;
        HandleInput();
        break;

    default:
        break;
    }
}

void SDLInputHandler::HandleInput()
{
    auto* state = State::GetSingleton();
    AutoRun(state);
    ToggleMovementMode(state);
    WalkOrSprint(state);
    ReloadConfig();
}

bool SDLInputHandler::IsModifierDown(SDL_Keymod mod)
{
    return (last_mod_state & mod) != 0;
}

bool SDLInputHandler::DidCommandChange(Command command, bool keydown)
{
    if (last_was_keydown != keydown)
    {
        return false;
    }

    auto* settings = Settings::GetSingleton();
    std::string setting_value;

    switch (command)
    {
    case TOGGLE_WALKSPEED:
        setting_value = *settings->toggle_walkspeed;
        break;
    case HOLD_WALKSPEED:
        setting_value = *settings->hold_walkspeed;
        break;
    case TOGGLE_AUTOFORWARD:
        setting_value = *settings->toggle_autoforward;
        break;
    case TOGGLE_MOVEMENT_MODE:
        setting_value = *settings->toggle_movement_mode;
        break;
    case HOLD_MOVEMENT_MODE:
        setting_value = *settings->hold_movement_mode;
        break;
    case RELOAD_CONFIG:
        setting_value = *settings->reload_config;
        break;
    default:
        return false;
    }

    auto keycombos = settings->GetBoundKeycombos(setting_value);
    for (const auto& keycombo : keycombos)
    {
        std::vector<std::string> parts = dku::string::split(keycombo, "+"sv);
        if (parts.empty())
        {
            continue;
        }
        std::string main_key = parts.back();
        parts.pop_back();
        std::vector<std::string> modifiers = parts;

        // Check modifiers
        bool shift_required = false;
        bool ctrl_required = false;
        bool alt_required = false;

        for (const auto& mod : modifiers)
        {
            if (mod == "shift") shift_required = true;
            else if (mod == "ctrl") ctrl_required = true;
            else if (mod == "alt") alt_required = true;
        }

        bool shift_ok = shift_required == IsModifierDown(KMOD_SHIFT);
        bool ctrl_ok = ctrl_required == IsModifierDown(KMOD_CTRL);
        bool alt_ok = alt_required == IsModifierDown(KMOD_ALT);

        if (!shift_ok || !ctrl_ok || !alt_ok)
        {
            continue;
        }

        // Check main key
        std::vector<std::string> key_parts = dku::string::split(main_key, ":"sv);
        if (key_parts.size() < 2)
        {
            continue;
        }

        std::string prefix = key_parts[0];
        std::string key_name = key_parts[1];

        if (prefix == "key")
        {
            // Keyboard key
            SDL_Keycode expected_keycode = VirtualKeyMap::GetKeycodeByName(main_key);
            if (last_keycode == expected_keycode && last_mouse_button == 0)
            {
                return true;
            }
        }
        else if (prefix == "mouse")
        {
            // Mouse button
            if (last_keycode == 0 && last_mouse_button != 0)
            {
                Uint8 expected_button = 0;
                if (key_name == "left") expected_button = SDL_BUTTON_LEFT;
                else if (key_name == "right") expected_button = SDL_BUTTON_RIGHT;
                else if (key_name == "middle") expected_button = SDL_BUTTON_MIDDLE;
                else if (key_name == "x1") expected_button = SDL_BUTTON_X1;
                else if (key_name == "x2") expected_button = SDL_BUTTON_X2;

                if (last_mouse_button == expected_button)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

void SDLInputHandler::AutoRun(State* state)
{
    if (DidCommandChange(TOGGLE_AUTOFORWARD, true) && state->IsCharacterMovementMode())
    {
        state->autoforward_toggled = !state->autoforward_toggled.load();
        return;
    }

    // Check if any movement key was pressed to cancel autoforward
    auto* settings = Settings::GetSingleton();
    auto forward_keys = state->character_forward_keys;
    auto backward_keys = state->character_backward_keys;

    for (const auto& keycombo : forward_keys)
    {
        std::vector<std::string> key_parts = dku::string::split(keycombo, ":"sv);
        if (key_parts.size() >= 2 && key_parts[0] == "key")
        {
            SDL_Keycode kc = VirtualKeyMap::GetKeycodeByName(keycombo);
            if (last_keycode == kc && last_was_keydown)
            {
                state->autoforward_toggled = false;
                return;
            }
        }
    }

    for (const auto& keycombo : backward_keys)
    {
        std::vector<std::string> key_parts = dku::string::split(keycombo, ":"sv);
        if (key_parts.size() >= 2 && key_parts[0] == "key")
        {
            SDL_Keycode kc = VirtualKeyMap::GetKeycodeByName(keycombo);
            if (last_keycode == kc && last_was_keydown)
            {
                state->autoforward_toggled = false;
                return;
            }
        }
    }

    if (DidCommandChange(TOGGLE_MOVEMENT_MODE, true) ||
        DidCommandChange(HOLD_MOVEMENT_MODE, true) ||
        DidCommandChange(HOLD_MOVEMENT_MODE, false))
    {
        state->autoforward_toggled = false;
    }
}

void SDLInputHandler::ToggleMovementMode(State* state)
{
    if (DidCommandChange(TOGGLE_MOVEMENT_MODE, true))
    {
        bool new_state = !state->IsMovementModeToggled();
        state->SetMovementModeToggled(new_state);
        return;
    }

    if (DidCommandChange(HOLD_MOVEMENT_MODE, true))
    {
        state->SetMovementModeHeld(true);
        return;
    }

    if (DidCommandChange(HOLD_MOVEMENT_MODE, false))
    {
        state->SetMovementModeHeld(false);
        return;
    }
}

void SDLInputHandler::WalkOrSprint(State* state)
{
    if (DidCommandChange(TOGGLE_WALKSPEED, true) && state->IsCharacterMovementMode())
    {
        state->walking_toggled = !state->walking_toggled.load();
        return;
    }

    if (DidCommandChange(HOLD_WALKSPEED, true) && state->IsCharacterMovementMode())
    {
        state->walking_held = true;
        return;
    }

    if (DidCommandChange(HOLD_WALKSPEED, false) && state->IsCharacterMovementMode())
    {
        state->walking_held = false;
        return;
    }
}

void SDLInputHandler::ReloadConfig()
{
    if (DidCommandChange(RELOAD_CONFIG, true))
    {
        Settings::GetSingleton()->Load();
    }
}

void SDLInputHandler::MouseLeftDown(const SDL_Event* event)
{
    auto* state = State::GetSingleton();

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT)
    {
        state->is_mouseleft_pressed = true;
    }
    else if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT)
    {
        state->is_mouseleft_pressed = false;
    }
}

void SDLInputHandler::MouseRightDown(const SDL_Event* event)
{
    auto* state = State::GetSingleton();

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_RIGHT)
    {
        state->rotation_validated = false;
    }
}
