#pragma once

class ASMPatchHandle;

struct FollowOnChararacterChangeAsm : Xbyak::CodeGenerator
{
    FollowOnChararacterChangeAsm()
    {
        test(rsp, rsp);
        nop();
        nop();
        nop();
    }
};

// When changing characters by e.g. pressing F1, follow newly selected character,
// instead of unfollowing.
// Works by hijacking controller behaviour.
class FollowOnChararacterChange
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
