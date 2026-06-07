#pragma once
#include <cstdint>
#include <vector>

namespace rslm {
	namespace crypto {
		
		//Comparaison en temp constant (resiste au timing attacks)
		//Meme si les Hash different en premier octect, le temp d'execution
		//reste identique -> impossible de deviner le hash octet par octet
		inline bool SafeCompare(const std::vector<uint8_t>& a,
			const std::vector<uint8_t>& b) {
			if (a.size() != b.size()) return false;
			
		uint8_t diff = 0;
		for (size_t i = 0; i < a.size(); ++i) {
			diff |= a[i] ^ b[i];
		}
		return diff == 0;
		}
	}//namespace crypto
	}//namespace rslm
