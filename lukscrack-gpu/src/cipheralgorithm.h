#ifndef LUKSCRACK_CIPHERALGORITHM_H
#define LUKSCRACK_CIPHERALGORITHM_H

#include <cstdlib>
#include <string>
#include <memory>

namespace lukscrack {

class CipherAlgorithm
{
public:
    virtual ~CipherAlgorithm() { }

    virtual void decrypt(const void *key, const void *data, size_t length, void *dest) const = 0;

    static std::shared_ptr<const CipherAlgorithm> get(const std::string &spec, const std::string &mode, size_t keySize);
};

} // namespace lukscrack

#endif // LUKSCRACK_CIPHERALGORITHM_H
