#pragma once
#include <string>
#include <cstdint>
#include "AccountType.h"

namespace rslm {
	namespace models {

		struct SystemInfo {
			std::string computerName;
			std::string osName;
			std::string osVersion;
			std::string architecture;
			uint32_t    cpuCores = 0;
			uint64_t    totalRam = 0;
			uint64_t    availableRam = 0;
			std::string gpu;
			std::string username;
			std::string ipAddress;
			std::string macAddress;
			AccountType accountType = AccountType::User;
			bool	    isAdmin = false;
		};
	}//namespace models
}//namespace rslm