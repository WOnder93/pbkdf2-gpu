/*
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
        MASTER_KEY_DIGEST_LENGTH = 20, /* LUKS_DIGESTSIZE */
        SALT_LENGTH = 32, /* LUKS_SALSIZE */
        SECTOR_SIZE = 512,
    };

private:
    std::string cipherName;
    std::string cipherMode;
    std::string hashSpec;

    std::size_t keySize;

    unsigned char masterKeyDigest[MASTER_KEY_DIGEST_LENGTH];
    unsigned char masterKeyDigestSalt[SALT_LENGTH];
    std::size_t masterKeyDigestIter;

    unsigned char keyslotSalt[SALT_LENGTH];
    std::size_t keyslotIter;
    std::size_t keyslotStripes;

    std::size_t keyMaterialSectors;
    std::unique_ptr<unsigned char[]> keyMaterial;

public:
    inline const std::string &getCipherName() const { return cipherName; }
    inline const std::string &getCipherMode() const { return cipherMode; }
    inline const std::string &getHashSpec() const { return hashSpec; }

    inline std::size_t getKeySize() const { return keySize; }

    inline const unsigned char *getMasterKeyDigest() const { return masterKeyDigest; }
    inline const unsigned char *getMasterKeyDigestSalt() const { return masterKeyDigestSalt; }
    inline std::size_t getMasterKeyDigestIter() const { return masterKeyDigestIter; }

    inline const unsigned char *getKeyslotSalt() const { return keyslotSalt; }
    inline std::size_t getKeyslotIter() const { return keyslotIter; }
    inline std::size_t getKeyslotStripes() const { return keyslotStripes; }

    inline std::size_t getKeyMaterialSectors() const { return keyMaterialSectors; }
    inline const unsigned char *getKeyMaterial() const { return keyMaterial.get(); }

    inline PasswordData() : keyMaterial() { }

    void readFromLuksHeader(std::istream &stream, std::size_t keyslot);
};

} // namespace lukscrack

#endif // LUKSCRACK_PASSWORDDATA_H
