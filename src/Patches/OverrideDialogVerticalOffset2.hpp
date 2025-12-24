#pragma once

class ASMPatchHandle;

struct OverrideDialogVerticalOffset2Asm : Xbyak::CodeGenerator
{
    OverrideDialogVerticalOffset2Asm()
    {
        nop();
        nop();
        nop();
    }
};

class OverrideDialogVerticalOffset2
{
public:
    static bool Prepare();
    static void Deactivate();
    static void Activate();
    static void Enable();

private:
    static inline std::array<uintptr_t, 1> addresses;
    static inline bool all_found = false;
    static inline std::unique_ptr<DKUtil::Hook::ASMPatchHandle,
        std::default_delete<DKUtil::Hook::ASMPatchHandle>>
        handle;
};
