#pragma once
#include <functional>
#include "SystemInfo.h"

namespace rslm {
	namespace client {
		namespace messages {
			
			using models::SystemInfo;
			using SystemInfoCallback = std::function<void(const SystemInfo&)>;

			class SystemInformationHandler {
			public:
				static void GetSystemInfo(SystemInfoCallback callback);
			};
		}//namespace messages
	}//namespace client
}//namespace rslm