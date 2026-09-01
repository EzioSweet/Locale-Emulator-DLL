#pragma once

#include <windows.h>
#include <unknwn.h>

extern "C" {
HRESULT WINAPI Proxy_DirectDrawCreate(GUID *lpGUID, PVOID *lplpDD, IUnknown *pUnkOuter);
HRESULT WINAPI Proxy_DirectDrawCreateEx(GUID *lpGUID, PVOID *lplpDD, REFIID iid, IUnknown *pUnkOuter);
HRESULT WINAPI Proxy_DirectDrawCreateClipper(DWORD dwFlags, PVOID *lplpDDClipper, IUnknown *pUnkOuter);
HRESULT WINAPI Proxy_DirectDrawEnumerateA(PVOID lpCallback, PVOID lpContext);
HRESULT WINAPI Proxy_DirectDrawEnumerateExA(PVOID lpCallback, PVOID lpContext, DWORD dwFlags);
HRESULT WINAPI Proxy_DirectDrawEnumerateW(PVOID lpCallback, PVOID lpContext);
HRESULT WINAPI Proxy_DirectDrawEnumerateExW(PVOID lpCallback, PVOID lpContext, DWORD dwFlags);
HRESULT WINAPI Proxy_DllCanUnloadNow(void);
HRESULT WINAPI Proxy_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv);
void WINAPI Proxy_AcquireDDThreadLock(void);
void WINAPI Proxy_ReleaseDDThreadLock(void);
DWORD WINAPI Proxy_CompleteCreateSysmemSurface(DWORD dw1);
HRESULT WINAPI Proxy_D3DParseUnknownCommand(LPVOID pvCommands, LPVOID *lpvUnhandledCommand);
DWORD WINAPI Proxy_DDGetAttachedSurfaceLcl(DWORD dw1, DWORD dw2, DWORD dw3);
DWORD WINAPI Proxy_DDInternalLock(DWORD dw1, DWORD dw2);
DWORD WINAPI Proxy_DDInternalUnlock(DWORD dw1);
HRESULT WINAPI Proxy_DSoundHelp(DWORD dw1, DWORD dw2, DWORD dw3);
DWORD WINAPI Proxy_GetDDSurfaceLocal(DWORD dw1, DWORD dw2, DWORD dw3);
DWORD WINAPI Proxy_GetOLEThunkData(DWORD dw1);
HRESULT WINAPI Proxy_GetSurfaceFromDC(HDC hdc, PVOID *p2, DWORD *p3);
DWORD WINAPI Proxy_RegisterSpecialCase(DWORD dw1, DWORD dw2, DWORD dw3, DWORD dw4);
HRESULT WINAPI Proxy_SetAppCompatData(DWORD dw1, DWORD dw2);
}
