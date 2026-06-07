#include "SHA256.h"
#include "SafeComparison.h"
#include <openssl/evp.h>
#include <fstream>
#include <stdexcept>

namespace rslm {
    namespace crypto {

        namespace {
            struct Sha256Ctx {
                EVP_MD_CTX* ctx;
                Sha256Ctx() : ctx(EVP_MD_CTX_new()) {
                    if (!ctx) throw std::runtime_error("EVP_MD_CTX_new failed");
                    EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr);
                }
                ~Sha256Ctx() { EVP_MD_CTX_free(ctx); }
                Sha256Ctx(const Sha256Ctx&) = delete;
                Sha256Ctx& operator=(const Sha256Ctx&) = delete;

                void Update(const uint8_t* data, size_t len) {
                    EVP_DigestUpdate(ctx, data, len);
                }

                std::vector<uint8_t> Finalize() {
                    std::vector<uint8_t> hash(SHA256::HASH_SIZE);
                    unsigned int hashLen = 0;
                    EVP_DigestFinal_ex(ctx, hash.data(), &hashLen);
                    return hash;
                }
            };
        }

        std::vector<uint8_t> SHA256::Hash(const std::vector<uint8_t>& data) {
            Sha256Ctx ctx;
            ctx.Update(data.data(), data.size());
            return ctx.Finalize();
        }

        std::vector<uint8_t> SHA256::Hash(const std::string& data) {
            Sha256Ctx ctx;
            ctx.Update(reinterpret_cast<const uint8_t*>(data.data()), data.size());
            return ctx.Finalize();
        }

        std::vector<uint8_t> SHA256::HashFile(const std::string& filePath) {
            std::ifstream file(filePath, std::ios::binary);
            if (!file) {
                throw std::runtime_error("Cannot open file: " + filePath);
            }

            Sha256Ctx ctx;
            std::vector<uint8_t> buffer(8192);

            while (file) {
                file.read(reinterpret_cast<char*>(buffer.data()), buffer.size());
                size_t bytesRead = static_cast<size_t>(file.gcount());
                if (bytesRead > 0) {
                    ctx.Update(buffer.data(), bytesRead);
                }
            }

            return ctx.Finalize();
        }

        std::string SHA256::ToHex(const std::vector<uint8_t>& hash) {
            static const char hexChars[] = "0123456789abcdef";
            std::string result;
            result.reserve(hash.size() * 2);
            for (uint8_t byte : hash) {
                result.push_back(hexChars[byte >> 4]);
                result.push_back(hexChars[byte & 0x0F]);
            }
            return result;
        }

        std::vector<uint8_t> SHA256::FromHex(const std::string& hex) {
            if (hex.size() % 2 != 0) {
                throw std::runtime_error("Hex string must have even length");
            }

            std::vector<uint8_t> result;
            result.reserve(hex.size() / 2);

            auto hexVal = [](char c) -> uint8_t {
                if (c >= '0' && c <= '9') return c - '0';
                if (c >= 'a' && c <= 'f') return c - 'a' + 10;
                if (c >= 'A' && c <= 'F') return c - 'A' + 10;
                throw std::runtime_error("Invalid hex character");
                };

            for (size_t i = 0; i < hex.size(); i += 2) {
                result.push_back((hexVal(hex[i]) << 4) | hexVal(hex[i + 1]));
            }
            return result;
        }

        bool SHA256::Verify(const std::vector<uint8_t>& h1,
            const std::vector<uint8_t>& h2) {
            return SafeCompare(h1, h2);
        }

    } // namespace crypto
} // namespace rslm