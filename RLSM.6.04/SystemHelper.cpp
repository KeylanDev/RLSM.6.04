#include "SystemHelper.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma warning(disable:4896)
#include <windows.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <comdef.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")
#else
#include <unistd.h>
#include <sys/utsname.h>
#include <pwd.h>
#endif

#include <stdexcept>

namespace rslm {
    namespace client {
        namespace helper {

            std::string SystemHelper::GetPcName() {
#ifdef _WIN32
                char buffer[MAX_COMPUTERNAME_LENGTH + 1] = {};
                DWORD size = sizeof(buffer);
                if (!GetComputerNameA(buffer, &size)) return "Unknown";
                return std::string(buffer);
#else
                struct utsname info;
                if (uname(&info) == 0) return std::string(info.nodename);
                return "Unknown";
#endif
            }

            std::string SystemHelper::GetUsername() {
#ifdef _WIN32
                char buffer[256] = {};
                DWORD size = sizeof(buffer);
                if (GetUserNameA(buffer, &size)) return std::string(buffer);
                return "Unknown";
#else
                uid_t uid = getuid();
                struct passwd* pw = getpwuid(uid);
                if (pw) return std::string(pw->pw_name);
                return "Unknown";
#endif
            }

            std::string SystemHelper::GetOSName() {
#ifdef _WIN32
                return "Windows";
#elif defined(__APPLE__)
                return "macOS";
#else
                return "Linux";
#endif
            }

            std::string SystemHelper::GetOSVersion() {
#ifdef _WIN32
                OSVERSIONINFOEXA info = {};
                info.dwOSVersionInfoSize = sizeof(info);
                typedef LONG(WINAPI* RtlGetVersionPtr)(OSVERSIONINFOEXA*);
                HMODULE ntdll = GetModuleHandleA("ntdll.dll");
                if (ntdll) {
                    auto RtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(GetProcAddress(ntdll, "RtlGetVersion"));
                    if (RtlGetVersion && RtlGetVersion(&info) == 0)
                        return std::to_string(info.dwMajorVersion) + "." + std::to_string(info.dwMinorVersion) + "." + std::to_string(info.dwBuildNumber);
                }
                return "Unknown";
#else
                struct utsname info;
                if (uname(&info) == 0) return std::string(info.release);
                return "Unknown";
#endif
            }

            std::string SystemHelper::GetArchitecture() {
#ifdef _WIN32
                SYSTEM_INFO info;
                GetNativeSystemInfo(&info);
                switch (info.wProcessorArchitecture) {
                case PROCESSOR_ARCHITECTURE_AMD64: return "x64";
                case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
                case PROCESSOR_ARCHITECTURE_ARM64: return "ARM64";
                default: return "Unknown";
                }
#else
                struct utsname info;
                if (uname(&info) == 0) return std::string(info.machine);
                return "Unknown";
#endif
            }

            bool SystemHelper::IsAdmin() {
#ifdef _WIN32
                BOOL isAdmin = FALSE;
                SID_IDENTIFIER_AUTHORITY ntAuth = SECURITY_NT_AUTHORITY;
                PSID adminGroup = nullptr;
                if (AllocateAndInitializeSid(&ntAuth, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
                    CheckTokenMembership(nullptr, adminGroup, &isAdmin);
                    FreeSid(adminGroup);
                }
                return isAdmin != FALSE;
#else
                return getuid() == 0;
#endif
            }

            std::string SystemHelper::GetHardwareId() {
#ifdef _WIN32
                char volumeName[MAX_PATH] = {}, fsName[MAX_PATH] = {};
                DWORD serialNumber = 0;
                if (GetVolumeInformationA("C:\\", volumeName, MAX_PATH, &serialNumber, nullptr, nullptr, fsName, MAX_PATH))
                    return std::to_string(serialNumber);
                return "Unknown";
#else
                return GetPcName();
#endif
            }

            uint32_t SystemHelper::GetCpuCores() {
#ifdef _WIN32
                SYSTEM_INFO info;
                GetSystemInfo(&info);
                return info.dwNumberOfProcessors;
#else
                return 1;
#endif
            }

            std::string SystemHelper::GetCpuName() {
#ifdef _WIN32
                HKEY hKey;
                if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                    char name[256] = {};
                    DWORD size = sizeof(name);
                    RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr, (LPBYTE)name, &size);
                    RegCloseKey(hKey);
                    return std::string(name);
                }
#endif
                return "Unknown CPU";
            }

            uint64_t SystemHelper::GetTotalRam() {
#ifdef _WIN32
                MEMORYSTATUSEX mem = {};
                mem.dwLength = sizeof(mem);
                GlobalMemoryStatusEx(&mem);
                return mem.ullTotalPhys;
#else
                return 0;
#endif
            }

            uint64_t SystemHelper::GetAvailableRam() {
#ifdef _WIN32
                MEMORYSTATUSEX mem = {};
                mem.dwLength = sizeof(mem);
                GlobalMemoryStatusEx(&mem);
                return mem.ullAvailPhys;
#else
                return 0;
#endif
            }

            std::string SystemHelper::GetGpuName() {
#ifdef _WIN32
                HKEY hKey;
                if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
                    char name[256] = {};
                    DWORD size = sizeof(name);
                    RegQueryValueExA(hKey, "DriverDesc", nullptr, nullptr, (LPBYTE)name, &size);
                    RegCloseKey(hKey);
                    return std::string(name);
                }
#endif
                return "Unknown GPU";
            }

            std::string SystemHelper::GetIpAddress() {
                char host[256];
                gethostname(host, sizeof(host));
                struct hostent* h = gethostbyname(host);
                if (h && h->h_addr_list[0])
                    return inet_ntoa(*(struct in_addr*)h->h_addr_list[0]);
                return "Unknown";
            }

            std::string SystemHelper::GetMacAddress() {
#ifdef _WIN32
                IP_ADAPTER_INFO info[16];
                DWORD size = sizeof(info);
                if (GetAdaptersInfo(info, &size) == ERROR_SUCCESS) {
                    char mac[18];
                    sprintf_s(mac, "%02X:%02X:%02X:%02X:%02X:%02X",
                        info[0].Address[0], info[0].Address[1], info[0].Address[2],
                        info[0].Address[3], info[0].Address[4], info[0].Address[5]);
                    return std::string(mac);
                }
#endif
                return "Unknown";
            }

        } // namespace helper
    } // namespace client
} // namespace rslm