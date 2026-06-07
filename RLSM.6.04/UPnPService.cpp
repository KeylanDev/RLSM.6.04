#include "UPnPService.h"

namespace rslm {
    namespace server {
        namespace networking {

            bool UPnPService::OpenPort(uint16_t internalPort, uint16_t externalPort,
                const std::string& description) {
                return false; // Placeholder
            }

            bool UPnPService::ClosePort(uint16_t externalPort) {
                return false;
            }

            bool UPnPService::IsAvailable() {
                return false;
            }

            std::string UPnPService::GetExternalIP() {
                return "";
            }

            void UPnPService::AutoSetup(uint16_t port, UPnPCallback callback) {
                if (callback) callback(false, "");
            }

        } // namespace networking
    } // namespace server
} // namespace rslm