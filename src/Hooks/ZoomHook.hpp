#pragma once

class ZoomHook
{
public:
    static void Enable();
    static bool Prepare();

private:
    static void OverrideFunc(int64_t a1, char a2);
    static inline std::add_pointer_t<decltype(OverrideFunc)> OriginalFunc;
    static inline std::array<uintptr_t, 2> addresses;
    static inline bool all_found = false;
};
