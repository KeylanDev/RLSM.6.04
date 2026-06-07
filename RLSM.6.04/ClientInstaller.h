#pragma once
#include <string>
#include <functional>

namespace rslm {
	namespace client {
		namespace setup {

			using InstallCallback = std::function<void(bool success, const std::string& error)>;
			
			class ClientInstaller {
			public:
				//Install le client sur la machine
				static void Install(const std::string& installPath, InstallCallback callback);

				//Verifie si le client est installe
				static bool IsInstalled();

				//Recupere le chemin d'installation
				static std::string GetInstallPath();
			};
		}//namespace setup
	}//namespace client
}//namespace rslm