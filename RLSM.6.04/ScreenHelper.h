#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <functional>

namespace rslm {
	namespace client {
		namespace helper {

			class ScreenHelper {
			public:
				static bool GetScreenSize(int& width, int& height);

				struct ScreenInfo {
					int x, y;
					int width, height;
					bool isPrimary;
				};
				static std::vector<ScreenInfo> GetAllScreens();

				static std::vector<uint8_t> CaptureScreen(int x, int y, int width, int height);
				static std::vector<uint8_t> CaptureScreenJpeg(int x, int y, int width, int height, int quality = 75);
				static std::vector<uint8_t> CaptureScreenBmp(int x, int y, int width, int height);
			};
		}//namespace helper
	}//namespace client
}//namespace rslm
