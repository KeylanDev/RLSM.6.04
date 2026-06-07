#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace rslm {
namespace crypto {
		
	class SHA256 {
	public:
		static constexpr size_t HASH_SIZE = 32; // 256 Bits 

		// Hash des donnees
		static std::vector<uint8_t> Hash(const std::vector<uint8_t>& data);
		static std::vector<uint8_t> Hash(const std::string& data);
		static std::vector<uint8_t> HashFile(const std::string& filePath);

		//Convertion hexadecimal
		static std::string ToHex(const std::vector<uint8_t>& hash);
		static std::vector<uint8_t> FromHex(const std::string& hex);

		//Comparaison en temps constant (anti-timing attack)
		static bool Verify(const std::vector<uint8_t>& h1,
			const std::vector<uint8_t>& h2);
};

}//namespace crypto
}//namespace rslm