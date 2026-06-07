#pragma once
#include <cstdint>

namespace rslm {

	enum class AccountType : uint8_t {
		User    = 0,
		Admin   = 1,
		Systeme = 2,
		Guest   = 3
	};
}