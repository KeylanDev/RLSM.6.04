#include "ShutdownHandler.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace rslm {
    namespace client {
        namespace messages {

            void ShutdownHandler::Execute(ShutdownAction action, ShutdownCallback callback) {
                bool success = true;

#ifdef _WIN32
                HANDLE hToken;
                if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
                    TOKEN_PRIVILEGES tp;
                    LookupPrivilegeValueA(nullptr, "SeShutdownPrivilege", &tp.Privileges[0].Luid);
                    tp.PrivilegeCount = 1;
                    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                    AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr);
                    CloseHandle(hToken);
                }

                UINT flags = 0;
                switch (action) {
                case ShutdownAction::Shutdown:  flags = EWX_SHUTDOWN | EWX_FORCE; break;
                case ShutdownAction::Restart:   flags = EWX_REBOOT | EWX_FORCE; break;
                case ShutdownAction::Logoff:    flags = EWX_LOGOFF | EWX_FORCE; break;
                default: break;
                }

                success = ExitWindowsEx(flags, SHTDN_REASON_MAJOR_OTHER) != FALSE;
#endif

                if (callback) callback(success);
            }

        } // namespace messages
    } // namespace client
} // namespace rslm