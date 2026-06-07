#pragma once
#include <string>
#include <cstdint>
#include "StartupType.h"

namespace rslm {
	namespace models {

		struct StartupItem {
			std::string name;
			std::string command;
			std::string registryPath;
			StartupType type = StartupType::Registry;
			bool	    enabled = true;
		};
	}//namespace models
}//namespace rslm