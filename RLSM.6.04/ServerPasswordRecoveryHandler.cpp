#include "ServerPasswordRecoveryHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerPasswordRecoveryHandler::RequestPasswords(const std::string& clientId,
                PasswordCallback callback) {
                if (callback) callback({});
            }

        } // namespace messages
    } // namespace server
} // namespace rslm