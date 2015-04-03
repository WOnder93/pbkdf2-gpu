#include "devicecrackingcontext.h"

namespace lukscrack {
namespace gpu {

DeviceCrackingContext::DeviceCrackingContext(
        const CrackingContext *crackingContext,
        PasswordDistributor *pwDistributor,
        std::function<PasswordFoundCallback> callback,
        const Device &device, size_t batchSize)
    : crackingContext(crackingContext),
      pwDistributor(pwDistributor),
      callback(callback),
      pc1(crackingContext, device, batchSize),
      pc2(crackingContext, device, batchSize),
      stop(false)
{
}

void DeviceCrackingContext::runCracking()
{
    std::mutex &pwMutex = pwDistributor->getMutex();
    PasswordGenerator &pwGen = *pwDistributor->getPasswordGenerator();

    bool firstLoop = true;
    bool lastLoop = false;
    while (true) {
        if (!firstLoop) {
            ssize_t res = pc1.processResults();
            if (res >= 0) {
                callback(pc1.getCurrentPasswords()[res]);
                stop = true;
            }
        }
        if (!lastLoop) {
            std::lock_guard<std::mutex> guard(pwMutex);
            if (!pc1.initializePasswords(pwGen)) {
                stop = true;
            }
        }

        if (!firstLoop) {
            pc2.endMKDigestUnit();
        }
        if (!lastLoop) {
            pc1.beginKeyslotUnit();
        }

        if (!firstLoop) {
            ssize_t res = pc2.processResults();
            if (res >= 0) {
                callback(pc2.getCurrentPasswords()[res]);
                stop = true;
            }
        }
        if (!lastLoop) {
            std::lock_guard<std::mutex> guard(pwMutex);
            if (!pc2.initializePasswords(pwGen)) {
                stop = true;
            }
        } else {
            break;
        }

        firstLoop = false;

        if (stop) {
            stop = false;
            lastLoop = true;
        }

        pc1.endKeyslotUnit();
        pc2.beginKeyslotUnit();

        pc1.decryptMasterKey();

        pc2.endKeyslotUnit();
        pc1.beginMKDigestUnit();

        pc2.decryptMasterKey();

        pc1.endMKDigestUnit();
        pc2.beginMKDigestUnit();
    }
}

void DeviceCrackingContext::requestStopCracking()
{
    stop = true;
}

} // namespace gpu
} // namespace lukscrack
