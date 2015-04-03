#ifndef LIBIVMODE_PLAINIVGENERATOR_H
#define LIBIVMODE_PLAINIVGENERATOR_H

#include "ivgenerator.h"

namespace libivmode {

class PlainIVGenerator : public IVGenerator
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
    inline PlainIVGenerator(const std::string &) { }

    std::shared_ptr<const IVGenerator::Context> createContext(
            size_t ivLength, const std::string &cipherName,
            const void *key, size_t keyLength) const override;
};

} // namespace libivmode

#endif // LIBIVMODE_PLAINIVGENERATOR_H
