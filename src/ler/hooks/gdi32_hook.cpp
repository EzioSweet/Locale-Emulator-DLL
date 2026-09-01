#include "ler/hooks/gdi32_hook.hpp"
#include "common/config.hpp"
#include "common/log.hpp"
#include "detours/detours.hpp"

namespace ler::hooks {

namespace {

using pfnCreateFontA = HFONT(WINAPI*)(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCSTR);
using pfnCreateFontW = HFONT(WINAPI*)(int, int, int, int, int, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, DWORD, LPCWSTR);
using pfnCreateFontIndirectA = HFONT(WINAPI*)(const LOGFONTA*);
using pfnCreateFontIndirectW = HFONT(WINAPI*)(const LOGFONTW*);
using pfnCreateFontIndirectExA = HFONT(WINAPI*)(const ENUMLOGFONTEXDVA*);
using pfnCreateFontIndirectExW = HFONT(WINAPI*)(const ENUMLOGFONTEXDVW*);

pfnCreateFontA True_CreateFontA = nullptr;
pfnCreateFontW True_CreateFontW = nullptr;
pfnCreateFontIndirectA True_CreateFontIndirectA = nullptr;
pfnCreateFontIndirectW True_CreateFontIndirectW = nullptr;
pfnCreateFontIndirectExA True_CreateFontIndirectExA = nullptr;
pfnCreateFontIndirectExW True_CreateFontIndirectExW = nullptr;

HFONT WINAPI Hook_CreateFontA(
    int cHeight, int cWidth, int cEscapement, int cOrientation,
    int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut,
    DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision,
    DWORD iQuality, DWORD iPitchAndFamily, LPCSTR pszFaceName) {

    if (common::Config::Instance().fix_font_charset()) {
        if (iCharSet == ANSI_CHARSET || iCharSet == DEFAULT_CHARSET || iCharSet == 0) {
            iCharSet = common::Config::Instance().charset();
        }
    }

    if (!pszFaceName || pszFaceName[0] == '\0' || _stricmp(pszFaceName, "System") == 0 || _stricmp(pszFaceName, "MS Sans Serif") == 0 || _stricmp(pszFaceName, "Arial") == 0) {
        pszFaceName = common::Config::Instance().default_font_a().c_str();
    }

    return True_CreateFontA ? True_CreateFontA(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName) : nullptr;
}

HFONT WINAPI Hook_CreateFontW(
    int cHeight, int cWidth, int cEscapement, int cOrientation,
    int cWeight, DWORD bItalic, DWORD bUnderline, DWORD bStrikeOut,
    DWORD iCharSet, DWORD iOutPrecision, DWORD iClipPrecision,
    DWORD iQuality, DWORD iPitchAndFamily, LPCWSTR pszFaceName) {

    if (common::Config::Instance().fix_font_charset()) {
        if (iCharSet == ANSI_CHARSET || iCharSet == DEFAULT_CHARSET || iCharSet == 0) {
            iCharSet = common::Config::Instance().charset();
        }
    }

    if (!pszFaceName || pszFaceName[0] == L'\0' || _wcsicmp(pszFaceName, L"System") == 0 || _wcsicmp(pszFaceName, L"MS Sans Serif") == 0 || _wcsicmp(pszFaceName, L"Arial") == 0) {
        pszFaceName = common::Config::Instance().default_font().c_str();
    }

    return True_CreateFontW ? True_CreateFontW(cHeight, cWidth, cEscapement, cOrientation, cWeight, bItalic, bUnderline, bStrikeOut, iCharSet, iOutPrecision, iClipPrecision, iQuality, iPitchAndFamily, pszFaceName) : nullptr;
}

HFONT WINAPI Hook_CreateFontIndirectA(const LOGFONTA* lplf) {
    if (!lplf) return nullptr;
    LOGFONTA lf = *lplf;

    if (common::Config::Instance().fix_font_charset()) {
        if (lf.lfCharSet == ANSI_CHARSET || lf.lfCharSet == DEFAULT_CHARSET || lf.lfCharSet == 0) {
            lf.lfCharSet = common::Config::Instance().charset();
        }
    }

    if (lf.lfFaceName[0] == '\0' || _stricmp(lf.lfFaceName, "System") == 0 || _stricmp(lf.lfFaceName, "MS Sans Serif") == 0 || _stricmp(lf.lfFaceName, "Arial") == 0) {
        strcpy_s(lf.lfFaceName, LF_FACESIZE, common::Config::Instance().default_font_a().c_str());
    }

    return True_CreateFontIndirectA ? True_CreateFontIndirectA(&lf) : nullptr;
}

HFONT WINAPI Hook_CreateFontIndirectW(const LOGFONTW* lplf) {
    if (!lplf) return nullptr;
    LOGFONTW lf = *lplf;

    if (common::Config::Instance().fix_font_charset()) {
        if (lf.lfCharSet == ANSI_CHARSET || lf.lfCharSet == DEFAULT_CHARSET || lf.lfCharSet == 0) {
            lf.lfCharSet = common::Config::Instance().charset();
        }
    }

    if (lf.lfFaceName[0] == L'\0' || _wcsicmp(lf.lfFaceName, L"System") == 0 || _wcsicmp(lf.lfFaceName, L"MS Sans Serif") == 0 || _wcsicmp(lf.lfFaceName, L"Arial") == 0) {
        wcscpy_s(lf.lfFaceName, LF_FACESIZE, common::Config::Instance().default_font().c_str());
    }

    return True_CreateFontIndirectW ? True_CreateFontIndirectW(&lf) : nullptr;
}

HFONT WINAPI Hook_CreateFontIndirectExA(const ENUMLOGFONTEXDVA* lpelfe) {
    if (!lpelfe) return nullptr;
    ENUMLOGFONTEXDVA elfe = *lpelfe;

    if (common::Config::Instance().fix_font_charset()) {
        if (elfe.elfEnumLogfontEx.elfLogFont.lfCharSet == ANSI_CHARSET || elfe.elfEnumLogfontEx.elfLogFont.lfCharSet == DEFAULT_CHARSET || elfe.elfEnumLogfontEx.elfLogFont.lfCharSet == 0) {
            elfe.elfEnumLogfontEx.elfLogFont.lfCharSet = common::Config::Instance().charset();
        }
    }

    if (elfe.elfEnumLogfontEx.elfLogFont.lfFaceName[0] == '\0' || _stricmp(elfe.elfEnumLogfontEx.elfLogFont.lfFaceName, "System") == 0 || _stricmp(elfe.elfEnumLogfontEx.elfLogFont.lfFaceName, "MS Sans Serif") == 0) {
        strcpy_s(elfe.elfEnumLogfontEx.elfLogFont.lfFaceName, LF_FACESIZE, common::Config::Instance().default_font_a().c_str());
    }

    return True_CreateFontIndirectExA ? True_CreateFontIndirectExA(&elfe) : nullptr;
}

HFONT WINAPI Hook_CreateFontIndirectExW(const ENUMLOGFONTEXDVW* lpelfe) {
    if (!lpelfe) return nullptr;
    ENUMLOGFONTEXDVW elfe = *lpelfe;

    if (common::Config::Instance().fix_font_charset()) {
        if (elfe.elfEnumLogfontEx.elfLogFont.lfCharSet == ANSI_CHARSET || elfe.elfEnumLogfontEx.elfLogFont.lfCharSet == DEFAULT_CHARSET || elfe.elfEnumLogfontEx.elfLogFont.lfCharSet == 0) {
            elfe.elfEnumLogfontEx.elfLogFont.lfCharSet = common::Config::Instance().charset();
        }
    }

    if (elfe.elfEnumLogfontEx.elfLogFont.lfFaceName[0] == L'\0' || _wcsicmp(elfe.elfEnumLogfontEx.elfLogFont.lfFaceName, L"System") == 0 || _wcsicmp(elfe.elfEnumLogfontEx.elfLogFont.lfFaceName, L"MS Sans Serif") == 0) {
        wcscpy_s(elfe.elfEnumLogfontEx.elfLogFont.lfFaceName, LF_FACESIZE, common::Config::Instance().default_font().c_str());
    }

    return True_CreateFontIndirectExW ? True_CreateFontIndirectExW(&elfe) : nullptr;
}

PVOID GetGdiProc(const char* name) {
    HMODULE hModule = ::GetModuleHandleW(L"gdi32full.dll");
    if (hModule) {
        PVOID proc = reinterpret_cast<PVOID>(::GetProcAddress(hModule, name));
        if (proc) return proc;
    }
    hModule = ::GetModuleHandleW(L"gdi32.dll");
    if (hModule) {
        return reinterpret_cast<PVOID>(::GetProcAddress(hModule, name));
    }
    return nullptr;
}

} // namespace

void Gdi32Hook::Attach() noexcept {
    True_CreateFontA = reinterpret_cast<pfnCreateFontA>(GetGdiProc("CreateFontA"));
    True_CreateFontW = reinterpret_cast<pfnCreateFontW>(GetGdiProc("CreateFontW"));
    True_CreateFontIndirectA = reinterpret_cast<pfnCreateFontIndirectA>(GetGdiProc("CreateFontIndirectA"));
    True_CreateFontIndirectW = reinterpret_cast<pfnCreateFontIndirectW>(GetGdiProc("CreateFontIndirectW"));
    True_CreateFontIndirectExA = reinterpret_cast<pfnCreateFontIndirectExA>(GetGdiProc("CreateFontIndirectExA"));
    True_CreateFontIndirectExW = reinterpret_cast<pfnCreateFontIndirectExW>(GetGdiProc("CreateFontIndirectExW"));

    if (True_CreateFontA) ::DetourAttach(&(PVOID&)True_CreateFontA, reinterpret_cast<PVOID>(Hook_CreateFontA));
    if (True_CreateFontW) ::DetourAttach(&(PVOID&)True_CreateFontW, reinterpret_cast<PVOID>(Hook_CreateFontW));
    if (True_CreateFontIndirectA) ::DetourAttach(&(PVOID&)True_CreateFontIndirectA, reinterpret_cast<PVOID>(Hook_CreateFontIndirectA));
    if (True_CreateFontIndirectW) ::DetourAttach(&(PVOID&)True_CreateFontIndirectW, reinterpret_cast<PVOID>(Hook_CreateFontIndirectW));
    if (True_CreateFontIndirectExA) ::DetourAttach(&(PVOID&)True_CreateFontIndirectExA, reinterpret_cast<PVOID>(Hook_CreateFontIndirectExA));
    if (True_CreateFontIndirectExW) ::DetourAttach(&(PVOID&)True_CreateFontIndirectExW, reinterpret_cast<PVOID>(Hook_CreateFontIndirectExW));
}

void Gdi32Hook::Detach() noexcept {
    if (True_CreateFontA) ::DetourDetach(&(PVOID&)True_CreateFontA, reinterpret_cast<PVOID>(Hook_CreateFontA));
    if (True_CreateFontW) ::DetourDetach(&(PVOID&)True_CreateFontW, reinterpret_cast<PVOID>(Hook_CreateFontW));
    if (True_CreateFontIndirectA) ::DetourDetach(&(PVOID&)True_CreateFontIndirectA, reinterpret_cast<PVOID>(Hook_CreateFontIndirectA));
    if (True_CreateFontIndirectW) ::DetourDetach(&(PVOID&)True_CreateFontIndirectW, reinterpret_cast<PVOID>(Hook_CreateFontIndirectW));
    if (True_CreateFontIndirectExA) ::DetourDetach(&(PVOID&)True_CreateFontIndirectExA, reinterpret_cast<PVOID>(Hook_CreateFontIndirectExA));
    if (True_CreateFontIndirectExW) ::DetourDetach(&(PVOID&)True_CreateFontIndirectExW, reinterpret_cast<PVOID>(Hook_CreateFontIndirectExW));
}

} // namespace ler::hooks
