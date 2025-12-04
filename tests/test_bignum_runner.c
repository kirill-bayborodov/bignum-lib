/**
 * @file    test_bignum_runner.c
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    04.12.2025
 *
 * @brief Интеграционный тест‑раннер для библиотеки libbignum.a.
 * @details Применяется для проверки достаточности сигнатур 
 *          в файле заголовка (header) при сборке и линковке
 *          статической библиотеки libbignum.a
 *
 * @history
 *   - rev. 1 (04.12.2025): Создание теста
 */  
#include "bignum.h"
#include <assert.h>
#include <stdio.h> 
 
void test_bignum_cmp_runner() {
 printf("Running test: test_bignum_cmp_runner... "); 
 bignum_t a = {0};  
 bignum_t b = {0};  	
 bignum_cmp(&a, &b);  
 assert(1);
 printf("PASSED\n");   
}

int test_bignum_div_u64_runner() {
 printf("Running test: test_bignum_div_u64_runner... "); 
 bignum_t q_dst = {0};
 bignum_t n_dst;
 n_dst.words[0] = 10;
 n_dst.len=1;
 uint64_t d_u64_dst = {2};
 uint64_t rem_u64_dst = {0};
 bignum_div_u64(&q_dst, &n_dst,  d_u64_dst, &rem_u64_dst); 
 assert(1);
 printf("PASSED\n");   
}
  
int test_bignum_mul_bignum_runner() {
 printf("Running test: test_bignum_mul_bignum_runner... "); 
 bignum_t res = {0}; 
 bignum_t a = {0}; 
 bignum_t b = {0}; 			
 bignum_mul_bignum(&res, &a, &b);
 assert(1);
 printf("PASSED\n");   
}
 
int test_bignum_mul_u64_runner() {
 printf("Running test: test_bignum_mul_u64_runner... "); 
 bignum_t res = {0};
 bignum_t a;	
 a.words[0] = 10;
 a.len=1;
 bignum_mul_u64(&res,&a, 5);
 assert(1);
 printf("PASSED\n");   
}
  
int test_bignum_shift_left_runner() {
 printf("Running test: test_bignum_shift_left_runner... "); 
 bignum_t num = {0}; 	
 bignum_shift_left(&num, 5);  
 assert(1);
 printf("PASSED\n");   
}
  
int test_bignum_shift_right_runner() {
 printf("Running test: test_bignum_shift_right_runner... "); 
 bignum_t num = {0}; 	
 bignum_shift_right(&num, 5);  
 assert(1);
 printf("PASSED\n");   
}
  
int test_bignum_sub_runner() {
 printf("Running test: test_bignum_sub_runner... "); 
 bignum_t res = {.words = {0}, .len = 0};  
 bignum_t a = {.words = {12345}, .len = 1};
 bignum_t b = {.words = {10000}, .len = 1};  	
 bignum_sub(&res, &a, &b);  
 assert(1);
 printf("PASSED\n");   
}

int main() {
    printf("\n--- Running bignum-lib Integration Tests ---\n");
    test_bignum_cmp_runner();
    test_bignum_div_u64_runner();
    test_bignum_mul_bignum_runner();
    test_bignum_mul_u64_runner();
    test_bignum_shift_left_runner();
    test_bignum_shift_right_runner();
    test_bignum_sub_runner();
    printf("--- All integration tests passed! ---\n");
    return 0;
}