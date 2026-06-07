#pragma once
#include "Server.h"
#include <memory>

namespace rslm {
	namespace server {
		namespace networking {

			class TcpServer : public Server {
			public:
				TcpServer();
				~TcpServer() override;

				bool Start(uint16_t port) override;
				void Stop() override;
				bool IsRunning() const override;
				bool Send(const std::string& clientId, const std::vector<uint8_t>& data) override;
				void DisconnectClient(const std::string& clientId) override;
				std::vector<std::string> GetConnectedClients()  const override;

			private:
				class Impl;
				std::unique_ptr<Impl> m_pImpl;
			};
		}
	}
}