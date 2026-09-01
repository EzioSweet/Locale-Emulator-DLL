#include "ler/nls_table_manager.hpp"
#include "ler/hook_manager.hpp"
#include "common/config.hpp"
#include "common/log.hpp"

extern "C" {

__declspec(dllexport) BOOL WINAPI InitLocaleEmulator(void) {
    auto& cfg = ler::common::Config::Instance();
    cfg.Load();

    ler::NlsTableManager::Instance().Initialize(cfg.ansi_code_page(), cfg.oem_code_page());
    return ler::HookManager::Instance().InstallAll() ? TRUE : FALSE;
}

__declspec(dllexport) BOOL WINAPI UninitLocaleEmulator(void) {
    ler::HookManager::Instance().UninstallAll();
    ler::NlsTableManager::Instance().Reset();
    return TRUE;
}

} // extern "C"

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    switch (fdwReason) {
        case DLL_PROCESS_ATTACH:
            ::DisableThreadLibraryCalls(hinstDLL);
            InitLocaleEmulator();
            break;
        case DLL_PROCESS_DETACH:
            UninitLocaleEmulator();
            break;
    }
    return TRUE;
}
