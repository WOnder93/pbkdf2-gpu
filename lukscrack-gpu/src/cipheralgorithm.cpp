#include "cipheralgorithm.h"

#include "crypto/crypto_backend.h"

#include <unordered_map>

namespace lukscrack {

class Cipher : public CipherAlgorithm {
private:
    std::string spec;
    std::string mode;
    size_t keySize;

public:
    inline Cipher(const std::string &spec, const std::string &mode, size_t keySize) :
        spec(spec), mode(mode), keySize(keySize) { }

    virtual void decrypt(const void *key, const void *data, size_t length, void *dest) const {
        using namespace crypto;

        struct crypt_cipher *c;
        int ret;
        // TODO: check & throw exceptions:
        ret = crypt_cipher_init(&c, spec.c_str(), mode.c_str(), key, keySize);
        ret = crypt_cipher_decrypt(c, (const char *)data, (char *)dest, length, NULL, 0);
        ret = crypt_cipher_destroy(c);
    }
};

std::shared_ptr<const CipherAlgorithm> CipherAlgorithm::get(const std::string &spec, const std::string &mode, size_t keySize) {
    return std::shared_ptr<Cipher>(new Cipher(spec, mode, keySize));
}

} // namespace lukscrack
