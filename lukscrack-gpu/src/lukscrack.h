#ifndef LUKSCRACK_LUKSCRACK_H
#define LUKSCRACK_LUKSCRACK_H

#include "passworddistributor.h"
#include "threadpool.h"
#include "gpu/devicecrackingcontext.h"

#include <thread>
#include <mutex>

namespace lukscrack {

using namespace libpbkdf2::compute::opencl;

class LuksCrack
{
private:
    PasswordDistributor pwDistributor;
    ThreadPool threadPool; /* for parallel key material decryption */

    gpu::CrackingContext context;
    std::vector<std::unique_ptr<gpu::DeviceCrackingContext>> devContexts;

    std::mutex passwordMutex;
    std::string password;
    bool passwordFound;

    void setFoundPassword(const std::string &found);

public:
    inline bool wasPasswordFound() const { return passwordFound; }
    inline const std::string &getPassword() const { return password; }

    LuksCrack(const GlobalContext *globalContext,
              const std::vector<Device> &devices,
              const PasswordData *passwordData,
              PasswordGenerator *pwGen,
              size_t threadPoolSize, size_t batchSize);

    void runCracking();
    void requestStopCracking();
};

} // namespace lukscrack

#endif // LUKSCRACK_LUKSCRACK_H
