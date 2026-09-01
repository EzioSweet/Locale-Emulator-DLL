#pragma once

#include "config.hpp"
#include <windows.h>
#include <string_view>
#include <format>

namespace ler::common {

inline void LogDebug(std::wstring_view message) noexcept {
    if (Config::Instance().debug_log()) {
        ::OutputDebugStringW(L"[LocaleEmulator] ");
        ::OutputDebugStringW(std::wstring(message).c_str());
        ::OutputDebugStringW(L"\n");
    }
}

} // namespace ler::common
