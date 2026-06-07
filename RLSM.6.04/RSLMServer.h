#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cstdint>

namespace rslm {
    namespace server {
        namespace networking {

            class RSLMServer {
            public:
                RSLMServer();
                ~RSLMServer();

                RSLMServer(const RSLMServer&) = delete;
                RSLMServer& operator=(const RSLMServer&) = delete;

                bool Start(uint16_t port);
                void Stop();
                bool IsRunning() const;

                void Broadcast(const std::vector<uint8_t>& data);
                bool SendTo(const std::string& clientId, const std::vector<uint8_t>& data);
                void DisconnectClient(const std::string& clientId);
                std::vector<std::string> GetClients() const;

                using ClientHandler = std::function<void(const std::string& clientId)>;
                using MessageHandler = std::function<void(const std::string& clientId, const std::vector<uint8_t>& data)>;

                void SetOnClientConnected(ClientHandler handler);
                void SetOnClientDisconnected(ClientHandler handler);
                void SetOnMessageReceived(MessageHandler handler);

            private:
                class Impl;
                std::unique_ptr<Impl> m_pImpl;
            };

        } // namespace networking
    } // namespace server
} // namespace rslm