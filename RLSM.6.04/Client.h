#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace rslm {
	namespace client {
		namespace networking {

			class Client {
			public:
				Client();
				virtual ~Client();

				//Connexion TCP
				virtual bool Connect(const std::string& host, uint16_t port) = 0;

				//Deconnexion
				virtual void Disconnect() = 0;

				//Etat de la connexion
				virtual bool IsConnected() const = 0;

				//Envoie de donnees
				virtual bool Send(const std::vector<uint8_t>& data) = 0;

				//Reception de donnees (bloquant ou callback)
				virtual void StartReceiving(std::function<void(const std::vector<uint8_t>&)> onData,
					                        std::function<void()> onDisconnect) = 0;
				//TLS
				virtual bool EnableTLS(const std::string& certPath, const std::string& keyPath) = 0;
										
				// Infos connexion
				virtual std::string GetRemoteAddress() const = 0;
				virtual uint16_t GetRemotePort() const = 0;
			};
		}//namespace networking
	}//namespace client
}//namespace rslm
