#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <memory>

namespace rslm {
	namespace client {
		namespace proxy {

			using DataCallback = std::function<void(const std::vector<uint8_t>&)>;
			using StatusCallback = std::function<void(bool connected)>;

			class ReverseProxyClient {
			public:
				ReverseProxyClient();
				~ReverseProxyClient();

				ReverseProxyClient(const ReverseProxyClient&) = delete;
				ReverseProxyClient& operator=(const ReverseProxyClient&) = delete;

				//Connecte au serveur proxy
				bool Connect(const std::string& host, uint16_t port);

				//Deconnecte
				void Disconnect();

				//verifie si connecte
				bool IsConnected() const;

				//Envoie des donnees au tunel
				bool Send(const std::vector<uint8_t>& data);

				//Callbacks
				void SetOnDataReceived(DataCallback callback);
				void SetOnStatusChanged(StatusCallback callback);

			private:
				class Impl;
				std::unique_ptr<Impl> m_pImpl;
			};

		}//namespace proxy
	}//namespace client
}//namespace rslm