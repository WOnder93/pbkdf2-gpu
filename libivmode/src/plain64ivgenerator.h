#ifndef LIBIVMODE_PLAIN64IVGENERATOR_H
#define LIBIVMODE_PLAIN64IVGENERATOR_H

#include "ivgenerator.h"

namespace libivmode {

class Plain64IVGenerator : public IVGenerator
{
private:
    class Context : public IVGenerator::Context
    {
    private:
        size_t ivLength;

    public:
        inline Context(size_t ivLength)
            : ivLength(ivLength)
        {
        }

        void generateIV(size_t sector, void *dest) const override;
    };

public:
    inline Plain64IVGenerator(const std::string &) { }

    std::shared_ptr<const IVGenerator::Context> createContext(
            size_t ivLength, const std::string &cipherName,
            const void *key, size_t keyLength) const override;
};

} // namespace libivmode

#endif // LIBIVMODE_PLAIN64IVGENERATOR_H
