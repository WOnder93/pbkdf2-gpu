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

#ifndef ALIGNMENT_H
#define ALIGNMENT_H

/* Minimal number of blocks of size 'block' to store 'length' units: */
#define BLOCK_COUNT(block, length) ((length) / (block) + ((length) % (block) != 0 ? 1 : 0))

/* Minimal size of buffer with size aligned to 'block' to store 'length' units: */
#define ALIGN(block, length) (BLOCK_COUNT(block, length) * (block))

#endif // ALIGNMENT_H
