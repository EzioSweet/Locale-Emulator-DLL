#include "proxies/ddraw_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealDdraw = nullptr;

void InitProxy() noexcept {
    if (!g_hRealDdraw) {
        g_hRealDdraw = ler::proxies::ProxyHelper::LoadSystemLibrary(L"ddraw.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealDdraw) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealDdraw, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(HRESULT, DirectDrawCreate, (GUID *lpGUID, PVOID *lplpDD, IUnknown *pUnkOuter), (lpGUID, lplpDD, pUnkOuter))
FORWARD_CALL(HRESULT, DirectDrawCreateEx, (GUID *lpGUID, PVOID *lplpDD, REFIID iid, IUnknown *pUnkOuter), (lpGUID, lplpDD, iid, pUnkOuter))
FORWARD_CALL(HRESULT, DirectDrawCreateClipper, (DWORD dwFlags, PVOID *lplpDDClipper, IUnknown *pUnkOuter), (dwFlags, lplpDDClipper, pUnkOuter))
FORWARD_CALL(HRESULT, DirectDrawEnumerateA, (PVOID lpCallback, PVOID lpContext), (lpCallback, lpContext))
FORWARD_CALL(HRESULT, DirectDrawEnumerateExA, (PVOID lpCallback, PVOID lpContext, DWORD dwFlags), (lpCallback, lpContext, dwFlags))
FORWARD_CALL(HRESULT, DirectDrawEnumerateW, (PVOID lpCallback, PVOID lpContext), (lpCallback, lpContext))
FORWARD_CALL(HRESULT, DirectDrawEnumerateExW, (PVOID lpCallback, PVOID lpContext, DWORD dwFlags), (lpCallback, lpContext, dwFlags))
FORWARD_CALL(HRESULT, DllCanUnloadNow, (void), ())
FORWARD_CALL(HRESULT, DllGetClassObject, (REFCLSID rclsid, REFIID riid, LPVOID *ppv), (rclsid, riid, ppv))
FORWARD_CALL(void, AcquireDDThreadLock, (void), ())
FORWARD_CALL(void, ReleaseDDThreadLock, (void), ())
FORWARD_CALL(DWORD, CompleteCreateSysmemSurface, (DWORD dw1), (dw1))
FORWARD_CALL(HRESULT, D3DParseUnknownCommand, (LPVOID pvCommands, LPVOID *lpvUnhandledCommand), (pvCommands, lpvUnhandledCommand))
FORWARD_CALL(DWORD, DDGetAttachedSurfaceLcl, (DWORD dw1, DWORD dw2, DWORD dw3), (dw1, dw2, dw3))
FORWARD_CALL(DWORD, DDInternalLock, (DWORD dw1, DWORD dw2), (dw1, dw2))
FORWARD_CALL(DWORD, DDInternalUnlock, (DWORD dw1), (dw1))
FORWARD_CALL(HRESULT, DSoundHelp, (DWORD dw1, DWORD dw2, DWORD dw3), (dw1, dw2, dw3))
FORWARD_CALL(DWORD, GetDDSurfaceLocal, (DWORD dw1, DWORD dw2, DWORD dw3), (dw1, dw2, dw3))
FORWARD_CALL(DWORD, GetOLEThunkData, (DWORD dw1), (dw1))
FORWARD_CALL(HRESULT, GetSurfaceFromDC, (HDC hdc, PVOID *p2, DWORD *p3), (hdc, p2, p3))
FORWARD_CALL(DWORD, RegisterSpecialCase, (DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4), (dw1, dw2, dw3, dw4))
FORWARD_CALL(HRESULT, SetAppCompatData, (DWORD dw1, DWORD dw2), (dw1, dw2))

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealDdraw) {
                ::FreeLibrary(g_hRealDdraw);
                g_hRealDdraw = nullptr;
            }
            break;
    }
    return TRUE;
}
