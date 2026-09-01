#pragma once

#include <windows.h>

extern "C" {
PVOID WINAPI Proxy_Direct3DCreate9(UINT SDKVersion);
HRESULT WINAPI Proxy_Direct3DCreate9Ex(UINT SDKVersion, PVOID *ppD3D);
PVOID WINAPI Proxy_Direct3DCreate9On12(UINT SDKVersion, PVOID pOverrideList, UINT NumOverrideEntries);
HRESULT WINAPI Proxy_Direct3DCreate9On12Ex(UINT SDKVersion, PVOID pOverrideList, UINT NumOverrideEntries, PVOID *ppD3D);
PVOID WINAPI Proxy_Direct3DShaderValidatorCreate9(void);
int WINAPI Proxy_D3DPERF_BeginEvent(DWORD col, LPCWSTR wszName);
int WINAPI Proxy_D3DPERF_EndEvent(void);
DWORD WINAPI Proxy_D3DPERF_GetStatus(void);
BOOL WINAPI Proxy_D3DPERF_QueryRepeatFrame(void);
void WINAPI Proxy_D3DPERF_SetMarker(DWORD col, LPCWSTR wszName);
void WINAPI Proxy_D3DPERF_SetOptions(DWORD dwOptions);
void WINAPI Proxy_D3DPERF_SetRegion(DWORD col, LPCWSTR wszName);
void WINAPI Proxy_DebugSetLevel(DWORD dw1);
void WINAPI Proxy_DebugSetMute(void);
int WINAPI Proxy_Direct3D9EnableMaximizedWindowedModeShim(UINT u1);
void WINAPI Proxy_PSGPError(PVOID p1, PVOID p2);
void WINAPI Proxy_PSGPSampleTexture(PVOID p1, UINT u2, PVOID p3);
}
