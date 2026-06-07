#pragma once
#include <vector>
#include <string>
#include <cstdint>

namespace rslm {
namespace crypto {

	class AES256 {
	public:
		static constexpr size_t KEY_SIZE   = 32;  // 256 Bits
		static constexpr size_t IV_SIZE    = 16; // 128 Bits
		static constexpr size_t BLOCK_SIZE = 16;

		// Genere une cle aleatoir
		static std::vector<uint8_t> GenerateKey();

		// Genere un IV aleatoire
		static std::vector<uint8_t> GenerateIV();

		// Chiffre : retorune IV + ciphertext
		static std::vector<uint8_t> Encrypt(
			const std::vector<uint8_t>& key,
			const std::vector<uint8_t>& plainData
		);

		// Dechiffre (Les 15 premiers octets = IV
		static std::vector<uint8_t> Decrypt(
			const std::vector<uint8_t>& key,
			const std::vector<uint8_t>& encryptedData
		);
	};

}//namespace crypto
}//namepace rslm
