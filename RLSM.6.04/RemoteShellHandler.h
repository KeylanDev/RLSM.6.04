#pragma once
#include <string>
#include <functional>

namespace rslm {
	namespace client {
		namespace messages {

			using ShellOutputCallback = std::function<void(const std::string& output)>;
			using ShellCloseCallback = std::function<void()>;

			class RemoteShellHandler {
			public:
				static bool Open(ShellOutputCallback onOutput, ShellCloseCallback onClose);
				static void SendCommand(const std::string& command);
				static void Close();
				static bool IsOpen();
				static std::string GetPrompt();
			};
		}//namespace messages
	}//namespace client
}//namespace rslm
