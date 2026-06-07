#include "NativeMethodsHelper.h"
#include <cstdio>
#include <memory>
#include <array>
#include <stdexcept>

namespace rslm {
    namespace client {
        namespace helper {

            bool NativeMethodsHelper::OpenRegistryKey(const std::string&, void**) {
                return false;
            }

            void NativeMethodsHelper::CloseRegistryKey(void*) {
            }

            std::string NativeMethodsHelper::ReadRegistryValue(const std::string&, const std::string&) {
                return "";
            }

            bool NativeMethodsHelper::WriteRegistryValue(const std::string&, const std::string&, const std::string&) {
                return false;
            }

            std::string NativeMethodsHelper::ExecuteCommand(const std::string& command) {
                std::array<char, 128> buffer;
                std::string result;

                FILE* pipe = _popen(command.c_str(), "r");
                if (!pipe) return "";

                while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe) != nullptr) {
                    result += buffer.data();
                }

                _pclose(pipe);
                return result;
            }

            uint32_t NativeMethodsHelper::GetCurrentProcessId() {
                return 0;
            }

            bool NativeMethodsHelper::ProcessExists(const std::string&) {
                return false;
            }

        } // namespace helper
    } // namespace client
} // namespace rslm