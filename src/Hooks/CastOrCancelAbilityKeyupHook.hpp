#pragma once

typedef int64_t QWORD;

class CastOrCancelAbilityKeyupHook
{
public:
    static void Enable();
    static bool Prepare();

private:
    static int64_t OverrideFunc(QWORD* a1, uint8_t a2);
    static inline std::add_pointer_t<decltype(OverrideFunc)> OriginalFunc;
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
};
