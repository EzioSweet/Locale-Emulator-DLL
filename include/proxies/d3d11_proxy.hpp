#pragma once

#include <windows.h>
#include <unknwn.h>

extern "C" {
HRESULT WINAPI Proxy_D3D11CreateDevice(PVOID pAdapter, int DriverType, HMODULE Software, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, PVOID *ppDevice, int *pFeatureLevel, PVOID *ppImmediateContext);
HRESULT WINAPI Proxy_D3D11CreateDeviceAndSwapChain(PVOID pAdapter, int DriverType, HMODULE Software, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, UINT SDKVersion, const PVOID pSwapChainDesc, PVOID *ppSwapChain, PVOID *ppDevice, int *pFeatureLevel, PVOID *ppImmediateContext);
HRESULT WINAPI Proxy_D3D11On12CreateDevice(IUnknown *pDevice, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, IUnknown * const *ppCommandQueues, UINT NumQueues, UINT NodeMask, PVOID *ppDevice, PVOID *ppImmediateContext, int *pChosenFeatureLevel);
HRESULT WINAPI Proxy_D3D11CreateDeviceForD3D12(IUnknown *pDevice, UINT Flags, const int *pFeatureLevels, UINT FeatureLevels, IUnknown * const *ppCommandQueues, UINT NumQueues, UINT NodeMask, PVOID *ppDevice, PVOID *ppImmediateContext, int *pChosenFeatureLevel);
HRESULT WINAPI Proxy_D3D11CoreCreateDevice(PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5);
HRESULT WINAPI Proxy_D3D11CoreCreateLayeredDevice(PVOID p1, PVOID p2, PVOID p3, PVOID p4, PVOID p5);
SIZE_T WINAPI Proxy_D3D11CoreGetLayeredDeviceSize(PVOID p1, PVOID p2);
HRESULT WINAPI Proxy_D3D11CoreRegisterLayers(PVOID p1, PVOID p2);
int WINAPI Proxy_D3DPerformance_BeginEvent(DWORD col, LPCWSTR wszName);
int WINAPI Proxy_D3DPerformance_EndEvent(void);
DWORD WINAPI Proxy_D3DPerformance_GetStatus(void);
void WINAPI Proxy_D3DPerformance_SetMarker(DWORD col, LPCWSTR wszName);
HRESULT WINAPI Proxy_CreateDirect3D11DeviceFromDXGIDevice(PVOID dxgiDevice, IUnknown **graphicsDevice);
HRESULT WINAPI Proxy_CreateDirect3D11SurfaceFromDXGISurface(PVOID dgxiSurface, IUnknown **graphicsSurface);
}
