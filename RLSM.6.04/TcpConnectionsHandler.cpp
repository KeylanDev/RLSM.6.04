#include "TcpConnectionsHandler.h"

namespace rslm {
    namespace client {
        namespace messages {

            void TcpConnectionsHandler::GetConnections(TcpListCallback callback) {
                std::vector<TcpConnection> connections;
                if (callback) callback(connections);
            }

        } // namespace messages
    } // namespace client
} // namespace rslm