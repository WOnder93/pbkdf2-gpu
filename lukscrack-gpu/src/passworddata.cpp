#include "passworddata.h"

#include <cstring>
#include <algorithm>

#define UINT16_BE(buffer) (\
    (std::uint_least16_t)(buffer)[0] << 8 | \
    (std::uint_least16_t)(buffer)[1])

#define UINT32_BE(buffer) (\
    (std::uint_least32_t)(buffer)[0] << 24 | \
    (std::uint_least32_t)(buffer)[1] << 16 | \
    (std::uint_least32_t)(buffer)[2] << 8 | \
    (std::uint_least32_t)(buffer)[3])

#define LUKS_PHDR_SIZE 592

#define LUKS_MAGIC_LENGTH 6
#define LUKS_KEY_ACTIVE   0x00AC71F3U
#define LUKS_KEY_DISABLED 0x0000DEADU

namespace lukscrack {

static const char *LUKS_MAGIC = "LUKS\xBA\xBE";

void PasswordData::readFromLuksHeader(std::istream &stream, size_t keyslot)
{
    if (keyslot > 8) {
        throw std::logic_error("Keyslot must be between 0-8!");
    }
    stream.exceptions(std::istream::eofbit | std::istream::failbit | std::istream::badbit);

    unsigned char buffer[LUKS_PHDR_SIZE];
    unsigned char *cursor = buffer;
    stream.read((char *)cursor, LUKS_PHDR_SIZE);
    if (std::memcmp(cursor, LUKS_MAGIC, LUKS_MAGIC_LENGTH) != 0) {
        throw InvalidLuksMagicException();
    }
    cursor += LUKS_MAGIC_LENGTH;

    std::uint_least16_t version = UINT16_BE(cursor);
    if (version > 1) {
        throw IncompatibleLuksVersionException(version);
    }
    cursor += 2;

    const char *cipherName = (const char *)cursor;
    if (std::find(cursor, cursor + 32, '\0') == cursor + 32) {
        throw StringNotTerminatedException("cipher-name");
    }
    cursor += 32;

    const char *cipherMode = (const char *)cursor;
    if (std::find(cursor, cursor + 32, '\0') == cursor + 32) {
        throw StringNotTerminatedException("cipher-mode");
    }
    cursor += 32;

    const char *hashSpec = (const char *)cursor;
    if (std::find(cursor, cursor + 32, '\0') == cursor + 32) {
        throw StringNotTerminatedException("hash-spec");
    }
    cursor += 32;

    /* payloadOffset */
    cursor += 4;

    std::uint_least32_t keySize = UINT32_BE(cursor);
    cursor += 4;

    const unsigned char *mkDigest = cursor;
    cursor += 20;

    const unsigned char *mkDigestSalt = cursor;
    cursor += 32;

    std::uint_least32_t mkDigestIter = UINT32_BE(cursor);
    cursor += 4;

    cursor += 40; /* skip the UUID */

    if (keyslot > 0) {
        /* jump to keyslot: */
        cursor += 48 * (keyslot - 1);

        std::uint_least32_t active = UINT32_BE(cursor);
        if (active == LUKS_KEY_DISABLED) {
            throw KeyslotDisabledException(keyslot);
        } else if (active != LUKS_KEY_ACTIVE) {
            throw KeyslotCorruptedException(keyslot);
        }
        cursor += 4;
    } else {
        bool found = false;
        for (size_t i = 0; i < 8; i++) {
             std::uint_least32_t active = UINT32_BE(cursor);
             if (active == LUKS_KEY_ACTIVE) {
                 cursor += 4;
                 found = true;
                 break;
             }
             cursor += 48;
        }
        if (!found) {
            throw NoActiveKeyslotException();
        }
    }

    std::uint_least32_t iter = UINT32_BE(cursor);
    cursor += 4;

    const unsigned char *keyslotSalt = cursor;
    cursor += 32;

    std::uint_least32_t keyMaterialOffset = UINT32_BE(cursor);
    cursor += 4;

    std::uint_least32_t stripes = UINT32_BE(cursor);
    cursor += 4;

    stream.seekg(keyMaterialOffset * SECTOR_SIZE, std::ios_base::beg);

    size_t keyMaterialSectors =
            (keySize * stripes) / SECTOR_SIZE +
            ((keySize * stripes) % SECTOR_SIZE != 0 ? 1 : 0);
    size_t keyMaterialLength = keyMaterialSectors * SECTOR_SIZE;
    auto keyMaterial = std::unique_ptr<unsigned char[]>(new unsigned char[keyMaterialLength]);
    stream.read((char *)keyMaterial.get(), keyMaterialLength);

    this->hashSpec.assign(hashSpec);
    this->cipherName.assign(cipherName);
    this->cipherMode.assign(cipherMode);

    this->keySize = keySize;

    std::memcpy(this->masterKeyDigest, mkDigest, MASTER_KEY_DIGEST_LENGTH);
    std::memcpy(this->masterKeyDigestSalt, mkDigestSalt, SALT_LENGTH);
    this->masterKeyDigestIter = mkDigestIter;

    std::memcpy(this->keyslotSalt, keyslotSalt, SALT_LENGTH);
    this->keyslotIter = iter;
    this->keyslotStripes = stripes;

    this->keyMaterialSectors = keyMaterialSectors;
    this->keyMaterial = std::move(keyMaterial);
}

} // namespace lukscrack
