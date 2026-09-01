#include "hook_manager.h"
#include "../../common/log.h"

BOOL LerInstallAllHooks(void) {
    DetourTransactionBegin();

    LerHookKernel32();
    LerHookGdi32();
    LerHookUser32();
    LerHookAdvapi32();
    LerHookNtdll();
    LerHookProcess();

    LONG status = DetourTransactionCommit();
    if (status != NO_ERROR) {
        return FALSE;
    }

    return TRUE;
}

BOOL LerUninstallAllHooks(void) {
    DetourTransactionBegin();

    LerUnhookKernel32();
    LerUnhookGdi32();
    LerUnhookUser32();
    LerUnhookAdvapi32();
    LerUnhookNtdll();
    LerUnhookProcess();

    LONG status = DetourTransactionCommit();
    if (status != NO_ERROR) {
        return FALSE;
    }

    return TRUE;
}
