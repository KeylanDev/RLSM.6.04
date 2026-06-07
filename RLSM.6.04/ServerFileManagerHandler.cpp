#include "ServerFileManagerHandler.h"

namespace rslm {
    namespace server {
        namespace messages {

            void ServerFileManagerHandler::RequestFileList(const std::string& clientId,
                const std::string& path,
                FileListCallback callback) {
                if (callback) callback({});
            }

            void ServerFileManagerHandler::RequestDownload(const std::string& clientId,
                const std::string& path,
                FileDataCallback callback) {
                if (callback) callback({});
            }

            void ServerFileManagerHandler::RequestUpload(const std::string& clientId,
                const std::string& remotePath,
                const std::vector<uint8_t>& data,
                StatusCallback callback) {
                if (callback) callback(false, "Not implemented");
            }

            void ServerFileManagerHandler::RequestDelete(const std::string& clientId,
                const std::string& path,
                StatusCallback callback) {
                if (callback) callback(false, "Not implemented");
            }

            void ServerFileManagerHandler::RequestExecute(const std::string& clientId,
                const std::string& path,
                StatusCallback callback) {
                if (callback) callback(false, "Not implemented");
            }

        } // namespace messages
    } // namespace server
} // namespace rslm