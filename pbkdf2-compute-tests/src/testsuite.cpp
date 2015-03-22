#include "testsuite.h"

namespace libpbkdf2 {
namespace compute {
namespace tests {

size_t TestSuite::runTests(std::ostream &out) const {
    size_t failed = 0;
    out << "Running test suite '" << name << "'..." << std::endl;
    for (const Test &test : tests) {
        try {
            test.run();
        } catch (const TestException &ex) {
            out << "  Test '" << test.getName() << "' failed:" << std::endl;
            out << "    " << ex.getMessage() << std::endl;
            failed++;
        } catch (const std::exception &ex) {
            out << "  Unhandled exception in test '" << test.getName() << "':" << std::endl;
            out << "    " << ex.what() << std::endl;
            failed++;
        }
    }
    if (failed == 0) {
        out << "  All tests passed!" << std::endl;
    } else {
        out << "  " << failed << "/" << tests.size() << " tests failed!" << std::endl;
    }
    return failed;
}

} // namespace tests
} // namespace compute
} // namespace libpbkdf2
