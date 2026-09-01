#pragma once

#include "common/types.hpp"
#include <windows.h>
#include <span>
#include <vector>
#include <string_view>

namespace ler {

class NlsTableManager {
public:
    NlsTableManager() noexcept = default;
    ~NlsTableManager() noexcept {
        Reset();
    }

    NlsTableManager(const NlsTableManager&) = delete;
    NlsTableManager& operator=(const NlsTableManager&) = delete;

    [[nodiscard]] static NlsTableManager& Instance() noexcept;

    bool Initialize(UINT ansiCodePage, UINT oemCodePage) noexcept;
    void Reset() noexcept;

private:
    [[nodiscard]] std::vector<uint8_t> LoadNlsFile(std::wstring_view fileName) noexcept;

    PBYTE mapped_memory_{nullptr};
    SIZE_T mapped_size_{0};
};

} // namespace ler
