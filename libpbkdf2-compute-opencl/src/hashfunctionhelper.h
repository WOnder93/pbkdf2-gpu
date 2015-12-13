#ifndef LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONHELPER_H
#define LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONHELPER_H

#include "openclwriter.h"

#include <string>
#include <vector>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class HashFunctionHelper
{
private:
    bool littleEndian;

    std::string wordClType;
    std::size_t wordBytes;

    std::size_t iblockWords;
    std::size_t oblockWords;

    std::size_t mlWords;

    std::vector<std::string> initState;

protected:
    HashFunctionHelper(
            bool littleEndian,
            const char *wordClType, std::size_t wordBytes,
            std::size_t iblockWords, std::size_t oblockWords,
            std::size_t mlWords, const char * const *initState);

public:
    bool isLittleEndian() const { return littleEndian; }

    const std::string &getWordClType() const { return wordClType; }
    std::size_t getWordBytes() const { return wordBytes; }

    std::size_t getInputBlockWords() const { return iblockWords; }
    std::size_t getOutputBlockWords() const { return oblockWords; }

    std::size_t getMessageLengthWords() const { return mlWords; }

    const std::vector<std::string> &getInitState() const { return initState; }

    virtual void writeDefinitions(OpenCLWriter &out) const = 0;

    virtual void writeUpdate(
            OpenCLWriter &out,
            const std::vector<std::string> &prevState,
            const std::vector<std::string> &inputBlock,
            const std::vector<std::string> &state,
            const std::vector<std::string> &buffer,
            bool swap = false) const = 0;

    static const HashFunctionHelper &getHelper(const std::string &hashSpec);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_HASHFUNCTIONHELPER_H
