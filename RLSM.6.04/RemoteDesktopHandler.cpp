#include "RemoteDesktopHandler.h"
#include "ScreenHelper.h"
#include "MouseAction.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>

namespace rslm {
    namespace client {
        namespace messages {

            static std::atomic<bool> g_streaming{ false };
            static FrameCallback g_frameCallback;
            static std::thread g_streamThread;
            static int g_quality = 75;
            static int g_fps = 10;

            void RemoteDesktopHandler::Start(int quality, int fps, FrameCallback onFrame) {
                Stop();
                g_quality = quality;
                g_fps = fps > 0 ? fps : 10;
                g_frameCallback = std::move(onFrame);
                g_streaming = true;

                g_streamThread = std::thread([]() {
                    int width = 0, height = 0;
                    helper::ScreenHelper::GetScreenSize(width, height);
                    if (width <= 0)  width = 1920;
                    if (height <= 0) height = 1080;

                    while (g_streaming) {
                        try {
                            auto jpeg = helper::ScreenHelper::CaptureScreenJpeg(
                                0, 0, width, height, g_quality);

                            if (!jpeg.empty() && g_frameCallback) {
                                g_frameCallback(jpeg, width, height);
                            }
                        }
                        catch (...) {}

                        auto delay = std::chrono::milliseconds(1000 / (g_fps > 0 ? g_fps : 10));
                        std::this_thread::sleep_for(delay);
                    }
                    });
            }

            void RemoteDesktopHandler::Stop() {
                g_streaming = false;
                if (g_streamThread.joinable()) {
                    g_streamThread.join();
                }
                g_frameCallback = nullptr;
            }

            bool RemoteDesktopHandler::IsStreaming() {
                return g_streaming;
            }

            void RemoteDesktopHandler::SendMouseEvent(int x, int y, int action, int wheelDelta) {
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

            void RemoteDesktopHandler::SendKeyEvent(int keyCode, bool keyDown) {
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