cmake_minimum_required(VERSION 3.20)
project(LocaleEmulatorDLL LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)

# Ensure 32-bit static runtime target
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -m32")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m32 -Wno-macro-redefined -Wno-ignored-attributes")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -m32 -static -static-libgcc")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32 -static -static-libgcc")

# Global output directory for all binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/detours
)

# ----------------------------------------------------
# 1. Build Proxies
# ----------------------------------------------------
macro(add_proxy_target target_name)
    add_library(${target_name} SHARED
        src/proxies/${target_name}_proxy.cpp
        src/proxies/proxy_helper.cpp
        src/proxies/defs/${target_name}.def
    )
    set_target_properties(${target_name} PROPERTIES
        OUTPUT_NAME "${target_name}"
        PREFIX ""
        SUFFIX ".dll"
        LINK_FLAGS "-static -static-libgcc -Wl,--kill-at"
    )
    target_link_libraries(${target_name} PRIVATE kernel32)
endmacro()

# 1.1 version.dll
add_proxy_target(version)

# 1.2 dsound.dll
add_proxy_target(dsound)

# 1.3 winmm.dll
add_proxy_target(winmm)

# 1.4 d3d9.dll
add_proxy_target(d3d9)

# 1.5 d3d11.dll
add_proxy_target(d3d11)

# 1.6 dxgi.dll
add_proxy_target(dxgi)

# 1.7 dinput8.dll
add_proxy_target(dinput8)

# 1.8 ddraw.dll
add_proxy_target(ddraw)

# ----------------------------------------------------
# 2. Build ler.dll (Locale Emulator Runtime with Detours embedded)
# ----------------------------------------------------
add_library(ler SHARED
    # Detours sources (compiled directly to .o/.obj)
    src/detours/detours.cpp
    src/detours/disasm.cpp
    src/detours/modules.cpp
    src/detours/image.cpp
    src/detours/creatwth.cpp

    # Locale Emulator sources
    src/common/config.cpp
    src/ler/nls_table_manager.cpp
    src/ler/hook_manager.cpp
    src/ler/hooks/kernel32_hook.cpp
    src/ler/hooks/gdi32_hook.cpp
    src/ler/hooks/user32_hook.cpp
    src/ler/hooks/advapi32_hook.cpp
    src/ler/hooks/ntdll_hook.cpp
    src/ler/hooks/process_hook.cpp
    src/ler/ler_main.cpp
    src/ler/ler.def
)
set_target_properties(ler PROPERTIES
    OUTPUT_NAME "ler"
    PREFIX ""
    SUFFIX ".dll"
    LINK_FLAGS "-static -static-libgcc -Wl,--kill-at"
)
target_compile_definitions(ler PRIVATE DETOURS_INTERNAL)
target_link_libraries(ler PRIVATE
    kernel32
    user32
    gdi32
    advapi32
    ntdll
)

# ----------------------------------------------------
# 3. Build test_game.exe (Verification Galgame Simulator)
# ----------------------------------------------------
add_executable(test_game
    src/test/test_game.cpp
)
set_target_properties(test_game PROPERTIES
    OUTPUT_NAME "test_game"
    LINK_FLAGS "-static -static-libgcc"
)
target_link_libraries(test_game PRIVATE
    version
    gdi32
    user32
    advapi32
)
add_dependencies(test_game version dsound winmm d3d9 d3d11 dxgi dinput8 ddraw ler)
