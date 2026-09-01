#include "proxies/dsound_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealDsound = nullptr;

void InitProxy() noexcept {
    if (!g_hRealDsound) {
        g_hRealDsound = ler::proxies::ProxyHelper::LoadSystemLibrary(L"dsound.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealDsound) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealDsound, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(HRESULT, DirectSoundCreate, (LPCGUID pcGuidDevice, LPVOID *ppDS, LPUNKNOWN pUnkOuter), (pcGuidDevice, ppDS, pUnkOuter))
FORWARD_CALL(HRESULT, DirectSoundEnumerateA, (LPVOID pDSEnumCallback, LPVOID pContext), (pDSEnumCallback, pContext))
FORWARD_CALL(HRESULT, DirectSoundEnumerateW, (LPVOID pDSEnumCallback, LPVOID pContext), (pDSEnumCallback, pContext))
FORWARD_CALL(HRESULT, DllCanUnloadNow, (void), ())
FORWARD_CALL(HRESULT, DllGetClassObject, (REFCLSID rclsid, REFIID riid, LPVOID *ppv), (rclsid, riid, ppv))
FORWARD_CALL(HRESULT, DirectSoundCaptureCreate, (LPCGUID pcGuidDevice, LPVOID *ppDSC, LPUNKNOWN pUnkOuter), (pcGuidDevice, ppDSC, pUnkOuter))
FORWARD_CALL(HRESULT, DirectSoundCaptureEnumerateA, (LPVOID pDSEnumCallback, LPVOID pContext), (pDSEnumCallback, pContext))
FORWARD_CALL(HRESULT, DirectSoundCaptureEnumerateW, (LPVOID pDSEnumCallback, LPVOID pContext), (pDSEnumCallback, pContext))
FORWARD_CALL(HRESULT, GetDeviceID, (LPCGUID pGuidSrc, LPGUID pGuidDest), (pGuidSrc, pGuidDest))
FORWARD_CALL(HRESULT, DirectSoundFullDuplexCreate, (LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice, LPVOID pcDSCBufferDesc, LPVOID pcDSBufferDesc, HWND hWnd, DWORD dwLevel, LPVOID *ppDSFD, LPVOID *ppDSCBuffer8, LPVOID *ppDSBuffer8, LPUNKNOWN pUnkOuter), (pcGuidCaptureDevice, pcGuidRenderDevice, pcDSCBufferDesc, pcDSBufferDesc, hWnd, dwLevel, ppDSFD, ppDSCBuffer8, ppDSBuffer8, pUnkOuter))
FORWARD_CALL(HRESULT, DirectSoundCreate8, (LPCGUID pcGuidDevice, LPVOID *ppDS8, LPUNKNOWN pUnkOuter), (pcGuidDevice, ppDS8, pUnkOuter))
FORWARD_CALL(HRESULT, DirectSoundCaptureCreate8, (LPCGUID pcGuidDevice, LPVOID *ppDSC8, LPUNKNOWN pUnkOuter), (pcGuidDevice, ppDSC8, pUnkOuter))

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealDsound) {
                ::FreeLibrary(g_hRealDsound);
                g_hRealDsound = nullptr;
            }
            break;
    }
    return TRUE;
}
