#include "ler/hooks/ntdll_hook.hpp"
#include "common/config.hpp"
#include "common/types.hpp"
#include "common/log.hpp"
#include "detours/detours.hpp"

namespace ler::hooks {

namespace {

enum SYSTEM_INFORMATION_CLASS {
    SystemBasicInformation = 0,
    SystemCurrentTimeZoneInformation = 44
};

#pragma pack(push, 4)
struct RTL_TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[32];
    common::TIME_FIELDS StandardStart;
    LONG StandardBias;
    WCHAR DaylightName[32];
    common::TIME_FIELDS DaylightStart;
    LONG DaylightBias;
};
#pragma pack(pop)

using pfnNtQuerySystemInformation = NTSTATUS(NTAPI*)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength);

using pfnNtInitializeNlsFiles = NTSTATUS(NTAPI*)(
    PVOID* BaseAddress,
    PLCID DefaultLocaleId,
    PLARGE_INTEGER DefaultCasingTableSize);

pfnNtQuerySystemInformation True_NtQuerySystemInformation = nullptr;
pfnNtInitializeNlsFiles True_NtInitializeNlsFiles = nullptr;

NTSTATUS NTAPI Hook_NtQuerySystemInformation(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength) {

    if (SystemInformationClass == SystemCurrentTimeZoneInformation) {
        if (SystemInformationLength < sizeof(RTL_TIME_ZONE_INFORMATION)) {
            if (ReturnLength) {
                *ReturnLength = sizeof(RTL_TIME_ZONE_INFORMATION);
            }
            return common::STATUS_INFO_LENGTH_MISMATCH;
        }

        auto pTz = reinterpret_cast<RTL_TIME_ZONE_INFORMATION*>(SystemInformation);
        ZeroMemory(pTz, sizeof(RTL_TIME_ZONE_INFORMATION));
        pTz->Bias = common::Config::Instance().timezone_bias();
        wcscpy_s(pTz->StandardName, 32, L"@tzres.dll,-632");
        wcscpy_s(pTz->DaylightName, 32, L"@tzres.dll,-631");
        pTz->StandardBias = 0;
        pTz->DaylightBias = 0;

        if (ReturnLength) {
            *ReturnLength = sizeof(RTL_TIME_ZONE_INFORMATION);
        }
        return common::STATUS_SUCCESS;
    }

    return True_NtQuerySystemInformation ? True_NtQuerySystemInformation(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength) : common::STATUS_NOT_IMPLEMENTED;
}

NTSTATUS NTAPI Hook_NtInitializeNlsFiles(
    PVOID* BaseAddress,
    PLCID DefaultLocaleId,
    PLARGE_INTEGER DefaultCasingTableSize) {

    NTSTATUS status = True_NtInitializeNlsFiles ? True_NtInitializeNlsFiles(BaseAddress, DefaultLocaleId, DefaultCasingTableSize) : common::STATUS_SUCCESS;
    if (common::NT_SUCCESS(status) && DefaultLocaleId) {
        *DefaultLocaleId = common::Config::Instance().locale_id();
    }
    return status;
}

} // namespace

void NtdllHook::Attach() noexcept {
    HMODULE hNtdll = ::GetModuleHandleW(L"ntdll.dll");
    if (!hNtdll) {
        return;
    }

    True_NtQuerySystemInformation = reinterpret_cast<pfnNtQuerySystemInformation>(::GetProcAddress(hNtdll, "NtQuerySystemInformation"));
    True_NtInitializeNlsFiles = reinterpret_cast<pfnNtInitializeNlsFiles>(::GetProcAddress(hNtdll, "NtInitializeNlsFiles"));

    if (True_NtQuerySystemInformation) {
        ::DetourAttach(&(PVOID&)True_NtQuerySystemInformation, reinterpret_cast<PVOID>(Hook_NtQuerySystemInformation));
    }
    if (True_NtInitializeNlsFiles) {
        ::DetourAttach(&(PVOID&)True_NtInitializeNlsFiles, reinterpret_cast<PVOID>(Hook_NtInitializeNlsFiles));
    }
}

void NtdllHook::Detach() noexcept {
    if (True_NtQuerySystemInformation) {
        ::DetourDetach(&(PVOID&)True_NtQuerySystemInformation, reinterpret_cast<PVOID>(Hook_NtQuerySystemInformation));
    }
    if (True_NtInitializeNlsFiles) {
        ::DetourDetach(&(PVOID&)True_NtInitializeNlsFiles, reinterpret_cast<PVOID>(Hook_NtInitializeNlsFiles));
    }
}

} // namespace ler::hooks
