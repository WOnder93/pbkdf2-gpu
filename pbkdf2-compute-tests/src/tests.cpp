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

#include "tests.h"

#include "testsuite.h"
#include "utils.h"

#include "pbkdf2testvector.h"

#include "libpbkdf2-compute-opencl/types.h"
#include "libpbkdf2-compute-cpu/types.h"

#include <cstring>

namespace libpbkdf2 {
namespace compute {
namespace tests {

using namespace pbkdf2_gpu::common;

template<class Types>
void runComputeTests(
        std::ostream &out, const std::string &platformName, const std::string &hashSpec,
        const typename Types::TGlobalContext *globalCtx, Logger *logger)
{
    auto &devices = globalCtx->getAllDevices();
    if (devices.size() == 0) {
        out << "No devices found, tests cannot be run!" << std::endl;
        return;
    }

    std::vector<Test> tests;
    auto &testVectors = PBKDF2TestVector::getStandardVectors(hashSpec);

    typename Types::THashFunctionContext hfContext(globalCtx, { devices[0] }, hashSpec);
    for (const PBKDF2TestVector &tv : testVectors) {
        tests.push_back(Test("Test vector '" + tv.getName() + "'", [&] {
            typename Types::TComputeContext computeContext(
                        &hfContext,
                        tv.getSaltData(), tv.getSaltLength(),
                        tv.getDerivedKeyLength(), tv.getIterationCount());
            typename Types::TDeviceContext deviceContext(&computeContext, devices[0]);
            typename Types::TProcessingUnit unit(&deviceContext, 1, logger);

            {
                auto passwords = unit.openPasswords();
                typename Types::TProcessingUnit::Passwords::Writer writer(passwords);
                writer.setPassword(
                            (const char *)tv.getPasswordData(),
                            tv.getPasswordLength());
            }

            unit.beginProcessing();
            unit.endProcessing();

            bool match = false;
            {
                auto keys = unit.openDerivedKeys();
                typename Types::TProcessingUnit::DerivedKeys::Reader reader(keys);
                match = memcmp(tv.getDerivedKeyData(),
                               reader.getDerivedKey(),
                               tv.getDerivedKeyLength()) == 0;
            }
            Utils::assert("Output should be equal to the expected output", match);
        }));
    }

    TestSuite suite("PBKDF2-HMAC-" + hashSpec + " " + platformName + " compute library tests (standard test vectors)", tests);
    suite.runTests(out);
}

static void runOpenCLTests(std::ostream &out, Logger *logger)
{
    using namespace libpbkdf2::compute::opencl;

    GlobalContext global("data");

    runComputeTests<Types>(out, "OpenCL", "sha1", &global, logger);
}

static void runCPUTests(std::ostream &out, Logger *logger)
{
    using namespace libpbkdf2::compute::cpu;

    GlobalContext global(nullptr);

    runComputeTests<Types>(out, "CPU", "sha1", &global, logger);
}

void Tests::runTests(std::ostream &out)
{
    RootLogger logger(&out);

    SubLogger openclLogger(&logger, "OpenCL: ");
    runOpenCLTests(out, &openclLogger);

    SubLogger cpuLogger(&logger, "CPU: ");
    runCPUTests(out, &cpuLogger);
}

} // namespace tests
} // namespace compute
} // namespace libpbkdf2
