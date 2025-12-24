#pragma once

class SettingsPtr
{
public:
    static int64_t Read();
    static bool Prepare();

private:
    static inline int64_t* settings_ptr = 0;
};
