#pragma once
#include <string>
#include <vector>

namespace rslm {
	namespace client {
		namespace io {

			struct DeviceInfo {
				std::string name;
				std::string description;
				std::string status;
			};
			
			class HardwareDevices {
			public:
				// Liste tous les peripheriques
				static std::vector<DeviceInfo> GetAll();

				//Liste par type : "disk", "network", "usb", "display"
				static std::vector<DeviceInfo> GetByType(const std::string& type);

			};
		}//namespace io
	}//namespace client
}//namespace rslm