#pragma once
#include <string>
#include <vector>
#include <functional>
#include "StartupItem.h"

namespace rslm {
	namespace server {
		namespace messages {

			using models::StartupItem;
			using StartupListCallback = std::function<void(const std::vector<StartupItem>&)>;
			using StartupStatusCallback = std::function<void(bool success)>;

			class ServerStartupManagerHandler {
			public:
				static void GetStartupItems(const std::string& clientId,
					                       StartupListCallback callback);
				static void AddStartupItem(const std::string& clientId,
					                       const StartupItem& item,
					                       StartupStatusCallback callback);
				static void RemoveStartupItem(const std::string& clientId,
					                       const std::string& name,
					                       StartupStatusCallback callback);
			};
		}//namespace messages
	}//namespace server
}//namespace rslm