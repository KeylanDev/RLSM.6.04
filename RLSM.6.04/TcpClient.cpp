#include "TcpClient.h"

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#include <thread>
#include <stdexcept>

namespace rslm {
    namespace client {
        namespace networking {

            struct TcpClient::Impl {
                int socketFd = -1;
                bool connected = false;
                bool running = false;
                std::string remoteAddress;
                uint16_t remotePort = 0;
                std::thread recvThread;
            };

            TcpClient::TcpClient() : m_pImpl(std::make_unique<Impl>()) {
#ifdef _WIN32
                WSADATA wsa;
                WSAStartup(MAKEWORD(2, 2), &wsa);
#endif
            }

            TcpClient::~TcpClient() {
                Disconnect();
            }

            bool TcpClient::Connect(const std::string& host, uint16_t port) {
                if (m_pImpl->connected) Disconnect();

                m_pImpl->socketFd = static_cast<int>(socket(AF_INET, SOCK_STREAM, 0));
                if (m_pImpl->socketFd < 0) return false;

                sockaddr_in addr = {};
                addr.sin_family = AF_INET;
                addr.sin_port = htons(port);

                if (inet_pton(AF_INET, host.c_str(), &addr.sin_addr) <= 0) {
#ifdef _WIN32
                    closesocket(m_pImpl->socketFd);
#else
                    close(m_pImpl->socketFd);
#endif
                    m_pImpl->socketFd = -1;
                    return false;
                }

                if (::connect(m_pImpl->socketFd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
#ifdef _WIN32
                    closesocket(m_pImpl->socketFd);
#else
                    close(m_pImpl->socketFd);
#endif
                    m_pImpl->socketFd = -1;
                    return false;
                }

                m_pImpl->connected = true;
                m_pImpl->remoteAddress = host;
                m_pImpl->remotePort = port;
                return true;
            }

            void TcpClient::Disconnect() {
                m_pImpl->running = false;

                if (m_pImpl->recvThread.joinable()) {
                    m_pImpl->recvThread.join();
                }

                if (m_pImpl->socketFd >= 0) {
#ifdef _WIN32
                    closesocket(m_pImpl->socketFd);
#else
                    close(m_pImpl->socketFd);
#endif
                    m_pImpl->socketFd = -1;
                }

                m_pImpl->connected = false;
            }

            bool TcpClient::IsConnected() const {
                return m_pImpl->connected;
            }

            bool TcpClient::Send(const std::vector<uint8_t>& data) {
                if (!m_pImpl->connected) return false;

                int sent = ::send(m_pImpl->socketFd,
                    reinterpret_cast<const char*>(data.data()),
                    static_cast<int>(data.size()), 0);
                return sent == static_cast<int>(data.size());
            }

            void TcpClient::StartReceiving(std::function<void(const std::vector<uint8_t>&)> onData,
                std::function<void()> onDisconnect) {
                m_pImpl->running = true;

                m_pImpl->recvThread = std::thread([this, onData, onDisconnect]() {
                    std::vector<uint8_t> buffer(65536);

                    while (m_pImpl->running) {
                        int bytes = ::recv(m_pImpl->socketFd,
                            reinterpret_cast<char*>(buffer.data()),
                            static_cast<int>(buffer.size()), 0);

                        if (bytes <= 0) {
                            m_pImpl->connected = false;
                            if (onDisconnect) onDisconnect();
                            break;
                        }

                        std::vector<uint8_t> received(buffer.begin(), buffer.begin() + bytes);
                        if (onData) onData(received);
                    }
                    });
            }

            bool TcpClient::EnableTLS(const std::string&, const std::string&) {
                return false; // À implémenter avec OpenSSL plus tard
            }

            std::string TcpClient::GetRemoteAddress() const {
                return m_pImpl->remoteAddress;
            }

            uint16_t TcpClient::GetRemotePort() const {
                return m_pImpl->remotePort;
            }

        } // namespace networking
    } // namespace client
} // namespace rslm