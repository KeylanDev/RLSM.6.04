#pragma once
#include <string>
#include <cstdint>

namespace rslm {
    namespace client {
        namespace helper {

            class SystemHelper {
            public:
                static std::string GetPcName();
                static std::string GetUsername();
                static std::string GetOSName();
                static std::string GetOSVersion();
                static std::string GetArchitecture();
                static bool IsAdmin();
                static std::string GetHardwareId();
                static uint32_t GetCpuCores();
                static std::string GetCpuName();
                static uint64_t GetTotalRam();
                static uint64_t GetAvailableRam();
                static std::string GetGpuName();
                static std::string GetIpAddress();
                static std::string GetMacAddress();
            };

        } // namespace helper
    } // namespace client
} // namespace rslm