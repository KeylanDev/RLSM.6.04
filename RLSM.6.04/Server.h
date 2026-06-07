#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>
#include <memory>

namespace rslm {
	namespace server {
		namespace networking {

			using OnClientConnected = std::function<void(const std::string& clientId)>;
			using OnClientDisconnected = std::function<void(const std::string& clientId)>;
			using OnDataReceived = std::function<void(const std::string& clientId, const std::vector<uint8_t>& data)>;

			class Server {
			public:
				Server();
				virtual ~Server();

				virtual bool Start(uint16_t port) = 0;
				virtual void Stop() = 0;
				virtual bool IsRunning() const = 0;
				virtual bool Send(const std::string& clientId, const std::vector<uint8_t>& data) = 0;
				virtual void DisconnectClient(const std::string& clientId) = 0;
				virtual std::vector<std::string> GetConnectedClients() const = 0;

				void SetOnClientConnected(OnClientConnected callback);
				void SetOnClientDisconnected(OnClientDisconnected callback);
				void SetOnDataReceived(OnDataReceived callback);

			protected:
				OnClientConnected m_onConnected;
				OnClientDisconnected m_onDisconnected;
				OnDataReceived m_onData;
			};
		} // namespace networking
	} //namespace server
} //namespace rslm