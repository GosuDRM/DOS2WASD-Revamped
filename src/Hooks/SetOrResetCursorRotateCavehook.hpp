#pragma once

using namespace DKUtil::Alias;

class SetOrResetCursorRotateCavehook
{
public:
    static void Enable();
    static bool Prepare();
    static void Func(int64_t set);

private:
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
    static inline std::unique_ptr<DKUtil::Hook::CaveHookHandle> handle;
};
