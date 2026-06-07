#pragma once
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cstdint>

namespace rslm {
    namespace client {
        namespace networking {

            using OnConnectedCallback = std::function<void()>;
            using OnDisconnectedCallback = std::function<void()>;
            using OnMessageCallback = std::function<void(const std::string& type, const std::string& data)>;
            using OnErrorCallback = std::function<void(const std::string& error)>;

            class RSLMClient {
            public:
                RSLMClient();
                ~RSLMClient();

                RSLMClient(const RSLMClient&) = delete;
                RSLMClient& operator=(const RSLMClient&) = delete;

                bool Connect(const std::string& host, uint16_t port);
                void Disconnect();
                bool IsConnected() const;

                bool Send(const std::string& type, const std::string& data);
                bool SendBinary(const std::vector<uint8_t>& data);

                void SetOnConnected(OnConnectedCallback callback);
                void SetOnDisconnected(OnDisconnectedCallback callback);
                void SetOnMessage(OnMessageCallback callback);
                void SetOnError(OnErrorCallback callback);

                std::string GetClientId() const;
                void SetClientId(const std::string& id);

                void SetClientTag(const std::string& tag);
                std::string GetClientTag() const;

            private:
                class Impl;
                std::unique_ptr<Impl> m_pImpl;
            };

        } // namespace networking
    } // namespace client
} // namespace rslm