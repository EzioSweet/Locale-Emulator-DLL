# Locale Emulator DLL (x86) - Galgame 通用免配置转区与 DLL 劫持注入组件

[English](README_EN.md) | [简体中文](README.md)

---

本项目基于 **Microsoft Detours** 与 **PEB NLS 重置技术**，完整复刻了 Locale Emulator (LE) 的核心转区功能，专为 x86（32 位）Windows 游戏（尤其是各种引擎的 Galgame / 视觉小说）量身打造。

---

## 🎯 核心原理与架构

项目编译产出**全套游戏代理劫持 DLL**与**转区运行时核心（`ler.dll`）**：

1. **转区核心 (`ler.dll`)**：
   - **PEB / NLS 重置**：自动解析系统 `c_932.nls` 和 `l_intl.nls`，调用 `RtlInitNlsTables` 与 `RtlResetRtlTranslations`，重置进程代码页指针为日区（CP932 / Shift-JIS）。
   - **Detours API Hook**：
     - `GetACP` / `GetOEMCP` -> `932`
     - `GetUserDefaultLCID` / `GetSystemDefaultLCID` / `GetThreadLocale` / `GetUserDefaultUILanguage` / `GetSystemDefaultUILanguage` -> `0x0411` (1041 / ja-JP)
     - `GetUserDefaultLocaleName` / `GetSystemDefaultLocaleName` -> `L"ja-JP"`
     - `GetTimeZoneInformation` / `GetDynamicTimeZoneInformation` -> 东京时区（UTC+9，Bias = -540）
     - `MultiByteToWideChar` / `WideCharToMultiByte` Shift-JIS 字符集适配
     - `CreateFontA/W` / `CreateFontIndirectA/W` 强制字符集为 `SHIFTJIS_CHARSET` (128) 并智能替换默认日文字体（如 `MS Gothic`）
     - `CreateWindowExA` / `SetWindowTextA` / `MessageBoxA` ANSI 文本转码，防止日文窗口标题与弹窗乱码
     - `RegQueryValueExA/W` 注册表代码页查询劫持
     - `CreateProcessA/W` 子进程环境继承

2. **通用代理 DLL 库（按游戏引擎选择放置）**：
   所有代理 DLL 均具备全量 API 透明转发功能，并在启动时自动拉起同目录下的 `ler.dll`：

| 代理 DLL | 导出特征 / 适用引擎与游戏类型 |
| :--- | :--- |
| **`dsound.dll`** | **BGI (Ethornell)** 等 DirectSound 引擎游戏（如《氷堂伊吹》、《大图书馆的牧羊人》等） |
| **`winmm.dll`** | **吉里吉里 (Kirikiri / KAG)**、**NScripter**、**CatSystem2**、**Majiro**、**RealLive** 等绝大多数传统 Galgame（几乎 95% 传统游戏均引用多媒体定时器与音频） |
| **`version.dll`** | **Unity** 游戏、**MSVC 运行时**游戏、Steam 移植游戏 |
| **`d3d9.dll`** | 大多数使用 **DirectX 9** 的 2D/3D Galgame 与 PC 游戏 |
| **`d3d11.dll`** | 使用 **DirectX 11** 的现代游戏、Unity 独立游戏 |
| **`dxgi.dll`** | 使用 **DirectX 10/11/12** 的图形交换链层游戏 |
| **`dinput8.dll`** | 使用 **DirectInput 8** 处理手柄/键鼠输入的游戏 |
| **`ddraw.dll`** | 早期 90 年代/2000 年代经典 **DirectDraw 2D** 视觉小说 |

---

## 🚀 使用方法

将 `build/bin/ler.dll` 与**对应游戏所使用的代理 DLL**（如 `dsound.dll` 或 `winmm.dll`）直接复制到游戏根目录（与游戏主 `.exe` 处于同一文件夹）：

```text
GalgameFolder/
├── Game.exe
├── dsound.dll (或 winmm.dll / version.dll 等)  <-- 代理加载器
├── ler.dll                                   <-- 转区核心
└── ler.ini                                   <-- (可选) 自定义配置
```

直接双击运行 `Game.exe`，游戏便会自动以日区环境启动！无需安装注册表项，无需管理员权限，无需外部启动器。

> [!TIP]
> **如何判断游戏需要哪个 DLL？**
> - 如果不确定游戏引用了哪个，通常直接放入 **`winmm.dll`** 或 **`dsound.dll`** 或 **`version.dll`** 即可生效；
> - 亦可将这几个常用代理 DLL 一同放入游戏目录，不会产生冲突。

---

## ⚙️ 可选配置文件 (`ler.ini`)

若无 `ler.ini`，程序默认以完整日区配置运行。若需自定义，可在游戏目录下创建 `ler.ini`：

```ini
[Locale]
# 代码页 (932 = Shift-JIS)
AnsiCodePage=932
OemCodePage=932

# 区域 ID (1041 = 0x0411 = ja-JP)
LocaleID=1041

# 字符集 (128 = SHIFTJIS_CHARSET)
Charset=128

# 默认日文字体名称 (例如: MS Gothic, Meiryo, Yu Gothic UI)
DefaultFont=MS Gothic

# 时区偏差（分钟，-540 = 东京 UTC+9）
TimezoneBias=-540

# 是否 Hook 界面语言 API (1 = 启用, 0 = 禁用)
HookUILanguage=1

# 是否修复 ANSI 窗口标题乱码 (1 = 启用, 0 = 禁用)
FixWindowText=1

# 是否自动修正字体字符集 (1 = 启用, 0 = 禁用)
FixFontCharset=1

# 是否输出调试日志 (1 = 启用, 0 = 禁用)
DebugLog=0
```

---

## 🛠️ 构建指南

### 环境要求
1. **CMake** >= 3.20
2. **Ninja**（推荐）或 **MinGW Make**
3. **32 位 MinGW 编译器**（例如 [llvm-mingw](https://github.com/mstorsjo/llvm-mingw/releases) `i686` 版本 或 MSYS2 `mingw32` 工具链），并将其 `bin` 路径加入系统环境变量 `PATH`。

### 一键构建
直接双击或在命令行中运行：

```cmd
build.bat
```

产物将统一输出至 `build/bin/` 目录下：
- `build/bin/ler.dll`
- `build/bin/dsound.dll`
- `build/bin/winmm.dll`
- `build/bin/version.dll`
- `build/bin/d3d9.dll`
- `build/bin/d3d11.dll`
- `build/bin/dxgi.dll`
- `build/bin/dinput8.dll`
- `build/bin/ddraw.dll`
- `build/bin/test_game.exe`（自动化验证测试套件）

---

## 💖 致谢 (Acknowledgements)

本项目在开发过程中离不开以下优秀开源项目与技术的启发与支持：

- **[Microsoft Detours](https://github.com/microsoft/Detours)**：提供高效、轻量且久经沙场检验的二进制 API 劫持与拦截引擎。
- **[Locale Emulator](https://github.com/xupefei/Locale-Emulator)** by **[@xupefei](https://github.com/xupefei)**：传奇且卓越的 Windows 本地转区工具，本项目借鉴了其关于 PEB NLS 注册与底层系统转区的深刻思路。
- **[Google Gemini](https://deepmind.google/technologies/gemini/)**：参与了本项目的 C++20 现代化面向对象架构重构、全套代理套件设计与全流程工程实现。

---

## 📄 许可证 (License)

本项目采用 [MIT License](LICENSE) 许可证开源。
