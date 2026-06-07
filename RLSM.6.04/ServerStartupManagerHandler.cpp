#include "ServerStartupManagerHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerStartupManagerHandler::GetStartupItems(const std::string& clientId,
                StartupListCallback callback) {
                if (callback) callback({});
            }

            void ServerStartupManagerHandler::AddStartupItem(const std::string& clientId,
                const StartupItem& item,
                StartupStatusCallback callback) {
                if (callback) callback(false);
            }

            void ServerStartupManagerHandler::RemoveStartupItem(const std::string& clientId,
                const std::string& name,
                StartupStatusCallback callback) {
                if (callback) callback(false);
            }

        } // namespace messages
    } // namespace server
} // namespace rslm