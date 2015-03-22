#ifndef ALIGNMENT_H
#define ALIGNMENT_H

/* Minimal number of blocks of size 'block' to store 'length' units: */
#define BLOCK_COUNT(block, length) ((length) / (block) + ((length) % (block) != 0 ? 1 : 0))

/* Minimal size of buffer with size aligned to 'block' to store 'length' units: */
#define ALIGN(block, length) (BLOCK_COUNT(block, length) * (block))

#endif // ALIGNMENT_H
