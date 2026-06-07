#include "ServerRegistryHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerRegistryHandler::ReadKey(const std::string& clientId,
                const std::string& path,
                RegistryCallback callback) {
                if (callback) callback({});
            }

            void ServerRegistryHandler::WriteValue(const std::string& clientId,
                const std::string& path,
                const std::string& name,
                const std::string& type,
                const std::string& data,
                RegistryStatusCallback callback) {
                if (callback) callback(false);
            }

            void ServerRegistryHandler::DeleteValue(const std::string& clientId,
                const std::string& path,
                const std::string& name,
                RegistryStatusCallback callback) {
                if (callback) callback(false);
            }

            void ServerRegistryHandler::DeleteKey(const std::string& clientId,
                const std::string& path,
                RegistryStatusCallback callback) {
                if (callback) callback(false);
            }

            void ServerRegistryHandler::EnumKeys(const std::string& clientId,
                const std::string& path,
                RegistryCallback callback) {
                if (callback) callback({});
            }

        } // namespace messages
    } // namespace server
} // namespace rslm