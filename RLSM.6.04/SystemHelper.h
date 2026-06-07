#pragma once
#include <string>

namespace rslm {
    namespace client {
        namespace helper {

            class SystemHelper {
            public:
                static std::string GetPcName();  // au lieu de GetComputerName
                static std::string GetUsername();
                static std::string GetOSName();
                static std::string GetOSVersion();
                static std::string GetArchitecture();
                static bool IsAdmin();
                static std::string GetHardwareId();
            };

        } // namespace helper
    } // namespace client
} // namespace rslm