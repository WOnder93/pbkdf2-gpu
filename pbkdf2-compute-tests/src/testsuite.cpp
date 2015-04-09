/*
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
