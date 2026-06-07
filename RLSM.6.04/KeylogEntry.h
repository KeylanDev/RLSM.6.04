#pragma once
#include <string>
#include <cstdint>

namespace rslm {
	namespace models {
		
		struct KeylogEntry {
			std::string windowTitle;
			std::string text; // Le texte tape
			int64_t     timestamp = 0; // quand
		};
	} // namespace models
}//namespace rslm