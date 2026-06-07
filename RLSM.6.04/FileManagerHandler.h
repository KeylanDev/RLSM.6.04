#pragma once
#include <string>
#include <functional>
#include "FileInfo.h"

namespace rslm {
	namespace client {
		namespace messages {

			using models::FileInfo;
			using FileListCallback = std::function<void(const std::vector<FileInfo>&)>;
			using FileDataCallback = std::function<void(const std::vector<uint8_t>&)>;
			using StatusCallback = std::function<void(bool, const std::string&)>;

			class FileManagerHandler {
			public:
					//Liste les fichier dun repertoire
					static void ListFiles(const std::string& path, FileListCallback callback);
					//Telecharge un fichier
					static void DownloadFile(const std::string& path, FileDataCallback callback);

					//Upload un fichier
					static void UploadFile(const std::string& remotePath,
						                   const std::vector<uint8_t>& data,
						                   StatusCallback callback);

					//Supprime un fichier/dossier
					static void Delete(const std::string& path, StatusCallback callback);

					//Renomme
					static void Rename(const std::string& oldPath,
						               const std::string& newPath,
					                   StatusCallback callback);

					//Execute un fichier
					static void Execute(const std::string& path, StatusCallback callback);
				    
			};
		}//namespace messages
	}//namespace client
}//namespace rslm