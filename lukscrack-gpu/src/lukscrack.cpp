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

#include "lukscrack.h"

namespace lukscrack {

LuksCrack::LuksCrack(
        const GlobalContext *globalContext, const std::vector<Device> &devices,
        const PasswordData *passwordData, PasswordGenerator *pwGen,
        std::size_t threadPoolSize, std::size_t batchSize, Logger *logger)
    : pwDistributor(pwGen), threadPool(threadPoolSize), logger(logger, "LuksCrack: "),
      context(globalContext, devices, passwordData), devContexts(),
      passwordMutex(), password(), passwordFound(false)
{
    devContexts.reserve(devices.size());
    for (auto &dev : devices) {
        devContexts.emplace_back(new gpu::DeviceCrackingContext(
                    &context, &pwDistributor, &threadPool,
                    [this] (const std::string &found) { setFoundPassword(found); },
                    dev, batchSize, &this->logger));
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
    for (std::size_t i = 0; i < devContexts.size(); i++) {
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
