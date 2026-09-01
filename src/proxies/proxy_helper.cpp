#include "proxies/proxy_helper.hpp"
#include <array>
#include <string>

namespace ler::proxies {

HMODULE ProxyHelper::LoadSystemLibrary(std::wstring_view dllName) noexcept {
    std::array<wchar_t, MAX_PATH> sysDir{};
    if (::GetSystemDirectoryW(sysDir.data(), MAX_PATH) == 0) {
        return nullptr;
    }

    std::wstring realPath = std::wstring(sysDir.data()) + L"\\" + std::wstring(dllName);
    return ::LoadLibraryExW(realPath.c_str(), nullptr, LOAD_WITH_ALTERED_SEARCH_PATH);
}

void ProxyHelper::LoadLerRuntime(HMODULE hProxyModule) noexcept {
    std::array<wchar_t, MAX_PATH> dllPath{};
    if (::GetModuleFileNameW(hProxyModule, dllPath.data(), MAX_PATH)) {
        wchar_t* pSlash = wcsrchr(dllPath.data(), L'\\');
        if (pSlash) {
            *(pSlash + 1) = L'\0';
            std::wstring lerPath = std::wstring(dllPath.data()) + L"ler.dll";
            HMODULE hLer = ::LoadLibraryW(lerPath.c_str());
            if (hLer) return;
        }
    }
    ::LoadLibraryW(L"ler.dll");
}

PVOID ProxyHelper::GetProc(HMODULE hModule, const char* procName) noexcept {
    if (!hModule || !procName) {
        return nullptr;
    }
    return reinterpret_cast<PVOID>(::GetProcAddress(hModule, procName));
}

} // namespace ler::proxies
