#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace rslm {
	namespace client {
		namespace messages {

			using FrameCallback = std::function<void(const std::vector<uint8_t>& jpegData, int width, int height)>;

				class RemoteDesktopHandler {
				public:
					static void Start(int quality, int fps, FrameCallback onFrame);
					static void Stop();
					static bool IsStreaming();
					static void SendMouseEvent(int x, int y, int action, int wheelDelta = 0);
					static void SendKeyEvent(int keyCode, bool keyDown);
			};
		}//namespace messages
	}//namespace client
}//namespace rslm
