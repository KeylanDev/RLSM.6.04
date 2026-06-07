#pragma once
#include <cstdint>

namespace rslm {

	enum class MouseAction : uint8_t {
		LeftDown   = 0,
		LeftUp     = 1,
		RightDown  = 2,
		RightUp    = 3,
		Move       = 4,
		Scroll     = 5,
		MiddleDown = 6,
		MiddleUp   = 7 
	};
}