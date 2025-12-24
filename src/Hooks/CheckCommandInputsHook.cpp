#include "CheckCommandInputsHook.hpp"
#include "InputFaker.hpp"
#include "Settings.hpp"
#include "State.hpp"
#include "GameCommand.hpp"

using enum GameCommand;

bool CheckCommandInputsHook::Prepare()
{
    std::array<uintptr_t, 1> address_array = { AsAddress(dku::Hook::Assembly::search_pattern<
        "E8 ?? ?? ?? FE 48 8B 03 48 85 C0 74 0B 48 8B D8 EB 36">()) };
    addresses = address_array;

    all_found = true;
    int i = 0;
    for (const auto& address : addresses)
    {
        if (!address)
        {
            State::GetSingleton()->mod_found_all_addresses = false;
            WARN("CheckCommandInputsHook #{} not found", i);
            all_found = false;
        }
        ++i;
    }
    return all_found;
}

void CheckCommandInputsHook::Enable()
{
    if (not all_found)
    {
        return;
    }
    int i = 0;
    for (const auto& address : addresses)
    {
        OriginalFunc = dku::Hook::write_call<5>(address, OverrideFunc);
        DEBUG("Hooked CheckCommandInputsHook #{}: {:X}", i, AsAddress(address));
        ++i;
    }
}

// Called in GameThread, every frame
int64_t CheckCommandInputsHook::OverrideFunc(int64_t* a1, int64_t a2)
{
    auto* state = State::GetSingleton();
    auto* settings = Settings::GetSingleton();

    if (state->order_force_stop)
    {
        state->is_force_stop = true;
        state->order_force_stop = false;
        InputFaker::SendCommand(ActionCancel, SDL_RELEASED);
    }



    return OriginalFunc(a1, a2);
}
