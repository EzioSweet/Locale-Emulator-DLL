#pragma once

#include <windows.h>

extern "C" {
HRESULT WINAPI Proxy_DirectSoundCreate(LPCGUID pcGuidDevice, LPVOID *ppDS, LPUNKNOWN pUnkOuter);
HRESULT WINAPI Proxy_DirectSoundEnumerateA(LPVOID pDSEnumCallback, LPVOID pContext);
HRESULT WINAPI Proxy_DirectSoundEnumerateW(LPVOID pDSEnumCallback, LPVOID pContext);
HRESULT WINAPI Proxy_DllCanUnloadNow(void);
HRESULT WINAPI Proxy_DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID *ppv);
HRESULT WINAPI Proxy_DirectSoundCaptureCreate(LPCGUID pcGuidDevice, LPVOID *ppDSC, LPUNKNOWN pUnkOuter);
HRESULT WINAPI Proxy_DirectSoundCaptureEnumerateA(LPVOID pDSEnumCallback, LPVOID pContext);
HRESULT WINAPI Proxy_DirectSoundCaptureEnumerateW(LPVOID pDSEnumCallback, LPVOID pContext);
HRESULT WINAPI Proxy_GetDeviceID(LPCGUID pGuidSrc, LPGUID pGuidDest);
HRESULT WINAPI Proxy_DirectSoundFullDuplexCreate(LPCGUID pcGuidCaptureDevice, LPCGUID pcGuidRenderDevice, LPVOID pcDSCBufferDesc, LPVOID pcDSBufferDesc, HWND hWnd, DWORD dwLevel, LPVOID *ppDSFD, LPVOID *ppDSCBuffer8, LPVOID *ppDSBuffer8, LPUNKNOWN pUnkOuter);
HRESULT WINAPI Proxy_DirectSoundCreate8(LPCGUID pcGuidDevice, LPVOID *ppDS8, LPUNKNOWN pUnkOuter);
HRESULT WINAPI Proxy_DirectSoundCaptureCreate8(LPCGUID pcGuidDevice, LPVOID *ppDSC8, LPUNKNOWN pUnkOuter);
}
