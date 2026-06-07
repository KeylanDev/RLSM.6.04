#pragma once
#include <string>
#include <functional>

namespace rslm {
	namespace client {
		namespace io {

			class Shell {
			public:
				static void* Open(const std::string& shellPath,
				                  std::function<void(const std::string&)> onOutput,
				                  std::function<void()> onClose = nullptr);

				static void Write(void* handle, const std::string& command);
				static void Close(void* handle);
				static bool IsAlive(void* handle);
			};
		}//namespace io
	}//namespace client
}//namespace rslm
