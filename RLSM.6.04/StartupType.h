#pragma once
#include <cstdint>

namespace rslm{

	enum class StartupType : uint8_t {
		Registry = 0,
		Folder   = 1,
		Service  = 2,
		Task     = 3
	};
} 