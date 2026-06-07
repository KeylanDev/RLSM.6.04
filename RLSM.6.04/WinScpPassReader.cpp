#include "WinScpPassReader.h"
#include <fstream>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")
#endif

namespace rslm {
    namespace client {
        namespace recovery {

            void WinScpPassReader::ReadPasswords(PasswordCallback callback) {
                std::vector<RecoveredPassword> passwords;

                if (callback) {
                    callback(passwords);
                }
            }

            std::string WinScpPassReader::GetConfigPath() {
#ifdef _WIN32
                char appData[MAX_PATH];
                if (SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, appData) == S_OK) {
                    return std::string(appData) + "\\Martin Prikryl\\WinSCP.ini";
                }
#endif
                return "";
            }

        } // namespace recovery
    } // namespace client
} // namespace rslm