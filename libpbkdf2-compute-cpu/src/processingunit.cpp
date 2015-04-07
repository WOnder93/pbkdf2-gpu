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

ProcessingUnit::Passwords::Writer::Writer(
        const Passwords &parent, size_t index)
{
    auto unit = parent.parent;
    it = unit->passwordBuffer.begin() + index;
}

void ProcessingUnit::Passwords::Writer::moveForward(size_t offset)
{
    it += offset;
}

void ProcessingUnit::Passwords::Writer::moveBackwards(size_t offset)
{
    it -= offset;
}

void ProcessingUnit::Passwords::Writer::setPassword(
        const void *pw, size_t pwSize) const
{
    it->assign((char *)pw, (char *)pw + pwSize);
}

ProcessingUnit::DerivedKeys::Reader::Reader(
        const DerivedKeys &parent, size_t index)
{
    auto unit = parent.parent;
    dkLength = unit->context->getParentContext()->getDerivedKeyLength();
    src = unit->dkBuffer.get() + index * dkLength;
}

void ProcessingUnit::DerivedKeys::Reader::moveForward(size_t offset)
{
    src += offset * dkLength;
}

void ProcessingUnit::DerivedKeys::Reader::moveBackwards(size_t offset)
{
    src -= offset * dkLength;
}

const void *ProcessingUnit::DerivedKeys::Reader::getDerivedKey() const
{
    return src;
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
