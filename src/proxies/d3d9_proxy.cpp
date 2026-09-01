#include "proxies/d3d9_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealD3d9 = nullptr;

void InitProxy() noexcept {
    if (!g_hRealD3d9) {
        g_hRealD3d9 = ler::proxies::ProxyHelper::LoadSystemLibrary(L"d3d9.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealD3d9) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealD3d9, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(PVOID, Direct3DCreate9, (UINT SDKVersion), (SDKVersion))
FORWARD_CALL(HRESULT, Direct3DCreate9Ex, (UINT SDKVersion, PVOID *ppD3D), (SDKVersion, ppD3D))
FORWARD_CALL(PVOID, Direct3DCreate9On12, (UINT SDKVersion, PVOID pOverrideList, UINT NumOverrideEntries), (SDKVersion, pOverrideList, NumOverrideEntries))
FORWARD_CALL(HRESULT, Direct3DCreate9On12Ex, (UINT SDKVersion, PVOID pOverrideList, UINT NumOverrideEntries, PVOID *ppD3D), (SDKVersion, pOverrideList, NumOverrideEntries, ppD3D))
FORWARD_CALL(PVOID, Direct3DShaderValidatorCreate9, (void), ())
FORWARD_CALL(int, D3DPERF_BeginEvent, (DWORD col, LPCWSTR wszName), (col, wszName))
FORWARD_CALL(int, D3DPERF_EndEvent, (void), ())
FORWARD_CALL(DWORD, D3DPERF_GetStatus, (void), ())
FORWARD_CALL(BOOL, D3DPERF_QueryRepeatFrame, (void), ())
FORWARD_CALL(void, D3DPERF_SetMarker, (DWORD col, LPCWSTR wszName), (col, wszName))
FORWARD_CALL(void, D3DPERF_SetOptions, (DWORD dwOptions), (dwOptions))
FORWARD_CALL(void, D3DPERF_SetRegion, (DWORD col, LPCWSTR wszName), (col, wszName))
FORWARD_CALL(void, DebugSetLevel, (DWORD dw1), (dw1))
FORWARD_CALL(void, DebugSetMute, (void), ())
FORWARD_CALL(int, Direct3D9EnableMaximizedWindowedModeShim, (UINT u1), (u1))
FORWARD_CALL(void, PSGPError, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(void, PSGPSampleTexture, (PVOID p1, UINT u2, PVOID p3), (p1, u2, p3))

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealD3d9) {
                ::FreeLibrary(g_hRealD3d9);
                g_hRealD3d9 = nullptr;
            }
            break;
    }
    return TRUE;
}
