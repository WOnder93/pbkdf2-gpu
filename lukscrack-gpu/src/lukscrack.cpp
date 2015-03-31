#include "lukscrack.h"

namespace lukscrack {

LuksCrack::LuksCrack(
        const GlobalContext *globalContext, const std::vector<Device> &devices,
        const PasswordData *passwordData,
        PasswordGenerator *pwGen, size_t batchSize)
    : globalContext(globalContext), pwDistributor(pwGen),
      context(globalContext, devices, passwordData),
      devContexts(), threads(devices.size()),
      passwordMutex(), password(), passwordFound(false)
{
    devContexts.reserve(devices.size());
    for (auto &dev : devices) {
        devContexts.emplace_back(new gpu::DeviceCrackingContext(
                    &context, &pwDistributor,
                    [this] (const std::string &found) { setFoundPassword(found); },
                    dev, batchSize));
    }
}

void LuksCrack::setFoundPassword(const std::string &found)
{
    std::lock_guard<std::mutex> guard(passwordMutex);
    password = found;
    passwordFound = true;
    requestStopCracking();
}

void LuksCrack::runCracking()
{
    for (size_t i = 0; i < devContexts.size(); i++) {
        threads[i] = std::thread(
                    &gpu::DeviceCrackingContext::runCracking,
                    devContexts[i].get());
    }

    for (auto &thread : threads) {
        thread.join();
    }
}

void LuksCrack::requestStopCracking()
{
    for (auto &dc : devContexts) {
        dc.get()->requestStopCracking();
    }
}

}
