#ifndef LUKSCRACK_PASSWORDDATA_H
#define LUKSCRACK_PASSWORDDATA_H

#include <cstdint>

#include <istream>
#include <string>
#include <memory>
#include <stdexcept>

namespace lukscrack {

class InvalidLuksHeaderException : public std::runtime_error
{
public:
    inline InvalidLuksHeaderException(const std::string &message)
        : std::runtime_error("Invalid LUKS header: " + message)
    {
    }
};

class InvalidLuksMagicException : public InvalidLuksHeaderException
{
public:
    inline InvalidLuksMagicException()
        : InvalidLuksHeaderException("Invalid magic!")
    {
    }
};

class IncompatibleLuksVersionException : public InvalidLuksHeaderException
{
public:
    inline IncompatibleLuksVersionException(std::uint_least16_t version)
        : InvalidLuksHeaderException("Incompatible version: " + std::to_string(version) + "!")
    {
    }
};

class StringNotTerminatedException : public InvalidLuksHeaderException
{
public:
    inline StringNotTerminatedException(const std::string &field)
        : InvalidLuksHeaderException(field + ": String not terminated!")
    {
    }
};

class KeyslotDisabledException : public InvalidLuksHeaderException
{
public:
    inline KeyslotDisabledException(int keyslot_num)
        : InvalidLuksHeaderException("Keyslot #" + std::to_string(keyslot_num) + " is disabled!")
    {
    }
};

class KeyslotCorruptedException : public InvalidLuksHeaderException
{
public:
    inline KeyslotCorruptedException(int keyslot_num)
        : InvalidLuksHeaderException("Keyslot #" + std::to_string(keyslot_num) + " is corrupted!")
    {
    }
};

class NoActiveKeyslotException : public InvalidLuksHeaderException
{
public:
    inline NoActiveKeyslotException()
        : InvalidLuksHeaderException("No active keyslot found!")
    {
    }
};

class PasswordData
{
public:
    enum {
        MASTER_KEY_DIGEST_LENGTH = 20, /* LUKS DIGESTSIZE */
        SALT_LENGTH = 32, /* LUKS_SALSIZE */
        SECTOR_SIZE = 512,
    };

private:
    std::string cipherName;
    std::string cipherMode;
    std::string hashSpec;

    size_t keySize;

    unsigned char masterKeyDigest[MASTER_KEY_DIGEST_LENGTH];
    unsigned char masterKeyDigestSalt[SALT_LENGTH];
    size_t masterKeyDigestIter;

    unsigned char keyslotSalt[SALT_LENGTH];
    size_t keyslotIter;
    size_t keyslotStripes;

    size_t keyMaterialSectors;
    std::unique_ptr<unsigned char[]> keyMaterial;

public:
    inline const std::string &getCipherName() const { return cipherName; }
    inline const std::string &getCipherMode() const { return cipherMode; }
    inline const std::string &getHashSpec() const { return hashSpec; }

    inline size_t getKeySize() const { return keySize; }

    inline const unsigned char *getMasterKeyDigest() const { return masterKeyDigest; }
    inline const unsigned char *getMasterKeyDigestSalt() const { return masterKeyDigestSalt; }
    inline size_t getMasterKeyDigestIter() const { return masterKeyDigestIter; }

    inline const unsigned char *getKeyslotSalt() const { return keyslotSalt; }
    inline size_t getKeyslotIter() const { return keyslotIter; }
    inline size_t getKeyslotStripes() const { return keyslotStripes; }

    inline size_t getKeyMaterialSectors() const { return keyMaterialSectors; }
    inline const unsigned char *getKeyMaterial() const { return keyMaterial.get(); }

    inline PasswordData() : keyMaterial() { }

    void readFromLuksHeader(std::istream &stream, size_t keyslot);
};

} // namespace lukscrack

#endif // LUKSCRACK_PASSWORDDATA_H
