#include "tests.h"

#include "testsuite.h"
#include "utils.h"

#include "pbkdf2testvector.h"

#include "libpbkdf2-compute-opencl/types.h"

#include <cstring>

namespace libpbkdf2 {
namespace compute {
namespace tests {

template<typename Types>
void runComputeTests(
        std::ostream &out, const std::string &platformName, const std::string &hashSpec,
        const typename Types::TGlobalContext *globalCtx, const typename Types::TDevice &dev)
{
    std::vector<Test> tests;
    auto &testVectors = PBKDF2TestVector::getStandardVectors(hashSpec);

    std::vector<typename Types::TDevice> devices(&dev, &dev + 1);
    typename Types::THashFunctionContext hfContext(globalCtx, devices, hashSpec);
    for (const PBKDF2TestVector &tv : testVectors) {
        tests.push_back(Test("Test vector '" + tv.getName() + "'", [&] {
            typename Types::TComputeContext computeContext(
                        &hfContext,
                        tv.getSaltData(), tv.getSaltLength(),
                        tv.getDerivedKeyLength(), tv.getIterationCount());
            typename Types::TDeviceContext deviceContext(&computeContext, 0 /* use the first (and only) device */);
            typename Types::TProcessingUnit unit(&deviceContext, 1);

            unit.writePasswords([&tv](const char *&password, size_t &passwordSize) {
                password = (const char *)tv.getPasswordData();
                passwordSize = tv.getPasswordLength();
            });
            unit.beginProcessing();
            unit.endProcessing();

            bool match = false;
            unit.readDerivedKeys([&match, &tv](typename Types::TProcessingUnit::KeyIterator &keyIter) {
                match = memcmp(tv.getDerivedKeyData(), keyIter.nextKey(), tv.getDerivedKeyLength()) == 0;
            });
            Utils::assert("Output should be equal to the expected output", match);
        }));
    }

    TestSuite suite("PBKDF2-HMAC-" + hashSpec + " " + platformName + " compute library tests (standard test vectors)", tests);
    suite.runTests(out);
}

static void runOpenCLTests(std::ostream &out)
{
    using namespace libpbkdf2::compute::opencl;

    cl::Platform platform = cl::Platform::getDefault();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if (devices.size() == 0) {
        out << "No devices found, tests cannot be run!" << std::endl;
        return;
    }

    GlobalContext global("data");

    runComputeTests<Types>(out, "OpenCL", "sha1", &global, devices[0]);
}

void Tests::runTests(std::ostream &out)
{
    runOpenCLTests(out);
}

} // namespace tests
} // namespace compute
} // namespace libpbkdf2
