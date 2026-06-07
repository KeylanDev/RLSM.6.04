#pragma once
#include <cstdint>

namespace rslm {

	enum class ReverseProxyType : uint8_t {
		SOCKS5 = 0,
		HTTP   = 1
	};
}