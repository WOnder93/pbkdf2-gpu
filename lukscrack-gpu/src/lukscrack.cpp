#include "lukscrack.h"

namespace lukscrack {

LuksCrack::LuksCrack(
        const GlobalContext *globalContext, const std::vector<Device> &devices,
        const PasswordData *passwordData, PasswordGenerator *pwGen,
        size_t threadPoolSize, size_t batchSize)
    : pwDistributor(pwGen), threadPool(threadPoolSize),
      context(globalContext, devices, passwordData), devContexts(),
      passwordMutex(), password(), passwordFound(false)
{
    devContexts.reserve(devices.size());
    for (auto &dev : devices) {
        devContexts.emplace_back(new gpu::DeviceCrackingContext(
                    &context, &pwDistributor, &threadPool,
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
    std::vector<std::thread> threads(devContexts.size());
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

} // namespace lukscrack
