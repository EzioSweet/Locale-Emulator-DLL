#pragma once

#include "hook_interface.hpp"

namespace ler::hooks {

class ProcessHook final : public IHookModule {
public:
    [[nodiscard]] std::string_view Name() const noexcept override { return "ProcessHook"; }
    void Attach() noexcept override;
    void Detach() noexcept override;
};

} // namespace ler::hooks
