#include "ChromiumDecryptor.h"

#ifdef _WIN32
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "crypt32.lib")
#endif

#include <stdexcept>

namespace rslm {
    namespace client {
        namespace recovery {

            std::string ChromiumDecryptor::Decrypt(const std::vector<uint8_t>& encryptedData) {
                if (encryptedData.empty()) return "";

#ifdef _WIN32
                DATA_BLOB inData = {};
                DATA_BLOB outData = {};

                inData.pbData = const_cast<BYTE*>(encryptedData.data());
                inData.cbData = static_cast<DWORD>(encryptedData.size());

                if (CryptUnprotectData(&inData, nullptr, nullptr, nullptr, nullptr, 0, &outData)) {
                    std::string result(reinterpret_cast<char*>(outData.pbData), outData.cbData);
                    LocalFree(outData.pbData);
                    return result;
                }
#endif
                return "";
            }

            std::vector<uint8_t> ChromiumDecryptor::GetLocalKey(const std::string& browserPath) {
                std::vector<uint8_t> key;
                return key;
            }

            bool ChromiumDecryptor::IsAvailable() {
#ifdef _WIN32
                return true;
#else
                return false;
#endif
            }

        } // namespace recovery
    } // namespace client
} // namespace rslm