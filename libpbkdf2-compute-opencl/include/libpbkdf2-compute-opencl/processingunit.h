#ifndef LIBPBKDF2_COMPUTE_OPENCL_PROCESSINGUNIT_H
#define LIBPBKDF2_COMPUTE_OPENCL_PROCESSINGUNIT_H

#include "devicecontext.h"

#include <functional>
#include <iterator>
#include <memory>

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class ProcessingUnit
{
private:
    const DeviceContext *context;
    size_t batchSize;

    cl::Buffer inputBuffer;
    cl::Buffer outputBuffer;
    cl::Buffer debugBuffer;

    cl::Kernel kernel;

    size_t inputSize;
    size_t outputSize;

    size_t outputBlocks;

    std::unique_ptr<cl_uint[]> inputDataBuffer;
    std::unique_ptr<cl_uint[]> outputDataBuffer;

    cl::Event event;

public:
    typedef void PasswordGenerator(const char *&password, size_t &passwordSize);
    class KeyIterator
    {
    private:
        const cl_uint *data;
        cl_uint *buffer;
        size_t outputBlockSize;
        size_t outputBlockCount;
        size_t count;
        size_t index;

    public:
        inline KeyIterator(const cl_uint *data, cl_uint *buffer,
                           size_t outputBlockSize, size_t outputBlockCount,
                           size_t count)
            : data(data), buffer(buffer),
              outputBlockSize(outputBlockSize), outputBlockCount(outputBlockCount),
              count(count), index(0)
        {
        }

        const void *nextKey()
        {
            if (index == count) {
                return nullptr;
            }

            cl_uint *dst = buffer;
            for (size_t outputBlock = 0; outputBlock < outputBlockCount; outputBlock++) {
                const cl_uint *src = data + outputBlock;
                for (size_t row = 0; row < outputBlockSize; row++) {
                    *dst = *src;
                    src += count * outputBlockCount;
                    dst += 1;
                }
            }
            data += outputBlockCount;
            index++;
            return buffer;
        }
    };
    typedef void KeyConsumer(KeyIterator &iter);

    inline size_t getBatchSize() const { return batchSize; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline ProcessingUnit() { }

    ProcessingUnit(const ProcessingUnit &) = delete;
    ProcessingUnit &operator=(const ProcessingUnit &) = delete;

    ProcessingUnit(ProcessingUnit &&) = default;
    ProcessingUnit &operator=(ProcessingUnit &&) = default;

    ProcessingUnit(const DeviceContext *context, size_t batchSize);

    void writePasswords(std::function<PasswordGenerator> passwordGenerator);
    void readDerivedKeys(std::function<KeyConsumer> keyConsumer);

    void beginProcessing();
    void endProcessing();
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_PROCESSINGUNIT_H
