/*
 * Copyright (C) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation: either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
