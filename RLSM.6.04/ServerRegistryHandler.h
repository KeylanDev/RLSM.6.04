#pragma once
#include <string>
#include <vector>
#include <functional>

namespace rslm {
	namespace server {
		namespace messages {

			struct RegistryValue {
				std::string name;
				std::string type;
				std::string data;
			};
			
			using RegistryCallback = std::function<void(const std::vector<RegistryValue>&)>;
			using RegistryStatusCallback = std::function<void(bool success)>;

				class ServerRegistryHandler {
				public:
					static void ReadKey(const std::string& clientId, const std::string& path,
						                RegistryCallback callback);
					static void WriteValue(const std::string& clientId, const std::string& path,
						                   const std::string& name, const std::string& type,
						                   const std::string& data, RegistryStatusCallback callback);
					static void DeleteValue(const std::string& clientId, const std::string& path,
						                   const std::string& name, RegistryStatusCallback  callback);
					static void DeleteKey(const std::string& clientId, const std::string& path,
						                  RegistryStatusCallback callback);
					static void EnumKeys(const std::string& clientId, const std::string& path,
						                RegistryCallback callback);
						
			};
		}//namespace messages
	}//namespace server
}// namespace rslm