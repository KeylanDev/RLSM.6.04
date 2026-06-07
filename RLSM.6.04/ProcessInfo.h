#pragma once
#include <string>
#include <cstdint>

namespace rslm {
	namespace models {

		struct ProcessInfo {
			uint32_t pid = 0;
			std::string name;
			std::string windowTitle;
			uint32_t threads = 0;
			uint64_t memoryUsage = 0;
			bool is64Bit = false;
		};
	}//namespace models
}//namespaced rslm