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

#include "processingunit.h"

namespace libpbkdf2 {
namespace compute {
namespace cpu {

ProcessingUnit::ProcessingUnit(
        const DeviceContext *context,
        std::size_t batchSize, Logger *logger)
    : context(context), batchSize(batchSize), logger(logger),
      taskFuture()
{
    auto computeContext = context->getParentContext();

    passwordBuffer.resize(batchSize);

    std::size_t dkLength = computeContext->getDerivedKeyLength();
    dkBuffer = std::unique_ptr<unsigned char[]>(
                new unsigned char[dkLength * batchSize]);
}

ProcessingUnit::PasswordWriter::PasswordWriter(
        ProcessingUnit &parent, std::size_t index)
{
    it = parent.passwordBuffer.begin() + index;
}

void ProcessingUnit::PasswordWriter::moveForward(std::size_t offset)
{
    it += offset;
}

void ProcessingUnit::PasswordWriter::moveBackwards(std::size_t offset)
{
    it -= offset;
}

void ProcessingUnit::PasswordWriter::setPassword(
        const void *pw, std::size_t pwSize) const
{
    it->assign((char *)pw, (char *)pw + pwSize);
}

ProcessingUnit::DerivedKeyReader::DerivedKeyReader(
        ProcessingUnit &parent, std::size_t index)
{
    dkLength = parent.context->getParentContext()->getDerivedKeyLength();
    src = parent.dkBuffer.get() + index * dkLength;
}

void ProcessingUnit::DerivedKeyReader::moveForward(std::size_t offset)
{
    src += offset * dkLength;
}

void ProcessingUnit::DerivedKeyReader::moveBackwards(std::size_t offset)
{
    src -= offset * dkLength;
}

const void *ProcessingUnit::DerivedKeyReader::getDerivedKey() const
{
    return src;
}

void ProcessingUnit::beginProcessing()
{
    auto &coreCtx = context->getCoreContext();
    taskFuture = coreCtx.enqueueTask([this] () {
        auto computeContext = context->getParentContext();
        auto hfContext = computeContext->getParentContext();

        const unsigned char *salt = (const unsigned char *)computeContext->getSaltData();
        std::size_t saltLength = computeContext->getSaltLength();
        std::size_t dkLength = computeContext->getDerivedKeyLength();
        std::size_t iterations = computeContext->getIterationCount();

        const EVP_MD *digest = hfContext->getDigest();

        unsigned char *dest = dkBuffer.get();
        for (std::size_t i = 0; i < batchSize; i++) {
            auto &pw = passwordBuffer[i];
            PKCS5_PBKDF2_HMAC(pw.data(), (int)pw.size(),
                              salt, saltLength, iterations,
                              digest, dkLength, dest);
            dest += dkLength;
        }
    });
}

void ProcessingUnit::endProcessing()
{
    taskFuture.wait();
    taskFuture = std::future<void>();
}

} // namespace cpu
} // namespace compute
} // namespace libpbkdf2
