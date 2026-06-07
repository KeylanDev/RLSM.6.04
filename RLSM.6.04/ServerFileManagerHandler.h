#pragma once
#include <string>
#include <vector>
#include <functional>
#include "FileInfo.h"

namespace rslm {
    namespace server {
        namespace messages {

            using models::FileInfo;
            using FileListCallback = std::function<void(const std::vector<FileInfo>&)>;
            using FileDataCallback = std::function<void(const std::vector<uint8_t>&)>;
            using StatusCallback = std::function<void(bool success, const std::string& error)>;

            class ServerFileManagerHandler {
            public:
                static void RequestFileList(const std::string& clientId, const std::string& path,
                    FileListCallback callback);
                static void RequestDownload(const std::string& clientId, const std::string& path,
                    FileDataCallback callback);
                static void RequestUpload(const std::string& clientId, const std::string& remotePath,
                    const std::vector<uint8_t>& data, StatusCallback callback);
                static void RequestDelete(const std::string& clientId, const std::string& path,
                    StatusCallback callback);
                static void RequestExecute(const std::string& clientId, const std::string& path,
                    StatusCallback callback);
            };

        } // namespace messages
    } // namespace server
} // namespace rslm