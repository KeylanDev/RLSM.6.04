#pragma once
#include <string>
#include <functional>

namespace rslm {
	namespace client {
		namespace setup {

			using UninstallCallback = std::function<void(bool success, const std::string& error)>;

			class ClientUninstaller {
			public:
				static void Uninstall(UninstallCallback callback);
				static void CleanTraces();
			};
		}//Namespace setup
	}//Namespace client
}//namespace rslm