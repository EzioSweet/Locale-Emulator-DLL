#include "ler/hooks/kernel32_hook.hpp"
#include "common/config.hpp"
#include "common/types.hpp"
#include "common/log.hpp"
#include "detours/detours.hpp"

namespace ler::hooks {

namespace {

using pfnGetACP = UINT(WINAPI*)(void);
using pfnGetOEMCP = UINT(WINAPI*)(void);
using pfnGetUserDefaultLCID = LCID(WINAPI*)(void);
using pfnGetSystemDefaultLCID = LCID(WINAPI*)(void);
using pfnGetThreadLocale = LCID(WINAPI*)(void);
using pfnGetUserDefaultUILanguage = LANGID(WINAPI*)(void);
using pfnGetSystemDefaultUILanguage = LANGID(WINAPI*)(void);
using pfnGetUserDefaultLangID = LANGID(WINAPI*)(void);
using pfnGetSystemDefaultLangID = LANGID(WINAPI*)(void);
using pfnGetUserDefaultLocaleName = int(WINAPI*)(LPWSTR, int);
using pfnGetSystemDefaultLocaleName = int(WINAPI*)(LPWSTR, int);
using pfnGetLocaleInfoA = int(WINAPI*)(LCID, LCTYPE, LPSTR, int);
using pfnGetLocaleInfoW = int(WINAPI*)(LCID, LCTYPE, LPWSTR, int);
using pfnGetLocaleInfoEx = int(WINAPI*)(LPCWSTR, LCTYPE, LPWSTR, int);
using pfnGetTimeZoneInformation = DWORD(WINAPI*)(LPTIME_ZONE_INFORMATION);
using pfnGetDynamicTimeZoneInformation = DWORD(WINAPI*)(PDYNAMIC_TIME_ZONE_INFORMATION);
using pfnCompareStringA = int(WINAPI*)(LCID, DWORD, LPCCH, int, LPCCH, int);
using pfnCompareStringW = int(WINAPI*)(LCID, DWORD, LPCWCH, int, LPCWCH, int);
using pfnLCMapStringA = int(WINAPI*)(LCID, DWORD, LPCSTR, int, LPSTR, int);
using pfnLCMapStringW = int(WINAPI*)(LCID, DWORD, LPCWSTR, int, LPWSTR, int);
using pfnMultiByteToWideChar = int(WINAPI*)(UINT, DWORD, LPCCH, int, LPWSTR, int);
using pfnWideCharToMultiByte = int(WINAPI*)(UINT, DWORD, LPCWCH, int, LPSTR, int, LPCCH, LPBOOL);

pfnGetACP True_GetACP = nullptr;
pfnGetOEMCP True_GetOEMCP = nullptr;
pfnGetUserDefaultLCID True_GetUserDefaultLCID = nullptr;
pfnGetSystemDefaultLCID True_GetSystemDefaultLCID = nullptr;
pfnGetThreadLocale True_GetThreadLocale = nullptr;
pfnGetUserDefaultUILanguage True_GetUserDefaultUILanguage = nullptr;
pfnGetSystemDefaultUILanguage True_GetSystemDefaultUILanguage = nullptr;
pfnGetUserDefaultLangID True_GetUserDefaultLangID = nullptr;
pfnGetSystemDefaultLangID True_GetSystemDefaultLangID = nullptr;
pfnGetUserDefaultLocaleName True_GetUserDefaultLocaleName = nullptr;
pfnGetSystemDefaultLocaleName True_GetSystemDefaultLocaleName = nullptr;
pfnGetLocaleInfoA True_GetLocaleInfoA = nullptr;
pfnGetLocaleInfoW True_GetLocaleInfoW = nullptr;
pfnGetLocaleInfoEx True_GetLocaleInfoEx = nullptr;
pfnGetTimeZoneInformation True_GetTimeZoneInformation = nullptr;
pfnGetDynamicTimeZoneInformation True_GetDynamicTimeZoneInformation = nullptr;
pfnCompareStringA True_CompareStringA = nullptr;
pfnCompareStringW True_CompareStringW = nullptr;
pfnLCMapStringA True_LCMapStringA = nullptr;
pfnLCMapStringW True_LCMapStringW = nullptr;
pfnMultiByteToWideChar True_MultiByteToWideChar = nullptr;
pfnWideCharToMultiByte True_WideCharToMultiByte = nullptr;

UINT WINAPI Hook_GetACP(void) {
    return common::Config::Instance().ansi_code_page();
}

UINT WINAPI Hook_GetOEMCP(void) {
    return common::Config::Instance().oem_code_page();
}

LCID WINAPI Hook_GetUserDefaultLCID(void) {
    return common::Config::Instance().locale_id();
}

LCID WINAPI Hook_GetSystemDefaultLCID(void) {
    return common::Config::Instance().locale_id();
}

LCID WINAPI Hook_GetThreadLocale(void) {
    return common::Config::Instance().locale_id();
}

LANGID WINAPI Hook_GetUserDefaultUILanguage(void) {
    if (common::Config::Instance().hook_ui_language()) {
        return static_cast<LANGID>(common::Config::Instance().locale_id());
    }
    return True_GetUserDefaultUILanguage ? True_GetUserDefaultUILanguage() : static_cast<LANGID>(common::Config::Instance().locale_id());
}

LANGID WINAPI Hook_GetSystemDefaultUILanguage(void) {
    if (common::Config::Instance().hook_ui_language()) {
        return static_cast<LANGID>(common::Config::Instance().locale_id());
    }
    return True_GetSystemDefaultUILanguage ? True_GetSystemDefaultUILanguage() : static_cast<LANGID>(common::Config::Instance().locale_id());
}

LANGID WINAPI Hook_GetUserDefaultLangID(void) {
    return static_cast<LANGID>(common::Config::Instance().locale_id());
}

LANGID WINAPI Hook_GetSystemDefaultLangID(void) {
    return static_cast<LANGID>(common::Config::Instance().locale_id());
}

int WINAPI Hook_GetUserDefaultLocaleName(LPWSTR lpLocaleName, int cchLocaleName) {
    const auto& name = common::Config::Instance().locale_name();
    if (!lpLocaleName || cchLocaleName <= 0) {
        return static_cast<int>(name.length() + 1);
    }
    wcsncpy_s(lpLocaleName, cchLocaleName, name.c_str(), _TRUNCATE);
    return static_cast<int>(wcslen(lpLocaleName) + 1);
}

int WINAPI Hook_GetSystemDefaultLocaleName(LPWSTR lpLocaleName, int cchLocaleName) {
    const auto& name = common::Config::Instance().locale_name();
    if (!lpLocaleName || cchLocaleName <= 0) {
        return static_cast<int>(name.length() + 1);
    }
    wcsncpy_s(lpLocaleName, cchLocaleName, name.c_str(), _TRUNCATE);
    return static_cast<int>(wcslen(lpLocaleName) + 1);
}

int WINAPI Hook_GetLocaleInfoA(LCID Locale, LCTYPE LCType, LPSTR lpLCData, int cchData) {
    if (Locale == LOCALE_USER_DEFAULT || Locale == LOCALE_SYSTEM_DEFAULT || Locale == 0) {
        Locale = common::Config::Instance().locale_id();
    }

    LCTYPE baseType = LCType & ~LOCALE_NOUSEROVERRIDE & ~LOCALE_USE_CP_ACP;

    if (baseType == common::LOCALE_IDEFAULTANSICODEPAGE_CONST || baseType == common::LOCALE_IDEFAULTCODEPAGE_CONST) {
        char acpStr[16];
        sprintf_s(acpStr, sizeof(acpStr), "%u", common::Config::Instance().ansi_code_page());
        int len = static_cast<int>(strlen(acpStr) + 1);
        if (!lpLCData || cchData <= 0) {
            return len;
        }
        strcpy_s(lpLCData, cchData, acpStr);
        return len;
    }

    if (baseType == common::LOCALE_IDEFAULTOEMCODEPAGE_CONST) {
        char oemStr[16];
        sprintf_s(oemStr, sizeof(oemStr), "%u", common::Config::Instance().oem_code_page());
        int len = static_cast<int>(strlen(oemStr) + 1);
        if (!lpLCData || cchData <= 0) {
            return len;
        }
        strcpy_s(lpLCData, cchData, oemStr);
        return len;
    }

    return True_GetLocaleInfoA ? True_GetLocaleInfoA(Locale, LCType, lpLCData, cchData) : 0;
}

int WINAPI Hook_GetLocaleInfoW(LCID Locale, LCTYPE LCType, LPWSTR lpLCData, int cchData) {
    if (Locale == LOCALE_USER_DEFAULT || Locale == LOCALE_SYSTEM_DEFAULT || Locale == 0) {
        Locale = common::Config::Instance().locale_id();
    }

    LCTYPE baseType = LCType & ~LOCALE_NOUSEROVERRIDE & ~LOCALE_USE_CP_ACP;

    if (baseType == common::LOCALE_IDEFAULTANSICODEPAGE_CONST || baseType == common::LOCALE_IDEFAULTCODEPAGE_CONST) {
        wchar_t acpStr[16];
        wsprintfW(acpStr, L"%u", common::Config::Instance().ansi_code_page());
        int len = static_cast<int>(wcslen(acpStr) + 1);
        if (!lpLCData || cchData <= 0) {
            return len;
        }
        wcscpy_s(lpLCData, cchData, acpStr);
        return len;
    }

    if (baseType == common::LOCALE_IDEFAULTOEMCODEPAGE_CONST) {
        wchar_t oemStr[16];
        wsprintfW(oemStr, L"%u", common::Config::Instance().oem_code_page());
        int len = static_cast<int>(wcslen(oemStr) + 1);
        if (!lpLCData || cchData <= 0) {
            return len;
        }
        wcscpy_s(lpLCData, cchData, oemStr);
        return len;
    }

    return True_GetLocaleInfoW ? True_GetLocaleInfoW(Locale, LCType, lpLCData, cchData) : 0;
}

int WINAPI Hook_GetLocaleInfoEx(LPCWSTR lpLocaleName, LCTYPE LCType, LPWSTR lpLCData, int cchData) {
    if (!lpLocaleName || wcscmp(lpLocaleName, LOCALE_NAME_USER_DEFAULT) == 0 || wcscmp(lpLocaleName, LOCALE_NAME_SYSTEM_DEFAULT) == 0) {
        lpLocaleName = common::Config::Instance().locale_name().c_str();
    }

    LCTYPE baseType = LCType & ~LOCALE_NOUSEROVERRIDE & ~LOCALE_USE_CP_ACP;

    if (baseType == common::LOCALE_IDEFAULTANSICODEPAGE_CONST || baseType == common::LOCALE_IDEFAULTCODEPAGE_CONST) {
        wchar_t acpStr[16];
        wsprintfW(acpStr, L"%u", common::Config::Instance().ansi_code_page());
        int len = static_cast<int>(wcslen(acpStr) + 1);
        if (!lpLCData || cchData <= 0) {
            return len;
        }
        wcscpy_s(lpLCData, cchData, acpStr);
        return len;
    }

    if (True_GetLocaleInfoEx) {
        return True_GetLocaleInfoEx(lpLocaleName, LCType, lpLCData, cchData);
    }
    return 0;
}

DWORD WINAPI Hook_GetTimeZoneInformation(LPTIME_ZONE_INFORMATION lpTimeZoneInformation) {
    if (!lpTimeZoneInformation) {
        return TIME_ZONE_ID_INVALID;
    }

    ZeroMemory(lpTimeZoneInformation, sizeof(TIME_ZONE_INFORMATION));
    lpTimeZoneInformation->Bias = common::Config::Instance().timezone_bias();
    wcscpy_s(lpTimeZoneInformation->StandardName, 32, L"Tokyo Standard Time");
    wcscpy_s(lpTimeZoneInformation->DaylightName, 32, L"Tokyo Daylight Time");
    lpTimeZoneInformation->StandardBias = 0;
    lpTimeZoneInformation->DaylightBias = 0;

    return TIME_ZONE_ID_STANDARD;
}

DWORD WINAPI Hook_GetDynamicTimeZoneInformation(PDYNAMIC_TIME_ZONE_INFORMATION pTimeZoneInformation) {
    if (!pTimeZoneInformation) {
        return TIME_ZONE_ID_INVALID;
    }

    ZeroMemory(pTimeZoneInformation, sizeof(DYNAMIC_TIME_ZONE_INFORMATION));
    pTimeZoneInformation->Bias = common::Config::Instance().timezone_bias();
    wcscpy_s(pTimeZoneInformation->StandardName, 32, L"Tokyo Standard Time");
    wcscpy_s(pTimeZoneInformation->DaylightName, 32, L"Tokyo Daylight Time");
    wcscpy_s(pTimeZoneInformation->TimeZoneKeyName, 128, L"Tokyo Standard Time");
    pTimeZoneInformation->StandardBias = 0;
    pTimeZoneInformation->DaylightBias = 0;
    pTimeZoneInformation->DynamicDaylightTimeDisabled = TRUE;

    return TIME_ZONE_ID_STANDARD;
}

int WINAPI Hook_CompareStringA(LCID Locale, DWORD dwCmpFlags, LPCCH lpString1, int cchCount1, LPCCH lpString2, int cchCount2) {
    if (Locale == LOCALE_USER_DEFAULT || Locale == LOCALE_SYSTEM_DEFAULT || Locale == 0) {
        Locale = common::Config::Instance().locale_id();
    }
    return True_CompareStringA ? True_CompareStringA(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2) : 0;
}

int WINAPI Hook_CompareStringW(LCID Locale, DWORD dwCmpFlags, LPCWCH lpString1, int cchCount1, LPCWCH lpString2, int cchCount2) {
    if (Locale == LOCALE_USER_DEFAULT || Locale == LOCALE_SYSTEM_DEFAULT || Locale == 0) {
        Locale = common::Config::Instance().locale_id();
    }
    return True_CompareStringW ? True_CompareStringW(Locale, dwCmpFlags, lpString1, cchCount1, lpString2, cchCount2) : 0;
}

int WINAPI Hook_LCMapStringA(LCID Locale, DWORD dwMapFlags, LPCSTR lpSrcStr, int cchSrc, LPSTR lpDestStr, int cchDest) {
    if (Locale == LOCALE_USER_DEFAULT || Locale == LOCALE_SYSTEM_DEFAULT || Locale == 0) {
        Locale = common::Config::Instance().locale_id();
    }
    return True_LCMapStringA ? True_LCMapStringA(Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest) : 0;
}

int WINAPI Hook_LCMapStringW(LCID Locale, DWORD dwMapFlags, LPCWSTR lpSrcStr, int cchSrc, LPWSTR lpDestStr, int cchDest) {
    if (Locale == LOCALE_USER_DEFAULT || Locale == LOCALE_SYSTEM_DEFAULT || Locale == 0) {
        Locale = common::Config::Instance().locale_id();
    }
    return True_LCMapStringW ? True_LCMapStringW(Locale, dwMapFlags, lpSrcStr, cchSrc, lpDestStr, cchDest) : 0;
}

int WINAPI Hook_MultiByteToWideChar(
    UINT CodePage, DWORD dwFlags,
    LPCCH lpMultiByteStr, int cbMultiByte,
    LPWSTR lpWideCharStr, int cchWideChar) {

    if (CodePage == CP_ACP || CodePage == 0) {
        CodePage = common::Config::Instance().ansi_code_page();
    } else if (CodePage == CP_OEMCP || CodePage == 1) {
        CodePage = common::Config::Instance().oem_code_page();
    }

    return True_MultiByteToWideChar ? True_MultiByteToWideChar(CodePage, dwFlags, lpMultiByteStr, cbMultiByte, lpWideCharStr, cchWideChar) : 0;
}

int WINAPI Hook_WideCharToMultiByte(
    UINT CodePage, DWORD dwFlags,
    LPCWCH lpWideCharStr, int cchWideChar,
    LPSTR lpMultiByteStr, int cbMultiByte,
    LPCCH lpDefaultChar, LPBOOL lpUsedDefaultChar) {

    if (CodePage == CP_ACP || CodePage == 0) {
        CodePage = common::Config::Instance().ansi_code_page();
    } else if (CodePage == CP_OEMCP || CodePage == 1) {
        CodePage = common::Config::Instance().oem_code_page();
    }

    return True_WideCharToMultiByte ? True_WideCharToMultiByte(CodePage, dwFlags, lpWideCharStr, cchWideChar, lpMultiByteStr, cbMultiByte, lpDefaultChar, lpUsedDefaultChar) : 0;
}

PVOID GetApiProc(const char* name) {
    HMODULE hModule = ::GetModuleHandleW(L"kernelbase.dll");
    if (hModule) {
        PVOID proc = reinterpret_cast<PVOID>(::GetProcAddress(hModule, name));
        if (proc) return proc;
    }
    hModule = ::GetModuleHandleW(L"kernel32.dll");
    if (hModule) {
        return reinterpret_cast<PVOID>(::GetProcAddress(hModule, name));
    }
    return nullptr;
}

} // namespace

void Kernel32Hook::Attach() noexcept {
    True_GetACP = reinterpret_cast<pfnGetACP>(GetApiProc("GetACP"));
    True_GetOEMCP = reinterpret_cast<pfnGetOEMCP>(GetApiProc("GetOEMCP"));
    True_GetUserDefaultLCID = reinterpret_cast<pfnGetUserDefaultLCID>(GetApiProc("GetUserDefaultLCID"));
    True_GetSystemDefaultLCID = reinterpret_cast<pfnGetSystemDefaultLCID>(GetApiProc("GetSystemDefaultLCID"));
    True_GetThreadLocale = reinterpret_cast<pfnGetThreadLocale>(GetApiProc("GetThreadLocale"));
    True_GetUserDefaultUILanguage = reinterpret_cast<pfnGetUserDefaultUILanguage>(GetApiProc("GetUserDefaultUILanguage"));
    True_GetSystemDefaultUILanguage = reinterpret_cast<pfnGetSystemDefaultUILanguage>(GetApiProc("GetSystemDefaultUILanguage"));
    True_GetUserDefaultLangID = reinterpret_cast<pfnGetUserDefaultLangID>(GetApiProc("GetUserDefaultLangID"));
    True_GetSystemDefaultLangID = reinterpret_cast<pfnGetSystemDefaultLangID>(GetApiProc("GetSystemDefaultLangID"));
    True_GetUserDefaultLocaleName = reinterpret_cast<pfnGetUserDefaultLocaleName>(GetApiProc("GetUserDefaultLocaleName"));
    True_GetSystemDefaultLocaleName = reinterpret_cast<pfnGetSystemDefaultLocaleName>(GetApiProc("GetSystemDefaultLocaleName"));
    True_GetLocaleInfoA = reinterpret_cast<pfnGetLocaleInfoA>(GetApiProc("GetLocaleInfoA"));
    True_GetLocaleInfoW = reinterpret_cast<pfnGetLocaleInfoW>(GetApiProc("GetLocaleInfoW"));
    True_GetLocaleInfoEx = reinterpret_cast<pfnGetLocaleInfoEx>(GetApiProc("GetLocaleInfoEx"));
    True_GetTimeZoneInformation = reinterpret_cast<pfnGetTimeZoneInformation>(GetApiProc("GetTimeZoneInformation"));
    True_GetDynamicTimeZoneInformation = reinterpret_cast<pfnGetDynamicTimeZoneInformation>(GetApiProc("GetDynamicTimeZoneInformation"));
    True_CompareStringA = reinterpret_cast<pfnCompareStringA>(GetApiProc("CompareStringA"));
    True_CompareStringW = reinterpret_cast<pfnCompareStringW>(GetApiProc("CompareStringW"));
    True_LCMapStringA = reinterpret_cast<pfnLCMapStringA>(GetApiProc("LCMapStringA"));
    True_LCMapStringW = reinterpret_cast<pfnLCMapStringW>(GetApiProc("LCMapStringW"));
    True_MultiByteToWideChar = reinterpret_cast<pfnMultiByteToWideChar>(GetApiProc("MultiByteToWideChar"));
    True_WideCharToMultiByte = reinterpret_cast<pfnWideCharToMultiByte>(GetApiProc("WideCharToMultiByte"));

    if (True_GetACP) ::DetourAttach(&(PVOID&)True_GetACP, reinterpret_cast<PVOID>(Hook_GetACP));
    if (True_GetOEMCP) ::DetourAttach(&(PVOID&)True_GetOEMCP, reinterpret_cast<PVOID>(Hook_GetOEMCP));
    if (True_GetUserDefaultLCID) ::DetourAttach(&(PVOID&)True_GetUserDefaultLCID, reinterpret_cast<PVOID>(Hook_GetUserDefaultLCID));
    if (True_GetSystemDefaultLCID) ::DetourAttach(&(PVOID&)True_GetSystemDefaultLCID, reinterpret_cast<PVOID>(Hook_GetSystemDefaultLCID));
    if (True_GetThreadLocale) ::DetourAttach(&(PVOID&)True_GetThreadLocale, reinterpret_cast<PVOID>(Hook_GetThreadLocale));
    if (True_GetUserDefaultUILanguage) ::DetourAttach(&(PVOID&)True_GetUserDefaultUILanguage, reinterpret_cast<PVOID>(Hook_GetUserDefaultUILanguage));
    if (True_GetSystemDefaultUILanguage) ::DetourAttach(&(PVOID&)True_GetSystemDefaultUILanguage, reinterpret_cast<PVOID>(Hook_GetSystemDefaultUILanguage));
    if (True_GetUserDefaultLangID) ::DetourAttach(&(PVOID&)True_GetUserDefaultLangID, reinterpret_cast<PVOID>(Hook_GetUserDefaultLangID));
    if (True_GetSystemDefaultLangID) ::DetourAttach(&(PVOID&)True_GetSystemDefaultLangID, reinterpret_cast<PVOID>(Hook_GetSystemDefaultLangID));
    if (True_GetUserDefaultLocaleName) ::DetourAttach(&(PVOID&)True_GetUserDefaultLocaleName, reinterpret_cast<PVOID>(Hook_GetUserDefaultLocaleName));
    if (True_GetSystemDefaultLocaleName) ::DetourAttach(&(PVOID&)True_GetSystemDefaultLocaleName, reinterpret_cast<PVOID>(Hook_GetSystemDefaultLocaleName));
    if (True_GetLocaleInfoA) ::DetourAttach(&(PVOID&)True_GetLocaleInfoA, reinterpret_cast<PVOID>(Hook_GetLocaleInfoA));
    if (True_GetLocaleInfoW) ::DetourAttach(&(PVOID&)True_GetLocaleInfoW, reinterpret_cast<PVOID>(Hook_GetLocaleInfoW));
    if (True_GetLocaleInfoEx) ::DetourAttach(&(PVOID&)True_GetLocaleInfoEx, reinterpret_cast<PVOID>(Hook_GetLocaleInfoEx));
    if (True_GetTimeZoneInformation) ::DetourAttach(&(PVOID&)True_GetTimeZoneInformation, reinterpret_cast<PVOID>(Hook_GetTimeZoneInformation));
    if (True_GetDynamicTimeZoneInformation) ::DetourAttach(&(PVOID&)True_GetDynamicTimeZoneInformation, reinterpret_cast<PVOID>(Hook_GetDynamicTimeZoneInformation));
    if (True_CompareStringA) ::DetourAttach(&(PVOID&)True_CompareStringA, reinterpret_cast<PVOID>(Hook_CompareStringA));
    if (True_CompareStringW) ::DetourAttach(&(PVOID&)True_CompareStringW, reinterpret_cast<PVOID>(Hook_CompareStringW));
    if (True_LCMapStringA) ::DetourAttach(&(PVOID&)True_LCMapStringA, reinterpret_cast<PVOID>(Hook_LCMapStringA));
    if (True_LCMapStringW) ::DetourAttach(&(PVOID&)True_LCMapStringW, reinterpret_cast<PVOID>(Hook_LCMapStringW));
    if (True_MultiByteToWideChar) ::DetourAttach(&(PVOID&)True_MultiByteToWideChar, reinterpret_cast<PVOID>(Hook_MultiByteToWideChar));
    if (True_WideCharToMultiByte) ::DetourAttach(&(PVOID&)True_WideCharToMultiByte, reinterpret_cast<PVOID>(Hook_WideCharToMultiByte));
}

void Kernel32Hook::Detach() noexcept {
    if (True_GetACP) ::DetourDetach(&(PVOID&)True_GetACP, reinterpret_cast<PVOID>(Hook_GetACP));
    if (True_GetOEMCP) ::DetourDetach(&(PVOID&)True_GetOEMCP, reinterpret_cast<PVOID>(Hook_GetOEMCP));
    if (True_GetUserDefaultLCID) ::DetourDetach(&(PVOID&)True_GetUserDefaultLCID, reinterpret_cast<PVOID>(Hook_GetUserDefaultLCID));
    if (True_GetSystemDefaultLCID) ::DetourDetach(&(PVOID&)True_GetSystemDefaultLCID, reinterpret_cast<PVOID>(Hook_GetSystemDefaultLCID));
    if (True_GetThreadLocale) ::DetourDetach(&(PVOID&)True_GetThreadLocale, reinterpret_cast<PVOID>(Hook_GetThreadLocale));
    if (True_GetUserDefaultUILanguage) ::DetourDetach(&(PVOID&)True_GetUserDefaultUILanguage, reinterpret_cast<PVOID>(Hook_GetUserDefaultUILanguage));
    if (True_GetSystemDefaultUILanguage) ::DetourDetach(&(PVOID&)True_GetSystemDefaultUILanguage, reinterpret_cast<PVOID>(Hook_GetSystemDefaultUILanguage));
    if (True_GetUserDefaultLangID) ::DetourDetach(&(PVOID&)True_GetUserDefaultLangID, reinterpret_cast<PVOID>(Hook_GetUserDefaultLangID));
    if (True_GetSystemDefaultLangID) ::DetourDetach(&(PVOID&)True_GetSystemDefaultLangID, reinterpret_cast<PVOID>(Hook_GetSystemDefaultLangID));
    if (True_GetUserDefaultLocaleName) ::DetourDetach(&(PVOID&)True_GetUserDefaultLocaleName, reinterpret_cast<PVOID>(Hook_GetUserDefaultLocaleName));
    if (True_GetSystemDefaultLocaleName) ::DetourDetach(&(PVOID&)True_GetSystemDefaultLocaleName, reinterpret_cast<PVOID>(Hook_GetSystemDefaultLocaleName));
    if (True_GetLocaleInfoA) ::DetourDetach(&(PVOID&)True_GetLocaleInfoA, reinterpret_cast<PVOID>(Hook_GetLocaleInfoA));
    if (True_GetLocaleInfoW) ::DetourDetach(&(PVOID&)True_GetLocaleInfoW, reinterpret_cast<PVOID>(Hook_GetLocaleInfoW));
    if (True_GetLocaleInfoEx) ::DetourDetach(&(PVOID&)True_GetLocaleInfoEx, reinterpret_cast<PVOID>(Hook_GetLocaleInfoEx));
    if (True_GetTimeZoneInformation) ::DetourDetach(&(PVOID&)True_GetTimeZoneInformation, reinterpret_cast<PVOID>(Hook_GetTimeZoneInformation));
    if (True_GetDynamicTimeZoneInformation) ::DetourDetach(&(PVOID&)True_GetDynamicTimeZoneInformation, reinterpret_cast<PVOID>(Hook_GetDynamicTimeZoneInformation));
    if (True_CompareStringA) ::DetourDetach(&(PVOID&)True_CompareStringA, reinterpret_cast<PVOID>(Hook_CompareStringA));
    if (True_CompareStringW) ::DetourDetach(&(PVOID&)True_CompareStringW, reinterpret_cast<PVOID>(Hook_CompareStringW));
    if (True_LCMapStringA) ::DetourDetach(&(PVOID&)True_LCMapStringA, reinterpret_cast<PVOID>(Hook_LCMapStringA));
    if (True_LCMapStringW) ::DetourDetach(&(PVOID&)True_LCMapStringW, reinterpret_cast<PVOID>(Hook_LCMapStringW));
    if (True_MultiByteToWideChar) ::DetourDetach(&(PVOID&)True_MultiByteToWideChar, reinterpret_cast<PVOID>(Hook_MultiByteToWideChar));
    if (True_WideCharToMultiByte) ::DetourDetach(&(PVOID&)True_WideCharToMultiByte, reinterpret_cast<PVOID>(Hook_WideCharToMultiByte));
}

} // namespace ler::hooks
