#pragma once

typedef int64_t QWORD;

using namespace DKUtil::Alias;

// Activates BlockCancelActionStoppingMovementPatch
class MouseDeltaCavehook
{
public:
    static void Enable();
    static bool Prepare();

private:
    static void __cdecl Func(int dx, int dy);
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
    static inline std::unique_ptr<DKUtil::Hook::CaveHookHandle> handle;
};
