#include "listpasswordgenerator.h"

namespace lukscrack {

bool ListPasswordGenerator::nextPassword(const char *&pwData, size_t &pwLength)
{
    if (stream->eof()) {
        return false;
    }
    passwordBuffer.clear();
    while (true) {
        char c;
        try {
            stream->get(c);
        } catch (const std::ios_base::failure &ex) {
            if (stream->eof()) {
                break;
            }
            throw ex;
        }
        if (c == '\n') {
            break;
        }
        passwordBuffer.push_back(c);
    }
    pwData = passwordBuffer.data();
    pwLength = passwordBuffer.size();
    return true;
}

} // namespace lukscrack
