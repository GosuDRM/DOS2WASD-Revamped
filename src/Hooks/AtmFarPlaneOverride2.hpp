#pragma once

class AtmFarPlaneOverride2
{
public:
    static void Enable();
    static bool Prepare();

private:
    static void* OverrideFunc(void* dest, const void* src, size_t size);
    static inline std::add_pointer_t<decltype(OverrideFunc)> OriginalFunc;
    static inline std::array<uintptr_t, 2> addresses;
    static inline bool all_found = false;
};
