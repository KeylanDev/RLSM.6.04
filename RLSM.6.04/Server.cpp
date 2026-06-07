#include "Server.h"

namespace rslm {
    namespace server {
        namespace networking {

            Server::Server() {
            }

            Server::~Server() {
            }

            void Server::SetOnClientConnected(OnClientConnected callback) {
                m_onConnected = std::move(callback);
            }

            void Server::SetOnClientDisconnected(OnClientDisconnected callback) {
                m_onDisconnected = std::move(callback);
            }

            void Server::SetOnDataReceived(OnDataReceived callback) {
                m_onData = std::move(callback);
            }

        } // namespace networking
    } // namespace server
} // namespace rslm