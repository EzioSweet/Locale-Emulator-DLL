#include "ler/hook_manager.hpp"
#include "ler/hooks/kernel32_hook.hpp"
#include "ler/hooks/gdi32_hook.hpp"
#include "ler/hooks/user32_hook.hpp"
#include "ler/hooks/advapi32_hook.hpp"
#include "ler/hooks/ntdll_hook.hpp"
#include "ler/hooks/process_hook.hpp"
#include "detours/detours.hpp"

namespace ler {

HookManager::HookManager() {
    modules_.emplace_back(std::make_unique<hooks::Kernel32Hook>());
    modules_.emplace_back(std::make_unique<hooks::Gdi32Hook>());
    modules_.emplace_back(std::make_unique<hooks::User32Hook>());
    modules_.emplace_back(std::make_unique<hooks::Advapi32Hook>());
    modules_.emplace_back(std::make_unique<hooks::NtdllHook>());
    modules_.emplace_back(std::make_unique<hooks::ProcessHook>());
}

HookManager& HookManager::Instance() noexcept {
    static HookManager instance;
    return instance;
}

bool HookManager::InstallAll() noexcept {
    if (installed_) {
        return true;
    }

    ::DetourTransactionBegin();

    for (auto& module : modules_) {
        module->Attach();
    }

    LONG status = ::DetourTransactionCommit();
    if (status == NO_ERROR) {
        installed_ = true;
        return true;
    }

    return false;
}

bool HookManager::UninstallAll() noexcept {
    if (!installed_) {
        return true;
    }

    ::DetourTransactionBegin();

    for (auto& module : modules_) {
        module->Detach();
    }

    LONG status = ::DetourTransactionCommit();
    if (status == NO_ERROR) {
        installed_ = false;
        return true;
    }

    return false;
}

} // namespace ler
