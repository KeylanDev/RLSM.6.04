#pragma once
#include <string>
#include <functional>

namespace rslm {
	namespace client {
		namespace setup{

			using StartupCallback = std::function<void(bool success)>;

			class ClientStartup {
			public:
				//Ajoute au demarrage de Windows
				static void AddToStartup(const std::string& name, const std::string& path,
					                     StartupCallback callback);
				//Retire du demarrage
				static void RemoveFromStartup(const std::string& name, StartupCallback callback);

				//Verifie si present au demarrage
				static bool IsInStartup(const std::string& name);
			};
		}//namespace setup
	}//namespace client
}//namespace rslm