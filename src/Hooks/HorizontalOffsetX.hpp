#pragma once

using namespace DKUtil::Alias;

class HorizontalOffsetX
{
public:
    static void Enable();
    static bool Prepare();
    // These conditions are copied from the games GetVerticalOffsetMult function to avoid
    // offsets during dialog etc.
    static bool ShouldNotUseOffsetNow(int64_t manager);

private:
    static float __cdecl Func(int64_t manager, float x);
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
    static inline std::unique_ptr<DKUtil::Hook::CaveHookHandle> handle;
};
