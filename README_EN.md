# Locale Emulator DLL (x86) - Universal Zero-Config Locale Emulation & Proxy Sideloading for Galgames

[English](README_EN.md) | [简体中文](README.md)

---

A modern C++20 reimplementation of the **Locale Emulator (LE)** core runtime, tailored for x86 (32-bit) Windows games (especially Visual Novels / Galgames). Powered by **Microsoft Detours** and **PEB NLS Table Injection**, it enables portable, zero-configuration Japanese locale emulation simply by dropping DLLs into the game directory.

---

## 🎯 Architecture & How It Works

This project produces a **Universal Game Proxy DLL Suite** and the **Locale Emulator Runtime Core (`ler.dll`)**:

### 1. Locale Emulation Core (`ler.dll`)
- **PEB / NLS Overrides**:
  - Dynamically loads system `c_932.nls` and `l_intl.nls` tables using WOW64-safe routines.
  - Calls `RtlInitNlsTables` and `RtlResetRtlTranslations` to rebind the process's default ANSI/OEM code page tables directly to CP932 (Shift-JIS) at the NT subsystem level.
- **Detours API Interception**:
  - **Code Page APIs**: `GetACP()` / `GetOEMCP()` -> `932`
  - **Locale & Language APIs**: `GetUserDefaultLCID()` / `GetSystemDefaultLCID()` / `GetThreadLocale()` / `GetUserDefaultUILanguage()` / `GetSystemDefaultUILanguage()` -> `0x0411` (`1041` / `ja-JP`)
  - **Locale Name APIs**: `GetUserDefaultLocaleName()` / `GetSystemDefaultLocaleName()` -> `L"ja-JP"`
  - **Time Zone APIs**: `GetTimeZoneInformation()` / `GetDynamicTimeZoneInformation()` -> Tokyo Time Zone (UTC+9, Bias = -540)
  - **String Conversion APIs**: `MultiByteToWideChar()` / `WideCharToMultiByte()` default code page redirection
  - **GDI Font Rendering**: `CreateFontA/W()`, `CreateFontIndirectA/W()` enforces `SHIFTJIS_CHARSET` (128) and replaces default system fallback fonts with Japanese fonts (e.g. `MS Gothic`)
  - **Window & Dialogs**: `CreateWindowExA()`, `SetWindowTextA()`, `MessageBoxA()` automatically converts ANSI strings to Unicode to prevent garbled Japanese window titles and dialog boxes
  - **Registry Queries**: `RegQueryValueExA/W()` redirects registry queries for system code page and installation language keys
  - **Process Spawning**: `CreateProcessA/W()` ensures spawned child processes inherit the emulation environment

### 2. Universal Proxy DLL Suite
Every proxy DLL provides transparent standard API forwarding to original Windows system DLLs while auto-loading `ler.dll` in its directory during `DLL_PROCESS_ATTACH`:

| Proxy DLL | Target Engine & Typical Game Categories |
| :--- | :--- |
| **`dsound.dll`** | **BGI (Ethornell)** engine games (e.g., *Hikari Do Ibuki*, *Daitoshokan no Hitsujikai*), DirectSound audio games |
| **`winmm.dll`** | **Kirikiri (吉里吉里 / KAG)**, **NScripter**, **CatSystem2**, **Majiro**, **RealLive**, etc. (Over 95% of classic Visual Novels import multimedia timer and audio APIs) |
| **`version.dll`** | **Unity** games, **MSVC runtime** applications, Steam ports |
| **`d3d9.dll`** | Classic **DirectX 9** 2D/3D Galgames and PC games |
| **`d3d11.dll`** | Modern **DirectX 11** games and Unity standalone games |
| **`dxgi.dll`** | **DirectX 10/11/12** games utilizing DXGI swap chains |
| **`dinput8.dll`** | Games utilizing **DirectInput 8** for gamepad / mouse / keyboard input |
| **`ddraw.dll`** | 1990s and early 2000s classic **DirectDraw 2D** Visual Novels |

---

## 🚀 Quick Start

Copy `build/bin/ler.dll` along with the **matching proxy DLL** (such as `winmm.dll` or `dsound.dll`) into the game root directory (where the game `.exe` is located):

```text
GalgameFolder/
├── Game.exe
├── dsound.dll (or winmm.dll / version.dll, etc.)  <-- Proxy Loader
├── ler.dll                                       <-- Emulation Core
└── ler.ini                                       <-- (Optional) Custom Config
```

Launch `Game.exe` directly. The game will automatically run in a Japanese locale environment without registry modifications, administrator privileges, or external launchers.

> [!TIP]
> **Which proxy DLL should I use?**
> - If you are unsure which DLL your game imports, placing **`winmm.dll`**, **`dsound.dll`**, or **`version.dll`** will work for most games.
> - You can also place multiple proxy DLLs in the game folder simultaneously without conflict.

---

## ⚙️ Optional Configuration (`ler.ini`)

If `ler.ini` is not present, the runtime automatically uses the optimal default Japanese settings. If customization is desired, create a `ler.ini` file in the game folder:

```ini
[Locale]
# ANSI Code Page (932 = Shift-JIS, 949 = Korean, 950 = Big5, 936 = GBK)
AnsiCodePage=932

# OEM Code Page
OemCodePage=932

# Locale ID (1041 = 0x0411 = ja-JP, 1042 = ko-KR, 1028 = zh-TW)
LocaleID=1041

# Font Charset (128 = SHIFTJIS_CHARSET)
Charset=128

# Fallback Font Face Name (e.g. MS Gothic, Meiryo, Yu Gothic UI)
DefaultFont=MS Gothic

# Timezone Bias in minutes (-540 = Tokyo UTC+9)
TimezoneBias=-540

# Hook UI Language APIs (1 = Enabled, 0 = Disabled)
HookUILanguage=1

# Fix ANSI window title and message box garbled text (1 = Enabled, 0 = Disabled)
FixWindowText=1

# Automatically replace ANSI charsets with target charset (1 = Enabled, 0 = Disabled)
FixFontCharset=1

# Output internal debug logs to debugger (1 = Enabled, 0 = Disabled)
DebugLog=0
```

---

## 🛠️ Build Instructions

### Prerequisites
1. **CMake** >= 3.20
2. **Ninja** (recommended) or **MinGW Make**
3. **32-bit MinGW Toolchain** (e.g., [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases) `i686` release or MSYS2 `mingw32`), added to system `PATH`.

### One-Click Build
Run the build script from terminal or double-click:

```cmd
build.bat
```

All compiled deliverables will be placed in `build/bin/`:
- `build/bin/ler.dll`
- `build/bin/dsound.dll`
- `build/bin/winmm.dll`
- `build/bin/version.dll`
- `build/bin/d3d9.dll`
- `build/bin/d3d11.dll`
- `build/bin/dxgi.dll`
- `build/bin/dinput8.dll`
- `build/bin/ddraw.dll`
- `build/bin/test_game.exe` (Automated verification test suite)

---

## 💖 Acknowledgements

Special thanks to the following open-source projects and technologies that made this project possible:

- **[Microsoft Detours](https://github.com/microsoft/Detours)**: For providing an efficient, lightweight, and robust binary API interception library.
- **[Locale Emulator](https://github.com/xupefei/Locale-Emulator)** by **[@xupefei](https://github.com/xupefei)**: The legendary Windows locale emulation utility whose architecture and NLS table design inspired this project.
- **[Google Gemini](https://deepmind.google/technologies/gemini/)**: For collaborative design, modern C++20 object-oriented refactoring, and comprehensive implementation assistance.

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).
