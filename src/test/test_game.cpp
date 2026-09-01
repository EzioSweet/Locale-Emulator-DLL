#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// Call an export from version.dll so the executable imports version.dll statically
extern "C" {
    DWORD WINAPI GetFileVersionInfoSizeA(LPCSTR lptstrFilename, LPDWORD lpdwHandle);
}

int main(void) {
    printf("============================================================\n");
    printf("   Galgame Locale Emulator Emulation Verification Test\n");
    printf("============================================================\n\n");

    // Touch version.dll export
    DWORD dummy = 0;
    GetFileVersionInfoSizeA("dummy.exe", &dummy);

    int totalTests = 0;
    int passedTests = 0;

    #define TEST_ASSERT(expr, msg) do { \
        totalTests++; \
        if (expr) { \
            printf("[PASS] %s\n", msg); \
            passedTests++; \
        } else { \
            printf("[FAIL] %s\n", msg); \
        } \
    } while(0)

    // Test 1: GetACP
    UINT acp = GetACP();
    printf("  -> GetACP() = %u (Expected: 932)\n", acp);
    TEST_ASSERT(acp == 932, "GetACP() == 932 (Japanese Shift-JIS)");

    // Test 2: GetOEMCP
    UINT oemcp = GetOEMCP();
    printf("  -> GetOEMCP() = %u (Expected: 932)\n", oemcp);
    TEST_ASSERT(oemcp == 932, "GetOEMCP() == 932");

    // Test 3: GetUserDefaultLCID
    LCID userLcid = GetUserDefaultLCID();
    printf("  -> GetUserDefaultLCID() = 0x%04X (Expected: 0x0411)\n", (unsigned int)userLcid);
    TEST_ASSERT(userLcid == 0x0411, "GetUserDefaultLCID() == 0x0411 (ja-JP)");

    // Test 4: GetSystemDefaultLCID
    LCID sysLcid = GetSystemDefaultLCID();
    printf("  -> GetSystemDefaultLCID() = 0x%04X (Expected: 0x0411)\n", (unsigned int)sysLcid);
    TEST_ASSERT(sysLcid == 0x0411, "GetSystemDefaultLCID() == 0x0411 (ja-JP)");

    // Test 5: GetThreadLocale
    LCID threadLcid = GetThreadLocale();
    printf("  -> GetThreadLocale() = 0x%04X (Expected: 0x0411)\n", (unsigned int)threadLcid);
    TEST_ASSERT(threadLcid == 0x0411, "GetThreadLocale() == 0x0411 (ja-JP)");

    // Test 6: GetUserDefaultUILanguage
    LANGID uiLang = GetUserDefaultUILanguage();
    printf("  -> GetUserDefaultUILanguage() = 0x%04X (Expected: 0x0411)\n", (unsigned int)uiLang);
    TEST_ASSERT(uiLang == 0x0411, "GetUserDefaultUILanguage() == 0x0411");

    // Test 7: GetSystemDefaultUILanguage
    LANGID sysUiLang = GetSystemDefaultUILanguage();
    printf("  -> GetSystemDefaultUILanguage() = 0x%04X (Expected: 0x0411)\n", (unsigned int)sysUiLang);
    TEST_ASSERT(sysUiLang == 0x0411, "GetSystemDefaultUILanguage() == 0x0411");

    // Test 8: GetLocaleInfoA for LOCALE_IDEFAULTANSICODEPAGE
    char localeAcp[16] = { 0 };
    GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE, localeAcp, sizeof(localeAcp));
    printf("  -> GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_IDEFAULTANSICODEPAGE) = \"%s\" (Expected: \"932\")\n", localeAcp);
    TEST_ASSERT(strcmp(localeAcp, "932") == 0, "GetLocaleInfoA returns \"932\"");

    // Test 9: TimeZoneInformation
    TIME_ZONE_INFORMATION tzi;
    ZeroMemory(&tzi, sizeof(tzi));
    GetTimeZoneInformation(&tzi);
    printf("  -> GetTimeZoneInformation().Bias = %ld (Expected: -540 for Tokyo UTC+9)\n", (long)tzi.Bias);
    TEST_ASSERT(tzi.Bias == -540, "GetTimeZoneInformation() Bias == -540 (Tokyo UTC+9)");

    // Test 10: GDI CreateFontA Charset override
    HFONT hFont = CreateFontA(
        16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "System"
    );
    if (hFont) {
        LOGFONTA lf;
        ZeroMemory(&lf, sizeof(lf));
        GetObjectA(hFont, sizeof(lf), &lf);
        printf("  -> GDI CreateFontA resulting lfCharSet = %u (Expected: 128 / SHIFTJIS_CHARSET)\n", (BYTE)lf.lfCharSet);
        printf("  -> GDI CreateFontA resulting lfFaceName = \"%s\"\n", lf.lfFaceName);
        TEST_ASSERT(lf.lfCharSet == SHIFTJIS_CHARSET, "CreateFontA overridden with SHIFTJIS_CHARSET (128)");
        DeleteObject(hFont);
    } else {
        TEST_ASSERT(FALSE, "CreateFontA failed");
    }

    // Test 11: MultiByteToWideChar with CP_ACP for Shift-JIS Japanese string
    // "日本語" in Shift-JIS: 0x93 0xFA 0x96 0x7B 0x8C 0xEA
    const char sjisText[] = "\x93\xfa\x96\x7b\x8c\xea";
    WCHAR wText[16] = { 0 };
    int convLen = MultiByteToWideChar(CP_ACP, 0, sjisText, -1, wText, sizeof(wText)/sizeof(WCHAR));
    printf("  -> MultiByteToWideChar(CP_ACP, \"\\x93\\xfa\\x96\\x7b\\x8c\\xea\") = L\"%ls\" (Length: %d)\n", wText, convLen);
    TEST_ASSERT(wcscmp(wText, L"日本語") == 0, "CP_ACP correctly decodes Shift-JIS \"日本語\"");

    // Test 12: Registry redirection for CodePage ACP via RegOpenKeyExW / RegQueryValueExW
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"System\\CurrentControlSet\\Control\\Nls\\CodePage", 0, KEY_QUERY_VALUE, &hKey) == ERROR_SUCCESS) {
        WCHAR regAcp[16] = { 0 };
        DWORD dwSize = sizeof(regAcp);
        DWORD dwType = 0;
        if (RegQueryValueExW(hKey, L"ACP", NULL, &dwType, (LPBYTE)regAcp, &dwSize) == ERROR_SUCCESS) {
            printf("  -> RegQueryValueExW(HKLM\\...\\CodePage, \"ACP\") = L\"%ls\" (Expected: L\"932\")\n", regAcp);
            TEST_ASSERT(wcscmp(regAcp, L"932") == 0, "RegQueryValueExW for CodePage ACP returns L\"932\"");
        } else {
            TEST_ASSERT(FALSE, "RegQueryValueExW failed");
        }
        RegCloseKey(hKey);
    } else {
        TEST_ASSERT(FALSE, "RegOpenKeyExW failed");
    }

    printf("\n------------------------------------------------------------\n");
    printf("Results: %d/%d tests passed (%.1f%%)\n", passedTests, totalTests, (float)passedTests / totalTests * 100.0f);
    printf("============================================================\n");

    return (passedTests == totalTests) ? 0 : 1;
}
