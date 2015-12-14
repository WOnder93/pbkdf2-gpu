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

#ifndef LUKSCRACK_GPU_DEVICECRACKINGCONTEXT_H
#define LUKSCRACK_GPU_DEVICECRACKINGCONTEXT_H

#include "passworddistributor.h"
#include "threadpool.h"
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
    PasswordDistributor *pwDistributor;
    std::function<PasswordFoundCallback> callback;
    SubLogger logger;

    BatchProcessingContext pc1, pc2, pc3;

    std::atomic_bool stop;

public:
    DeviceCrackingContext() { }

    DeviceCrackingContext(
            const CrackingContext *crackingContext,
            PasswordDistributor *pwDistributor,
            ThreadPool *threadPool,
            std::function<PasswordFoundCallback> callback,
            const Device &device, std::size_t batchSize,
            Logger *logger);

    void runCracking();

    void requestStopCracking();
};

} // namespace gpu
} // namespace lukscrack

#endif // LUKSCRACK_GPU_DEVICECRACKINGCONTEXT_H
