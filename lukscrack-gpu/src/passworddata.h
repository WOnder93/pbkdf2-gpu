#ifndef LUKSCRACK_PASSWORDDATA_H
#define LUKSCRACK_PASSWORDDATA_H

#include <cstdint>

#include <istream>
#include <string>

namespace lukscrack {

class PasswordData
{
public:
    enum {
        MASTER_KEY_DIGEST_SIZE = 20, /* LUKS DIGESTSIZE */
        SALT_SIZE = 32, /* LUKS_SALSIZE */
    };

private:
    std::string cipherName;
    std::string cipherMode;
    std::string hashSpec;

    uint_least32_t keySize;

    char masterKeyDigest[MASTER_KEY_DIGEST_SIZE];
    char masterKeyDigestSalt[SALT_SIZE];
    uint_least32_t masterKeyDigestIter;

    char keyslotSalt[SALT_SIZE];
    uint_least32_t keyslotIter;
    uint_least32_t keyslotStripes;

    char *keyMaterial;

public:
    inline const std::string &getCipherName() const { return cipherName; }
    inline const std::string &getCipherMode() const { return cipherMode; }
    inline const std::string &getHashSpec() const { return hashSpec; }

    inline uint_least32_t getKeySize() const { return keySize; }

    inline const char *getMasterKeyDigest() const { return masterKeyDigest; }
    inline const char *getMasterKeyDigestSalt() const { return masterKeyDigestSalt; }
    inline uint_least32_t getMasterKeyDigestIter() const { return masterKeyDigestIter; }

    inline const char *getKeyslotSalt() const { return keyslotSalt; }
    inline uint_least32_t getKeyslotIter() const { return keyslotIter; }
    inline uint_least32_t getKeyslotStripes() const { return keyslotStripes; }

    inline const char *getKeyMaterial() const { return keyMaterial; }

    PasswordData() : keyMaterial(nullptr) { }
    PasswordData(const PasswordData &other) = delete;
    PasswordData &operator=(const PasswordData &other) = delete;

    ~PasswordData() {
        delete[] keyMaterial;
    }

    void readFromLuksHeader(std::istream &stream, int keyslot);
};

} // namespace lukscrack

#endif // LUKSCRACK_PASSWORDDATA_H
