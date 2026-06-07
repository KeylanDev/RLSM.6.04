#pragma once
#include <string>
#include <vector>
#include <functional>
#include "ProcessInfo.h"

namespace rslm {
	namespace server {
		namespace messages {

			using models::ProcessInfo;
			using ProcessListCallback = std::function<void(const std::vector<ProcessInfo>&)>;
			using ProcessStatusCallback = std::function<void(bool success)>;

			class ServerTaskManagerHandler {
			public:
				static void GetProcesses(const std::string& clientId, ProcessListCallback callback);
				static void KillProcess(const std::string& clientId, uint32_t pid,
					                    ProcessStatusCallback callback);
				static void StartProcess(const std::string& clientId, const std::string& path,
					                    ProcessStatusCallback callback);
				static void SuspendProcess(const std::string& clientId, uint32_t pid,
					                    ProcessStatusCallback callback);
				static void ResumeProcess(const std::string& clientId, uint32_t pid,
					                    ProcessStatusCallback callback);
			};
		}//namespace messages
	}//namespace server
}//namespace rslm