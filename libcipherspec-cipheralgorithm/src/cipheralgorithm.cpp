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

#include "cipheralgorithm.h"

#include <map>
#include <tuple>
#include <climits>

namespace libcipherspec {
namespace cipheralgorithm {

CipherAlgorithm::EncryptionContex::EncryptionContex(
        const CipherAlgorithm *algorithm, const void *key)
    : algorithm(algorithm), key(key)
{
}

void CipherAlgorithm::EncryptionContex::encrypt(
        const void *data, std::size_t dataLength,
        const void *iv, void *dest) const
{
    if (dataLength % algorithm->getBlockLength() != 0) {
        throw std::logic_error("dataLength must be aligned to block size!");
    }
    if (dataLength > INT_MAX) {
        throw CipherException("dataLength too large!");
    }
    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_CIPHER_CTX_set_padding(&ctx, 0);
    if (EVP_EncryptInit(&ctx, algorithm->cipher, (const unsigned char *)key, (const unsigned char *)iv) != 1) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        throw CipherException("OpenSSL error!");
    }
    int outl;
    if (EVP_EncryptUpdate(&ctx, (unsigned char *)dest, &outl, (const unsigned char *)data, (int)dataLength) != 1) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        throw CipherException("OpenSSL error!");
    }
    EVP_CIPHER_CTX_cleanup(&ctx);
}

CipherAlgorithm::DecryptionContex::DecryptionContex(
        const CipherAlgorithm *algorithm, const void *key)
    : algorithm(algorithm), key(key)
{
}

void CipherAlgorithm::DecryptionContex::decrypt(
        const void *data, std::size_t dataLength,
        const void *iv, void *dest) const
{
    if (dataLength % algorithm->getBlockLength() != 0) {
        throw std::logic_error("dataLength must be aligned to block size!");
    }
    if (dataLength > INT_MAX) {
        throw std::runtime_error("dataLength too large!");
    }
    EVP_CIPHER_CTX ctx;
    EVP_CIPHER_CTX_init(&ctx);
    EVP_CIPHER_CTX_set_padding(&ctx, 0);
    if (EVP_DecryptInit(&ctx, algorithm->cipher, (const unsigned char *)key, (const unsigned char *)iv) != 1) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        throw CipherException("OpenSSL error!");
    }
    int outl;
    if (EVP_DecryptUpdate(&ctx, (unsigned char *)dest, &outl, (const unsigned char *)data, (int)dataLength) != 1) {
        EVP_CIPHER_CTX_cleanup(&ctx);
        throw CipherException("OpenSSL error!");
    }
    EVP_CIPHER_CTX_cleanup(&ctx);
}

std::size_t CipherAlgorithm::getKeySize() const
{
    return (std::size_t)::EVP_CIPHER_key_length(cipher);
}
std::size_t CipherAlgorithm::getBlockLength() const
{
    return (std::size_t)::EVP_CIPHER_block_size(cipher);
}
std::size_t CipherAlgorithm::getIVLength() const
{
    return (std::size_t)::EVP_CIPHER_iv_length(cipher);
}

const CipherAlgorithm &CipherAlgorithm::getAlgorithm(
        const std::string &cipherName, std::size_t keySize,
        const std::string &cipherMode)
{
    typedef std::tuple<std::string, std::size_t, std::string> Key;
    static const std::map<Key, CipherAlgorithm> algorithms = {
        { Key { "aes",   16, "ecb" }, CipherAlgorithm(::EVP_aes_128_ecb()) },
        { Key { "aes",   16, "cbc" }, CipherAlgorithm(::EVP_aes_128_cbc()) },
        { Key { "aes",   32, "xts" }, CipherAlgorithm(::EVP_aes_128_xts()) },
        { Key { "aes",   24, "ecb" }, CipherAlgorithm(::EVP_aes_192_ecb()) },
        { Key { "aes",   24, "cbc" }, CipherAlgorithm(::EVP_aes_192_cbc()) },
        { Key { "aes",   32, "ecb" }, CipherAlgorithm(::EVP_aes_256_ecb()) },
        { Key { "aes",   32, "cbc" }, CipherAlgorithm(::EVP_aes_256_cbc()) },
        { Key { "aes",   64, "xts" }, CipherAlgorithm(::EVP_aes_256_xts()) },
        { Key { "cast5", 16, "ecb" }, CipherAlgorithm(::EVP_cast5_ecb()) },
        { Key { "cast5", 16, "cbc" }, CipherAlgorithm(::EVP_cast5_cbc()) },
    };

    return algorithms.at(std::forward_as_tuple(cipherName, keySize, cipherMode));
}

} // namespace cipheralgorithm
} // namespace libcipherspec
