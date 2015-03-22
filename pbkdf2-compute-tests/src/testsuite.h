#ifndef LIBPBKDF2_COMPUTE_TESTS_TESTSUITE_H
#define LIBPBKDF2_COMPUTE_TESTS_TESTSUITE_H

#include "test.h"

#include <vector>
#include <string>
#include <iostream>

namespace libpbkdf2 {
namespace compute {
namespace tests {

class TestSuite
{
private:
    std::string name;
    std::vector<Test> tests;

public:
    inline const std::string &getName() const { return name; }

    inline TestSuite(const std::string &name, const std::vector<Test> &tests) :
        name(name), tests(tests) { }

    inline size_t runTests() const {
        return runTests(std::cerr);
    }
    size_t runTests(std::ostream &out) const;
};

} // namespace tests
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_TESTS_TESTSUITE_H
