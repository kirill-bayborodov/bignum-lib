#include <stdio.h>
#include <assert.h>
#include "bignum.h" // Используем главный публичный API

void test_shift_left_availability() {
    printf("Running test: Shift Left Availability...\n");
    bignum_t num = { .len = 1, .words = {1} };
    
    // Просто вызываем функцию, чтобы убедиться, что она слинковалась и доступна
    bignum_status_t status = bignum_shift_left(&num, 1);
    
    // Проверяем базовую корректность
    assert(status == BIGNUM_SUCCESS);
    assert(num.words[0] == 2);
    
    printf("Test PASSED.\n");
}

int main() {
    printf("--- Running bignum-lib Integration Tests ---\n");
    test_shift_left_availability();
    // В будущем здесь будут вызовы тестов для других функций
    printf("--- All integration tests passed! ---\n");
    return 0;
}