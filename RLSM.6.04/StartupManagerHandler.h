#pragma once
#include <vector>
#include <functional>
#include "StartupItem.h"

namespace rslm {
	namespace client {
		namespace messages {

			using models::StartupItem;
			using StartupListCallback = std::function<void(const std::vector<StartupItem>&)>;
			using StartupStatusCallback = std::function<void(bool success)>;

			class StartupManagerHandler {

			public:
				static void GetStartupItems(StartupListCallback callback);
				static void AddStartupItem(const StartupItem& item, StartupStatusCallback callback);
				static void RemoveStartupItem(const std::string& name, StartupStatusCallback callback);
			};
		}//namespace messages
	}//namespace client
}//namespace rslm