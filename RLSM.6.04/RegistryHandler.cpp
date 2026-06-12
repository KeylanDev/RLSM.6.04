#pragma warning(disable: 4996)
#define _CRT_SECURE_NO_WARNINGS
#include "RegistryHandler.h"
#include "NativeMethodsHelper.h"
#include <windows.h>
#include <iostream>
#include <vector>

namespace rslm {
    namespace client {
        namespace messages {

            // Convertit un nom de racine en handle Windows
            HKEY GetRootKey(const std::string& path) {
                if (path == "HKEY_LOCAL_MACHINE" || path == "HKLM")
                    return HKEY_LOCAL_MACHINE;
                if (path == "HKEY_CURRENT_USER" || path == "HKCU")
                    return HKEY_CURRENT_USER;
                if (path == "HKEY_CLASSES_ROOT" || path == "HKCR")
                    return HKEY_CLASSES_ROOT;
                if (path == "HKEY_CURRENT_CONFIG")
                    return HKEY_CURRENT_CONFIG;
                if (path == "HKEY_USERS" || path == "HKU")
                    return HKEY_USERS;
                return nullptr;
            }

            // Extrait le sous-chemin après la racine
            std::string GetSubKeyPath(const std::string& path) {
                size_t pos = path.find('\\');
                if (pos != std::string::npos) {
                    return path.substr(pos + 1);
                }
                return "";
            }

            // Convertir le type de registre en string
            std::string RegTypeToString(DWORD type) {
                switch (type) {
                case REG_SZ: return "REG_SZ";
                case REG_EXPAND_SZ: return "REG_EXPAND_SZ";
                case REG_BINARY: return "REG_BINARY";
                case REG_DWORD: return "REG_DWORD";
                case REG_QWORD: return "REG_QWORD";
                case REG_MULTI_SZ: return "REG_MULTI_SZ";
                default: return "REG_UNKNOWN";
                }
            }

            // Lire les valeurs d'une clé de registre
            void RegistryHandler::ReadKey(const std::string& path, RegistryCallback callback) {
                std::vector<RegistryValue> values;

                std::cout << "[Registry] ReadKey - path: " << path << std::endl;

                HKEY rootKey = GetRootKey(path);
                if (!rootKey) {
                    std::cout << "[Registry] rootKey is NULL" << std::endl;
                    if (callback) callback(values);
                    return;
                }

                std::string subPath = GetSubKeyPath(path);
                std::cout << "[Registry] subPath: '" << subPath << "'" << std::endl;

                HKEY hKey = nullptr;
                LONG result = RegOpenKeyExA(rootKey, subPath.c_str(), 0, KEY_READ, &hKey);
                std::cout << "[Registry] RegOpenKeyExA result: " << result << std::endl;

                if (result != ERROR_SUCCESS) {
                    if (callback) callback(values);
                    return;
                }

                DWORD index = 0;
                char valueName[256];
                DWORD valueNameSize = 256;
                DWORD valueType = 0;
                BYTE valueData[65536];
                DWORD valueDataSize = 65536;

                while (true) {
                    valueNameSize = 256;
                    valueDataSize = 65536;

                    result = RegEnumValueA(hKey, index, valueName, &valueNameSize, nullptr, &valueType, valueData, &valueDataSize);

                    if (result == ERROR_NO_MORE_ITEMS) break;
                    if (result == ERROR_SUCCESS) {
                        RegistryValue val;
                        val.name = std::string(valueName, valueNameSize);
                        val.type = RegTypeToString(valueType);

                        if (valueType == REG_SZ || valueType == REG_EXPAND_SZ) {
                            val.data = std::string((char*)valueData);
                        }
                        else if (valueType == REG_DWORD) {
                            DWORD dwValue = *(DWORD*)valueData;
                            val.data = std::to_string(dwValue);
                        }
                        else if (valueType == REG_QWORD) {
                            ULONGLONG qwValue = *(ULONGLONG*)valueData;
                            val.data = std::to_string(qwValue);
                        }
                        else if (valueType == REG_BINARY) {
                            char hex[1024] = { 0 };
                            for (DWORD i = 0; i < valueDataSize && i < 256; i++) {
                                sprintf(hex + strlen(hex), "%02X ", valueData[i]);
                            }
                            val.data = hex;
                        }
                        else {
                            val.data = "(binary data)";
                        }

                        values.push_back(val);
                        std::cout << "[Registry] Found value: " << val.name << " = " << val.data << std::endl;
                    }
                    index++;
                }

                RegCloseKey(hKey);
                std::cout << "[Registry] Total values found: " << values.size() << std::endl;
                if (callback) callback(values);
            }

            // Énumérer les sous-clés
            void RegistryHandler::EnumKeys(const std::string& path, RegistryCallback callback) {
                std::vector<RegistryValue> values;

                std::cout << "[Registry] EnumKeys - path: " << path << std::endl;

                HKEY rootKey = GetRootKey(path);
                if (!rootKey) {
                    std::cout << "[Registry] rootKey is NULL" << std::endl;
                    if (callback) callback(values);
                    return;
                }

                std::string subPath = GetSubKeyPath(path);
                std::cout << "[Registry] subPath: '" << subPath << "'" << std::endl;

                HKEY hKey = nullptr;
                LONG result = RegOpenKeyExA(rootKey, subPath.c_str(), 0, KEY_READ, &hKey);
                std::cout << "[Registry] RegOpenKeyExA result: " << result << std::endl;

                if (result != ERROR_SUCCESS) {
                    if (callback) callback(values);
                    return;
                }

                DWORD index = 0;
                char subKeyName[256];
                DWORD subKeyNameSize = 256;

                while (true) {
                    subKeyNameSize = 256;
                    result = RegEnumKeyExA(hKey, index, subKeyName, &subKeyNameSize,
                        nullptr, nullptr, nullptr, nullptr);

                    if (result == ERROR_NO_MORE_ITEMS) break;
                    if (result == ERROR_SUCCESS) {
                        RegistryValue val;
                        val.name = std::string(subKeyName, subKeyNameSize);
                        val.type = "KEY";
                        val.data = "";
                        values.push_back(val);
                        std::cout << "[Registry] Found subkey: " << val.name << std::endl;
                    }
                    index++;
                }

                RegCloseKey(hKey);
                std::cout << "[Registry] Total subkeys found: " << values.size() << std::endl;
                if (callback) callback(values);
            }

            void RegistryHandler::WriteValue(const std::string& path,
                const std::string& name,
                const std::string& type,
                const std::string& data,
                RegistryStatusCallback callback) {
                bool success = false;

                HKEY rootKey = GetRootKey(path);
                if (!rootKey) {
                    if (callback) callback(success);
                    return;
                }

                std::string subPath = GetSubKeyPath(path);
                HKEY hKey = nullptr;
                LONG result = RegOpenKeyExA(rootKey, subPath.c_str(), 0, KEY_SET_VALUE, &hKey);

                if (result == ERROR_SUCCESS) {
                    DWORD dwType = REG_SZ;
                    if (type == "REG_DWORD") dwType = REG_DWORD;
                    else if (type == "REG_QWORD") dwType = REG_QWORD;
                    else if (type == "REG_BINARY") dwType = REG_BINARY;

                    result = RegSetValueExA(hKey, name.c_str(), 0, dwType, (const BYTE*)data.c_str(), static_cast<DWORD>(data.size()));
                    success = (result == ERROR_SUCCESS);
                    RegCloseKey(hKey);
                }

                if (callback) callback(success);
            }

            void RegistryHandler::DeleteValue(const std::string& path,
                const std::string& name,
                RegistryStatusCallback callback) {
                bool success = false;

                HKEY rootKey = GetRootKey(path);
                if (!rootKey) {
                    if (callback) callback(success);
                    return;
                }

                std::string subPath = GetSubKeyPath(path);
                HKEY hKey = nullptr;
                LONG result = RegOpenKeyExA(rootKey, subPath.c_str(), 0, KEY_SET_VALUE, &hKey);

                if (result == ERROR_SUCCESS) {
                    result = RegDeleteValueA(hKey, name.c_str());
                    success = (result == ERROR_SUCCESS);
                    RegCloseKey(hKey);
                }

                if (callback) callback(success);
            }

            void RegistryHandler::DeleteKey(const std::string& path,
                RegistryStatusCallback callback) {
                bool success = false;

                size_t pos = path.find_last_of('\\');
                if (pos != std::string::npos) {
                    std::string parentPath = path.substr(0, pos);
                    std::string keyName = path.substr(pos + 1);

                    HKEY rootKey = GetRootKey(parentPath);
                    if (!rootKey) {
                        if (callback) callback(success);
                        return;
                    }

                    std::string subPath = GetSubKeyPath(parentPath);
                    HKEY hKey = nullptr;
                    LONG result = RegOpenKeyExA(rootKey, subPath.c_str(), 0, KEY_SET_VALUE, &hKey);

                    if (result == ERROR_SUCCESS) {
                        result = RegDeleteKeyA(hKey, keyName.c_str());
                        success = (result == ERROR_SUCCESS);
                        RegCloseKey(hKey);
                    }
                }

                if (callback) callback(success);
            }

        } // namespace messages
    } // namespace client
} // namespace rslm