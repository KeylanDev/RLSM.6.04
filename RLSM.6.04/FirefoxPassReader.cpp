#include "FirefoxPassReader.h"
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

            void FirefoxPassReader::ReadPasswords(PasswordCallback callback) {
                std::vector<RecoveredPassword> passwords;

                if (callback) {
                    callback(passwords);
                }
            }

            std::string FirefoxPassReader::GetProfilePath() {
#ifdef _WIN32
                char appData[MAX_PATH];
                if (SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, appData) == S_OK) {
                    return std::string(appData) + "\\Mozilla\\Firefox\\Profiles";
                }
#endif
                return "";
            }

        } // namespace recovery
    } // namespace client
} // namespace rslm