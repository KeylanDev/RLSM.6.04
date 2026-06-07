#pragma once
#include <cstdint>

namespace rslm {
	
	enum class ProcessAction : uint8_t {
		Start       = 0,
		Kill        = 1,
		Suspend     = 2,
		Resume      = 3,
		SetPriority = 4
	};
}