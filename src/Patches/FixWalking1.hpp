#pragma once

class ASMPatchHandle;

struct FixWalking1Asm : Xbyak::CodeGenerator
{
    FixWalking1Asm()
    {
        xor_(eax, eax);
        nop();
    }
};

// When changing Analog Stick Selection in the menu, ignore it.
class FixWalking1
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
