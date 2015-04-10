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

        void encrypt(const void *data, std::size_t dataLength,
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

        void decrypt(const void *data, std::size_t dataLength,
                     const void *iv, void *dest) const;
    };

    std::size_t getKeySize() const;
    std::size_t getBlockLength() const;
    std::size_t getIVLength() const;

    static const CipherAlgorithm &getAlgorithm(
            const std::string &cipherName, std::size_t keySize,
            const std::string &cipherMode);
};

} // namespace cipheralgorithm
} // namespace libcipherspec

#endif // LIBCIPHERSPEC_CIPHERALGORITHM_CIPHERALGORITHM_H
