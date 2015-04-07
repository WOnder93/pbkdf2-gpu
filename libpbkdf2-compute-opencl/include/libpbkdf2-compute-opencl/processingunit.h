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

    cl::CommandQueue cmdQueue;

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
    class Passwords
    {
    private:
        const ProcessingUnit *parent;
        void *hostBuffer;

    public:
        class Writer
        {
        private:
            cl_uint *dest;

            size_t count;
            size_t inputSize;
            const HashAlgorithm *hashAlg;
            std::unique_ptr<cl_uint[]> buffer;

        public:
            Writer(const Passwords &parent, size_t index = 0);

            void moveForward(size_t offset);
            void moveBackwards(size_t offset);

            void setPassword(const void *pw, size_t pwSize) const;
        };

        Passwords(const ProcessingUnit *parent);
        ~Passwords();
    };

    class DerivedKeys
    {
    private:
        const ProcessingUnit *parent;
        void *hostBuffer;

    public:
        class Reader
        {
        private:
            const cl_uint *src;

            size_t count;
            size_t outputSize;
            size_t outputBlockCount;
            size_t outputBlockSize;
            std::unique_ptr<cl_uint[]> buffer;

        public:
            Reader(const DerivedKeys &parent, size_t index = 0);

            void moveForward(size_t offset);
            void moveBackwards(size_t offset);

            const void *getDerivedKey() const;
        };

        DerivedKeys(const ProcessingUnit *parent);
        ~DerivedKeys();
    };

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

    inline Passwords openPasswords() { return Passwords(this); }
    inline DerivedKeys openDerivedKeys() { return DerivedKeys(this); }

    void beginProcessing();
    void endProcessing();
};

} // namespace opencl
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_OPENCL_PROCESSINGUNIT_H
