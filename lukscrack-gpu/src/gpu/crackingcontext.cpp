#include "crackingcontext.h"

namespace lukscrack {
namespace gpu {

CrackingContext::CrackingContext(const GlobalContext *global, const std::vector<Device> &devices, const PasswordData *passwordData)
    : passwordData(passwordData)
{
    auto &hashSpec = passwordData->getHashSpec();
    auto &cipherName = passwordData->getCipherName();
    auto &mode = passwordData->getCipherMode();
    size_t keySize = passwordData->getKeySize();
    size_t afStripes = passwordData->getKeyslotStripes();

    size_t delim = mode.find('-');
    std::string cipherMode, ivmode;
    if (delim == std::string::npos) {
        cipherMode.assign(mode);
    } else {
        cipherMode.assign(mode.begin(), mode.begin() + delim);
        ivmode.assign(mode.begin() + delim + 1, mode.end());
    }

    decryptor = SectorDecryptor(cipherName, keySize, cipherMode, ivmode);
    afMerger = AFMerger(keySize, afStripes, hashSpec);

    hfContext = HashFunctionContext(global, devices, passwordData->getHashSpec());

    keyslotContext = ComputeContext(
                &hfContext,
                passwordData->getKeyslotSalt(), PasswordData::SALT_LENGTH,
                passwordData->getKeySize(), passwordData->getKeyslotIter());
    mkDigestContext = ComputeContext(
                &hfContext,
                passwordData->getMasterKeyDigestSalt(), PasswordData::SALT_LENGTH,
                PasswordData::MASTER_KEY_DIGEST_LENGTH, passwordData->getMasterKeyDigestIter());
}

} // namespace gpu
} // namespace lukscrack
