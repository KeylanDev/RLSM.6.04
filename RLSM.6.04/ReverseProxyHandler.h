#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace rslm {
	namespace client {
		namespace messages {

			using ProxyStatusCallback = std::function<void(bool success, const std::string& error)>;
			using ProxyDataCallback = std::function<void(const std::vector<uint8_t>& data)>;

			class ReverseProxyHandler {
			public: 
				static void Start(const std::string& targetHost, uint16_t targetPort,
					              ProxyStatusCallback callback);

				static void Stop();
				static void SendData(const std::vector<uint8_t>& data);
				static void SetOnDataReceived(ProxyDataCallback callback);
				static bool IsRunning();
			};
		}//namespace messages
	}//namespace client
}//namespace rslm