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
    typedef void PasswordGenerator(const char *&password, size_t &passwordSize);
    class KeyIterator
    {
    private:
        const char *data;
        size_t dataSize;
        size_t index;
        size_t count;

    public:
        inline KeyIterator(const char *first, size_t dataSize, size_t count)
            : data(first), dataSize(dataSize), index(0), count(count) { }

        inline const void *nextKey()
        {
            if (index == count) {
                return nullptr;
            }
            const void *ret = data;
            data += dataSize;
            index++;
            return ret;
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

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2

#endif // LIBPBKDF2_COMPUTE_CPU_PROCESSINGUNIT_H
