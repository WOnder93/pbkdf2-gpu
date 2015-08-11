/*
 * PBKDF2-HMAC generic OpenCL kernel
 *
 * Copyright (c) 2012, Sayantan Datta <std2048 at gmail dot com>
 * Copyright (c) 2015, Ondrej Mosnacek <omosnacek@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Based on software published under the following license:
 *
 *     This software is Copyright (c) 2012 Sayantan Datta <std2048 at gmail dot com>
 *     and it is hereby released to the general public under the following terms:
 *     Redistribution and use in source and binary forms, with or without modification, are permitted.
 *     Based on S3nf implementation http://openwall.info/wiki/john/MSCash2
 *     Modified to support salts upto 19 characters. Bug in orginal code allowed only upto 8 characters.
 */

#ifndef HASH_NAME
#error "Please include a hash function definition before including this file."
#endif

/*
 * A hash function definition (HF) file must define all of the following:
 *
 * TYPES:
 *   hash_word_t - the numeric 'word' type that the HF operates on
 *
 * MACROS:
 *   HASH_NAME - a string constant containing a human-readable name of the hash function
 *
 *   HASH_WORD_BYTES - the number of bytes in a word
 *   HASH_WORD_HOST2DEV(word) - a function/macro that transforms a word obtained
 *       from the host memory to a proper value (endianness should be swapped here
 *       if necessary)
 *   HASH_WORD_DEV2HOST(word) - an inverse function to the above
 *
 *   HASH_IBLOCK_WORDS - the number of words per input block
 *   HASH_OBLOCK_WORDS - the number of words per output block/HF state
 *   HASH_IBLOCK_UNROLL(block) - a macro that unrolls an input block in the form
 *       of an array into a comma-separated argument list (to be passed to a function)
 *   HASH_OBLOCK_UNROLL(block) - like above, but for an output block
 *   HASH_INITSTATE_UNROLL - like HASH_IBLOCK_UNROLL, but unrolls the initial state values
 *
 *   HASH_ML_WORDS - number of words used to represent message length
 *
 * FUNCTIONS:
 *   hash_update_block(<unrolled previous state>, block, state)
 *       updates the state with the given input block
 *   hash_digest_digest(<unrolled previous state>, buffer, state)
 *       updates the state with a digest (stored in buffer) properly padded by
 *       the message length of (HASH_IBLOCK_WORDS + HASH_OBLOCK_WORDS) words
 */

#define BYTES_TO_WORDS(n) ((n) / HASH_WORD_BYTES)
#define WORDS_TO_BYTES(n) ((n) * HASH_WORD_BYTES)

#define HASH_IBLOCK_LENGTH  WORDS_TO_BYTES(HASH_IBLOCK_WORDS)
#define HASH_OBLOCK_LENGTH  WORDS_TO_BYTES(HASH_OBLOCK_WORDS)
#define HASH_ML_LENGTH      WORDS_TO_BYTES(HASH_ML_WORDS)

/* TODO: make optional: */
#define DEBUG_LOG

/* Logging functions: */
#ifdef DEBUG_LOG
#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

typedef struct output_stream {
    __global char *buffer;
    uint pos;
} output_stream_t;

inline void stream_init(__private output_stream_t *stream, __global char *buffer)
{
    stream->buffer = buffer;
    stream->pos = 0;
}

inline void stream_close(__private output_stream_t *stream)
{
    stream->buffer[stream->pos++] = '\0';
}

inline void dump_l(__private output_stream_t *stream, __constant char *str)
{
    while (*str != '\0') {
        stream->buffer[stream->pos++] = *str;
        ++str;
    }
}

inline void dump_s(__private output_stream_t *stream, __private char *str)
{
    while (*str != '\0') {
        stream->buffer[stream->pos++] = *str;
        ++str;
    }
}

inline void dump_ui(__private output_stream_t *stream, uint i)
{
    uint rev = 0;
    while (i != 0) {
        uint d = i / 10;
        rev *= 10;
        rev += i % 10;
        i = d;
    }
    if (rev == 0) {
        stream->buffer[stream->pos++] = '0';
    } else {
        do {
            uint d = rev / 10;
            uint r = rev % 10;
            stream->buffer[stream->pos++] = '0' + r;
            rev = d;
        } while (rev != 0);
    }
}

inline void dump_uix(__private output_stream_t *stream, uint n)
{
    for (uint i = 0; i < 8; i++) {
        uint r = (n >> (32 - ((i + 1) << 2))) & 0xf;
        stream->buffer[stream->pos++] = r < 0xa ? '0' + r : 'a' + (r - 0xa);
    }
}

inline void dump_uix_a(__private output_stream_t *stream, __private uint *array, uint length) {
    dump_l(stream, "[");
    for (uint i = 0; i < length; i++) {
        if (i > 0) { dump_l(stream, " "); }
        dump_uix(stream, array[i]);
    }
    dump_l(stream, "]");
}

inline void dump_hash(__private output_stream_t *out, __private hash_word_t *hash)
{
    dump_l(out, HASH_NAME);
    dump_uix_a(out, hash, HASH_OBLOCK_WORDS);
}

#endif /* DEBUG_LOG */

/* PBKDF2 functions: */
#ifndef SALT_LENGTH
#error "SALT_LENGTH not defined!"
#endif /* SALT_LENGTH */

inline void pbkdf2_init(
    __private output_stream_t *dbg,
    __constant hash_word_t *salt, uint dk_block_index,
    const __private hash_word_t *istate,
    __private uint *state)
{
    const __private hash_word_t *prev_state = istate;

    /* Process contigous blocks of salt: */
    {
        hash_word_t buffer[HASH_IBLOCK_WORDS];
        for (uint i = 0; i < SALT_LENGTH / HASH_IBLOCK_LENGTH; i++) {
            for (uint k = 0; k < HASH_IBLOCK_WORDS; k++) {
                buffer[k] = HASH_WORD_HOST2DEV(salt[k]);
            }
            hash_update_block(HASH_OBLOCK_UNROLL(prev_state), buffer, state);
            salt += HASH_IBLOCK_WORDS;
            prev_state = state;
        }
    }

    /* Prepare and process the last (possibly partial) block of salt with the padding: */
    /* (may be one or two blocks) */
#define TAIL_BLOCKS ((SALT_LENGTH % HASH_IBLOCK_LENGTH) / (HASH_IBLOCK_LENGTH - 4 - HASH_ML_LENGTH) + 1)
    {
        hash_word_t tail[TAIL_BLOCKS * HASH_IBLOCK_WORDS];
        {
            uint i = 0;
            /* Process the rest of the salt: */
            /* (except the last (SALT_LENGTH % 4) bytes) */
            for (; i < BYTES_TO_WORDS(SALT_LENGTH % HASH_IBLOCK_LENGTH); i++) {
                tail[i] = HASH_WORD_HOST2DEV(salt[i]);
            }

            /* Append DK block index: */
            if ((SALT_LENGTH % 4) == 0) {
                tail[i] = dk_block_index + 1;
                i++;
                tail[i] = 0x80000000;
                i++;
            } else {
                tail[i] = HASH_WORD_HOST2DEV(salt[i]) | ((dk_block_index + 1) >> ((SALT_LENGTH % 4) << 3));
                i++;
                tail[i] = ((dk_block_index + 1) << (32 - ((SALT_LENGTH % 4) << 3))) | ((uint)0x80000000 >> ((SALT_LENGTH % 4) << 3));
                i++;
            }

            /* Pad with zeroes: */
            for (; i < TAIL_BLOCKS * HASH_IBLOCK_WORDS - 1; i++) {
                tail[i] = 0x00000000;
            }

            /* Put SHA1 message length at the end of the last block: */
            tail[TAIL_BLOCKS * HASH_IBLOCK_WORDS - 1] = (HASH_IBLOCK_LENGTH + SALT_LENGTH + 4) * 8 /*(bits in byte)*/;
        }

#ifdef ENABLE_LOGGING
    dump_l(dbg, "Tail: ");
    dump_uix_a(dbg, tail, TAIL_BLOCKS * HASH_IBLOCK_WORDS);
    dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

        /* Process the prepared blocks: */
        __private hash_word_t *input = tail;
        for (uint i = 0; i < TAIL_BLOCKS; i++) {
            hash_update_block(HASH_OBLOCK_UNROLL(prev_state), input, state);
            input += HASH_IBLOCK_WORDS;
            prev_state = state;
        }
        /* From now on 'prev_state' definitely refers to 'state' */
        /* which contains the half-done HMAC of iteration 0.     */
    }
#undef TAIL_BLOCKS

#ifdef ENABLE_LOGGING
    dump_l(dbg, "Iteration 0 semi-digest: ");
    dump_hash(dbg, state);
    dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */
}

inline void pbkdf2_iter(
    __private output_stream_t *dbg,
    const __private hash_word_t *istate, const __private hash_word_t *ostate,
    uint iterations,
    __private hash_word_t *dk, __private hash_word_t *state)
{
    hash_word_t buffer[HASH_IBLOCK_WORDS];

    /* Complete the HMAC of the first iteration: */
    hash_digest_digest(HASH_OBLOCK_UNROLL(ostate), state, buffer);

#ifdef ENABLE_LOGGING
    dump_l(dbg, "Iteration 0 digest: ");
    dump_hash(dbg, buffer);
    dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

    /* Copy the result of iteration 0 to the final result buffer: */
    for (uint i = 0; i < HASH_OBLOCK_WORDS; i++) {
        dk[i] = buffer[i];
    }

    /* Perform the remaining iterations: */
    for (uint i = 1; i < iterations; i++) {
        hash_digest_digest(HASH_OBLOCK_UNROLL(istate), buffer, state);

#ifdef ENABLE_LOGGING
        dump_l(dbg, "Iteration ");
        dump_ui(dbg, i);
        dump_l(dbg, " semi-digest: ");
        dump_hash(dbg, state);
        dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

        hash_digest_digest(HASH_OBLOCK_UNROLL(ostate), state, buffer);

#ifdef ENABLE_LOGGING
        dump_l(dbg, "Iteration ");
        dump_ui(dbg, i);
        dump_l(dbg, " digest: ");
        dump_hash(dbg, buffer);
        dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */

        /* XOR the result of this iteration into the final result: */
        for (uint i = 0; i < HASH_OBLOCK_WORDS; i++) {
            dk[i] ^= buffer[i];
        }

#ifdef ENABLE_LOGGING
        dump_l(dbg, "Iteration ");
        dump_ui(dbg, i);
        dump_l(dbg, " digest XOR'd: ");
        dump_hash(dbg, dk);
        dump_l(dbg, "\n");
#endif /* ENABLE_LOGGING */
    }
}

/* Main kernel: */
__kernel
void pbkdf2_kernel(
    const __global hash_word_t *input,
    __global hash_word_t *output,
    __constant hash_word_t *salt,
    uint dk_blocks,
    uint iterations,
    uint batchSize,
    __global char *debug_buffer)
{
#ifdef DEBUG_LOG
    output_stream_t dbg;
    stream_init(&dbg, debug_buffer);
#endif /* DEBUG_LOG */

    uint input_block_index = (uint)get_global_id(0);
    uint input_pos = input_block_index;

    uint ipad_state[HASH_OBLOCK_WORDS];
    uint opad_state[HASH_OBLOCK_WORDS];
    {
        uint ipad[HASH_IBLOCK_WORDS];
        uint opad[HASH_IBLOCK_WORDS];

        for (uint row = 0; row < HASH_IBLOCK_WORDS; row++) {
            uint in = HASH_WORD_HOST2DEV(input[input_pos + row * batchSize]);

#ifdef ENABLE_LOGGING
            dump_uix(&dbg, in);
            dump_l(&dbg, " ");
#endif /* ENABLE_LOGGING */

            ipad[row] = in ^ 0x36363636;
            opad[row] = in ^ 0x5C5C5C5C;
        }
#ifdef ENABLE_LOGGING
        dump_l(&dbg, "\n");
#endif /* ENABLE_LOGGING */

        hash_update_block(HASH_INITSTATE_UNROLL, ipad, ipad_state);
        hash_update_block(HASH_INITSTATE_UNROLL, opad, opad_state);
    }

    uint dk_block_index = (uint)get_global_id(1);

    uint buffer[HASH_IBLOCK_WORDS];
    pbkdf2_init(&dbg, salt, dk_block_index, ipad_state, buffer);

    uint dk[HASH_OBLOCK_WORDS];
    pbkdf2_iter(&dbg, ipad_state, opad_state, iterations, dk, buffer);

    uint output_block_index = input_block_index * dk_blocks + dk_block_index;
    uint output_pos = output_block_index;

    for (uint i = 0; i < HASH_OBLOCK_WORDS; i++) {
        output[output_pos + i * batchSize * dk_blocks] = HASH_WORD_DEV2HOST(dk[i]);
    }

#ifdef DEBUG_LOG
    stream_close(&dbg);
#endif /* DEBUG_LOG */
}
