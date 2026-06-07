#include "ServerTaskManagerHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerTaskManagerHandler::GetProcesses(const std::string& clientId,
                ProcessListCallback callback) {
                if (callback) callback({});
            }

            void ServerTaskManagerHandler::KillProcess(const std::string& clientId,
                uint32_t pid,
                ProcessStatusCallback callback) {
                if (callback) callback(false);
            }

            void ServerTaskManagerHandler::StartProcess(const std::string& clientId,
                const std::string& path,
                ProcessStatusCallback callback) {
                if (callback) callback(false);
            }

            void ServerTaskManagerHandler::SuspendProcess(const std::string& clientId,
                uint32_t pid,
                ProcessStatusCallback callback) {
                if (callback) callback(false);
            }

            void ServerTaskManagerHandler::ResumeProcess(const std::string& clientId,
                uint32_t pid,
                ProcessStatusCallback callback) {
                if (callback) callback(false);
            }

        } // namespace messages
    } // namespace server
} // namespace rslm