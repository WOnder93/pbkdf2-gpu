#ifndef LIBPBKDF2_COMPUTE_OPENCL_COMPUTECONTEXT_H
#define LIBPBKDF2_COMPUTE_OPENCL_COMPUTECONTEXT_H

#include "hashfunctioncontext.h"

namespace libpbkdf2 {
namespace compute {
namespace opencl {

class ComputeContext
{
    const HashFunctionContext *parentContext;

    cl::Program prog;
    cl::Buffer saltBuffer;
    size_t saltBufferSize;

    const void *saltData;
    size_t saltLength;
    size_t dkLength;
    size_t iterationCount;

public:
    inline const HashFunctionContext *getParentContext() const { return parentContext; }
    inline const cl::Program &getProgram() const { return prog; }
    inline const cl::Buffer &getSaltBuffer() const { return saltBuffer; }
    inline size_t getSaltBufferSize() const { return saltBufferSize; }
    inline const void *getSaltData() const { return saltData; }
    inline size_t getSaltLength() const { return saltLength; }
    inline size_t getDerivedKeyLength() const { return dkLength; }
    inline size_t getIterationCount() const { return iterationCount; }

    /**
     * @brief Empty constructor.
     * NOTE: Calling methods other than the destructor on an instance initialized
     * with empty constructor results in undefined behavior.
     */
    inline ComputeContext() { }

    ComputeContext(const ComputeContext &) = delete;
    ComputeContext &operator=(const ComputeContext &) = delete;

    ComputeContext(ComputeContext &&) = default;
    ComputeContext &operator=(ComputeContext &&) = default;

    ComputeContext(const HashFunctionContext *parentContext,
                   const void *salt, size_t saltLength,
                   size_t dkLength, size_t iterationCount);
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_COMPUTECONTEXT_H
