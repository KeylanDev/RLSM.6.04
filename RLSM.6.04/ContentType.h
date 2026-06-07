#pragma once
#include <cstdint>

namespace rslm {

	enum class ContentType : uint8_t {
		Text      = 0,
		Image     = 1,
		File      = 2,
		Command   = 3,
		Response  = 4,
		Stream    = 5
	};
}