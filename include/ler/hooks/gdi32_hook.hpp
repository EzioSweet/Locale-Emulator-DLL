#pragma once

#include "hook_interface.hpp"

namespace ler::hooks {

class Gdi32Hook final : public IHookModule {
public:
    [[nodiscard]] std::string_view Name() const noexcept override { return "Gdi32Hook"; }
    void Attach() noexcept override;
    void Detach() noexcept override;
};

} // namespace ler::hooks
