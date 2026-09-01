#pragma once

#include "hook_interface.hpp"

namespace ler::hooks {

class User32Hook final : public IHookModule {
public:
    [[nodiscard]] std::string_view Name() const noexcept override { return "User32Hook"; }
    void Attach() noexcept override;
    void Detach() noexcept override;
};

} // namespace ler::hooks
