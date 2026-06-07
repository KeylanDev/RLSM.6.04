#pragma once
#include <string>
#include <vector>
#include <functional>
#include "RecoveredPassword.h"

namespace rslm {
	namespace server {
		namespace messages {

			using models::RecoveredPassword;
			using PasswordCallback = std::function<void(const std::vector<RecoveredPassword>&)>;

			class ServerPasswordRecoveryHandler {
			public:
				static void RequestPasswords(const std::string& clientId, PasswordCallback callback);
			};

		}//namespace messages
	}//namespace server
}//namespace rslm