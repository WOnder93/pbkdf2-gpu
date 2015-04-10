/*
 * Based on AFsplitter (lib/luks1/af.c) from Cryptsetup
 * <https://gitlab.com/cryptsetup/cryptsetup>
 *
 * Copyright (C) 2004, Clemens Fruhwirth <clemens@endorphin.org>
 * Copyright (C) 2009-2012, Red Hat, Inc. All rights reserved.
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

#ifndef LUKSCRACK_AFMERGER_H
#define LUKSCRACK_AFMERGER_H

#include "libhashspec-hashalgorithm/hashalgorithm.h"

#include <string>
#include <memory>

namespace lukscrack {

using namespace libhashspec::hashalgorithm;

class AFMerger
{
private:
    const HashAlgorithm *hashAlg;
    std::size_t blockSize;
    std::size_t blockCount;

    std::size_t digestSize;
    std::size_t subblockCount;
    std::size_t subblockPadding;

    void merge(const void *src, void *dst, unsigned char *buffer) const;

public:
    class Context
    {
    private:
        const AFMerger *parent;
        std::unique_ptr<unsigned char[]> buffer;

    public:
        Context(const AFMerger *parent);

        inline void merge(const void *src, void *dst) const
        {
            parent->merge(src, dst, buffer.get());
        }
    };

    inline AFMerger() { }

    AFMerger(std::size_t blockSize, std::size_t blockCount,
             const std::string &hashSpec);
};

} // namespace lukscrack

#endif // LUKSCRACK_AFMERGER_H
