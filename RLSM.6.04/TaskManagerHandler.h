#pragma once
#include <vector>
#include <functional>
#include "ProcessInfo.h"

namespace rslm {
	namespace client{
		namespace messages {

			using models::ProcessInfo;
			using ProcessListCallback = std::function<void(const std::vector<ProcessInfo>&)>;
			using ProcessStatusCallback = std::function<void(bool success)>;

			class TaskManagerHandler {
			public:
				static void GetProcesses(ProcessListCallback callback);
				static void KillProcess(uint32_t pid, ProcessStatusCallback callback);
				static void StartProcess(const std::string& path, ProcessStatusCallback callback);
				static void SuspendProcess(uint32_t pid, ProcessStatusCallback callback);
				static void ResumeProcess(uint32_t pid, ProcessStatusCallback callback);
			};
		}//namespace messages
	}//namespace client
}//namespace rslm