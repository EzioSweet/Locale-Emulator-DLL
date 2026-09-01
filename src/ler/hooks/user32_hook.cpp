#include "ler/hooks/user32_hook.hpp"
#include "common/config.hpp"
#include "common/log.hpp"
#include "detours/detours.hpp"
#include <vector>

namespace ler::hooks {

namespace {

using pfnCreateWindowExA = HWND(WINAPI*)(DWORD, LPCSTR, LPCSTR, DWORD, int, int, int, int, HWND, HMENU, HINSTANCE, LPVOID);
using pfnSetWindowTextA = BOOL(WINAPI*)(HWND, LPCSTR);
using pfnGetWindowTextA = int(WINAPI*)(HWND, LPSTR, int);
using pfnMessageBoxA = int(WINAPI*)(HWND, LPCSTR, LPCSTR, UINT);
using pfnMessageBoxTimeoutA = int(WINAPI*)(HWND, LPCSTR, LPCSTR, UINT, WORD, DWORD);

pfnCreateWindowExA True_CreateWindowExA = nullptr;
pfnSetWindowTextA True_SetWindowTextA = nullptr;
pfnGetWindowTextA True_GetWindowTextA = nullptr;
pfnMessageBoxA True_MessageBoxA = nullptr;
pfnMessageBoxTimeoutA True_MessageBoxTimeoutA = nullptr;

std::wstring AnsiToWide(LPCSTR ansiStr, UINT codePage) {
    if (!ansiStr) return {};
    if (reinterpret_cast<ULONG_PTR>(ansiStr) <= 0xFFFF) {
        return {};
    }
    int reqLen = ::MultiByteToWideChar(codePage, 0, ansiStr, -1, nullptr, 0);
    if (reqLen <= 0) return {};
    std::wstring result(reqLen - 1, L'\0');
    ::MultiByteToWideChar(codePage, 0, ansiStr, -1, result.data(), reqLen);
    return result;
}

HWND WINAPI Hook_CreateWindowExA(
    DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName,
    DWORD dwStyle, int X, int Y, int nWidth, int nHeight,
    HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) {

    if (!common::Config::Instance().fix_window_text()) {
        return True_CreateWindowExA ? True_CreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam) : nullptr;
    }

    std::wstring classNameW;
    LPCWSTR pClassNameW = nullptr;
    if (reinterpret_cast<ULONG_PTR>(lpClassName) > 0xFFFF) {
        classNameW = AnsiToWide(lpClassName, common::Config::Instance().ansi_code_page());
        pClassNameW = classNameW.c_str();
    } else {
        pClassNameW = reinterpret_cast<LPCWSTR>(lpClassName);
    }

    std::wstring windowNameW = AnsiToWide(lpWindowName, common::Config::Instance().ansi_code_page());
    LPCWSTR pWindowNameW = (reinterpret_cast<ULONG_PTR>(lpWindowName) > 0xFFFF) ? windowNameW.c_str() : reinterpret_cast<LPCWSTR>(lpWindowName);

    return ::CreateWindowExW(
        dwExStyle, pClassNameW, pWindowNameW,
        dwStyle, X, Y, nWidth, nHeight,
        hWndParent, hMenu, hInstance, lpParam);
}

BOOL WINAPI Hook_SetWindowTextA(HWND hWnd, LPCSTR lpString) {
    if (!common::Config::Instance().fix_window_text() || !lpString) {
        return True_SetWindowTextA ? True_SetWindowTextA(hWnd, lpString) : FALSE;
    }

    std::wstring wStr = AnsiToWide(lpString, common::Config::Instance().ansi_code_page());
    return ::SetWindowTextW(hWnd, wStr.c_str());
}

int WINAPI Hook_GetWindowTextA(HWND hWnd, LPSTR lpString, int nMaxCount) {
    if (!common::Config::Instance().fix_window_text() || !lpString || nMaxCount <= 0) {
        return True_GetWindowTextA ? True_GetWindowTextA(hWnd, lpString, nMaxCount) : 0;
    }

    int wLen = ::GetWindowTextLengthW(hWnd);
    if (wLen <= 0) {
        lpString[0] = '\0';
        return 0;
    }

    std::vector<wchar_t> wBuf(wLen + 1);
    ::GetWindowTextW(hWnd, wBuf.data(), wLen + 1);

    int convLen = ::WideCharToMultiByte(common::Config::Instance().ansi_code_page(), 0, wBuf.data(), -1, lpString, nMaxCount, nullptr, nullptr);
    if (convLen > 0) {
        return convLen - 1;
    }

    return 0;
}

int WINAPI Hook_MessageBoxA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType) {
    if (!common::Config::Instance().fix_window_text()) {
        return True_MessageBoxA ? True_MessageBoxA(hWnd, lpText, lpCaption, uType) : 0;
    }

    std::wstring wText = AnsiToWide(lpText, common::Config::Instance().ansi_code_page());
    std::wstring wCaption = AnsiToWide(lpCaption, common::Config::Instance().ansi_code_page());

    return ::MessageBoxW(hWnd, wText.c_str(), wCaption.c_str(), uType);
}

int WINAPI Hook_MessageBoxTimeoutA(HWND hWnd, LPCSTR lpText, LPCSTR lpCaption, UINT uType, WORD wLanguageId, DWORD dwMilliseconds) {
    if (!common::Config::Instance().fix_window_text()) {
        return True_MessageBoxTimeoutA ? True_MessageBoxTimeoutA(hWnd, lpText, lpCaption, uType, wLanguageId, dwMilliseconds) : 0;
    }

    std::wstring wText = AnsiToWide(lpText, common::Config::Instance().ansi_code_page());
    std::wstring wCaption = AnsiToWide(lpCaption, common::Config::Instance().ansi_code_page());

    using pfnMessageBoxTimeoutW = int(WINAPI*)(HWND, LPCWSTR, LPCWSTR, UINT, WORD, DWORD);
    HMODULE hUser32 = ::GetModuleHandleW(L"user32.dll");
    if (hUser32) {
        auto proc = reinterpret_cast<pfnMessageBoxTimeoutW>(::GetProcAddress(hUser32, "MessageBoxTimeoutW"));
        if (proc) {
            return proc(hWnd, wText.c_str(), wCaption.c_str(), uType, wLanguageId, dwMilliseconds);
        }
    }

    return ::MessageBoxW(hWnd, wText.c_str(), wCaption.c_str(), uType);
}

PVOID GetUserProc(const char* name) {
    HMODULE hModule = ::GetModuleHandleW(L"user32.dll");
    if (hModule) {
        return reinterpret_cast<PVOID>(::GetProcAddress(hModule, name));
    }
    return nullptr;
}

} // namespace

void User32Hook::Attach() noexcept {
    True_CreateWindowExA = reinterpret_cast<pfnCreateWindowExA>(GetUserProc("CreateWindowExA"));
    True_SetWindowTextA = reinterpret_cast<pfnSetWindowTextA>(GetUserProc("SetWindowTextA"));
    True_GetWindowTextA = reinterpret_cast<pfnGetWindowTextA>(GetUserProc("GetWindowTextA"));
    True_MessageBoxA = reinterpret_cast<pfnMessageBoxA>(GetUserProc("MessageBoxA"));
    True_MessageBoxTimeoutA = reinterpret_cast<pfnMessageBoxTimeoutA>(GetUserProc("MessageBoxTimeoutA"));

    if (True_CreateWindowExA) ::DetourAttach(&(PVOID&)True_CreateWindowExA, reinterpret_cast<PVOID>(Hook_CreateWindowExA));
    if (True_SetWindowTextA) ::DetourAttach(&(PVOID&)True_SetWindowTextA, reinterpret_cast<PVOID>(Hook_SetWindowTextA));
    if (True_GetWindowTextA) ::DetourAttach(&(PVOID&)True_GetWindowTextA, reinterpret_cast<PVOID>(Hook_GetWindowTextA));
    if (True_MessageBoxA) ::DetourAttach(&(PVOID&)True_MessageBoxA, reinterpret_cast<PVOID>(Hook_MessageBoxA));
    if (True_MessageBoxTimeoutA) ::DetourAttach(&(PVOID&)True_MessageBoxTimeoutA, reinterpret_cast<PVOID>(Hook_MessageBoxTimeoutA));
}

void User32Hook::Detach() noexcept {
    if (True_CreateWindowExA) ::DetourDetach(&(PVOID&)True_CreateWindowExA, reinterpret_cast<PVOID>(Hook_CreateWindowExA));
    if (True_SetWindowTextA) ::DetourDetach(&(PVOID&)True_SetWindowTextA, reinterpret_cast<PVOID>(Hook_SetWindowTextA));
    if (True_GetWindowTextA) ::DetourDetach(&(PVOID&)True_GetWindowTextA, reinterpret_cast<PVOID>(Hook_GetWindowTextA));
    if (True_MessageBoxA) ::DetourDetach(&(PVOID&)True_MessageBoxA, reinterpret_cast<PVOID>(Hook_MessageBoxA));
    if (True_MessageBoxTimeoutA) ::DetourDetach(&(PVOID&)True_MessageBoxTimeoutA, reinterpret_cast<PVOID>(Hook_MessageBoxTimeoutA));
}

} // namespace ler::hooks
