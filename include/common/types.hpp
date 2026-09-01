#pragma once

#include <windows.h>
#include <string_view>
#include <memory>

namespace ler::common {

// NT Status Codes
constexpr NTSTATUS STATUS_SUCCESS = 0x00000000L;
constexpr NTSTATUS STATUS_NOT_IMPLEMENTED = ((NTSTATUS)0xC0000002L);
constexpr NTSTATUS STATUS_INFO_LENGTH_MISMATCH = ((NTSTATUS)0xC0000004L);
constexpr NTSTATUS STATUS_NOT_FOUND = ((NTSTATUS)0xC0000225L);

inline bool NT_SUCCESS(NTSTATUS status) noexcept {
    return status >= 0;
}

// Locale Info Constants
constexpr LCTYPE LOCALE_IDEFAULTANSICODEPAGE_CONST = 0x00001004;
constexpr LCTYPE LOCALE_IDEFAULTCODEPAGE_CONST = 0x0000000B;
constexpr LCTYPE LOCALE_IDEFAULTOEMCODEPAGE_CONST = 0x00000014;

// RAII Handle Wrapper
class UniqueHandle {
public:
    UniqueHandle() noexcept : handle_(INVALID_HANDLE_VALUE) {}
    explicit UniqueHandle(HANDLE handle) noexcept : handle_(handle) {}
    ~UniqueHandle() noexcept {
        reset();
    }

    UniqueHandle(const UniqueHandle&) = delete;
    UniqueHandle& operator=(const UniqueHandle&) = delete;

    UniqueHandle(UniqueHandle&& other) noexcept : handle_(other.handle_) {
        other.handle_ = INVALID_HANDLE_VALUE;
    }

    UniqueHandle& operator=(UniqueHandle&& other) noexcept {
        if (this != &other) {
            reset(other.handle_);
            other.handle_ = INVALID_HANDLE_VALUE;
        }
        return *this;
    }

    [[nodiscard]] HANDLE get() const noexcept { return handle_; }
    [[nodiscard]] bool is_valid() const noexcept {
        return handle_ != INVALID_HANDLE_VALUE && handle_ != nullptr;
    }
    explicit operator bool() const noexcept { return is_valid(); }

    void reset(HANDLE newHandle = INVALID_HANDLE_VALUE) noexcept {
        if (is_valid()) {
            ::CloseHandle(handle_);
        }
        handle_ = newHandle;
    }

    [[nodiscard]] HANDLE release() noexcept {
        HANDLE h = handle_;
        handle_ = INVALID_HANDLE_VALUE;
        return h;
    }

private:
    HANDLE handle_;
};

// RAII WOW64 File System Redirection Guard
class Wow64FsGuard {
public:
    Wow64FsGuard() noexcept {
        HMODULE hK32 = ::GetModuleHandleW(L"kernel32.dll");
        if (hK32) {
            auto pDisable = reinterpret_cast<BOOL(WINAPI*)(PVOID*)>(::GetProcAddress(hK32, "Wow64DisableWow64FsRedirection"));
            if (pDisable) {
                pDisable(&old_value_);
                disabled_ = true;
            }
        }
    }

    ~Wow64FsGuard() noexcept {
        if (disabled_) {
            HMODULE hK32 = ::GetModuleHandleW(L"kernel32.dll");
            if (hK32) {
                auto pRevert = reinterpret_cast<BOOL(WINAPI*)(PVOID)>(::GetProcAddress(hK32, "Wow64RevertWow64FsRedirection"));
                if (pRevert) {
                    pRevert(old_value_);
                }
            }
        }
    }

    Wow64FsGuard(const Wow64FsGuard&) = delete;
    Wow64FsGuard& operator=(const Wow64FsGuard&) = delete;

private:
    PVOID old_value_{nullptr};
    bool disabled_{false};
};

// Inline PEB accessor for x86
[[nodiscard]] inline PVOID GetCurrentPeb() noexcept {
#if defined(_M_IX86) || defined(__i386__)
    return reinterpret_cast<PVOID>(__readfsdword(0x30));
#else
    return nullptr;
#endif
}

// Native Time & NLS Structs
#pragma pack(push, 4)
struct TIME_FIELDS {
    SHORT Year;
    SHORT Month;
    SHORT Day;
    SHORT Hour;
    SHORT Minute;
    SHORT Second;
    SHORT Milliseconds;
    SHORT Weekday;
};

struct CPTABLEINFO {
    USHORT CodePage;
    USHORT MaximumCharacterSize;
    USHORT DefaultChar;
    USHORT UniDefaultChar;
    USHORT TransDefaultChar;
    USHORT TransUniDefaultChar;
    USHORT DBCSCodePage;
    UCHAR  LeadByte[12];
    PUSHORT MultiByteTable;
    PVOID   WideCharTable;
    PUSHORT DBCSRanges;
    PUSHORT DBCSOffsets;
};

struct NLSTABLEINFO {
    CPTABLEINFO AnsiTableInfo;
    CPTABLEINFO OemTableInfo;
    PUSHORT UnicodeCaseTable;
};
#pragma pack(pop)

using pfnRtlInitNlsTables = VOID(NTAPI*)(
    PUSHORT AnsiNlsBase,
    PUSHORT OemNlsBase,
    PUSHORT UnicodeCaseTable,
    NLSTABLEINFO* NlsTableInfo);

using pfnRtlResetRtlTranslations = VOID(NTAPI*)(
    NLSTABLEINFO* NlsTableInfo);

} // namespace ler::common
