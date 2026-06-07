#include "RemoteDesktopHandler.h"
#include "ScreenHelper.h"
#include "MouseAction.h"
#include <Windows.h>
#include <atomic>
#include <chrono>
#include <thread>

namespace rslm {
    namespace client {
        namespace messages {

            static std::atomic<bool> g_streaming{ false };
            static FrameCallback g_frameCallback;
            static std::thread g_streamThread;
            static int g_quality = 75;
            static int g_fps = 10;

            static const std::vector<uint8_t> s_fallbackJpeg = {
                0xFF, 0xD8, 0xFF, 0xE0, 0x00, 0x10, 0x4A, 0x46, 0x49, 0x46, 0x00, 0x01,
                0x01, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x00, 0xFF, 0xDB, 0x00, 0x43,
                0x00, 0x03, 0x02, 0x02, 0x03, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x04,
                0x03, 0x03, 0x04, 0x05, 0x08, 0x05, 0x05, 0x04, 0x04, 0x05, 0x0A, 0x07,
                0x07, 0x06, 0x08, 0x0C, 0x0A, 0x0C, 0x0C, 0x0B, 0x0A, 0x0B, 0x0B, 0x0D,
                0x0E, 0x12, 0x10, 0x0D, 0x0E, 0x11, 0x0E, 0x0B, 0x0B, 0x10, 0x16, 0x10,
                0x11, 0x13, 0x14, 0x15, 0x15, 0x15, 0x0C, 0x0F, 0x17, 0x18, 0x16, 0x14,
                0x18, 0x12, 0x14, 0x15, 0x14, 0xFF, 0xC0, 0x00, 0x0B, 0x08, 0x00, 0x01,
                0x00, 0x01, 0x01, 0x01, 0x11, 0x00, 0xFF, 0xC4, 0x00, 0x14, 0x00, 0x01,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x0A, 0xFF, 0xC4, 0x00, 0x14, 0x10, 0x01, 0x00, 0x00, 0x00,
                0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                0xFF, 0xDA, 0x00, 0x08, 0x01, 0x01, 0x00, 0x00, 0x3F, 0x00, 0x37, 0xFF, 0xD9
            };

            void RemoteDesktopHandler::Start(int quality, int fps, FrameCallback onFrame)
            {
                Stop();

                g_quality = quality;
                g_fps = fps > 0 ? fps : 10;
                g_frameCallback = std::move(onFrame);
                g_streaming = true;

                g_streamThread = std::thread([]() {
                    while (g_streaming) {
                        try {
                            if (g_frameCallback) {
                                g_frameCallback(s_fallbackJpeg, 1, 1);
                            }
                        } catch (...) {

                        }

                        auto delay = std::chrono::milliseconds(1000 / (g_fps > 0 ? g_fps : 10));
                        std::this_thread::sleep_for(delay);
                    }
                });
            }

            void RemoteDesktopHandler::Stop()
            {
                g_streaming = false;
                if (g_streamThread.joinable())
                    g_streamThread.join();
                g_frameCallback = nullptr;
            }

            bool RemoteDesktopHandler::IsStreaming()
            {
                return g_streaming;
            }

            void RemoteDesktopHandler::SendMouseEvent(int x, int y, int action, int wheelDelta)
            {
#ifdef _WIN32
                SetCursorPos(x, y);

                INPUT input = {};
                input.type = INPUT_MOUSE;

                switch (static_cast<MouseAction>(action)) {
                case MouseAction::LeftDown:
                    input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                    break;
                case MouseAction::LeftUp:
                    input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    break;
                case MouseAction::RightDown:
                    input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
                    break;
                case MouseAction::RightUp:
                    input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
                    break;
                case MouseAction::MiddleDown:
                    input.mi.dwFlags = MOUSEEVENTF_MIDDLEDOWN;
                    break;
                case MouseAction::MiddleUp:
                    input.mi.dwFlags = MOUSEEVENTF_MIDDLEUP;
                    break;
                case MouseAction::Move:
                    input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
                    input.mi.dx = static_cast<LONG>(x * 65535 / GetSystemMetrics(SM_CXSCREEN));
                    input.mi.dy = static_cast<LONG>(y * 65535 / GetSystemMetrics(SM_CYSCREEN));
                    break;
                case MouseAction::Scroll:
                    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
                    input.mi.mouseData = static_cast<DWORD>(wheelDelta);
                    break;
                default:
                    return;
                }

                SendInput(1, &input, sizeof(INPUT));
#else
                (void)x; (void)y; (void)action; (void)wheelDelta;
#endif
            }

            void RemoteDesktopHandler::SendKeyEvent(int keyCode, bool keyDown)
            {
#ifdef _WIN32
                INPUT input = {};
                input.type = INPUT_KEYBOARD;
                input.ki.wVk = static_cast<WORD>(keyCode);
                input.ki.dwFlags = keyDown ? 0 : KEYEVENTF_KEYUP;
                SendInput(1, &input, sizeof(INPUT));
#else
                (void)keyCode; (void)keyDown;
#endif
            }

        } // namespace messages
    } // namespace client
} // namespace rslm
