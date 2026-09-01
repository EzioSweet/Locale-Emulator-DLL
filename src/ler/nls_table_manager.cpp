#include "ler/nls_table_manager.hpp"
#include "common/log.hpp"
#include <array>
#include <algorithm>

namespace ler {

NlsTableManager& NlsTableManager::Instance() noexcept {
    static NlsTableManager instance;
    return instance;
}

std::vector<uint8_t> NlsTableManager::LoadNlsFile(std::wstring_view fileName) noexcept {
    std::array<wchar_t, MAX_PATH> winDir{};
    if (::GetWindowsDirectoryW(winDir.data(), MAX_PATH) == 0) {
        return {};
    }

    std::wstring path;
    common::UniqueHandle hFile;

    // 1. Try Sysnative for 64-bit Windows
    path = std::wstring(winDir.data()) + L"\\Sysnative\\" + std::wstring(fileName);
    hFile.reset(::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));

    // 2. Try disabling WOW64 redirection and opening from System32
    if (!hFile.is_valid()) {
        common::Wow64FsGuard guard;
        path = std::wstring(winDir.data()) + L"\\System32\\" + std::wstring(fileName);
        hFile.reset(::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    }

    // 3. Fallback standard System32
    if (!hFile.is_valid()) {
        path = std::wstring(winDir.data()) + L"\\System32\\" + std::wstring(fileName);
        hFile.reset(::CreateFileW(path.c_str(), GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr));
    }

    if (!hFile.is_valid()) {
        return {};
    }

    DWORD fileSize = ::GetFileSize(hFile.get(), nullptr);
    if (fileSize == INVALID_FILE_SIZE || fileSize == 0) {
        return {};
    }

    std::vector<uint8_t> buffer(fileSize);
    DWORD bytesRead = 0;
    if (!::ReadFile(hFile.get(), buffer.data(), fileSize, &bytesRead, nullptr) || bytesRead != fileSize) {
        return {};
    }

    return buffer;
}

constexpr size_t AlignUp(size_t val, size_t align) noexcept {
    return (val + (align - 1)) & ~(align - 1);
}

bool NlsTableManager::Initialize(UINT ansiCodePage, UINT oemCodePage) noexcept {
    Reset();

    HMODULE hNtdll = ::GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        return false;
    }

    auto RtlInitNlsTables_fn = reinterpret_cast<common::pfnRtlInitNlsTables>(::GetProcAddress(hNtdll, "RtlInitNlsTables"));
    auto RtlResetRtlTranslations_fn = reinterpret_cast<common::pfnRtlResetRtlTranslations>(::GetProcAddress(hNtdll, "RtlResetRtlTranslations"));

    if (!RtlInitNlsTables_fn || !RtlResetRtlTranslations_fn) {
        return false;
    }

    std::wstring ansiFileName = L"c_" + std::to_wstring(ansiCodePage) + L".nls";
    std::wstring oemFileName = L"c_" + std::to_wstring(oemCodePage) + L".nls";

    auto ansiData = LoadNlsFile(ansiFileName);
    auto oemData = (ansiCodePage == oemCodePage) ? ansiData : LoadNlsFile(oemFileName);
    auto langData = LoadNlsFile(L"l_intl.nls");

    if (ansiData.empty() || oemData.empty() || langData.empty()) {
        return false;
    }

    size_t ansiOffset = 0;
    size_t oemOffset = AlignUp(ansiData.size(), 16);
    size_t unicodeCaseOffset = oemOffset + AlignUp(oemData.size(), 16);
    size_t totalSize = unicodeCaseOffset + AlignUp(langData.size(), 16);

    auto pCombinedNls = reinterpret_cast<PBYTE>(::VirtualAlloc(nullptr, totalSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!pCombinedNls) {
        return false;
    }

    std::copy(ansiData.begin(), ansiData.end(), pCombinedNls + ansiOffset);
    std::copy(oemData.begin(), oemData.end(), pCombinedNls + oemOffset);
    std::copy(langData.begin(), langData.end(), pCombinedNls + unicodeCaseOffset);

    DWORD oldProtect = 0;
    ::VirtualProtect(pCombinedNls, totalSize, PAGE_READONLY, &oldProtect);

    common::NLSTABLEINFO nlsTableInfo{};
    RtlInitNlsTables_fn(
        reinterpret_cast<PUSHORT>(pCombinedNls + ansiOffset),
        reinterpret_cast<PUSHORT>(pCombinedNls + oemOffset),
        reinterpret_cast<PUSHORT>(pCombinedNls + unicodeCaseOffset),
        &nlsTableInfo
    );

    RtlResetRtlTranslations_fn(&nlsTableInfo);

    auto pPeb = reinterpret_cast<PBYTE>(common::GetCurrentPeb());
    if (pPeb) {
        *reinterpret_cast<PVOID*>(pPeb + 0x58) = (pCombinedNls + ansiOffset);
        *reinterpret_cast<PVOID*>(pPeb + 0x5c) = (pCombinedNls + oemOffset);
        *reinterpret_cast<PVOID*>(pPeb + 0x60) = (pCombinedNls + unicodeCaseOffset);
    }

    mapped_memory_ = pCombinedNls;
    mapped_size_ = totalSize;
    return true;
}

void NlsTableManager::Reset() noexcept {
    if (mapped_memory_) {
        ::VirtualFree(mapped_memory_, 0, MEM_RELEASE);
        mapped_memory_ = nullptr;
        mapped_size_ = 0;
    }
}

} // namespace ler
