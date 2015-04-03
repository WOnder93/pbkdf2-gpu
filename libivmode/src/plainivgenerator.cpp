#include "plainivgenerator.h"

#include <cstring>

namespace libivmode {

void PlainIVGenerator::Context::generateIV(size_t sector, void *dest) const
{
    unsigned char *cursor = (unsigned char *)dest;
    cursor[0] = sector         & 0xff;
    cursor[1] = (sector >>  8) & 0xff;
    cursor[2] = (sector >> 16) & 0xff;
    cursor[3] = (sector >> 24) & 0xff;
    std::memset(cursor + 4, 0, ivLength - 4);
}

std::shared_ptr<const IVGenerator::Context>
    PlainIVGenerator::createContext(
        size_t ivLength, const std::string &, const void *, size_t) const
{
    return std::make_shared<Context>(ivLength);
}

} // namespace libivmode
