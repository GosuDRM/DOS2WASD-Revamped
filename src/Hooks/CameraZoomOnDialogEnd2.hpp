#pragma once

using namespace DKUtil::Alias;

class CameraZoomOnDialogEnd2
{
public:
    static void Enable();
    static bool Prepare();
    static void Func(int64_t a1);

private:
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
    static inline std::unique_ptr<DKUtil::Hook::CaveHookHandle> handle;
};
