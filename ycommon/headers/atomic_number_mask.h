#ifndef YCOMMON_HEADERS_ATOMIC_NUMBER_MASK_H
#define YCOMMON_HEADERS_ATOMIC_NUMBER_MASK_H

#include <stdint.h>

/****
* The following macros are for solving compare and swap ABA problems.
* Using a 64 bit integer, 8 bits are reserved for a mask which is meant to
* increment everytime the value is changed.
****/

// Left 8 bits are for the counter mask
#define NUM_MASK_BITS 8
#define MASK_SHIFT (64-NUM_MASK_BITS)

// These are masks for each section
#define CNT_MASK (static_cast<uint64_t>(0xFF) << MASK_SHIFT)
#define NUM_MASK (~CNT_MASK)

#define GET_CNT(VALUE) ((VALUE & CNT_MASK) >> MASK_SHIFT)
#define GET_NUM(VALUE) (VALUE & NUM_MASK)

#define CONSTRUCT_VALUE(COUNT, NUMBER) \
    (((COUNT) << MASK_SHIFT) | (NUM_MASK & NUMBER))

#define CONSTRUCT_NEXT_VALUE(COUNT, NUMBER) CONSTRUCT_VALUE(COUNT+1, NUMBER)

#endif // YCOMMON_HEADERS_ATOMIC_NUMBER_MASK_H
