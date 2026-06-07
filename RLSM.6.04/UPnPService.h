#pragma once
#include <string>
#include <functional>
#include <cstdint>

namespace rslm {
	namespace server {
		namespace networking {

			using UPnPCallback = std::function<void(bool success, const std::string& externalIp)>;

			class UPnPService {
			public:
				//Tente d'ouvrir un port via UPnP
				static bool OpenPort(uint16_t internalPort, uint16_t externalPort,
					const std::string& description = "RSLM Server");

				//Ferme un port ouvert via UPnP
				static bool ClosePort(uint16_t externalPort);

				//Verifie si UPnP est disponible sur le reseau
				static bool IsAvailable();

				//Recupere L'IP externe via UPnP
				static std::string GetExternalIP();

				//Tente tout automatiquement 
				static void AutoSetup(uint16_t port, UPnPCallback callback);
			};
		}// namespace networking 
	}//namespace server
}//namespace rslm