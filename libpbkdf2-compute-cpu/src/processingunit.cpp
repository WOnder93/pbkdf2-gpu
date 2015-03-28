#include "processingunit.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

ProcessingUnit::ProcessingUnit(const DeviceContext *context, size_t batchSize)
    : context(context), batchSize(batchSize), taskFuture()
{
    auto computeContext = context->getParentContext();

    passwordBuffer.resize(batchSize);

    unsigned char *dkBuferPtr = new unsigned char[computeContext->getDerivedKeyLength() * batchSize];
    if (dkBuferPtr == nullptr) {
        throw std::bad_alloc();
    }
    dkBuffer = std::unique_ptr<unsigned char[]>(dkBuferPtr);
}

void ProcessingUnit::writePasswords(std::function<PasswordGenerator> passwordGenerator)
{
    for (size_t i = 0; i < batchSize; i++) {
        const char *pw;
        size_t pwSize;
        passwordGenerator(pw, pwSize);

        passwordBuffer[i] = std::string(pw, pw + pwSize);
    }
}

void ProcessingUnit::readDerivedKeys(std::function<KeyConsumer> keyConsumer)
{
    auto computeContext = context->getParentContext();

    KeyIterator keyIter((const char *)dkBuffer.get(), computeContext->getDerivedKeyLength(), batchSize);
    keyConsumer(keyIter);
}

void ProcessingUnit::beginProcessing()
{
    auto &coreCtx = context->getCoreContext();
    taskFuture = coreCtx.enqueueTask([this] () {
        auto computeContext = context->getParentContext();
        auto hfContext = computeContext->getParentContext();

        const unsigned char *salt = (const unsigned char *)computeContext->getSaltData();
        size_t saltLength = computeContext->getSaltLength();
        size_t dkLength = computeContext->getDerivedKeyLength();
        size_t iterations = computeContext->getIterationCount();

        const EVP_MD *digest = hfContext->getDigest();

        unsigned char *dest = dkBuffer.get();
        for (size_t i = 0; i < batchSize; i++) {
            auto &pw = passwordBuffer[i];
            PKCS5_PBKDF2_HMAC(pw.data(), (int)pw.size(),
                              salt, saltLength, iterations,
                              digest, dkLength, dest);
            dest += dkLength;
        }
    });
}

void ProcessingUnit::endProcessing()
{
    taskFuture.wait();
    taskFuture = std::future<void>();
}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2
