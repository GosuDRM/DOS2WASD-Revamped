#include "VersionInfo.hpp"

void VersionInfo::Print(HMODULE a_hModule)
{
    PrintGameVersion();
    PrintModVersion(a_hModule);
}

void VersionInfo::PrintModVersion(HMODULE a_hModule)
{
    WCHAR mod_dll_file_name[_MAX_PATH];
    GetModuleFileName(a_hModule, mod_dll_file_name, _MAX_PATH);
    VersionInfo::PrintModuleVersion(mod_dll_file_name);
}

void VersionInfo::PrintGameVersion()
{
    std::string_view process_name_stringview = dku::Hook::GetProcessName();
    std::string process_name_string = { process_name_stringview.begin(),
        process_name_stringview.end() };
    std::wstring process_name_wstring(process_name_string.begin(), process_name_string.end());
    VersionInfo::PrintModuleVersion(process_name_wstring);
}

// Convert wstring to string using Windows API (replaces deprecated std::codecvt)
static std::string WStringToString(const std::wstring& wstr)
{
    if (wstr.empty())
    {
        return std::string();
    }
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), nullptr, 0, nullptr, nullptr);
    std::string result(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), (int)wstr.size(), &result[0], size_needed, nullptr, nullptr);
    return result;
}

void VersionInfo::PrintModuleVersion(std::wstring file_name)
{
    DWORD size = 0;
    DWORD handle = 0;
    size = GetFileVersionInfoSize(file_name.c_str(), &handle);
    if (size == 0)
    {
        return;
    }
    std::vector<BYTE> versionInfo(size);
    if (!GetFileVersionInfo(file_name.c_str(), handle, size, versionInfo.data()))
    {
        return;
    }
    UINT len = 0;
    VS_FIXEDFILEINFO* vsfi = NULL;
    if (!VerQueryValue(versionInfo.data(), L"\\", (void**)&vsfi, &len) || len == 0)
    {
        return;
    }
    int aVersion[4];
    aVersion[0] = HIWORD(vsfi->dwFileVersionMS);
    aVersion[1] = LOWORD(vsfi->dwFileVersionMS);
    aVersion[2] = HIWORD(vsfi->dwFileVersionLS);
    aVersion[3] = LOWORD(vsfi->dwFileVersionLS);
    std::string file_name_narrow = WStringToString(file_name);

}

