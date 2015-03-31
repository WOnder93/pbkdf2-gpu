#ifndef LUKSCRACK_PASSWORDDISTRIBUTOR_H
#define LUKSCRACK_PASSWORDDISTRIBUTOR_H

#include "passwordgenerator.h"

#include <memory>
#include <mutex>

namespace lukscrack {

class PasswordDistributor
{
private:
    PasswordGenerator *generator;
    std::mutex mutex;

public:
    inline PasswordGenerator *getPasswordGenerator() const { return generator; }
    inline std::mutex &getMutex() { return mutex; }

    inline PasswordDistributor(PasswordGenerator *generator)
        : generator(generator), mutex()
    {
    }
};

} // namespace lukscrack

#endif // LUKSCRACK_PASSWORDDISTRIBUTOR_H
