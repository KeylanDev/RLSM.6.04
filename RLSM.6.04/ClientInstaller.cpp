#include "ClientInstaller.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <fstream>

namespace rslm {
    namespace client {
        namespace setup {

            void ClientInstaller::Install(const std::string& installPath, InstallCallback callback) {
                bool success = false;
                std::string error;

#ifdef _WIN32
                if (CreateDirectoryA(installPath.c_str(), nullptr) ||
                    GetLastError() == ERROR_ALREADY_EXISTS) {
                    success = true;
                }
                else {
                    error = "Failed to create directory: " + installPath;
                }
#else
                success = true;
#endif

                if (callback) {
                    callback(success, error);
                }
            }

            bool ClientInstaller::IsInstalled() {
#ifdef _WIN32
                char path[MAX_PATH];
                if (GetModuleFileNameA(nullptr, path, MAX_PATH)) {
                    return std::ifstream(path).good();
                }
#endif
                return true;
            }

            std::string ClientInstaller::GetInstallPath() {
#ifdef _WIN32
                char path[MAX_PATH];
                if (GetModuleFileNameA(nullptr, path, MAX_PATH)) {
                    std::string fullPath(path);
                    size_t pos = fullPath.find_last_of("\\/");
                    if (pos != std::string::npos) {
                        return fullPath.substr(0, pos);
                    }
                }
#endif
                return ".";
            }

        } // namespace setup
    } // namespace client
} // namespace rslm