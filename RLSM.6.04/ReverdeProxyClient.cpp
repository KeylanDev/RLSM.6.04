#include "ReverseProxyClient.h"

namespace rslm {
    namespace client {
        namespace proxy {

            struct ReverseProxyClient::Impl {
                bool connected = false;
                DataCallback onData;
                StatusCallback onStatus;
            };

            ReverseProxyClient::ReverseProxyClient()
                : m_pImpl(std::make_unique<Impl>()) {
            }

            ReverseProxyClient::~ReverseProxyClient() {
                Disconnect();
            }

            bool ReverseProxyClient::Connect(const std::string& host, uint16_t port) {
                m_pImpl->connected = true;
                if (m_pImpl->onStatus) {
                    m_pImpl->onStatus(true);
                }
                return true;
            }

            void ReverseProxyClient::Disconnect() {
                m_pImpl->connected = false;
                if (m_pImpl->onStatus) {
                    m_pImpl->onStatus(false);
                }
            }

            bool ReverseProxyClient::IsConnected() const {
                return m_pImpl->connected;
            }

            bool ReverseProxyClient::Send(const std::vector<uint8_t>& data) {
                return m_pImpl->connected;
            }

            void ReverseProxyClient::SetOnDataReceived(DataCallback callback) {
                m_pImpl->onData = std::move(callback);
            }

            void ReverseProxyClient::SetOnStatusChanged(StatusCallback callback) {
                m_pImpl->onStatus = std::move(callback);
            }

        } // namespace proxy
    } // namespace client
} // namespace rslm