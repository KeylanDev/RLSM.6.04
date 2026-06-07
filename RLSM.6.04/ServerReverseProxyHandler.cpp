#include "ServerReverseProxyHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerReverseProxyHandler::StartProxy(const std::string& clientId,
                const std::string& targetHost,
                uint16_t targetPort,
                ProxyStatusCallback callback) {
                if (callback) callback(false, "Not implemented");
            }

            void ServerReverseProxyHandler::StopProxy(const std::string& clientId) {
            }

        } // namespace messages
    } // namespace server
} // namespace rslm