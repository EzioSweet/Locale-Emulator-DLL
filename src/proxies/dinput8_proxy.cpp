#include "proxies/dinput8_proxy.hpp"
#include "proxies/proxy_helper.hpp"

namespace {

HMODULE g_hRealDinput8 = nullptr;

void InitProxy() noexcept {
    if (!g_hRealDinput8) {
        g_hRealDinput8 = ler::proxies::ProxyHelper::LoadSystemLibrary(L"dinput8.dll");
    }
}

#define FORWARD_CALL(ret_type, name, args_decl, args_pass) \
    typedef ret_type (WINAPI *pfn_##name) args_decl; \
    static pfn_##name Orig_##name = nullptr; \
    extern "C" ret_type WINAPI Proxy_##name args_decl { \
        InitProxy(); \
        if (!Orig_##name && g_hRealDinput8) { \
            Orig_##name = reinterpret_cast<pfn_##name>(ler::proxies::ProxyHelper::GetProc(g_hRealDinput8, #name)); \
        } \
        if (Orig_##name) return Orig_##name args_pass; \
        return (ret_type)0; \
    }

FORWARD_CALL(HRESULT, DirectInput8Create, (HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter), (hinst, dwVersion, riidltf, ppvOut, punkOuter))
FORWARD_CALL(HRESULT, DllCanUnloadNow, (void), ())
FORWARD_CALL(HRESULT, DllGetClassObject, (REFCLSID rclsid, REFIID riid, LPVOID *ppv), (rclsid, riid, ppv))
FORWARD_CALL(HRESULT, DllRegisterServer, (void), ())
FORWARD_CALL(HRESULT, DllUnregisterServer, (void), ())
FORWARD_CALL(const void *, GetdfDIJoystick, (void), ())

} // namespace

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitProxy();
            ler::proxies::ProxyHelper::LoadLerRuntime(hinstDLL);
            break;
        case DLL_PROCESS_DETACH:
            if (g_hRealDinput8) {
                ::FreeLibrary(g_hRealDinput8);
                g_hRealDinput8 = nullptr;
            }
            break;
    }
    return TRUE;
}
