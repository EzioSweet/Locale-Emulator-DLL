#pragma once

#include <windows.h>

extern "C" {
HRESULT WINAPI Proxy_CreateDXGIFactory(REFIID riid, void **ppFactory);
HRESULT WINAPI Proxy_CreateDXGIFactory1(REFIID riid, void **ppFactory);
HRESULT WINAPI Proxy_CreateDXGIFactory2(UINT Flags, REFIID riid, void **ppFactory);
HRESULT WINAPI Proxy_DXGIDumpJournal(PVOID p1);
HRESULT WINAPI Proxy_DXGID3D10CreateDevice(PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5, PVOID p6);
HRESULT WINAPI Proxy_DXGID3D10CreateLayeredDevice(PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5);
SIZE_T WINAPI Proxy_DXGID3D10GetLayeredDeviceSize(PVOID p1, PVOID p2);
HRESULT WINAPI Proxy_DXGID3D10RegisterLayers(PVOID p1, PVOID p2);
HRESULT WINAPI Proxy_DXGIDeclareAdapterRemovalSupport(void);
HRESULT WINAPI Proxy_DXGIGetDebugInterface1(UINT Flags, REFIID riid, void **pDebug);
HRESULT WINAPI Proxy_DXGIReportAdapterConfiguration(PVOID p1);
HRESULT WINAPI Proxy_PIXBeginCapture(PVOID p1, PVOID p2);
HRESULT WINAPI Proxy_PIXEndCapture(PVOID p1);
DWORD WINAPI Proxy_PIXGetCaptureState(void);
HRESULT WINAPI Proxy_ApplyCompatResolutionQuirking(PVOID p1, PVOID p2);
HRESULT WINAPI Proxy_CompatString(PVOID p1, PVOID p2, PVOID p3, PVOID p4);
HRESULT WINAPI Proxy_CompatValue(PVOID p1, PVOID p2);
HRESULT WINAPI Proxy_SetAppCompatStringPointer(PVOID p1, PVOID p2);
HRESULT WINAPI Proxy_UpdateHMDEmulationStatus(PVOID p1);
}
