#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include "MouseAction.h"

namespace rslm {
	namespace server {
		namespace messages {
			
			using FrameCallback = std::function<void(const std::vector<uint8_t>& jpegData)>;

			class ServerRemoteDesktopHandler {
			public:
				static void StartStream(const std::string& clientId, int quality, int fps,
					                    FrameCallback onFrame);
				static void StopStream(const std::string& clientId);
				static void SendMouseEvent(const std::string& clientId, int x, int y,
				                           MouseAction action, int wheelDelta = 0);
				static void SendKeyEvent(const std::string& clientId, int keyCode, bool keyDown);
			};
		}//namespace messages
	}//namespace server
}//namespace rslm