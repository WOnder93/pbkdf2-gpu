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

typedef uint hash_word_t;

#define HASH_WORD_SWITCH_ENDIANNESS(v) (\
    (((hash_word_t)(v) & 0xff) << 24) | \
    (((hash_word_t)(v) <<  8) & 0x00ff0000) | \
    (((hash_word_t)(v) >>  8) & 0x0000ff00) | \
    (((hash_word_t)(v) >> 24) & 0x000000ff))

#define HASH_WORD_BYTES 4

#define HASH_WORD_REPEAT_BYTE(b) ((b) | ((b) << 8) | ((b) << 16) | ((b) << 24))
