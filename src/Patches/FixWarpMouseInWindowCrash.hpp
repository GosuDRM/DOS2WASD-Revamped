#pragma once

class ASMPatchHandle;

struct FixWarpMouseInWindowCrashAsm : Xbyak::CodeGenerator
{
    FixWarpMouseInWindowCrashAsm()
    {
        nop();
        nop();
        nop();
        nop();
        nop();
        nop();
    }
};

// This crash happens when you rotate the character in the equipment panel preview.
// Looks like an infinite loop in SDL_WarpMouseInWindow.
// Easy workaround is to just nop this. Cursor isn't clamped then anymore, but who cares.
class FixWarpMouseInWindowCrash
{
public:
    static bool Prepare();
    static void Activate();
    static void Deactivate();
    static void Enable();

private:
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
    static inline std::unique_ptr<DKUtil::Hook::ASMPatchHandle,
        std::default_delete<DKUtil::Hook::ASMPatchHandle>>
        handle;
};
