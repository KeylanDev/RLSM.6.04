#pragma once
#include <vector>
#include <functional>
#include <cstdint>
#include "TcpConnection.h"

namespace rslm {
	namespace client {
		namespace messages {

			using models::TcpConnection;
			using TcpListCallback = std::function<void(const std::vector<TcpConnection>&)>;

			class TcpConnectionsHandler {
			public:
				static void GetConnections(TcpListCallback callback);
			};
		}//namespace messages
	}//namespace client
}//namespace rslm