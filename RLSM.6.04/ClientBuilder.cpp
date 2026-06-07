#include "ClientBuilder.h"

namespace rslm {
    namespace server {
        namespace build {

            void ClientBuilder::Build(const std::string& host, uint16_t port,
                const std::string& password,
                const std::string& outputPath,
                BuildCallback callback) {
                if (callback) callback(false, "", "Not implemented");
            }

            void ClientBuilder::SetIcon(const std::string& iconPath) {
            }

            void ClientBuilder::SetAssemblyInfo(const std::string& productName,
                const std::string& description) {
            }

        } // namespace build
    } // namespace server
} // namespace rslm