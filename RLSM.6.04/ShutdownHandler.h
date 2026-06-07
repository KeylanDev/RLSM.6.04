#pragma once
#include <functional>
#include "ShutdownAction.h"

namespace rslm {
    namespace client {
        namespace messages {

            using ShutdownCallback = std::function<void(bool success)>;

            class ShutdownHandler {
            public:
                static void Execute(ShutdownAction action, ShutdownCallback callback);
            };

        } // namespace messages
    } // namespace client
} // namespace rslm