#pragma once
#include <string>
#include <cstdint>

namespace rslm {
	namespace client {
		namespace config {

			// Placeholders pour le builder (taille fixe dans le binaire)
			// Ces valeurs seront remplacées par le builder
			extern char g_serverHost[64];
			extern int g_serverPort;
			extern bool g_enablePersistence;
			extern bool g_hideOnStart;

			struct Settings {
				// Récupère les valeurs des placeholders
				std::string serverHost = std::string(g_serverHost);
				uint16_t serverPort = static_cast<uint16_t>(g_serverPort);
				std::string serverPassword = "";

				uint32_t reconnectDelay = 5;

				std::string clientTag = "RSLM-Client";
				std::string hardwareId = "";

				uint8_t desktopQuality = 75;
				uint8_t desktopFPS = 10;

				bool keyloggerEnable = false;
				bool autoStart = false;
				bool hideOnStart = g_hideOnStart;
			};
		}// namespace config
	}// namespace client
}// namespace rslm