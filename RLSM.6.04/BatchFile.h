#pragma once
#include <string>

namespace rslm {
	namespace client {
		namespace io {

			class BatchFile {
			public:
				//Cree un fichier batch temporaire et l'execute
				static bool Execute(const std::string& commands);

				//Sauvegarde un batch sans L'executer
				static bool Save(const std::string& path, const std::string& commands);

				//Execute un batch existant
				static bool Run(const std::string& path);
			};
		}// namespace io
	}//namespace client
}//namespace rslm