#include "RemoteShellHandler.h"
#include "Shell.h"
#include "SystemHelper.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace rslm {
    namespace client {
        namespace messages {

            static void* g_shellHandle = nullptr;
            static ShellOutputCallback g_onOutput;
            static ShellCloseCallback g_onClose;

            std::string RemoteShellHandler::GetPrompt() {
#ifdef _WIN32
                char cwd[MAX_PATH] = {};
                if (GetCurrentDirectoryA(MAX_PATH, cwd) > 0)
                    return std::string(cwd) + ">";
#endif
                auto user = helper::SystemHelper::GetUsername();
                auto host = helper::SystemHelper::GetPcName();
                if (!user.empty() && !host.empty())
                    return user + "@" + host + ":~$ ";
                return "$ ";
            }

            bool RemoteShellHandler::Open(ShellOutputCallback onOutput, ShellCloseCallback onClose) {
                if (g_shellHandle && IsOpen()) {
                    g_onOutput = std::move(onOutput);
                    g_onClose = std::move(onClose);
                    return true;
                }

                Close();

                g_onOutput = std::move(onOutput);
                g_onClose = std::move(onClose);

                g_shellHandle = io::Shell::Open("",
                    [](const std::string& output) {
                        if (g_onOutput) g_onOutput(output);
                    },
                    []() {
                        g_shellHandle = nullptr;
                        if (g_onClose) g_onClose();
                    });

                return g_shellHandle != nullptr;
            }

            void RemoteShellHandler::SendCommand(const std::string& command) {
                if (!g_shellHandle || !IsOpen()) {
                    Open(g_onOutput, g_onClose);
                }
                if (g_shellHandle)
                    io::Shell::Write(g_shellHandle, command);
            }

            void RemoteShellHandler::Close() {
                if (g_shellHandle) {
                    io::Shell::Close(g_shellHandle);
                    g_shellHandle = nullptr;
                }
            }

            bool RemoteShellHandler::IsOpen() {
                return io::Shell::IsAlive(g_shellHandle);
            }

        } // namespace messages
    } // namespace client
} // namespace rslm
