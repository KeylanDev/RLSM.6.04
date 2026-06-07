#pragma once
#include <string>
#include <functional>
#include "SystemInfo.h"

namespace rslm {
	namespace server {
		namespace messages {

			using models::SystemInfo;
			using SystemInfoCallback = std::function<void(const SystemInfo&)>;

			class ServerSystemInformationHandler {
			public:
				static void RequestSystemInfo(const std::string& clientId,
					                          SystemInfoCallback callback);
			};
		}//namespace messages
	}//namespace server
}//namespace rslm