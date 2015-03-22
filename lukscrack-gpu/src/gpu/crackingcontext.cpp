#include "crackingcontext.h"

namespace lukscrack {
namespace gpu {

CrackingContext::CrackingContext(const GlobalContext *global, const std::vector<cl::Device> &devices, const PasswordData *passwordData)
    : passwordData(passwordData)
{
    hfContext = HashFunctionContext(global, devices, passwordData->getHashSpec());

    keyslotContext = ComputeContext(
                &hfContext,
                passwordData->getKeyslotSalt(), PasswordData::SALT_SIZE,
                passwordData->getKeySize(), passwordData->getKeyslotIter());
    mkDigestContext = ComputeContext(
                &hfContext,
                passwordData->getMasterKeyDigestSalt(), PasswordData::SALT_SIZE,
                PasswordData::MASTER_KEY_DIGEST_SIZE, passwordData->getMasterKeyDigestIter());
}

} // namespace gpu
} // namespace lukscrack
