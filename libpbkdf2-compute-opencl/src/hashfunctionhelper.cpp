#include "hashfunctionhelper.h"

#include "sha1hashfunctionhelper.h"

#include <unordered_map>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

HashFunctionHelper::HashFunctionHelper(
        bool littleEndian,
        const char *wordClType, std::size_t wordBytes,
        std::size_t iblockWords, std::size_t oblockWords,
        std::size_t mlWords, const char * const *initState)
    : littleEndian(littleEndian),
      wordClType(wordClType), wordBytes(wordBytes),
      iblockWords(iblockWords), oblockWords(oblockWords),
      mlWords(mlWords), initState()
{
    this->initState.reserve(oblockWords);
    for (std::size_t i = 0; i < oblockWords; i++) {
        this->initState.emplace_back(initState[i]);
    }
}

const HashFunctionHelper &HashFunctionHelper::getHelper(const std::string &hashSpec)
{
    static const Sha1HashFunctionHelper SHA1;

    static const std::unordered_map<std::string, const HashFunctionHelper &> helpers = {
        { "sha1", SHA1 },
    };

    return helpers.at(hashSpec);
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

