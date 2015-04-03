#include "plain64ivgenerator.h"

#include <cstring>

namespace libivmode {

void Plain64IVGenerator::Context::generateIV(size_t sector, void *dest) const
{
    unsigned char *cursor = (unsigned char *)dest;
    cursor[0] = sector         & 0xff;
    cursor[1] = (sector >>  8) & 0xff;
    cursor[2] = (sector >> 16) & 0xff;
    cursor[3] = (sector >> 24) & 0xff;
    cursor[4] = (sector >> 32) & 0xff;
    cursor[5] = (sector >> 40) & 0xff;
    cursor[6] = (sector >> 48) & 0xff;
    cursor[7] = (sector >> 56) & 0xff;
    std::memset(cursor + 8, 0, ivLength - 8);
}

std::shared_ptr<const IVGenerator::Context>
    Plain64IVGenerator::createContext(
        size_t ivLength, const std::string &, const void *, size_t) const
{
    return std::make_shared<Context>(ivLength);
}

} // namespace libivmode
