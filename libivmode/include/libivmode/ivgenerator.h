#ifndef LIBIVMODE_IVGENERATOR_H
#define LIBIVMODE_IVGENERATOR_H

#include <string>
#include <memory>

namespace libivmode {

class IVGenerator
{
public:
    class Context
    {
    public:
        virtual ~Context();

        virtual void generateIV(size_t sector, void *dest) const = 0;
    };

    virtual ~IVGenerator();

    virtual std::shared_ptr<const Context> createContext(
            size_t ivLength,
            /* these arguments are used by essiv and such: */
            const std::string &cipherName,
            const void *key, size_t keyLength) const = 0;

    static std::shared_ptr<const IVGenerator> getGenerator(const std::string &ivmode);
};

} // namespace libivmode

#endif // LIBIVMODE_IVGENERATOR_H
