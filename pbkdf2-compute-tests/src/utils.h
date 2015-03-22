#ifndef LIBPBKDF2_COMPUTE_TESTS_UTILS_H
#define LIBPBKDF2_COMPUTE_TESTS_UTILS_H

#include "testexception.h"

#include <string>
#include <sstream>

namespace libpbkdf2 {
namespace compute {
namespace tests {

namespace Utils
{
    inline void fail(std::string reason) {
        throw TestException(reason);
    }

    inline void assert(std::string assertion, bool condition) {
        if (!condition) {
            throw TestException("Assertion failed: " + assertion);
        }
    }

    template<class T>
    inline void assertEquals(T actual, T expected) {
        std::ostringstream buf;
        buf << "'" << actual << "' should equal '" << expected << "'";
        assert(buf.str(), actual == expected);
    }
}

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_UTILS_H
