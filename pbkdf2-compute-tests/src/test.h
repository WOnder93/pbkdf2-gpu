#ifndef LIBPBKDF2_COMPUTE_TESTS_TEST_H
#define LIBPBKDF2_COMPUTE_TESTS_TEST_H

#include "testexception.h"

#include <string>
#include <functional>

namespace libpbkdf2 {
namespace compute {
namespace tests {

class Test
{
private:
    std::string name;
    std::function<void()> body;

public:
    inline const std::string &getName() const { return name; }

    inline Test(const std::string &name, std::function<void()> body) :
        name(name), body(body) { }

    inline void run() const {
        body();
    }
};

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_TEST_H
