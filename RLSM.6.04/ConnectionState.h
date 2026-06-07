#pragma once
#include <cstdint>

namespace  rslm{
	
	enum class ConnectionState : uint8_t {
		Disconnected  = 0,
		Connecting	  = 1,
		Connected     = 2,
		Authenticated = 3
	};
} 