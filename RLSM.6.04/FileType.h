#pragma once
#include <cstdint>

namespace rslm {

	enum class FileType : uint8_t {
		File      = 0,
		Directory = 1,
		Drive     = 2,
		Symlink   = 3
	};
} 