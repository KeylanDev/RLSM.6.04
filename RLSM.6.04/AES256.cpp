#include "AES256.h"
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdexcept>

namespace rslm {
    namespace crypto {

        // RAII pour le contexte OpenSSL
        namespace {
            struct EvpCipherCtx {
                EVP_CIPHER_CTX* ctx;
                EvpCipherCtx() : ctx(EVP_CIPHER_CTX_new()) {
                    if (!ctx) throw std::runtime_error("EVP_CIPHER_CTX_new failed");
                }
                ~EvpCipherCtx() { EVP_CIPHER_CTX_free(ctx); }
                EvpCipherCtx(const EvpCipherCtx&) = delete;
                EvpCipherCtx& operator=(const EvpCipherCtx&) = delete;
            };
        }

        std::vector<uint8_t> AES256::GenerateKey() {
            std::vector<uint8_t> key(KEY_SIZE);
            if (RAND_bytes(key.data(), KEY_SIZE) != 1) {
                throw std::runtime_error("RAND_bytes failed");
            }
            return key;
        }

        std::vector<uint8_t> AES256::GenerateIV() {
            std::vector<uint8_t> iv(IV_SIZE);
            if (RAND_bytes(iv.data(), IV_SIZE) != 1) {
                throw std::runtime_error("RAND_bytes failed");
            }
            return iv;
        }

        std::vector<uint8_t> AES256::Encrypt(const std::vector<uint8_t>& key,
            const std::vector<uint8_t>& plainData) {
            if (key.size() != KEY_SIZE) {
                throw std::runtime_error("AES256: key must be 32 bytes");
            }

            std::vector<uint8_t> iv = GenerateIV();
            EvpCipherCtx ctx;

            EVP_EncryptInit_ex(ctx.ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv.data());

            std::vector<uint8_t> cipher(plainData.size() + BLOCK_SIZE);
            int outLen = 0, totalLen = 0;

            EVP_EncryptUpdate(ctx.ctx, cipher.data(), &outLen,
                plainData.data(), static_cast<int>(plainData.size()));
            totalLen = outLen;

            EVP_EncryptFinal_ex(ctx.ctx, cipher.data() + totalLen, &outLen);
            totalLen += outLen;
            cipher.resize(totalLen);

            std::vector<uint8_t> result;
            result.reserve(IV_SIZE + cipher.size());
            result.insert(result.end(), iv.begin(), iv.end());
            result.insert(result.end(), cipher.begin(), cipher.end());
            return result;
        }

        std::vector<uint8_t> AES256::Decrypt(const std::vector<uint8_t>& key,
            const std::vector<uint8_t>& encryptedData) {
            if (key.size() != KEY_SIZE) {
                throw std::runtime_error("AES256: key must be 32 bytes");
            }
            if (encryptedData.size() < IV_SIZE + BLOCK_SIZE) {
                throw std::runtime_error("AES256: encrypted data too short");
            }

            const uint8_t* iv = encryptedData.data();
            const uint8_t* cipherData = encryptedData.data() + IV_SIZE;
            size_t cipherSize = encryptedData.size() - IV_SIZE;

            EvpCipherCtx ctx;
            EVP_DecryptInit_ex(ctx.ctx, EVP_aes_256_cbc(), nullptr, key.data(), iv);

            std::vector<uint8_t> plain(cipherSize);
            int outLen = 0, totalLen = 0;

            EVP_DecryptUpdate(ctx.ctx, plain.data(), &outLen,
                cipherData, static_cast<int>(cipherSize));
            totalLen = outLen;

            EVP_DecryptFinal_ex(ctx.ctx, plain.data() + totalLen, &outLen);
            totalLen += outLen;
            plain.resize(totalLen);
            return plain;
        }

    } // namespace crypto
} // namespace rslm