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
        const Device &device, std::size_t batchSize,
        Logger *logger)
    : pwDistributor(pwDistributor),
      callback(callback), logger(logger, device.getName() + ": "),
      pc1(crackingContext, "pc1", device, threadPool, batchSize, &this->logger),
      pc2(crackingContext, "pc2", device, threadPool, batchSize, &this->logger),
      pc3(crackingContext, "pc3", device, threadPool, batchSize, &this->logger),
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

            logger << "pc1: Processing results..." << std::endl;
            std::size_t matchIndex;
            if (pc1.processResults(matchIndex)) {
                callback(pc1.getCurrentPasswords()[matchIndex]);
                stop = true;
            }
            logger << "pc1: Done processing results." << std::endl;
        }
        if (!lastLoop) {
            std::lock_guard<std::mutex> guard(pwMutex);
            logger << "pc1: Initializing passwords..." << std::endl;
            if (!pc1.initializePasswords(pwGen)) {
                stop = true;
            }
            logger << "pc1: Done initializing passwords." << std::endl;
            pc1.beginKeyslotUnit();
        }

        if (!firstLoop) {
            pc3.endKeyslotUnit();
            logger << "pc3: Decrypting master key..." << std::endl;
            pc3.decryptMasterKey();
            logger << "pc3: Done decrypting master key." << std::endl;
            pc3.beginMKDigestUnit();

            pc2.endMKDigestUnit();
            logger << "pc2: Processing results..." << std::endl;
            std::size_t matchIndex;
            if (pc2.processResults(matchIndex)) {
                callback(pc2.getCurrentPasswords()[matchIndex]);
                stop = true;
            }
            logger << "pc2: Done processing results." << std::endl;
        }
        if (!lastLoop) {
            {
                std::lock_guard<std::mutex> guard(pwMutex);
                logger << "pc2: Initializing passwords..." << std::endl;
                if (!pc2.initializePasswords(pwGen)) {
                    stop = true;
                }
                logger << "pc2: Done initializing passwords." << std::endl;
            }
            pc2.beginKeyslotUnit();

            pc1.endKeyslotUnit();
            logger << "pc1: Decrypting master key..." << std::endl;
            pc1.decryptMasterKey();
            logger << "pc1: Done decrypting master key." << std::endl;
            pc1.beginMKDigestUnit();
        }

        if (!firstLoop) {
            pc3.endMKDigestUnit();
            logger << "pc3: Processing results..." << std::endl;
            std::size_t matchIndex;
            if (pc3.processResults(matchIndex)) {
                callback(pc3.getCurrentPasswords()[matchIndex]);
                stop = true;
            }
            logger << "pc3: Done processing results." << std::endl;
        }
        if (!lastLoop) {
            {
                std::lock_guard<std::mutex> guard(pwMutex);
                logger << "pc3: Initializing passwords..." << std::endl;
                if (!pc3.initializePasswords(pwGen)) {
                    stop = true;
                }
                logger << "pc3: Done initializing passwords." << std::endl;
            }
            pc3.beginKeyslotUnit();

            pc2.endKeyslotUnit();
            logger << "pc2: Decrypting master key..." << std::endl;
            pc2.decryptMasterKey();
            logger << "pc2: Done decrypting master key." << std::endl;
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
