#include "ClientStartup.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace rslm {
    namespace client {
        namespace setup {

            void ClientStartup::AddToStartup(const std::string& name,
                const std::string& path,
                StartupCallback callback) {
                bool success = false;

#ifdef _WIN32
                HKEY hKey = nullptr;
                if (RegOpenKeyExA(HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                    0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
                    if (RegSetValueExA(hKey, name.c_str(), 0, REG_SZ,
                        reinterpret_cast<const BYTE*>(path.c_str()),
                        static_cast<DWORD>(path.size() + 1)) == ERROR_SUCCESS) {
                        success = true;
                    }
                    RegCloseKey(hKey);
                }
#endif

                if (callback) callback(success);
            }

            void ClientStartup::RemoveFromStartup(const std::string& name,
                StartupCallback callback) {
                bool success = false;

#ifdef _WIN32
                HKEY hKey = nullptr;
                if (RegOpenKeyExA(HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                    0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
                    if (RegDeleteValueA(hKey, name.c_str()) == ERROR_SUCCESS) {
                        success = true;
                    }
                    RegCloseKey(hKey);
                }
#endif

                if (callback) callback(success);
            }

            bool ClientStartup::IsInStartup(const std::string& name) {
#ifdef _WIN32
                HKEY hKey = nullptr;
                if (RegOpenKeyExA(HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                    0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                    DWORD type = 0;
                    DWORD size = 0;
                    LONG result = RegQueryValueExA(hKey, name.c_str(), nullptr, &type, nullptr, &size);
                    RegCloseKey(hKey);
                    return result == ERROR_SUCCESS;
                }
#endif
                return false;
            }

        } // namespace setup
    } // namespace client
} // namespace rslm