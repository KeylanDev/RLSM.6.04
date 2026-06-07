#pragma once
#include <string>
#include <nlohmann/json.hpp>

namespace rslm {
	namespace client {
		namespace helper {

			using json = nlohmann::json;

			class JsonHelper {
			public:
				// Lit un fichier JSON
				static json ReadFromFile(const std::string& path);

				//Ecrit dans un fichier JSON
				static bool WriteToFile(const std::string& path, const json& data);

				//Parse depuis une string
				static json Parse(const std::string& jsonStr);

				//Convertit en string formatee 
				static std::string ToString(const json& data, bool pretty = false);
			};
		}// namespace helper
	}// namespace client
} // namespace rslm