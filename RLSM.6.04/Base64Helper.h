#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rslm {
	namespace util {

		inline std::string Base64Encode(const std::vector<uint8_t>& data) {
			static const char* kTable =
				"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

			std::string out;
			out.reserve(((data.size() + 2) / 3) * 4);

			size_t i = 0;
			while (i + 2 < data.size()) {
				uint32_t n = (static_cast<uint32_t>(data[i]) << 16)
					| (static_cast<uint32_t>(data[i + 1]) << 8)
					| static_cast<uint32_t>(data[i + 2]);
				out.push_back(kTable[(n >> 18) & 0x3F]);
				out.push_back(kTable[(n >> 12) & 0x3F]);
				out.push_back(kTable[(n >> 6) & 0x3F]);
				out.push_back(kTable[n & 0x3F]);
				i += 3;
			}

			if (i < data.size()) {
				uint32_t n = static_cast<uint32_t>(data[i]) << 16;
				if (i + 1 < data.size())
					n |= static_cast<uint32_t>(data[i + 1]) << 8;

				out.push_back(kTable[(n >> 18) & 0x3F]);
				out.push_back(kTable[(n >> 12) & 0x3F]);
				out.push_back((i + 1 < data.size()) ? kTable[(n >> 6) & 0x3F] : '=');
				out.push_back('=');
			}

			return out;
		}

		inline std::vector<uint8_t> Base64Decode(const std::string& input) {
			auto valueOf = [](char c) -> int {
				if (c >= 'A' && c <= 'Z') return c - 'A';
				if (c >= 'a' && c <= 'z') return c - 'a' + 26;
				if (c >= '0' && c <= '9') return c - '0' + 52;
				if (c == '+') return 62;
				if (c == '/') return 63;
				return -1;
			};

			std::vector<uint8_t> out;
			int val = 0;
			int valb = -8;

			for (char c : input) {
				if (c == '=') break;
				int d = valueOf(c);
				if (d < 0) continue;
				val = (val << 6) + d;
				valb += 6;
				if (valb >= 0) {
					out.push_back(static_cast<uint8_t>((val >> valb) & 0xFF));
					valb -= 8;
				}
			}

			return out;
		}

	} // namespace util
} // namespace rslm
