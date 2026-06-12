#include "TcpServer.h"
#include <mutex>
#include <vector>
#include <algorithm>
#include <iostream>
#include <thread>
#include <fstream>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#endif

namespace rslm {
    namespace server {
        namespace networking {

            // Fonction utilitaire pour nettoyer les caractères non UTF-8
            std::string sanitize(const std::string& input) {
                std::string output;
                for (unsigned char c : input) {
                    if ((c >= 0x20 && c <= 0x7E) ||  // ASCII imprimable
                        c == 0x09 || c == 0x0A || c == 0x0D ||  // tab, LF, CR
                        (c >= 0xC2 && c <= 0xDF) ||  // UTF-8 2 bytes début
                        (c >= 0xE0 && c <= 0xEF)) {  // UTF-8 3 bytes début
                        output += c;
                    }
                    else {
                        output += ' ';  // Remplacer les caractères invalides
                    }
                }
                return output;
            }

            // ============================================================
            // SERVEUR HTTP POUR PARTAGER L'AGENT (SANS CHIFFREMENT)
            // ============================================================

            static void HandleHttpRequest(SOCKET clientSocket, const std::string& request) {
                // Cherche si la requête demande update.dat
                if (request.find("GET /update.dat") != std::string::npos) {
                    // Lire le fichier update.dat
                    std::ifstream file("update.dat", std::ios::binary | std::ios::ate);

                    if (file.is_open()) {
                        std::streamsize size = file.tellg();
                        file.seekg(0, std::ios::beg);

                        std::vector<char> buffer(size);
                        if (file.read(buffer.data(), size)) {
                            // Envoyer la réponse HTTP (sans chiffrement)
                            std::string response = "HTTP/1.1 200 OK\r\n";
                            response += "Content-Type: application/octet-stream\r\n";
                            response += "Content-Length: " + std::to_string(size) + "\r\n";
                            response += "Connection: close\r\n\r\n";

                            send(clientSocket, response.c_str(), (int)response.size(), 0);
                            send(clientSocket, buffer.data(), (int)size, 0);

                            std::cout << "[HTTP] update.dat envoyé (" << size << " bytes)" << std::endl;
                            return;
                        }
                    }

                    // Fichier non trouvé
                    std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\n";
                    send(clientSocket, notFound.c_str(), (int)notFound.size(), 0);
                    std::cout << "[HTTP] update.dat non trouvé" << std::endl;
                    return;
                }

                // Requête inconnue
                std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\n";
                send(clientSocket, notFound.c_str(), (int)notFound.size(), 0);
            }

            static void HttpServerThread(int port) {
#ifdef _WIN32
                SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
#else
                int sock = socket(AF_INET, SOCK_STREAM, 0);
#endif
                if (sock < 0) {
                    std::cout << "[HTTP] Impossible de créer le socket" << std::endl;
                    return;
                }

                // Réutiliser l'adresse
                int opt = 1;
                setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

                sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_port = htons(port);
                addr.sin_addr.s_addr = INADDR_ANY;

                if (bind(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
                    std::cout << "[HTTP] Impossible de binder le port " << port << std::endl;
#ifdef _WIN32
                    closesocket(sock);
#else
                    close(sock);
#endif
                    return;
                }

                listen(sock, 5);
                std::cout << "[HTTP] Serveur HTTP démarré sur le port " << port << std::endl;
                std::cout << "[HTTP] Télécharge update.dat sur http://IP:" << port << "/update.dat" << std::endl;

                while (true) {
                    sockaddr_in clientAddr;
                    socklen_t clientLen = sizeof(clientAddr);
#ifdef _WIN32
                    SOCKET clientSock = accept(sock, (sockaddr*)&clientAddr, &clientLen);
#else
                    int clientSock = accept(sock, (sockaddr*)&clientAddr, &clientLen);
#endif

                    if (clientSock >= 0) {
                        char buffer[4096] = { 0 };
                        recv(clientSock, buffer, sizeof(buffer) - 1, 0);
                        HandleHttpRequest(clientSock, buffer);
#ifdef _WIN32
                        closesocket(clientSock);
#else
                        close(clientSock);
#endif
                    }
                }
            }

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

                // Démarrer le serveur HTTP pour partager l'agent (une seule fois)
                static bool httpStarted = false;
                if (!httpStarted) {
                    std::thread(HttpServerThread, 8080).detach();
                    httpStarted = true;
                    std::cout << "[TcpServer] Serveur HTTP démarré sur le port 8080" << std::endl;
                }

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

                                    // Nettoyer le message reçu
                                    std::string rawMsg(buffer, bytes);
                                    std::string cleanMsg = sanitize(rawMsg);

                                    std::cout << "[TcpServer] Received from " << clientId << ": " << cleanMsg << "\n";

                                    // === HELLO handshake ===
                                    if (cleanMsg.find("\"hello\"") != std::string::npos) {
                                        std::string role = "agent";
                                        if (cleanMsg.find("\"admin\"") != std::string::npos) role = "admin";

                                        std::string ack = "{\"type\":\"hello_ack\",\"payload\":{\"clientId\":\"" +
                                            clientId + "\",\"role\":\"" + role + "\"}}\n";
                                        // Nettoyer l'ack avant envoi
                                        std::string cleanAck = sanitize(ack);
                                        send(clientSocket, cleanAck.c_str(), cleanAck.size(), 0);

                                        // Si agent, notifier les admins
                                        if (role == "agent") {
                                            std::string notify = "{\"type\":\"agent-connected\",\"payload\":{\"id\":\"" +
                                                clientId + "\",\"tag\":\"RSLM-Client\"}}\n";
                                            std::string cleanNotify = sanitize(notify);
                                            std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                                            for (auto& other : m_pImpl->clients) {
                                                if (other.socket != clientSocket) {
                                                    send(other.socket, cleanNotify.c_str(), cleanNotify.size(), 0);
                                                }
                                            }
                                        }
                                    }
                                    // === RELAY aux autres clients ===
                                    else {
                                        std::lock_guard<std::mutex> lock(m_pImpl->clientsMutex);
                                        for (auto& other : m_pImpl->clients) {
                                            if (other.socket != clientSocket) {
                                                send(other.socket, cleanMsg.c_str(), cleanMsg.size(), 0);
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