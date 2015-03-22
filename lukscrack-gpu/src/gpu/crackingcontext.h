#ifndef LUKSCRACK_GPU_CRACKINGCONTEXT_H
#define LUKSCRACK_GPU_CRACKINGCONTEXT_H

// TODO: make generic
#include "libpbkdf2-compute-opencl/computecontext.h"

#include "passworddata.h"

namespace lukscrack {
namespace gpu {

using namespace libpbkdf2::compute::opencl;

class CrackingContext
{
private:
    const PasswordData *passwordData;

    HashFunctionContext hfContext;

    ComputeContext keyslotContext;
    ComputeContext mkDigestContext;

public:
    inline const PasswordData *getPasswordData() const { return passwordData; }

    inline const ComputeContext &getKeyslotContext() const { return keyslotContext; }
    inline const ComputeContext &getMKDigestContext() const { return mkDigestContext; }

    CrackingContext(
            const GlobalContext *global,
            const std::vector<cl::Device> &devices,
            const PasswordData *passwordData);
};

} // namespace gpu
} // namespace lukscrack

#endif // LUKSCRACK_GPU_CRACKINGCONTEXT_H
