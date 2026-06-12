#include "Shell.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#endif

#include <atomic>
#include <chrono>
#include <mutex>
#include <thread>
#include <vector>

namespace rslm {
    namespace client {
        namespace io {

#ifdef _WIN32

            struct ShellData {
                HANDLE hProcess = nullptr;
                HANDLE hThread = nullptr;
                HANDLE hStdIn = nullptr;
                HANDLE hStdOut = nullptr;
                std::atomic<bool> alive{ false };
                std::mutex ioMutex;
                std::function<void(const std::string&)> onOutput;
                std::function<void()> onClose;
            };

            void* Shell::Open(const std::string& shellPath,
                std::function<void(const std::string&)> onOutput,
                std::function<void()> onClose) {
                auto* data = new ShellData();
                data->onOutput = std::move(onOutput);
                data->onClose = std::move(onClose);

                HANDLE hReadPipe = nullptr;
                HANDLE hWritePipe = nullptr;
                HANDLE hReadIn = nullptr;
                HANDLE hWriteIn = nullptr;

                SECURITY_ATTRIBUTES sa = { sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };

                if (!CreatePipe(&hReadPipe, &hWritePipe, &sa, 0)) {
                    delete data;
                    return nullptr;
                }

                if (!CreatePipe(&hReadIn, &hWriteIn, &sa, 0)) {
                    CloseHandle(hReadPipe);
                    CloseHandle(hWritePipe);
                    delete data;
                    return nullptr;
                }

                STARTUPINFOA si = {};
                si.cb = sizeof(STARTUPINFOA);
                si.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
                si.wShowWindow = SW_HIDE;
                si.hStdOutput = hWritePipe;
                si.hStdError = hWritePipe;
                si.hStdInput = hReadIn;

                PROCESS_INFORMATION pi = {};

                std::string cmd = shellPath.empty() ? "cmd.exe" : shellPath;
                std::vector<char> cmdLine(cmd.begin(), cmd.end());
                cmdLine.push_back('\0');

                if (!CreateProcessA(nullptr, cmdLine.data(),
                    nullptr, nullptr, TRUE, CREATE_NO_WINDOW,
                    nullptr, nullptr, &si, &pi)) {
                    CloseHandle(hReadPipe);
                    CloseHandle(hWritePipe);
                    CloseHandle(hReadIn);
                    CloseHandle(hWriteIn);
                    delete data;
                    return nullptr;
                }

                data->hProcess = pi.hProcess;
                data->hThread = pi.hThread;
                data->hStdIn = hWriteIn;
                data->hStdOut = hReadPipe;
                data->alive = true;

                CloseHandle(hWritePipe);
                CloseHandle(hReadIn);

                // Thread de lecture avec buffer agrandi et envoi par morceaux
                std::thread([data]() {
                    char buffer[65536];  // Buffer agrandi à 64KB
                    while (data->alive) {
                        DWORD bytesRead = 0;
                        BOOL ok = ReadFile(data->hStdOut, buffer, sizeof(buffer) - 1, &bytesRead, nullptr);

                        if (!ok || bytesRead == 0) {
                            DWORD exitCode = STILL_ACTIVE;
                            if (data->hProcess)
                                GetExitCodeProcess(data->hProcess, &exitCode);

                            if (exitCode != STILL_ACTIVE)
                                break;

                            std::this_thread::sleep_for(std::chrono::milliseconds(50));
                            continue;
                        }

                        buffer[bytesRead] = '\0';
                        if (data->onOutput) {
                            // Envoyer par petits morceaux pour éviter les timeouts
                            std::string output(buffer, bytesRead);
                            size_t pos = 0;
                            while (pos < output.size()) {
                                size_t chunk = std::min<size_t>(4096, output.size() - pos);
                                data->onOutput(output.substr(pos, chunk));
                                pos += chunk;
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            }
                        }
                    }

                    data->alive = false;
                    if (data->onClose)
                        data->onClose();
                    }).detach();

                return data;
            }

            void Shell::Write(void* handle, const std::string& command) {
                if (!handle) return;
                auto* data = static_cast<ShellData*>(handle);
                if (!data->alive) return;

                std::lock_guard<std::mutex> lock(data->ioMutex);
                if (!data->hStdIn) return;

                std::string cmd = command + "\r\n";
                DWORD written = 0;
                WriteFile(data->hStdIn, cmd.c_str(), static_cast<DWORD>(cmd.size()), &written, nullptr);
                FlushFileBuffers(data->hStdIn);
            }

            void Shell::Close(void* handle) {
                if (!handle) return;
                auto* data = static_cast<ShellData*>(handle);

                data->alive = false;

                {
                    std::lock_guard<std::mutex> lock(data->ioMutex);
                    if (data->hStdIn) {
                        WriteFile(data->hStdIn, "exit\r\n", 6, nullptr, nullptr);
                        FlushFileBuffers(data->hStdIn);
                        CloseHandle(data->hStdIn);
                        data->hStdIn = nullptr;
                    }
                    if (data->hStdOut) {
                        CloseHandle(data->hStdOut);
                        data->hStdOut = nullptr;
                    }
                }

                if (data->hProcess) {
                    WaitForSingleObject(data->hProcess, 2000);
                    TerminateProcess(data->hProcess, 0);
                    CloseHandle(data->hProcess);
                    data->hProcess = nullptr;
                }
                if (data->hThread) {
                    CloseHandle(data->hThread);
                    data->hThread = nullptr;
                }

                delete data;
            }

            bool Shell::IsAlive(void* handle) {
                if (!handle) return false;
                auto* data = static_cast<ShellData*>(handle);
                if (!data->alive) return false;

                if (data->hProcess) {
                    DWORD exitCode = STILL_ACTIVE;
                    if (GetExitCodeProcess(data->hProcess, &exitCode) && exitCode != STILL_ACTIVE) {
                        data->alive = false;
                        return false;
                    }
                }
                return true;
            }

#else

            struct ShellData {
                pid_t pid = -1;
                int stdinWrite = -1;
                int stdoutRead = -1;
                std::atomic<bool> alive{ false };
                std::mutex ioMutex;
                std::function<void(const std::string&)> onOutput;
                std::function<void()> onClose;
            };

            void* Shell::Open(const std::string& shellPath,
                std::function<void(const std::string&)> onOutput,
                std::function<void()> onClose) {
                auto* data = new ShellData();
                data->onOutput = std::move(onOutput);
                data->onClose = std::move(onClose);

                int stdoutPipe[2];
                int stdinPipe[2];
                if (pipe(stdoutPipe) != 0 || pipe(stdinPipe) != 0) {
                    delete data;
                    return nullptr;
                }

                pid_t pid = fork();
                if (pid < 0) {
                    close(stdoutPipe[0]); close(stdoutPipe[1]);
                    close(stdinPipe[0]); close(stdinPipe[1]);
                    delete data;
                    return nullptr;
                }

                if (pid == 0) {
                    close(stdoutPipe[0]);
                    close(stdinPipe[1]);
                    dup2(stdoutPipe[1], STDOUT_FILENO);
                    dup2(stdoutPipe[1], STDERR_FILENO);
                    dup2(stdinPipe[0], STDIN_FILENO);
                    close(stdoutPipe[1]);
                    close(stdinPipe[0]);

                    const char* shell = shellPath.empty() ? "/bin/bash" : shellPath.c_str();
                    execl(shell, shell, "-i", nullptr);
                    _exit(127);
                }

                close(stdoutPipe[1]);
                close(stdinPipe[0]);

                data->pid = pid;
                data->stdoutRead = stdoutPipe[0];
                data->stdinWrite = stdinPipe[1];
                data->alive = true;

                std::thread([data]() {
                    char buffer[65536];
                    while (data->alive) {
                        ssize_t bytesRead = read(data->stdoutRead, buffer, sizeof(buffer) - 1);
                        if (bytesRead <= 0) break;

                        buffer[bytesRead] = '\0';
                        if (data->onOutput) {
                            std::string output(buffer, bytesRead);
                            size_t pos = 0;
                            while (pos < output.size()) {
                                size_t chunk = std::min<size_t>(4096, output.size() - pos);
                                data->onOutput(output.substr(pos, chunk));
                                pos += chunk;
                                std::this_thread::sleep_for(std::chrono::milliseconds(10));
                            }
                        }
                    }

                    data->alive = false;
                    if (data->onClose)
                        data->onClose();
                    }).detach();

                return data;
            }

            void Shell::Write(void* handle, const std::string& command) {
                if (!handle) return;
                auto* data = static_cast<ShellData*>(handle);
                if (!data->alive) return;

                std::lock_guard<std::mutex> lock(data->ioMutex);
                std::string cmd = command + "\n";
                write(data->stdinWrite, cmd.c_str(), cmd.size());
            }

            void Shell::Close(void* handle) {
                if (!handle) return;
                auto* data = static_cast<ShellData*>(handle);
                data->alive = false;

                {
                    std::lock_guard<std::mutex> lock(data->ioMutex);
                    if (data->stdinWrite >= 0) {
                        write(data->stdinWrite, "exit\n", 5);
                        close(data->stdinWrite);
                        data->stdinWrite = -1;
                    }
                    if (data->stdoutRead >= 0) {
                        close(data->stdoutRead);
                        data->stdoutRead = -1;
                    }
                }

                if (data->pid > 0) {
                    kill(data->pid, SIGTERM);
                    waitpid(data->pid, nullptr, WNOHANG);
                }

                delete data;
            }

            bool Shell::IsAlive(void* handle) {
                if (!handle) return false;
                auto* data = static_cast<ShellData*>(handle);
                if (!data->alive) return false;

                if (data->pid > 0) {
                    int status = 0;
                    pid_t result = waitpid(data->pid, &status, WNOHANG);
                    if (result == data->pid) {
                        data->alive = false;
                        return false;
                    }
                }
                return true;
            }

#endif

        } // namespace io
    } // namespace client
} // namespace rslm