#include "ClientUninstaller.h"
#include "ClientStartup.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <cstdio>

namespace rslm {
    namespace client {
        namespace setup {

            void ClientUninstaller::Uninstall(UninstallCallback callback) {
                bool success = false;
                std::string error;

                // Retire du démarrage
                ClientStartup::RemoveFromStartup("RSLMClient", nullptr);

                success = true;

                if (callback) {
                    callback(success, error);
                }
            }

            void ClientUninstaller::CleanTraces() {
#ifdef _WIN32
                // Supprime la clé de registre
                RegDeleteKeyA(HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run\\RSLMClient");
#endif
            }

        } // namespace setup
    } // namespace client
} // namespace rslm