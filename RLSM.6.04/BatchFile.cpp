#include "BatchFile.h"

#ifdef _WIN32
#include <windows.h>
#endif

#include <fstream>
#include <cstdio>
#include <stdexcept>

namespace rslm {
    namespace client {
        namespace io {

            bool BatchFile::Execute(const std::string& commands) {
#ifdef _WIN32
                char tempPath[MAX_PATH];
                char tempFile[MAX_PATH];

                if (!GetTempPathA(MAX_PATH, tempPath)) return false;
                if (!GetTempFileNameA(tempPath, "rslm", 0, tempFile)) return false;

                std::string batPath = std::string(tempFile) + ".bat";

                if (!Save(batPath, commands)) return false;

                std::string cmd = "cmd.exe /c \"" + batPath + "\"";
                STARTUPINFOA si = { sizeof(STARTUPINFOA) };
                si.dwFlags = STARTF_USESHOWWINDOW;
                si.wShowWindow = SW_HIDE;

                PROCESS_INFORMATION pi = {};

                bool success = CreateProcessA(nullptr, const_cast<char*>(cmd.c_str()),
                    nullptr, nullptr, FALSE,
                    CREATE_NO_WINDOW, nullptr, nullptr,
                    &si, &pi) != FALSE;

                if (success) {
                    WaitForSingleObject(pi.hProcess, 30000); // 30 secondes max
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }

                std::remove(batPath.c_str());
                return success;
#else
                return false;
#endif
            }

            bool BatchFile::Save(const std::string& path, const std::string& commands) {
                std::ofstream file(path);
                if (!file) return false;

                file << "@echo off\r\n";
                file << commands;
                file.close();
                return true;
            }

            bool BatchFile::Run(const std::string& path) {
#ifdef _WIN32
                std::string cmd = "cmd.exe /c \"" + path + "\"";

                STARTUPINFOA si = { sizeof(STARTUPINFOA) };
                si.dwFlags = STARTF_USESHOWWINDOW;
                si.wShowWindow = SW_HIDE;

                PROCESS_INFORMATION pi = {};

                bool success = CreateProcessA(nullptr, const_cast<char*>(cmd.c_str()),
                    nullptr, nullptr, FALSE,
                    CREATE_NO_WINDOW, nullptr, nullptr,
                    &si, &pi) != FALSE;

                if (success) {
                    WaitForSingleObject(pi.hProcess, 30000);
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }

                return success;
#else
                return false;
#endif
            }

        } // namespace io
    } // namespace client
} // namespace rslm