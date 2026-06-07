#include "RSLMClient.h"
#include <nlohmann/json.hpp>

namespace rslm {
    namespace client {
        namespace networking {

            using json = nlohmann::json;

            struct RSLMClient::Impl {
                std::string clientId;
                std::string clientTag;
                bool connected = false;

                OnConnectedCallback    onConnected;
                OnDisconnectedCallback onDisconnected;
                OnMessageCallback      onMessage;
                OnErrorCallback        onError;
            };

            RSLMClient::RSLMClient() : m_pImpl(std::make_unique<Impl>()) {
            }

            RSLMClient::~RSLMClient() {
                Disconnect();
            }

            bool RSLMClient::Connect(const std::string& host, uint16_t port) {
                m_pImpl->connected = true;
                if (m_pImpl->onConnected) {
                    m_pImpl->onConnected();
                }
                return true;
            }

            void RSLMClient::Disconnect() {
                m_pImpl->connected = false;
                if (m_pImpl->onDisconnected) {
                    m_pImpl->onDisconnected();
                }
            }

            bool RSLMClient::IsConnected() const {
                return m_pImpl->connected;
            }

            bool RSLMClient::Send(const std::string& type, const std::string& data) {
                if (!m_pImpl->connected) return false;

                json msg = {
                    {"type", type},
                    {"senderId", m_pImpl->clientId},
                    {"clientTag", m_pImpl->clientTag},
                    {"payload", data}
                };

                return !msg.dump().empty();
            }

            bool RSLMClient::SendBinary(const std::vector<uint8_t>& data) {
                return Send("binary", std::string(data.begin(), data.end()));
            }

            void RSLMClient::SetOnConnected(OnConnectedCallback callback) {
                m_pImpl->onConnected = std::move(callback);
            }

            void RSLMClient::SetOnDisconnected(OnDisconnectedCallback callback) {
                m_pImpl->onDisconnected = std::move(callback);
            }

            void RSLMClient::SetOnMessage(OnMessageCallback callback) {
                m_pImpl->onMessage = std::move(callback);
            }

            void RSLMClient::SetOnError(OnErrorCallback callback) {
                m_pImpl->onError = std::move(callback);
            }

            std::string RSLMClient::GetClientId() const {
                return m_pImpl->clientId;
            }

            void RSLMClient::SetClientId(const std::string& id) {
                m_pImpl->clientId = id;
            }

            void RSLMClient::SetClientTag(const std::string& tag) {
                m_pImpl->clientTag = tag;
            }

            std::string RSLMClient::GetClientTag() const {
                return m_pImpl->clientTag;
            }

        } // namespace networking
    } // namespace client
} // namespace rslm