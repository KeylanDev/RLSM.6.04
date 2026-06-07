#pragma once
#include <string>
#include <cstdint>

namespace rslm {
	namespace client {
		namespace helper {


			
			//Wrapper autour des appels systeme notifs
			class NativeMethodsHelper {
			public:
		     
			//ouvre une cle de registre (windows)
			static bool OpenRegistryKey(const std::string& path, void** key);

			//Ferme une cle dans le registre
			static void CloseRegistryKey(void* key);

			//lit une valeur dans le registre
			static std::string ReadRegistryValue(const std::string& path,
												 const std::string& valueName);

			//Ecrit une valeur dans le registre
			static bool WriteRegistryValue(const std::string& path,
				                           const std::string& valueName,
				                           const std::string& value);
			//Execute une commande shell
			static std::string ExecuteCommand(const std::string& command);

			//Obtient le PID du processus courant
			static uint32_t GetCurrentProcessId();

			//Verifie si un processus existe
			static bool ProcessExists(const std::string& processName);
			};
		}//namespace helper
	}//namespace client
}//namespace rslm