#ifndef LUKSCRACK_PASSWORDGENERATOR_H
#define LUKSCRACK_PASSWORDGENERATOR_H

#include <string>

namespace lukscrack {

class PasswordGenerator
{
public:
    virtual ~PasswordGenerator() { }

    virtual bool nextPassword(const char *&pwData, size_t &pwLength);
};

} // namespace lukscrack

#endif // LUKSCRACK_PASSWORDGENERATOR_H
