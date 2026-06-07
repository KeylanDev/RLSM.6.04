#pragma once
#include <cstdint>

namespace rslm {

    enum class ShutdownAction : uint8_t {
        Shutdown = 0,
        Restart = 1,
        Standby = 2,
        Logoff = 3,
        Hibernate = 4
    };

} // namespace rslm