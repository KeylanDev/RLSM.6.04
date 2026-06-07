#include "StartupManagerHandler.h"
#include "ClientStartup.h"

namespace rslm {
    namespace client {
        namespace messages {

            void StartupManagerHandler::GetStartupItems(StartupListCallback callback) {
                std::vector<StartupItem> items;
                if (callback) callback(items);
            }

            void StartupManagerHandler::AddStartupItem(const StartupItem& item, StartupStatusCallback callback) {
                setup::ClientStartup::AddToStartup(item.name, item.command,
                    [callback](bool success) {
                        if (callback) callback(success);
                    });
            }

            void StartupManagerHandler::RemoveStartupItem(const std::string& name, StartupStatusCallback callback) {
                setup::ClientStartup::RemoveFromStartup(name,
                    [callback](bool success) {
                        if (callback) callback(success);
                    });
            }

        } // namespace messages
    } // namespace client
} // namespace rslm