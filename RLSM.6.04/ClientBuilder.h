#pragma once
#include <string>
#include <functional>
#include <cstdint>

namespace rslm {
	namespace server {
		namespace build {

			using BuildCallback = std::function<void(bool success, const std::string& outputPath,
				                                     const std::string& error)>;

			class ClientBuilder {
			public:
				static void Build(const std::string& host, uint16_t port,
					const std::string& password,
					const std::string& outputPath,
					BuildCallback callback);

				static void SetIcon(const std::string& iconPath);
				static void SetAssemblyInfo(const std::string& productName,
					const std::string& description);
			};
		}//namespace build
	}//namespace server
}//namespace rslm