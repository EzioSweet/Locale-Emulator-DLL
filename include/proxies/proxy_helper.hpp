#pragma once

#include <windows.h>
#include <string_view>

namespace ler::proxies {

class ProxyHelper {
public:
    [[nodiscard]] static HMODULE LoadSystemLibrary(std::wstring_view dllName) noexcept;
    static void LoadLerRuntime(HMODULE hProxyModule) noexcept;
    [[nodiscard]] static PVOID GetProc(HMODULE hModule, const char* procName) noexcept;
};

} // namespace ler::proxies
