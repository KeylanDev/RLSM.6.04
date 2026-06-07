#include "HardwareDevices.h"

#ifdef _WIN32
#include <windows.h>
#include <setupapi.h>
#pragma comment(lib, "setupapi.lib")
#endif

#include <stdexcept>

namespace rslm {
    namespace client {
        namespace io {

            std::vector<DeviceInfo> HardwareDevices::GetAll() {
                std::vector<DeviceInfo> devices;

#ifdef _WIN32
                HDEVINFO deviceInfoSet = SetupDiGetClassDevsA(
                    nullptr, nullptr, nullptr,
                    DIGCF_PRESENT | DIGCF_ALLCLASSES
                );

                if (deviceInfoSet == INVALID_HANDLE_VALUE) {
                    return devices;
                }

                SP_DEVINFO_DATA deviceInfoData = {};
                deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);

                for (DWORD i = 0; SetupDiEnumDeviceInfo(deviceInfoSet, i, &deviceInfoData); ++i) {
                    char name[256] = {};
                    char desc[256] = {};

                    SetupDiGetDeviceRegistryPropertyA(
                        deviceInfoSet, &deviceInfoData,
                        SPDRP_FRIENDLYNAME, nullptr,
                        reinterpret_cast<PBYTE>(name), sizeof(name), nullptr
                    );

                    SetupDiGetDeviceRegistryPropertyA(
                        deviceInfoSet, &deviceInfoData,
                        SPDRP_DEVICEDESC, nullptr,
                        reinterpret_cast<PBYTE>(desc), sizeof(desc), nullptr
                    );

                    DeviceInfo info;
                    info.name = name;
                    info.description = desc;
                    info.status = "OK";
                    devices.push_back(info);
                }

                SetupDiDestroyDeviceInfoList(deviceInfoSet);
#endif

                return devices;
            }

            std::vector<DeviceInfo> HardwareDevices::GetByType(const std::string& type) {
                std::vector<DeviceInfo> all = GetAll();
                std::vector<DeviceInfo> filtered;

                for (const auto& device : all) {
                    std::string lowerName = device.name;
                    std::string lowerType = type;

                    for (auto& c : lowerName) c = static_cast<char>(tolower(c));
                    for (auto& c : lowerType) c = static_cast<char>(tolower(c));

                    if (lowerName.find(lowerType) != std::string::npos) {
                        filtered.push_back(device);
                    }
                }

                return filtered;
            }

        } // namespace io
    } // namespace client
} // namespace rslm