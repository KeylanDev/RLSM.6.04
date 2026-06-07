#include "ServerRemoteShellHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerRemoteShellHandler::OpenShell(const std::string& clientId,
                ShellOutputCallback onOutput) {
            }

            void ServerRemoteShellHandler::SendCommand(const std::string& clientId,
                const std::string& command) {
            }

            void ServerRemoteShellHandler::CloseShell(const std::string& clientId) {
            }

        } // namespace messages
    } // namespace server
} // namespace rslm