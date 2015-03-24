#include "processingunit.h"

#include "alignment.h"

#include <cstring>

#define DEBUG_BUFFER_SIZE 4

namespace libpbkdf2 {
namespace compute {
namespace opencl {

ProcessingUnit::ProcessingUnit(const DeviceContext *context, size_t batchSize)
    : context(context), batchSize(batchSize)
{
    auto computeContext = context->getParentContext();
    auto hfContext = computeContext->getParentContext();

    auto hashAlg = hfContext->getHashAlgorithm();
    size_t ibl = hashAlg->getInputBlockLength();
    size_t obl = hashAlg->getOutputBlockLength();

    size_t inputLength = ibl;
    size_t outputLength = ALIGN(obl, computeContext->getDerivedKeyLength());

    inputSize = inputLength / sizeof(cl_uint);
    outputSize = outputLength / sizeof(cl_uint);

    inputDataBuffer = std::unique_ptr<cl_uint[]>(new cl_uint[inputSize]);
    outputDataBuffer = std::unique_ptr<cl_uint[]>(new cl_uint[outputSize]);

    outputBlocks = outputLength / obl;

    auto &clContext = hfContext->getContext();
    inputBuffer = cl::Buffer(clContext, CL_MEM_READ_ONLY, inputLength * batchSize);
    outputBuffer = cl::Buffer(clContext, CL_MEM_WRITE_ONLY, outputLength * batchSize);
    debugBuffer = cl::Buffer(clContext, CL_MEM_WRITE_ONLY, DEBUG_BUFFER_SIZE);

    kernel = cl::Kernel(computeContext->getProgram(), "pbkdf2_kernel");
    kernel.setArg<cl::Buffer>(0, inputBuffer);
    kernel.setArg<cl::Buffer>(1, outputBuffer);
    kernel.setArg<cl::Buffer>(2, computeContext->getSaltBuffer());
    kernel.setArg<cl_uint>   (3, outputBlocks);
    kernel.setArg<cl_uint>   (4, computeContext->getIterationCount());
    kernel.setArg<cl_uint>   (5, batchSize);
    kernel.setArg<cl::Buffer>(6, debugBuffer);
}

void ProcessingUnit::writePasswords(std::function<PasswordGenerator> passwordGenerator)
{
    auto &cmdQueue = context->getCommandQueue();
    auto computeContext = context->getParentContext();
    auto hfContext = computeContext->getParentContext();
    auto hashAlg = hfContext->getHashAlgorithm();

    size_t ibl = hashAlg->getInputBlockLength();
    size_t obl = hashAlg->getOutputBlockLength();

    size_t inputBufferSize = inputSize * batchSize * sizeof(cl_uint);
    void *inputHostBuffer = cmdQueue.enqueueMapBuffer(inputBuffer, true, CL_MAP_WRITE, 0, inputBufferSize);

    cl_uint *dest = (cl_uint *)inputHostBuffer;
    cl_uint *buffer = inputDataBuffer.get();
    for (size_t col = 0; col < batchSize; col++) {
        const char *pw;
        size_t pwSize;
        passwordGenerator(pw, pwSize);

        if (pwSize > ibl) {
            /* pre-hash password according to HMAC spec: */
            hashAlg->computeDigest(pw, pwSize, buffer);
            std::memset((char *)buffer + obl, 0, ibl - obl);
        } else {
            std::memcpy((char *)buffer, pw, pwSize);
            std::memset((char *)buffer + pwSize, 0, ibl - pwSize);
        }

        cl_uint *dest2 = dest;
        for (size_t row = 0; row < inputSize; row++) {
            *dest2 = buffer[row];
            dest2 += batchSize;
        }

        dest += 1;
    }

    cmdQueue.enqueueUnmapMemObject(inputBuffer, inputHostBuffer);
}

void ProcessingUnit::readDerivedKeys(std::function<KeyConsumer> keyConsumer)
{
    auto &cmdQueue = context->getCommandQueue();

    size_t outputBufferSize = outputSize * batchSize * sizeof(cl_uint);
    void *outputHostBuffer = cmdQueue.enqueueMapBuffer(outputBuffer, true, CL_MAP_READ, 0, outputBufferSize);

    KeyIterator keyIter((cl_uint *)outputHostBuffer, outputDataBuffer.get(), outputSize / outputBlocks, outputBlocks, batchSize);
    keyConsumer(keyIter);
    cmdQueue.enqueueUnmapMemObject(outputBuffer, outputHostBuffer);
}

void ProcessingUnit::beginProcessing()
{
    auto &cmdQueue = context->getCommandQueue();
    cmdQueue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(batchSize, outputBlocks), cl::NullRange, nullptr, &event);
}

void ProcessingUnit::endProcessing()
{
    event.wait();
    event = cl::Event();
}

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2
