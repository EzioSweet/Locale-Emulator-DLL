#pragma once

#include <windows.h>
#include <string>
#include <string_view>

namespace ler::common {

class Config {
public:
    Config() noexcept = default;

    // Singleton access
    [[nodiscard]] static Config& Instance() noexcept;

    // Load from specific INI file path, or auto-detect in module dir
    void Load(std::wstring_view iniPath = {}) noexcept;

    // Config Properties
    [[nodiscard]] UINT ansi_code_page() const noexcept { return ansi_code_page_; }
    [[nodiscard]] UINT oem_code_page() const noexcept { return oem_code_page_; }
    [[nodiscard]] LCID locale_id() const noexcept { return locale_id_; }
    [[nodiscard]] BYTE charset() const noexcept { return charset_; }
    [[nodiscard]] const std::wstring& default_font() const noexcept { return default_font_; }
    [[nodiscard]] const std::string& default_font_a() const noexcept { return default_font_a_; }
    [[nodiscard]] LONG timezone_bias() const noexcept { return timezone_bias_; }
    [[nodiscard]] const std::wstring& locale_name() const noexcept { return locale_name_; }
    [[nodiscard]] bool hook_ui_language() const noexcept { return hook_ui_language_; }
    [[nodiscard]] bool fix_window_text() const noexcept { return fix_window_text_; }
    [[nodiscard]] bool fix_font_charset() const noexcept { return fix_font_charset_; }
    [[nodiscard]] bool debug_log() const noexcept { return debug_log_; }

private:
    UINT ansi_code_page_{932};
    UINT oem_code_page_{932};
    LCID locale_id_{0x0411};
    BYTE charset_{128};
    std::wstring default_font_{L"MS Gothic"};
    std::string default_font_a_{"MS Gothic"};
    LONG timezone_bias_{-540};
    std::wstring locale_name_{L"ja-JP"};
    bool hook_ui_language_{true};
    bool fix_window_text_{true};
    bool fix_font_charset_{true};
    bool debug_log_{false};
};

} // namespace ler::common
