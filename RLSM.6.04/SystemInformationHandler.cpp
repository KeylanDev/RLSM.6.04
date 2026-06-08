#include "SystemInformationHandler.h"
#include "SystemHelper.h"

namespace rslm {
    namespace client {
        namespace messages {

            void SystemInformationHandler::GetSystemInfo(SystemInfoCallback callback) {
                SystemInfo info;
                info.computerName = helper::SystemHelper::GetPcName();
                info.username = helper::SystemHelper::GetUsername();
                info.osName = helper::SystemHelper::GetOSName();
                info.osVersion = helper::SystemHelper::GetOSVersion();
                info.architecture = helper::SystemHelper::GetArchitecture();
                info.isAdmin = helper::SystemHelper::IsAdmin();
                info.cpuCores = helper::SystemHelper::GetCpuCores();
                info.totalRam = helper::SystemHelper::GetTotalRam();
                info.availableRam = helper::SystemHelper::GetAvailableRam();
                info.gpu = helper::SystemHelper::GetGpuName();
                info.ipAddress = helper::SystemHelper::GetIpAddress();
                info.macAddress = helper::SystemHelper::GetMacAddress();

                if (callback) callback(info);
            }

        } // namespace messages
    } // namespace client
} // namespace rslm