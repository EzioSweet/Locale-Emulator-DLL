#pragma once

#include <windows.h>
#include <unknwn.h>

extern "C" {
HRESULT WINAPI Proxy_DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID *ppvOut, LPUNKNOWN punkOuter);
HRESULT WINAPI Proxy_DllCanUnloadNow(void);
HRESULT WINAPI Proxy_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv);
HRESULT WINAPI Proxy_DllRegisterServer(void);
HRESULT WINAPI Proxy_DllUnregisterServer(void);
const void * WINAPI Proxy_GetdfDIJoystick(void);
}
