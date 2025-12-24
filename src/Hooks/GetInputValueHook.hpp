#pragma once

#include <array>

class GetInputValueHook
{
public:
    static void Enable();
    static bool Prepare();

private:
    static int64_t OverrideFunc(int64_t player_input_controller_ptr, int64_t in_out_value_vec_ptr,
        DWORD* command_id_ptr, int a4);
    static inline std::add_pointer_t<decltype(OverrideFunc)> OriginalFunc;
    static inline std::array<uintptr_t, 8> addresses;
    static inline bool all_found = false;
};
