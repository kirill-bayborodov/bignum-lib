/**
 * @file test_bignum_runner.c
 * @brief Integration smoke tests for the unified libbignum distribution.
 * @details The runner is compiled against the generated single-header and
 * linked with the aggregate static archive. It deliberately covers both the
 * retained legacy arithmetic family and representative newer modules.
 */
#include "bignum.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void test_core_and_legacy(void) {
    bignum_t a, b, result, quotient, remainder;
    uint64_t rem = 0;
    bignum_init_u64(&a, 10);
    bignum_init_u64(&b, 2);
    bignum_init(&result);
    bignum_init(&quotient);
    bignum_init(&remainder);
    assert(bignum_add_u64(&result, &a, 5) == 0);
    assert(bignum_mul_u64(&result, &a, 5) == 0);
    assert(bignum_div_u64(&quotient, &a, 2, &rem) == 0);
    assert(rem == 0 && quotient.words[0] == 5);
    assert(bignum_mul_bignum(&result, &a, &b) == 0);
    assert(bignum_div_bignum(&a, &b, &quotient, &remainder) == 0);
    assert(bignum_cmp(&a, &b) >= 0);
    assert(bignum_shift_left(&a, 1) == 0);
    assert(bignum_shift_right(&a, 1) == 0);
}

static void test_core_based_modules(void) {
    bignum_t a, b, result, copy;
    int bit = 0;
    bignum_init_u64(&a, 42);
    bignum_init_u64(&b, 15);
    bignum_init(&result);
    bignum_init(&copy);
    assert(bignum_abs(&a) == 0);
    assert(bignum_copy(&copy, &a) == 0);
    assert(bignum_bit_set(&copy, 63, 1) == 0);
    assert(bignum_bit_test(&copy, 63, &bit) == 0 && bit == 1);
    assert(bignum_bit_clear(&copy, 63) == 0);
    assert(bignum_and(&result, &a, &b) == 0);
    assert(bignum_or(&result, &a, &b) == 0);
    assert(bignum_xor(&result, &a, &b) == 0);
    assert(bignum_not(&result, &a) == 0);
    assert(bignum_mul_pow5(&result, 3) == 0);
    assert(bignum_mul_pow10(&result, 2) == 0);
    assert(bignum_from_hex(&result, "2a") == 0);
    assert(result.len == 1 && result.words[0] == 42);
}

int main(void) {
    puts("--- Running bignum-lib integration tests ---");
    test_core_and_legacy();
    test_core_based_modules();
    puts("--- All integration tests passed! ---");
    return 0;
}
