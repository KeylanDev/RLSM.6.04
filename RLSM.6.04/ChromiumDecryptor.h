#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rslm {
    namespace client {
        namespace recovery {

            class ChromiumDecryptor {
            public:
                static std::string Decrypt(const std::vector<uint8_t>& encryptedData);
                static std::vector<uint8_t> GetLocalKey(const std::string& browserPath);
                static bool IsAvailable();
            };

        } // namespace recovery
    } // namespace client
} // namespace rslm