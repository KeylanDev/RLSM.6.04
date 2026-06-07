#include "ServerSystemInformationHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerSystemInformationHandler::RequestSystemInfo(const std::string& clientId,
                SystemInfoCallback callback) {
                if (callback) callback({});
            }

        } // namespace messages
    } // namespace server
} // namespace rslm