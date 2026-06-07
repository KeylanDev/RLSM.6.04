#pragma once
#include <string>
#include <cstdint>

namespace rslm {
	namespace models {

		struct TcpConnection {
			std::string localAddress;
			uint16_t localPort = 0;
			std::string remoteAddress;
			uint16_t remotePort = 0;
			std::string state; // ESTABLISHED, LISTENING, ECT.
			uint32_t pid = 0;
		};
	}//namespace models
}//namespace rslm