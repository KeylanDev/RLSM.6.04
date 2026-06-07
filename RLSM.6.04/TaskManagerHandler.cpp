#include "TaskManagerHandler.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>
#include <string>
#endif

namespace rslm {
    namespace client {
        namespace messages {

            void TaskManagerHandler::GetProcesses(ProcessListCallback callback) {
                std::vector<ProcessInfo> processes;

#ifdef _WIN32
                HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
                if (hSnapshot != INVALID_HANDLE_VALUE) {
                    PROCESSENTRY32 pe32;
                    pe32.dwSize = sizeof(PROCESSENTRY32);

                    if (Process32First(hSnapshot, &pe32)) {
                        do {
                            ProcessInfo info;
                            info.pid = static_cast<uint32_t>(pe32.th32ProcessID);

                            // Convert WCHAR to std::string
                            int len = WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, nullptr, 0, nullptr, nullptr);
                            std::vector<char> buf(len);
                            WideCharToMultiByte(CP_ACP, 0, pe32.szExeFile, -1, buf.data(), len, nullptr, nullptr);
                            info.name = buf.data();

                            info.threads = static_cast<uint32_t>(pe32.cntThreads);

                            // Get memory usage
                            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pe32.th32ProcessID);
                            if (hProcess) {
                                PROCESS_MEMORY_COUNTERS pmc;
                                if (GetProcessMemoryInfo(hProcess, &pmc, sizeof(pmc))) {
                                    info.memoryUsage = pmc.WorkingSetSize;
                                }
                                CloseHandle(hProcess);
                            }

                            processes.push_back(info);
                        } while (Process32Next(hSnapshot, &pe32));
                    }
                    CloseHandle(hSnapshot);
                }
#endif

                if (callback) callback(processes);
            }

            void TaskManagerHandler::KillProcess(uint32_t pid, ProcessStatusCallback callback) {
                bool success = false;

#ifdef _WIN32
                HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
                if (hProcess) {
                    success = TerminateProcess(hProcess, 0) != FALSE;
                    CloseHandle(hProcess);
                }
#endif

                if (callback) callback(success);
            }

            void TaskManagerHandler::StartProcess(const std::string& path, ProcessStatusCallback callback) {
                bool success = false;

#ifdef _WIN32
                STARTUPINFOA si = {};
                si.cb = sizeof(si);
                PROCESS_INFORMATION pi = {};

                success = CreateProcessA(
                    nullptr,
                    const_cast<char*>(path.c_str()),
                    nullptr,
                    nullptr,
                    FALSE,
                    0,
                    nullptr,
                    nullptr,
                    &si,
                    &pi
                ) != FALSE;

                if (success) {
                    CloseHandle(pi.hProcess);
                    CloseHandle(pi.hThread);
                }
#endif

                if (callback) callback(success);
            }

            void TaskManagerHandler::SuspendProcess(uint32_t pid, ProcessStatusCallback callback) {
                bool success = false;
                if (callback) callback(success);
            }

            void TaskManagerHandler::ResumeProcess(uint32_t pid, ProcessStatusCallback callback) {
                bool success = false;
                if (callback) callback(success);
            }

        } // namespace messages
    } // namespace client
} // namespace rslm
