#ifndef LIBPBKDF2_COMPUTE_OPENCL_GLOBALCONTEXT_H
#define LIBPBKDF2_COMPUTE_OPENCL_GLOBALCONTEXT_H

#include <string>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class GlobalContext
{
private:
    std::string dataDirectory;

public:
    inline const std::string &getDataDirectory() const { return dataDirectory; }

    inline GlobalContext(const std::string &dataDirectory)
        : dataDirectory(dataDirectory) { }
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_GLOBALCONTEXT_H
