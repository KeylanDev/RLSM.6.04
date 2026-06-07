#include "RSLMServer.h"
#include "TcpServer.h"
#include <nlohmann/json.hpp>
#include <mutex>

namespace rslm {
    namespace server {
        namespace networking {

            using json = nlohmann::json;

            struct RSLMServer::Impl {
                TcpServer tcpServer;
                ClientHandler onClientConnected;
                ClientHandler onClientDisconnected;
                MessageHandler onMessageReceived;
                bool running = false;
            };

            RSLMServer::RSLMServer() : m_pImpl(std::make_unique<Impl>()) {
            }

            RSLMServer::~RSLMServer() {
                Stop();
            }

            bool RSLMServer::Start(uint16_t port) {
                if (m_pImpl->running) return false;
                if (!m_pImpl->tcpServer.Start(port)) return false;
                m_pImpl->running = true;
                return true;
            }

            void RSLMServer::Stop() {
                m_pImpl->running = false;
                m_pImpl->tcpServer.Stop();
            }

            bool RSLMServer::IsRunning() const {
                return m_pImpl->running;
            }

            void RSLMServer::Broadcast(const std::vector<uint8_t>& data) {
                auto clients = m_pImpl->tcpServer.GetConnectedClients();
                for (const auto& id : clients) {
                    m_pImpl->tcpServer.Send(id, data);
                }
            }

            bool RSLMServer::SendTo(const std::string& clientId, const std::vector<uint8_t>& data) {
                return m_pImpl->tcpServer.Send(clientId, data);
            }

            void RSLMServer::DisconnectClient(const std::string& clientId) {
                m_pImpl->tcpServer.DisconnectClient(clientId);
            }

            std::vector<std::string> RSLMServer::GetClients() const {
                return m_pImpl->tcpServer.GetConnectedClients();
            }

            void RSLMServer::SetOnClientConnected(ClientHandler handler) {
                m_pImpl->onClientConnected = std::move(handler);
            }

            void RSLMServer::SetOnClientDisconnected(ClientHandler handler) {
                m_pImpl->onClientDisconnected = std::move(handler);
            }

            void RSLMServer::SetOnMessageReceived(MessageHandler handler) {
                m_pImpl->onMessageReceived = std::move(handler);
            }

        } // namespace networking
    } // namespace server
} // namespace rslm