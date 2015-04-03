#ifndef LUKSCRACK_LISTPASSWORDGENERATOR_H
#define LUKSCRACK_LISTPASSWORDGENERATOR_H

#include "passwordgenerator.h"

#include <memory>
#include <iostream>
#include <fstream>

namespace lukscrack {

class ListPasswordGenerator : public PasswordGenerator
{
private:
    std::unique_ptr<std::istream> streamOwner;
    std::istream *stream;
    std::string passwordBuffer;

public:
    inline ListPasswordGenerator(std::string fileName)
        : streamOwner(), passwordBuffer()
    {
        if (fileName == "-") {
            stream = &std::cin;
        } else {
            stream = new std::ifstream(fileName);
            streamOwner = std::unique_ptr<std::istream>(stream);
        }
        stream->exceptions(std::ios::failbit | std::ios::badbit);
    }

    bool nextPassword(const char *&pwData, size_t &pwLength) override;
};

} // namespace lukscrack

#endif // LUKSCRACK_LISTPASSWORDGENERATOR_H
