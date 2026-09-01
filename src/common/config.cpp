#include "common/config.hpp"
#include <array>
#include <vector>

namespace ler::common {

Config& Config::Instance() noexcept {
    static Config instance;
    return instance;
}

void Config::Load(std::wstring_view iniPath) noexcept {
    std::wstring path;
    if (!iniPath.empty()) {
        path = iniPath;
    } else {
        std::array<wchar_t, MAX_PATH> modulePath{};
        if (::GetModuleFileNameW(nullptr, modulePath.data(), MAX_PATH)) {
            wchar_t* pSlash = wcsrchr(modulePath.data(), L'\\');
            if (pSlash) {
                *(pSlash + 1) = L'\0';
                path = modulePath.data();
                path += L"ler.ini";
            }
        }
    }

    if (path.empty() || ::GetFileAttributesW(path.c_str()) == INVALID_FILE_ATTRIBUTES) {
        return;
    }

    ansi_code_page_ = static_cast<UINT>(::GetPrivateProfileIntW(L"Locale", L"AnsiCodePage", 932, path.c_str()));
    oem_code_page_ = static_cast<UINT>(::GetPrivateProfileIntW(L"Locale", L"OemCodePage", 932, path.c_str()));
    locale_id_ = static_cast<LCID>(::GetPrivateProfileIntW(L"Locale", L"LocaleID", 1041, path.c_str()));
    charset_ = static_cast<BYTE>(::GetPrivateProfileIntW(L"Locale", L"Charset", 128, path.c_str()));
    timezone_bias_ = static_cast<LONG>(::GetPrivateProfileIntW(L"Locale", L"TimezoneBias", -540, path.c_str()));
    hook_ui_language_ = (::GetPrivateProfileIntW(L"Locale", L"HookUILanguage", 1, path.c_str()) != 0);
    fix_window_text_ = (::GetPrivateProfileIntW(L"Locale", L"FixWindowText", 1, path.c_str()) != 0);
    fix_font_charset_ = (::GetPrivateProfileIntW(L"Locale", L"FixFontCharset", 1, path.c_str()) != 0);
    debug_log_ = (::GetPrivateProfileIntW(L"Locale", L"DebugLog", 0, path.c_str()) != 0);

    std::array<wchar_t, 64> fontBuf{};
    if (::GetPrivateProfileStringW(L"Locale", L"DefaultFont", L"MS Gothic", fontBuf.data(), static_cast<DWORD>(fontBuf.size()), path.c_str()) > 0) {
        default_font_ = fontBuf.data();
        
        int len = ::WideCharToMultiByte(ansi_code_page_, 0, default_font_.c_str(), -1, nullptr, 0, nullptr, nullptr);
        if (len > 0) {
            std::vector<char> ansiBuf(len);
            ::WideCharToMultiByte(ansi_code_page_, 0, default_font_.c_str(), -1, ansiBuf.data(), len, nullptr, nullptr);
            default_font_a_ = ansiBuf.data();
        }
    }
}

} // namespace ler::common
