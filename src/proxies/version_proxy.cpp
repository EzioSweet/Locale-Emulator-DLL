#include "proxies/version_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealVersion = nullptr;

void InitProxy() noexcept {
    if (!g_hRealVersion) {
        g_hRealVersion = ler::proxies::ProxyHelper::LoadSystemLibrary(L"version.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealVersion) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealVersion, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(BOOL, GetFileVersionInfoA, (LPCSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData), (lptstrFilename, dwHandle, dwLen, lpData))
FORWARD_CALL(BOOL, GetFileVersionInfoW, (LPCWSTR lptstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData), (lptstrFilename, dwHandle, dwLen, lpData))
FORWARD_CALL(DWORD, GetFileVersionInfoSizeA, (LPCSTR lptstrFilename, LPDWORD lpdwHandle), (lptstrFilename, lpdwHandle))
FORWARD_CALL(DWORD, GetFileVersionInfoSizeW, (LPCWSTR lptstrFilename, LPDWORD lpdwHandle), (lptstrFilename, lpdwHandle))
FORWARD_CALL(BOOL, VerQueryValueA, (LPCVOID pBlock, LPCSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen), (pBlock, lpSubBlock, lplpBuffer, puLen))
FORWARD_CALL(BOOL, VerQueryValueW, (LPCVOID pBlock, LPCWSTR lpSubBlock, LPVOID *lplpBuffer, PUINT puLen), (pBlock, lpSubBlock, lplpBuffer, puLen))
FORWARD_CALL(DWORD, VerLanguageNameA, (DWORD wLang, LPSTR szLang, DWORD cchLang), (wLang, szLang, cchLang))
FORWARD_CALL(DWORD, VerLanguageNameW, (DWORD wLang, LPWSTR szLang, DWORD cchLang), (wLang, szLang, cchLang))
FORWARD_CALL(DWORD, VerFindFileA, (DWORD uFlags, LPCSTR szFileName, LPCSTR szWinDir, LPCSTR szAppDir, LPSTR szCurDir, PUINT lpuCurDirLen, LPSTR szDestDir, PUINT lpuDestDirLen), (uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen))
FORWARD_CALL(DWORD, VerFindFileW, (DWORD uFlags, LPCWSTR szFileName, LPCWSTR szWinDir, LPCWSTR szAppDir, LPWSTR szCurDir, PUINT lpuCurDirLen, LPWSTR szDestDir, PUINT lpuDestDirLen), (uFlags, szFileName, szWinDir, szAppDir, szCurDir, lpuCurDirLen, szDestDir, lpuDestDirLen))
FORWARD_CALL(DWORD, VerInstallFileA, (DWORD uFlags, LPCSTR szSrcFileName, LPCSTR szDestFileName, LPCSTR szSrcDir, LPCSTR szDestDir, LPCSTR szCurDir, LPSTR szTmpFile, PUINT lpuTmpFileLen), (uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen))
FORWARD_CALL(DWORD, VerInstallFileW, (DWORD uFlags, LPCWSTR szSrcFileName, LPCWSTR szDestFileName, LPCWSTR szSrcDir, LPCWSTR szDestDir, LPCWSTR szCurDir, LPWSTR szTmpFile, PUINT lpuTmpFileLen), (uFlags, szSrcFileName, szDestFileName, szSrcDir, szDestDir, szCurDir, szTmpFile, lpuTmpFileLen))
FORWARD_CALL(BOOL, GetFileVersionInfoExA, (DWORD dwFlags, LPCSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData), (dwFlags, lpwstrFilename, dwHandle, dwLen, lpData))
FORWARD_CALL(BOOL, GetFileVersionInfoExW, (DWORD dwFlags, LPCWSTR lpwstrFilename, DWORD dwHandle, DWORD dwLen, LPVOID lpData), (dwFlags, lpwstrFilename, dwHandle, dwLen, lpData))
FORWARD_CALL(DWORD, GetFileVersionInfoSizeExA, (DWORD dwFlags, LPCSTR lpwstrFilename, LPDWORD lpdwHandle), (dwFlags, lpwstrFilename, lpdwHandle))
FORWARD_CALL(DWORD, GetFileVersionInfoSizeExW, (DWORD dwFlags, LPCWSTR lpwstrFilename, LPDWORD lpdwHandle), (dwFlags, lpwstrFilename, lpdwHandle))
FORWARD_CALL(int, GetFileVersionInfoByHandle, (int unknown1, HANDLE handle, int unknown2), (unknown1, handle, unknown2))

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealVersion) {
                ::FreeLibrary(g_hRealVersion);
                g_hRealVersion = nullptr;
            }
            break;
    }
    return TRUE;
}
