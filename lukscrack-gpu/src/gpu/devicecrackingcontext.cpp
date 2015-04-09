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

#include "devicecrackingcontext.h"

namespace lukscrack {
namespace gpu {

DeviceCrackingContext::DeviceCrackingContext(
        const CrackingContext *crackingContext,
        PasswordDistributor *pwDistributor,
        ThreadPool *threadPool,
        std::function<PasswordFoundCallback> callback,
        const Device &device, size_t batchSize)
    : pwDistributor(pwDistributor),
      callback(callback),
      pc1(crackingContext, device, threadPool, batchSize),
      pc2(crackingContext, device, threadPool, batchSize),
      pc3(crackingContext, device, threadPool, batchSize),
      stop(false)
{
}

void DeviceCrackingContext::runCracking()
{
    std::mutex &pwMutex = pwDistributor->getMutex();
    PasswordGenerator &pwGen = *pwDistributor->getPasswordGenerator();

    /*
     * Processing loops in the following magic pattern:
     *
     * pc1.endMKD()
     * pc1.finish()
     * pc1.init()
     * pc1.beginKS()
     *
     * pc3.endKS()
     * pc3.decrypt()
     * pc3.beginMKD()
     *
     * pc2.endMKD()
     * pc2.finish()
     * pc2.init()
     * pc2.beginKS()
     *
     * pc1.endKS()
     * pc1.decrypt()
     * pc1.beginMKD()
     *
     * pc3.endMKD()
     * pc3.finish()
     * pc3.init()
     * pc3.beginKS()
     *
     * pc2.endKS()
     * pc2.decrypt()
     * pc2.beginMKD()
     *
     * This ensures that both CPU and GPU are optimally utilized.
     */
    bool firstLoop = true;
    bool lastLoop = false;
    while (true) {
        if (!firstLoop) {
            pc1.endMKDigestUnit();
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
            pc1.beginKeyslotUnit();
        }

        if (!firstLoop) {
            pc3.endKeyslotUnit();
            pc3.decryptMasterKey();
            pc3.beginMKDigestUnit();

            pc2.endMKDigestUnit();
            ssize_t res = pc2.processResults();
            if (res >= 0) {
                callback(pc2.getCurrentPasswords()[res]);
                stop = true;
            }
        }
        if (!lastLoop) {
            {
                std::lock_guard<std::mutex> guard(pwMutex);
                if (!pc2.initializePasswords(pwGen)) {
                    stop = true;
                }
            }
            pc2.beginKeyslotUnit();

            pc1.endKeyslotUnit();
            pc1.decryptMasterKey();
            pc1.beginMKDigestUnit();
        }

        if (!firstLoop) {
            pc3.endMKDigestUnit();
            ssize_t res = pc3.processResults();
            if (res >= 0) {
                callback(pc3.getCurrentPasswords()[res]);
                stop = true;
            }
        }
        if (!lastLoop) {
            {
                std::lock_guard<std::mutex> guard(pwMutex);
                if (!pc3.initializePasswords(pwGen)) {
                    stop = true;
                }
            }
            pc3.beginKeyslotUnit();

            pc2.endKeyslotUnit();
            pc2.decryptMasterKey();
            pc2.beginMKDigestUnit();
        }

        if (lastLoop) {
            break;
        }

        firstLoop = false;

        if (stop) {
            lastLoop = true;
        }
    }
}

void DeviceCrackingContext::requestStopCracking()
{
    stop = true;
}

} // namespace gpu
} // namespace lukscrack
