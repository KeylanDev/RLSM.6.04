#include "FileManagerHandler.h"
#include <fstream>
#include <filesystem>

namespace rslm {
    namespace client {
        namespace messages {

            void FileManagerHandler::ListFiles(const std::string& path, FileListCallback callback) {
                std::vector<FileInfo> files;

                try {
                    for (const auto& entry : std::filesystem::directory_iterator(path)) {
                        FileInfo info;
                        info.name = entry.path().filename().string();
                        info.path = entry.path().string();
                        info.type = entry.is_directory() ? FileType::Directory : FileType::File;
                        info.size = entry.is_directory() ? 0 : entry.file_size();
                        files.push_back(info);
                    }
                }
                catch (...) {}

                if (callback) callback(files);
            }

            void FileManagerHandler::DownloadFile(const std::string& path, FileDataCallback callback) {
                std::vector<uint8_t> data;

                std::ifstream file(path, std::ios::binary | std::ios::ate);
                if (file) {
                    size_t size = static_cast<size_t>(file.tellg());
                    file.seekg(0);
                    data.resize(size);
                    file.read(reinterpret_cast<char*>(data.data()), size);
                }

                if (callback) callback(data);
            }

            void FileManagerHandler::UploadFile(const std::string& remotePath,
                const std::vector<uint8_t>& data,
                StatusCallback callback) {
                std::ofstream file(remotePath, std::ios::binary);
                bool success = false;

                if (file) {
                    file.write(reinterpret_cast<const char*>(data.data()), data.size());
                    success = file.good();
                }

                if (callback) callback(success, success ? "" : "Write failed");
            }

            void FileManagerHandler::Delete(const std::string& path, StatusCallback callback) {
                bool success = false;
                std::string error;

                try {
                    if (std::filesystem::is_directory(path)) {
                        success = std::filesystem::remove_all(path) > 0;
                    }
                    else {
                        success = std::filesystem::remove(path);
                    }
                }
                catch (const std::exception& e) {
                    error = e.what();
                }

                if (callback) callback(success, error);
            }

            void FileManagerHandler::Rename(const std::string& oldPath,
                const std::string& newPath,
                StatusCallback callback) {
                bool success = false;
                std::string error;

                try {
                    std::filesystem::rename(oldPath, newPath);
                    success = true;
                }
                catch (const std::exception& e) {
                    error = e.what();
                }

                if (callback) callback(success, error);
            }

            void FileManagerHandler::Execute(const std::string& path, StatusCallback callback) {
#ifdef _WIN32
                std::string cmd = "cmd.exe /c start \"\" \"" + path + "\"";
                system(cmd.c_str());
                if (callback) callback(true, "");
#else
                if (callback) callback(false, "Not supported");
#endif
            }

        } // namespace messages
    } // namespace client
} // namespace rslm