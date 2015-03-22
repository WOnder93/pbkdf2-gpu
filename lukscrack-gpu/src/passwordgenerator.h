#ifndef LUKSCRACK_PASSWORDGENERATOR_H
#define LUKSCRACK_PASSWORDGENERATOR_H

#include <string>

namespace lukscrack {

class PasswordGenerator
{
public:
    virtual ~PasswordGenerator() { }

    virtual const std::string &getCurrentPassword() const;
    virtual bool nextPassword();
};

} // namespace lukscrack

#endif // LUKSCRACK_PASSWORDGENERATOR_H
