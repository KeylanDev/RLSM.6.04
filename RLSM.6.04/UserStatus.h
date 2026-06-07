#pragma once
#include <cstdint>

namespace rslm {

	enum class UserStatus : uint8_t {
		Idle   =  0,
		Active = 1,
		Locked = 2,
	};
}