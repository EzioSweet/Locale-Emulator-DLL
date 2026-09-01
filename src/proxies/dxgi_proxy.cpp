#include "proxies/dxgi_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealDxgi = nullptr;

void InitProxy() noexcept {
    if (!g_hRealDxgi) {
        g_hRealDxgi = ler::proxies::ProxyHelper::LoadSystemLibrary(L"dxgi.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealDxgi) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealDxgi, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(HRESULT, CreateDXGIFactory, (REFIID riid, void **ppFactory), (riid, ppFactory))
FORWARD_CALL(HRESULT, CreateDXGIFactory1, (REFIID riid, void **ppFactory), (riid, ppFactory))
FORWARD_CALL(HRESULT, CreateDXGIFactory2, (UINT Flags, REFIID riid, void **ppFactory), (Flags, riid, ppFactory))
FORWARD_CALL(HRESULT, DXGIDumpJournal, (PVOID p1), (p1))
FORWARD_CALL(HRESULT, DXGID3D10CreateDevice, (PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5, PVOID p6), (p1, p2, p3, p4, p5, p6))
FORWARD_CALL(HRESULT, DXGID3D10CreateLayeredDevice, (PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5), (p1, p2, p3, p4, p5))
FORWARD_CALL(SIZE_T, DXGID3D10GetLayeredDeviceSize, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(HRESULT, DXGID3D10RegisterLayers, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(HRESULT, DXGIDeclareAdapterRemovalSupport, (void), ())
FORWARD_CALL(HRESULT, DXGIGetDebugInterface1, (UINT Flags, REFIID riid, void **pDebug), (Flags, riid, pDebug))
FORWARD_CALL(HRESULT, DXGIReportAdapterConfiguration, (PVOID p1), (p1))
FORWARD_CALL(HRESULT, PIXBeginCapture, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(HRESULT, PIXEndCapture, (PVOID p1), (p1))
FORWARD_CALL(DWORD, PIXGetCaptureState, (void), ())
FORWARD_CALL(HRESULT, ApplyCompatResolutionQuirking, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(HRESULT, CompatString, (PVOID p1, PVOID p2, PVOID p3, PVOID p4), (p1, p2, p3, p4))
FORWARD_CALL(HRESULT, CompatValue, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(HRESULT, SetAppCompatStringPointer, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(HRESULT, UpdateHMDEmulationStatus, (PVOID p1), (p1))

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealDxgi) {
                ::FreeLibrary(g_hRealDxgi);
                g_hRealDxgi = nullptr;
            }
            break;
    }
    return TRUE;
}
