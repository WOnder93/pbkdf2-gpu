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
static void runComputeTests(
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
        tests.push_back(Test("Test vector '" + tv.getParams().getName() + "'", [&] {
            typename Types::TComputeContext computeContext(
                        &hfContext,
                        tv.getParams().getSaltData(), tv.getParams().getSaltLength(),
                        tv.getParams().getDerivedKeyLength(),
                        tv.getParams().getIterationCount());
            typename Types::TDeviceContext deviceContext(&computeContext, devices[0]);
            typename Types::TProcessingUnit unit(&deviceContext, 1, logger);

            {
                typename Types::TProcessingUnit::PasswordWriter writer(unit);
                writer.setPassword(
                            (const char *)tv.getParams().getPasswordData(),
                            tv.getParams().getPasswordLength());
            }

            unit.beginProcessing();
            unit.endProcessing();

            bool match = false;
            {
                typename Types::TProcessingUnit::DerivedKeyReader reader(unit);
                match = memcmp(tv.getDerivedKeyData(),
                               reader.getDerivedKey(),
                               tv.getParams().getDerivedKeyLength()) == 0;
            }
            Utils::assert("Output should be equal to the expected output", match);
        }));
    }

    TestSuite suite("PBKDF2-HMAC-" + hashSpec + " " + platformName + " compute library tests (standard test vectors)", tests);
    suite.runTests(out);
}

static void runCPUvsOpenCLTests(std::ostream &out, const std::string &hashSpec,
                              const cpu::GlobalContext *globalCpu,
                              const opencl::GlobalContext *globalCl,
                              Logger *logger)
{
    auto &devicesCpu = globalCpu->getAllDevices();
    auto &devicesCl = globalCl->getAllDevices();
    if (devicesCpu.empty()) {
        out << "No CPU devices, tests cannot be run!" << std::endl;
        return;
    }
    if (devicesCl.empty()) {
        out << "No OpenCL devices, tests cannot be run!" << std::endl;
        return;
    }

    std::vector<Test> tests;

    cpu::HashFunctionContext hfCtxCpu(globalCpu, devicesCpu, hashSpec);
    opencl::HashFunctionContext hfCtxCl(globalCl, devicesCl, hashSpec);
    for (const PBKDF2Parameters &tp : PBKDF2Parameters::testParameters) {
        tests.push_back(Test("Test parameters '" + tp.getName() + "'", [&](){
            cpu::ComputeContext computeCtxCpu(
                        &hfCtxCpu, tp.getSaltData(), tp.getSaltLength(),
                        tp.getDerivedKeyLength(), tp.getIterationCount());
            opencl::ComputeContext computeCtxCl(
                        &hfCtxCl, tp.getSaltData(), tp.getSaltLength(),
                        tp.getDerivedKeyLength(), tp.getIterationCount());

            cpu::DeviceContext deviceCtxCpu(&computeCtxCpu, devicesCpu[0]);
            opencl::DeviceContext deviceCtxCl(&computeCtxCl, devicesCl[0]);

            cpu::ProcessingUnit unitCpu(&deviceCtxCpu, 1, logger);
            opencl::ProcessingUnit unitCl(&deviceCtxCl, 1, logger);

            {
                cpu::ProcessingUnit::PasswordWriter writerCpu(unitCpu);
                opencl::ProcessingUnit::PasswordWriter writerCl(unitCl);

                writerCpu.setPassword(
                            (const char *)tp.getPasswordData(),
                            tp.getPasswordLength());
                writerCl.setPassword(
                            (const char *)tp.getPasswordData(),
                            tp.getPasswordLength());
            }

            unitCpu.beginProcessing();
            unitCl.beginProcessing();
            unitCpu.endProcessing();
            unitCl.endProcessing();

            bool match = false;
            {
                cpu::ProcessingUnit::DerivedKeyReader readerCpu(unitCpu);
                opencl::ProcessingUnit::DerivedKeyReader readerCl(unitCl);

                match = memcmp(readerCpu.getDerivedKey(),
                               readerCl.getDerivedKey(),
                               tp.getDerivedKeyLength()) == 0;
            }
            Utils::assert("Output of CPU module should equal the output of OpenCL mode", match);
        }));
    }

    TestSuite suite("PBKDF2-HMAC-" + hashSpec + " CPU vs OpenCL comparison", tests);
    suite.runTests(out);
}

static void runAllCPUvsOpenCLTests(std::ostream &out, Logger *logger)
{
    cpu::GlobalContext globalCpu(nullptr);
    opencl::GlobalContext globalCl(nullptr);

    //runCPUvsOpenCLTests(out, "ripemd160", &globalCpu, &globalCl, logger);
}

static void runOpenCLTests(std::ostream &out, Logger *logger)
{
    using namespace libpbkdf2::compute::opencl;

    GlobalContext global(nullptr);

    runComputeTests<Types>(out, "OpenCL", "sha1",   &global, logger);
    runComputeTests<Types>(out, "OpenCL", "sha256", &global, logger);
}

static void runCPUTests(std::ostream &out, Logger *logger)
{
    using namespace libpbkdf2::compute::cpu;

    GlobalContext global(nullptr);

    runComputeTests<Types>(out, "CPU", "ripemd160", &global, logger);
    runComputeTests<Types>(out, "CPU", "sha1",      &global, logger);
    runComputeTests<Types>(out, "CPU", "sha256",    &global, logger);
    runComputeTests<Types>(out, "CPU", "sha512",    &global, logger);
}

void Tests::runTests(std::ostream &out)
{
    RootLogger logger(&out);

    SubLogger cpuVsOpenClLogger(&logger, "CPU vs OpenCL: ");
    runAllCPUvsOpenCLTests(out, &cpuVsOpenClLogger);

    SubLogger openclLogger(&logger, "OpenCL: ");
    runOpenCLTests(out, &openclLogger);

    SubLogger cpuLogger(&logger, "CPU: ");
    runCPUTests(out, &cpuLogger);
}

} // namespace tests
} // namespace compute
} // namespace libpbkdf2
