#include "TcpServer.h"
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace rslm {
    namespace server {
        namespace networking {

            struct ClientInfo {
                std::string id;
                int socket = -1;
            };

            struct TcpServer::Impl {
                int listenSocket = -1;
                bool running = false;
                std::vector<ClientInfo> clients;
                std::mutex clientsMutex;
            };

            TcpServer::TcpServer() : m_pImpl(std::make_unique<Impl>()) {
#ifdef _WIN32
                WSADATA wsa;
                WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
            }

            TcpServer::~TcpServer() {
                Stop();
            }

            bool TcpServer::Start(uint16_t port) {
                if (m_pImpl->running) return false;

                m_pImpl->listenSocket = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
                if (m_pImpl->listenSocket < 0) return false;

                sockaddr_in addr = {};
                addr.sin_family = AF_INET;
                addr.sin_port = htons(port);
                addr.sin_addr.s_addr = INADDR_ANY;

                if (bind(m_pImpl->listenSocket, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
                    return false;

                if (listen(m_pImpl->listenSocket, 10) < 0)
                    return false;

                m_pImpl->running = true;

                std::thread([this]() {
                    while (m_pImpl->running) {
                        sockaddr_in clientAddr = {};
                        socklen_t addrLen = sizeof(clientAddr);
                        int clientSocket = accept(m_pImpl->listenSocket, reinterpret_cast<sockaddr*>(&clientAddr), &addrLen);

                        if (clientSocket >= 0) {
                            ClientInfo info;
                            info.socket = clientSocket;
                            info.id = "client_" + std::to_string(clientSocket);

                            {
                                std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                                m_pImpl->clients.push_back(info);
                            }

                            std::cout << "[TcpServer] Client connected: " << info.id << "\n";

                            std::thread([this, clientSocket, clientId = info.id]() {
                                char buffer[65536];
                                while (m_pImpl->running) {
                                    int bytes = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
                                    if (bytes <= 0) {
                                        std::cout << "[TcpServer] Client disconnected: " << clientId << "\n";
                                        DisconnectClient(clientId);
                                        break;
                                    }
                                    buffer[bytes] = '\0';
                                    std::string msg(buffer);
                                    std::cout << "[TcpServer] Received from " << clientId << ": " << msg << "\n";

                                    // === HELLO handshake ===
                                    if (msg.find("\"hello\"") != std::string::npos) {
                                        std::string role = "agent";
                                        if (msg.find("\"admin\"") != std::string::npos) role = "admin";

                                        std::string ack = "{\"type\":\"hello_ack\",\"payload\":{\"clientId\":\"" +
                                            clientId + "\",\"role\":\"" + role + "\"}}\n";
                                        send(clientSocket, ack.c_str(), ack.size(), 0);

                                        // Si agent, notifier les admins
                                        if (role == "agent") {
                                            std::string notify = "{\"type\":\"agent-connected\",\"payload\":{\"id\":\"" +
                                                clientId + "\",\"tag\":\"RSLM-Client\"}}\n";
                                            std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                                            for (auto& other : m_pImpl->clients) {
                                                if (other.socket != clientSocket) {
                                                    send(other.socket, notify.c_str(), notify.size(), 0);
                                                }
                                            }
                                        }
                                    }
                                    // === RELAY aux autres clients ===
                                    else {
                                        std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                                        for (auto& other : m_pImpl->clients) {
                                            if (other.socket != clientSocket) {
                                                send(other.socket, buffer, bytes, 0);
                                            }
                                        }
                                    }
                                }
                                }).detach();
                        }
                    }
                    }).detach();

                return true;
            }

            void TcpServer::Stop() {
                m_pImpl->running = false;

                if (m_pImpl->listenSocket >= 0) {
#ifdef _WIN32
                    closesocket(m_pImpl->listenSocket);
#else
                    close(m_pImpl->listenSocket);
#endif
                    m_pImpl->listenSocket = -1;
                }

                for (auto& client : m_pImpl->clients) {
#ifdef _WIN32
                    closesocket(client.socket);
#else
                    close(client.socket);
#endif
                }
                m_pImpl->clients.clear();
            }

            bool TcpServer::IsRunning() const {
                return m_pImpl->running;
            }

            bool TcpServer::Send(const std::string& clientId, const std::vector<uint8_t>& data) {
                std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                for (auto& client : m_pImpl->clients) {
                    if (client.id == clientId) {
                        ::send(client.socket, reinterpret_cast<const char*>(data.data()),
                            static_cast<int>(data.size()), 0);
                        return true;
                    }
                }
                return false;
            }

            void TcpServer::DisconnectClient(const std::string& clientId) {
                std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                auto it = std::find_if(m_pImpl->clients.begin(), m_pImpl->clients.end(),
                    [&](const ClientInfo& c) { return c.id == clientId; });
                if (it != m_pImpl->clients.end()) {
#ifdef _WIN32
                    closesocket(it->socket);
#else
                    close(it->socket);
#endif
                    m_pImpl->clients.erase(it);
                }
            }

            std::vector<std::string> TcpServer::GetConnectedClients() const {
                std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                std::vector<std::string> ids;
                for (const auto& client : m_pImpl->clients) {
                    ids.push_back(client.id);
                }
                return ids;
            }

        } // namespace networking
    } // namespace server
} // namespace rslm