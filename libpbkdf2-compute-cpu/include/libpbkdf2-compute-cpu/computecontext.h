#ifndef LIBPBKDF2_COMPUTE_CPU_COMPUTECONTEXT_H
#define LIBPBKDF2_COMPUTE_CPU_COMPUTECONTEXT_H

#include "hashfunctioncontext.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class ComputeContext
{
private:
    const HashFunctionContext *parentContext;

    const void *saltData;
    size_t saltLength;
    size_t dkLength;
    size_t iterationCount;

public:
    inline const HashFunctionContext *getParentContext() const { return parentContext; }

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

    inline ComputeContext(const HashFunctionContext *parentContext,
                   const void *salt, size_t saltLength,
                   size_t dkLength, size_t iterationCount)
        : parentContext(parentContext), saltData(salt), saltLength(saltLength),
          dkLength(dkLength), iterationCount(iterationCount)
    {
    }
};

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_COMPUTECONTEXT_H
