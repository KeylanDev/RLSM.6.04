#pragma once
#include <string>
#include <vector>
#include <functional>

namespace rslm {
	namespace client {
		namespace messages {
			
			struct RegistryValue {
				std::string name;
				std::string type; // "REG_SZ", "REG_DWORD", etc.
				std::string data; 
			};

			using RegistryCallback = std::function<void(const std::vector<RegistryValue>&)>;
			using RegistryStatusCallback = std::function<void(bool success)>;

			class RegistryHandler {
			public:
				//Lit uen cle
				static void ReadKey(const std::string& path, RegistryCallback callback);

				//Ecrit une valeur
				static void WriteValue(const std::string& path,
					                   const std::string& name,
					                   const std::string& type,
					                   const std::string& data,
					                   RegistryStatusCallback callback);

				//Supprime une valeur
				static void DeleteValue(const std::string& path,
					                    const std::string& name,
					                   RegistryStatusCallback callback);

				//Supprime une cle
				static void DeleteKey(const std::string& path, RegistryStatusCallback callback);

				//Enumere les sous-cles
				static void EnumKeys(const std::string& path, RegistryCallback callback);
			};
		}//namespace messages
	}//namespace client
}//namespace rslm