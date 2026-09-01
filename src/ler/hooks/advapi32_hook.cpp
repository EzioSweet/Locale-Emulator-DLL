#include "ler/hooks/advapi32_hook.hpp"
#include "common/config.hpp"
#include "common/log.hpp"
#include "detours/detours.hpp"

namespace ler::hooks {

namespace {

using pfnRegQueryValueExA = LSTATUS(WINAPI*)(HKEY, LPCSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);
using pfnRegQueryValueExW = LSTATUS(WINAPI*)(HKEY, LPCWSTR, LPDWORD, LPDWORD, LPBYTE, LPDWORD);

pfnRegQueryValueExA True_RegQueryValueExA = nullptr;
pfnRegQueryValueExW True_RegQueryValueExW = nullptr;

LSTATUS WINAPI Hook_RegQueryValueExA(
    HKEY hKey, LPCSTR lpValueName, LPDWORD lpReserved,
    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {

    if (lpValueName) {
        if (_stricmp(lpValueName, "ACP") == 0) {
            char acpStr[16];
            sprintf_s(acpStr, sizeof(acpStr), "%u", common::Config::Instance().ansi_code_page());
            DWORD reqLen = static_cast<DWORD>(strlen(acpStr) + 1);
            if (lpType) *lpType = REG_SZ;
            if (lpcbData) {
                if (!lpData || *lpcbData < reqLen) {
                    *lpcbData = reqLen;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = reqLen;
            }
            if (lpData) {
                strcpy_s(reinterpret_cast<char*>(lpData), *lpcbData, acpStr);
            }
            return ERROR_SUCCESS;
        } else if (_stricmp(lpValueName, "OEMCP") == 0) {
            char oemStr[16];
            sprintf_s(oemStr, sizeof(oemStr), "%u", common::Config::Instance().oem_code_page());
            DWORD reqLen = static_cast<DWORD>(strlen(oemStr) + 1);
            if (lpType) *lpType = REG_SZ;
            if (lpcbData) {
                if (!lpData || *lpcbData < reqLen) {
                    *lpcbData = reqLen;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = reqLen;
            }
            if (lpData) {
                strcpy_s(reinterpret_cast<char*>(lpData), *lpcbData, oemStr);
            }
            return ERROR_SUCCESS;
        } else if (_stricmp(lpValueName, "Default") == 0 || _stricmp(lpValueName, "InstallLanguage") == 0) {
            char lcidStr[16];
            sprintf_s(lcidStr, sizeof(lcidStr), "%04x", static_cast<UINT>(common::Config::Instance().locale_id()));
            DWORD reqLen = static_cast<DWORD>(strlen(lcidStr) + 1);
            if (lpType) *lpType = REG_SZ;
            if (lpcbData) {
                if (!lpData || *lpcbData < reqLen) {
                    *lpcbData = reqLen;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = reqLen;
            }
            if (lpData) {
                strcpy_s(reinterpret_cast<char*>(lpData), *lpcbData, lcidStr);
            }
            return ERROR_SUCCESS;
        }
    }

    return True_RegQueryValueExA ? True_RegQueryValueExA(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData) : ERROR_CALL_NOT_IMPLEMENTED;
}

LSTATUS WINAPI Hook_RegQueryValueExW(
    HKEY hKey, LPCWSTR lpValueName, LPDWORD lpReserved,
    LPDWORD lpType, LPBYTE lpData, LPDWORD lpcbData) {

    if (lpValueName) {
        if (_wcsicmp(lpValueName, L"ACP") == 0) {
            wchar_t acpStr[16];
            wsprintfW(acpStr, L"%u", common::Config::Instance().ansi_code_page());
            DWORD reqLen = static_cast<DWORD>((wcslen(acpStr) + 1) * sizeof(WCHAR));
            if (lpType) *lpType = REG_SZ;
            if (lpcbData) {
                if (!lpData || *lpcbData < reqLen) {
                    *lpcbData = reqLen;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = reqLen;
            }
            if (lpData) {
                wcscpy_s(reinterpret_cast<wchar_t*>(lpData), *lpcbData / sizeof(WCHAR), acpStr);
            }
            return ERROR_SUCCESS;
        } else if (_wcsicmp(lpValueName, L"OEMCP") == 0) {
            wchar_t oemStr[16];
            wsprintfW(oemStr, L"%u", common::Config::Instance().oem_code_page());
            DWORD reqLen = static_cast<DWORD>((wcslen(oemStr) + 1) * sizeof(WCHAR));
            if (lpType) *lpType = REG_SZ;
            if (lpcbData) {
                if (!lpData || *lpcbData < reqLen) {
                    *lpcbData = reqLen;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = reqLen;
            }
            if (lpData) {
                wcscpy_s(reinterpret_cast<wchar_t*>(lpData), *lpcbData / sizeof(WCHAR), oemStr);
            }
            return ERROR_SUCCESS;
        } else if (_wcsicmp(lpValueName, L"Default") == 0 || _wcsicmp(lpValueName, L"InstallLanguage") == 0) {
            wchar_t lcidStr[16];
            wsprintfW(lcidStr, L"%04x", static_cast<UINT>(common::Config::Instance().locale_id()));
            DWORD reqLen = static_cast<DWORD>((wcslen(lcidStr) + 1) * sizeof(WCHAR));
            if (lpType) *lpType = REG_SZ;
            if (lpcbData) {
                if (!lpData || *lpcbData < reqLen) {
                    *lpcbData = reqLen;
                    return lpData ? ERROR_MORE_DATA : ERROR_SUCCESS;
                }
                *lpcbData = reqLen;
            }
            if (lpData) {
                wcscpy_s(reinterpret_cast<wchar_t*>(lpData), *lpcbData / sizeof(WCHAR), lcidStr);
            }
            return ERROR_SUCCESS;
        }
    }

    return True_RegQueryValueExW ? True_RegQueryValueExW(hKey, lpValueName, lpReserved, lpType, lpData, lpcbData) : ERROR_CALL_NOT_IMPLEMENTED;
}

} // namespace

void Advapi32Hook::Attach() noexcept {
    HMODULE hModule = ::GetModuleHandleW(L"advapi32.dll");
    if (!hModule) {
        hModule = ::GetModuleHandleW(L"kernelbase.dll");
    }
    if (hModule) {
        True_RegQueryValueExA = reinterpret_cast<pfnRegQueryValueExA>(::GetProcAddress(hModule, "RegQueryValueExA"));
        True_RegQueryValueExW = reinterpret_cast<pfnRegQueryValueExW>(::GetProcAddress(hModule, "RegQueryValueExW"));
    }

    if (True_RegQueryValueExA) ::DetourAttach(&(PVOID&)True_RegQueryValueExA, reinterpret_cast<PVOID>(Hook_RegQueryValueExA));
    if (True_RegQueryValueExW) ::DetourAttach(&(PVOID&)True_RegQueryValueExW, reinterpret_cast<PVOID>(Hook_RegQueryValueExW));
}

void Advapi32Hook::Detach() noexcept {
    if (True_RegQueryValueExA) ::DetourDetach(&(PVOID&)True_RegQueryValueExA, reinterpret_cast<PVOID>(Hook_RegQueryValueExA));
    if (True_RegQueryValueExW) ::DetourDetach(&(PVOID&)True_RegQueryValueExW, reinterpret_cast<PVOID>(Hook_RegQueryValueExW));
}

} // namespace ler::hooks
