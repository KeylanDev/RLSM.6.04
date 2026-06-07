#include "ChromePassReader.h"
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

            void ChromePassReader::ReadPasswords(PasswordCallback callback) {
                std::vector<RecoveredPassword> passwords;
                if (callback) {
                    callback(passwords);
                }
            }

            std::string ChromePassReader::GetBrowserPath() {
#ifdef _WIN32
                char localAppData[MAX_PATH];
                if (SHGetFolderPathA(nullptr, CSIDL_LOCAL_APPDATA, nullptr, 0, localAppData) == S_OK) {
                    return std::string(localAppData) + "\\Google\\Chrome\\User Data\\Default";
                }
#endif
                return "";
            }

        } // namespace recovery
    } // namespace client
} // namespace rslm