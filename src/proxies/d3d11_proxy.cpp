#include "proxies/d3d11_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealD3d11 = nullptr;

void InitProxy() noexcept {
    if (!g_hRealD3d11) {
        g_hRealD3d11 = ler::proxies::ProxyHelper::LoadSystemLibrary(L"d3d11.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealD3d11) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealD3d11, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(HRESULT, D3D11CreateDevice, (PVOID pAdapter, int DriverType, HMODULE Software, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, PVOID *ppDevice, int *pFeatureLevel, PVOID *ppImmediateContext), (pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, ppDevice, pFeatureLevel, ppImmediateContext))
FORWARD_CALL(HRESULT, D3D11CreateDeviceAndSwapChain, (PVOID pAdapter, int DriverType, HMODULE Software, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const PVOID pSwapChainDesc, PVOID *ppSwapChain, PVOID *ppDevice, int *pFeatureLevel, PVOID *ppImmediateContext), (pAdapter, DriverType, Software, Flags, pFeatureLevels, FeatureLevels, SDKVersion, pSwapChainDesc, ppSwapChain, ppDevice, pFeatureLevel, ppImmediateContext))
FORWARD_CALL(HRESULT, D3D11On12CreateDevice, (IUnknown *pDevice, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, IUnknown * const *ppCommandQueues, UINT NumQueues, UINT NodeMask, PVOID *ppDevice, PVOID *ppImmediateContext, int *pChosenFeatureLevel), (pDevice, Flags, pFeatureLevels, FeatureLevels, ppCommandQueues, NumQueues, NodeMask, ppDevice, ppImmediateContext, pChosenFeatureLevel))
FORWARD_CALL(HRESULT, D3D11CreateDeviceForD3D12, (IUnknown *pDevice, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, IUnknown * const *ppCommandQueues, UINT NumQueues, UINT NodeMask, PVOID *ppDevice, PVOID *ppImmediateContext, int *pChosenFeatureLevel), (pDevice, Flags, pFeatureLevels, FeatureLevels, ppCommandQueues, NumQueues, NodeMask, ppDevice, ppImmediateContext, pChosenFeatureLevel))
FORWARD_CALL(HRESULT, D3D11CoreCreateDevice, (PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5), (p1, p2, p3, p4, p5))
FORWARD_CALL(HRESULT, D3D11CoreCreateLayeredDevice, (PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5), (p1, p2, p3, p4, p5))
FORWARD_CALL(SIZE_T, D3D11CoreGetLayeredDeviceSize, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(HRESULT, D3D11CoreRegisterLayers, (PVOID p1, PVOID p2), (p1, p2))
FORWARD_CALL(int, D3DPerformance_BeginEvent, (DWORD col, LPCWSTR wszName), (col, wszName))
FORWARD_CALL(int, D3DPerformance_EndEvent, (void), ())
FORWARD_CALL(DWORD, D3DPerformance_GetStatus, (void), ())
FORWARD_CALL(void, D3DPerformance_SetMarker, (DWORD col, LPCWSTR wszName), (col, wszName))
FORWARD_CALL(HRESULT, CreateDirect3D11DeviceFromDXGIDevice, (PVOID dxgiDevice, IUnknown **graphicsDevice), (dxgiDevice, graphicsDevice))
FORWARD_CALL(HRESULT, CreateDirect3D11SurfaceFromDXGISurface, (PVOID dgxiSurface, IUnknown **graphicsSurface), (dgxiSurface, graphicsSurface))

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealD3d11) {
                ::FreeLibrary(g_hRealD3d11);
                g_hRealD3d11 = nullptr;
            }
            break;
    }
    return TRUE;
}
