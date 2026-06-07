#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rslm {
	namespace models{

		struct RecoveredPassword {
			std::string url;
			std::string username;
			std::string password;
			std::string application; //  "Chrome", "Firefox", "FilleZilla", etc.
		};

		using PasswordList = std::vector<RecoveredPassword>;
	}//namespace models
}//namespace rslm