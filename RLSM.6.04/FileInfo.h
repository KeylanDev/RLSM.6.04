#pragma once
#include <string>
#include <cstdint>
#include "FileType.h"

namespace rslm {
	namespace models {

		struct FileInfo {
			std::string name;
			std::string path;
			FileType  type = FileType::File;
			uint64_t  size = 0;
			int64_t   lastModified = 0; //timestamp
		};
	}
}