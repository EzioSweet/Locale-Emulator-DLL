#pragma once

#include "hooks/hook_interface.hpp"
#include <vector>
#include <memory>

namespace ler {

class HookManager {
public:
    HookManager();
    ~HookManager() = default;

    HookManager(const HookManager&) = delete;
    HookManager& operator=(const HookManager&) = delete;

    [[nodiscard]] static HookManager& Instance() noexcept;

    bool InstallAll() noexcept;
    bool UninstallAll() noexcept;

private:
    std::vector<std::unique_ptr<hooks::IHookModule>> modules_;
    bool installed_{false};
};

} // namespace ler
