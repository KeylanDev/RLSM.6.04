#pragma once
#include <string>
#include <functional>
#include <cstdint>

namespace rslm {
	namespace server {
		namespace messages {

			using ProxyStatusCallback = std::function<void(bool success, const std::string& error)>;

			class ServerReverseProxyHandler {
			public:
				static void StartProxy(const std::string& clientId,
				                       const std::string& targetHost, uint16_t targetPort,
					                   ProxyStatusCallback callback);
				static void StopProxy(const std::string& clientId);
			};
		}//namespace messages
	}//namespace server
}//namespace rslm