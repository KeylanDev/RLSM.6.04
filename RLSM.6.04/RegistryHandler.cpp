#include "RegistryHandler.h"
#include "NativeMethodsHelper.h"

namespace rslm {
    namespace client {
        namespace messages {

            void RegistryHandler::ReadKey(const std::string& path, RegistryCallback callback) {
                std::vector<RegistryValue> values;
                // Placeholder - énumération complète à implémenter
                if (callback) callback(values);
            }

            void RegistryHandler::WriteValue(const std::string& path,
                const std::string& name,
                const std::string& type,
                const std::string& data,
                RegistryStatusCallback callback) {
                bool success = helper::NativeMethodsHelper::WriteRegistryValue(path, name, data);
                if (callback) callback(success);
            }

            void RegistryHandler::DeleteValue(const std::string& path,
                const std::string& name,
                RegistryStatusCallback callback) {
                bool success = false;
                if (callback) callback(success);
            }

            void RegistryHandler::DeleteKey(const std::string& path,
                RegistryStatusCallback callback) {
                bool success = false;
                if (callback) callback(success);
            }

            void RegistryHandler::EnumKeys(const std::string& path, RegistryCallback callback) {
                std::vector<RegistryValue> values;
                if (callback) callback(values);
            }

        } // namespace messages
    } // namespace client
} // namespace rslm