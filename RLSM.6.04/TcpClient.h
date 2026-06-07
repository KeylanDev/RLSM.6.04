#pragma once
#include "Client.h"
#include <memory>

namespace rslm {
	namespace client {
		namespace networking {

			class TcpClient : public Client {
			public:
				TcpClient();
				~TcpClient() override;

				bool Connect(const std::string& host, uint16_t port) override;
				void Disconnect() override;
				bool IsConnected() const override;
				bool Send(const std::vector<uint8_t>& data) override;
				void StartReceiving(std::function<void(const std::vector<uint8_t>&)> onData,
					                std::function<void()> onDisconnect) override;
				bool EnableTLS(const std::string& certPath, const std::string& keyPath) override;
				std::string GetRemoteAddress() const override;
				uint16_t GetRemotePort() const override;

			private:
				class Impl;
				std::unique_ptr<Impl> m_pImpl;
			};
		}//namespace networking
	}//namespace client
}//namespace rslm