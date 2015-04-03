#ifndef LIBCIPHERSPEC_CIPHERALGORITHM_CIPHERALGORITHM_H
#define LIBCIPHERSPEC_CIPHERALGORITHM_CIPHERALGORITHM_H

#include <string>
#include <stdexcept>

#include <openssl/evp.h>

namespace libcipherspec {
namespace cipheralgorithm {

class CipherException : public std::runtime_error
{
public:
    inline CipherException(const std::string &message)
        : std::runtime_error(message)
    {
    }
};

class CipherAlgorithm
{
private:
    const ::EVP_CIPHER *cipher;

    inline CipherAlgorithm(const ::EVP_CIPHER *cipher)
        : cipher(cipher)
    {
    }

public:
    class EncryptionContex
    {
    private:
        const CipherAlgorithm *algorithm;
        const void *key;

    public:
        inline EncryptionContex() { }

        EncryptionContex(const CipherAlgorithm *algorithm, const void *key);

        void encrypt(const void *data, size_t dataLength,
                     const void *iv, void *dest) const;
    };
    class DecryptionContex
    {
    private:
        const CipherAlgorithm *algorithm;
        const void *key;

    public:
        inline DecryptionContex() { }

        DecryptionContex(const CipherAlgorithm *algorithm, const void *key);

        void decrypt(const void *data, size_t dataLength,
                     const void *iv, void *dest) const;
    };

    size_t getKeySize() const;
    size_t getBlockLength() const;
    size_t getIVLength() const;

    static const CipherAlgorithm &getAlgorithm(
            const std::string &cipherName, size_t keySize,
            const std::string &cipherMode);
};

} // namespace cipheralgorithm
} // namespace libcipherspec

#endif // LIBCIPHERSPEC_CIPHERALGORITHM_CIPHERALGORITHM_H
