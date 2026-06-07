#pragma once
#include <string>
#include <cstdint>

namespace rslm {
	namespace client {
		namespace helper {
			
			class DateTimeHelper {
			public:
				//TimeStamp actuel en milisecondes
				static int64_t Now();

				//Timestamp -> string lisible
				static std::string ToString(int64_t timestamp);

				//String lisible -> timestamp
				static int64_t FromString(const std::string& dateStr);

				//Format : "YYYY-MM-DD HH:MM:SS"
				static std::string GetCurrentFormatted();
			};
		}// namespace helper
	}// namespace client
}//namespace rslm