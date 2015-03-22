#include "benchmark.h"

#include "libpbkdf2-compute-opencl/types.h"
#include "libpbkdf2-compute-cpu/types.h"

#include <iostream>

using namespace std;
using namespace libpbkdf2::compute;

int main()
{
    cl::Platform platform = cl::Platform::getDefault();

    std::vector<cl::Device> devices;
    platform.getDevices(CL_DEVICE_TYPE_ALL, &devices);

    if (devices.size() == 0) {
        cerr << "No devices!" << endl;
        return 1;
    }

    std::cout << "Benchmarking OpenCL..." << std::endl;

    opencl::GlobalContext global("data");
    runBenchmark<opencl::Types>(&global, devices[0], "sha1", "saltySalt", 9,
            2 * 1024 /*iter*/, 20 /*dkLen*/, 4 * 1024 /*batchSize*/, 10 /*sampleCount*/);


    std::cout << "Benchmarking CPU..." << std::endl;

    runBenchmark<cpu::Types>(nullptr, nullptr, "sha1", "saltySalt", 9,
            2 * 1024 /*iter*/, 20 /*dkLen*/, 4 * 1024 /*batchSize*/, 10 /*sampleCount*/);
    return 0;
}
