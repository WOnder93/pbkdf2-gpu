#ifndef LIBPBKDF2_COMPUTE_CPU_PROCESSINGUNIT_H
#define LIBPBKDF2_COMPUTE_CPU_PROCESSINGUNIT_H

#include "devicecontext.h"

#include <vector>
#include <memory>
#include <functional>
#include <future>

namespace libpbkdf2 {
namespace compute {
namespace cpu {

class ProcessingUnit
{
private:
    const DeviceContext *context;
    size_t batchSize;

    std::vector<std::string> passwordBuffer;
    std::unique_ptr<unsigned char[]> dkBuffer;

    std::future<void> taskFuture;

public:
    class Passwords
    {
    private:
        ProcessingUnit *parent;

    public:
        class Writer
        {
        private:
            std::vector<std::string>::iterator it;

        public:
            Writer(const Passwords &parent, size_t index = 0);

            void moveForward(size_t offset);
            void moveBackwards(size_t offset);

            void setPassword(const void *pw, size_t pwSize) const;
        };

        inline Passwords(ProcessingUnit *parent)
            : parent(parent)
        {
        }
    };

    class DerivedKeys
    {
    private:
        const ProcessingUnit *parent;

    public:
        class Reader
        {
        private:
            size_t dkLength;
            const unsigned char *src;

        public:
            Reader(const DerivedKeys &parent, size_t index = 0);

            void moveForward(size_t offset);
            void moveBackwards(size_t offset);

            const void *getDerivedKey() const;
        };

        inline DerivedKeys(ProcessingUnit *parent)
            : parent(parent)
        {
        }
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

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_PROCESSINGUNIT_H
