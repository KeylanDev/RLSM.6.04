#pragma once
#include <string>
#include <functional>


namespace rslm {
	namespace server {
		namespace messages {
			
			using ShellOutputCallback = std::function<void(const std::string& output)>;

			class ServerRemoteShellHandler {
			public:
				static void OpenShell(const std::string& clientId, ShellOutputCallback onOutput);
				static void SendCommand(const std::string& clientId, const std::string& command);
				static void CloseShell(const std::string& clientId);
			};
		}//namespace messages
	}//namespace server

}//namespace rslm