#include "ler/hooks/process_hook.hpp"
#include "common/config.hpp"
#include "common/log.hpp"
#include "detours/detours.hpp"

namespace ler::hooks {

namespace {

using pfnCreateProcessA = BOOL(WINAPI*)(
    LPCSTR, LPSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL, DWORD, LPVOID, LPCSTR, LPSTARTUPINFOA, LPPROCESS_INFORMATION);

using pfnCreateProcessW = BOOL(WINAPI*)(
    LPCWSTR, LPWSTR, LPSECURITY_ATTRIBUTES, LPSECURITY_ATTRIBUTES,
    BOOL, DWORD, LPVOID, LPCWSTR, LPSTARTUPINFOW, LPPROCESS_INFORMATION);

pfnCreateProcessA True_CreateProcessA = nullptr;
pfnCreateProcessW True_CreateProcessW = nullptr;

BOOL WINAPI Hook_CreateProcessA(
    LPCSTR lpApplicationName,
    LPSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCSTR lpCurrentDirectory,
    LPSTARTUPINFOA lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation) {

    return True_CreateProcessA ? True_CreateProcessA(
        lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags,
        lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation) : FALSE;
}

BOOL WINAPI Hook_CreateProcessW(
    LPCWSTR lpApplicationName,
    LPWSTR lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL bInheritHandles,
    DWORD dwCreationFlags,
    LPVOID lpEnvironment,
    LPCWSTR lpCurrentDirectory,
    LPSTARTUPINFOW lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation) {

    return True_CreateProcessW ? True_CreateProcessW(
        lpApplicationName, lpCommandLine, lpProcessAttributes,
        lpThreadAttributes, bInheritHandles, dwCreationFlags,
        lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation) : FALSE;
}

} // namespace

void ProcessHook::Attach() noexcept {
    HMODULE hK32 = ::GetModuleHandleW(L"kernel32.dll");
    if (hK32) {
        True_CreateProcessA = reinterpret_cast<pfnCreateProcessA>(::GetProcAddress(hK32, "CreateProcessA"));
        True_CreateProcessW = reinterpret_cast<pfnCreateProcessW>(::GetProcAddress(hK32, "CreateProcessW"));
    }

    if (True_CreateProcessA) ::DetourAttach(&(PVOID&)True_CreateProcessA, reinterpret_cast<PVOID>(Hook_CreateProcessA));
    if (True_CreateProcessW) ::DetourAttach(&(PVOID&)True_CreateProcessW, reinterpret_cast<PVOID>(Hook_CreateProcessW));
}

void ProcessHook::Detach() noexcept {
    if (True_CreateProcessA) ::DetourDetach(&(PVOID&)True_CreateProcessA, reinterpret_cast<PVOID>(Hook_CreateProcessA));
    if (True_CreateProcessW) ::DetourDetach(&(PVOID&)True_CreateProcessW, reinterpret_cast<PVOID>(Hook_CreateProcessW));
}

} // namespace ler::hooks
