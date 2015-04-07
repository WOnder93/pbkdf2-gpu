#ifndef LUKSCRACK_AFMERGER_H
#define LUKSCRACK_AFMERGER_H

#include "libhashspec-hashalgorithm/hashalgorithm.h"

#include <string>
#include <memory>

namespace lukscrack {

using namespace libhashspec::hashalgorithm;

class AFMerger
{
private:
    const HashAlgorithm *hashAlg;
    size_t blockSize;
    size_t blockCount;

    size_t digestSize;
    size_t subblockCount;
    size_t subblockPadding;

    void merge(const void *src, void *dst, unsigned char *buffer) const;

public:
    class Context
    {
    private:
        const AFMerger *parent;
        std::unique_ptr<unsigned char[]> buffer;

    public:
        Context(const AFMerger *parent);

        inline void merge(const void *src, void *dst) const
        {
            parent->merge(src, dst, buffer.get());
        }
    };

    inline AFMerger() { }

    AFMerger(size_t blockSize, size_t blockCount,
             const std::string &hashSpec);
};

} // namespace lukscrack

#endif // LUKSCRACK_AFMERGER_H
