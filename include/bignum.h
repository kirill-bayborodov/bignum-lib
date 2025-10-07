#ifndef BIGNUM_LIB_H
#define BIGNUM_LIB_H

/**
 * @file    bignum.h
 * @author  git@bayborodov.com
 * @version 0.1.0
 * @brief   Unified public API for the bignum-lib.
 */

// 1. Include core data structures from bignum-common
#include "common/bignum.h"

// 2. Include function prototypes from bignum-shift-left
#include "bignum-shift-left/bignum_shift_left.h"

// 3. (Future) Include prototypes from other modules here...
// #include "bignum-add/bignum_add.h"

#endif // BIGNUM_LIB_H