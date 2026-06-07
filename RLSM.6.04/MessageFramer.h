#pragma once
#include <string>
#include <functional>
#include <cstddef>

namespace rslm {
	namespace net {

		class MessageFramer {
		public:
			void Append(const char* data, size_t length,
			            const std::function<void(const std::string& line)>& onLine) {
				buffer_.append(data, length);

				size_t pos = 0;
				while ((pos = buffer_.find('\n')) != std::string::npos) {
					std::string line = buffer_.substr(0, pos);
					buffer_.erase(0, pos + 1);

					while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
						line.pop_back();

					if (!line.empty() && onLine)
						onLine(line);
				}
			}

			void Clear() { buffer_.clear(); }

		private:
			std::string buffer_;
		};

	} // namespace net
} // namespace rslm
