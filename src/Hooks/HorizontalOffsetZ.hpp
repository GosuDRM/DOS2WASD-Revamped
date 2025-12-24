#pragma once

using namespace DKUtil::Alias;

class HorizontalOffsetZ
{
public:
    static void Enable();
    static bool Prepare();

private:
    static float __cdecl Func(int64_t manager, float z);
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
    static inline std::unique_ptr<DKUtil::Hook::CaveHookHandle> handle;
};
