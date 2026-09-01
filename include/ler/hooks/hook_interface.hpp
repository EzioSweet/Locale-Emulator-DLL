#pragma once

#include <windows.h>
#include <string_view>

namespace ler::hooks {

class IHookModule {
public:
    virtual ~IHookModule() = default;

    [[nodiscard]] virtual std::string_view Name() const noexcept = 0;
    virtual void Attach() noexcept = 0;
    virtual void Detach() noexcept = 0;
};

} // namespace ler::hooks
