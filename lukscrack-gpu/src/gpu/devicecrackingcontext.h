#ifndef LUKSCRACK_GPU_DEVICECRACKINGCONTEXT_H
#define LUKSCRACK_GPU_DEVICECRACKINGCONTEXT_H

#include "passworddistributor.h"
#include "batchprocessingcontext.h"

#include <thread>
#include <atomic>
#include <functional>

namespace lukscrack {
namespace gpu {

typedef void PasswordFoundCallback(const std::string &password);

class DeviceCrackingContext
{
private:
    const CrackingContext *crackingContext;
    PasswordDistributor *pwDistributor;
    std::function<PasswordFoundCallback> callback;

    BatchProcessingContext pc1, pc2, pc3;

    std::atomic_bool stop;

public:
    inline DeviceCrackingContext() { }

    DeviceCrackingContext(const CrackingContext *crackingContext,
                          PasswordDistributor *pwDistributor,
                          std::function<PasswordFoundCallback> callback,
                          const Device &device, size_t batchSize);

    void runCracking();

    void requestStopCracking();
};

} // namespace gpu
} // namespace lukscrack

#endif // LUKSCRACK_GPU_DEVICECRACKINGCONTEXT_H
