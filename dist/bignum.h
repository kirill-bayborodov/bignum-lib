#ifndef BIGNUM_SINGLE_H
#define BIGNUM_SINGLE_H

/**
 * @file    bignum.h
 * @author  git@bayborodov.com
 * @version 1.0.8
 * @date    17.06.2026
 *
 * @brief   Определение базовой структуры для арифметики с большими числами.
 *
 * @details
 *   Структура `bignum_t` является основой для всех высокоточных
 *   целочисленных вычислений в алгоритме форматирования `double`.
 *   Размер массива `words` (32 * 64 = 2048 бита) выбран для покрытия
 *   всех крайних случаев, возникающих при преобразовании `double`.
 *
 *   Коды ошибок, относящиеся только к парсингу строк
 *   (BIGNUM_ERROR_BAD_BASE/PARSE/EMPTY), объявлены в `bignum_helpers.h`.
 *
 * @history
 *   - rev. 1 (01.08.2025): Первоначальное создание.
 *   - rev. 2 (03.10.2025): Дополнение для github.
 *   - rev. 3 (17.06.2026): Добавлены функции инициализации, нормализации.
 *   - rev. 4 (17.06.2026): Реализация вынесена в bignum_common.c.
 *   - rev. 5 (17.06.2026): Реализован парсинг hex/dec;
 *                          bignum_is_zero перенесён в .c.
 *   - rev. 6 (17.06.2026): Парсинг вынесен в `bignum_helpers`.
 *   - rev. 7 (17.06.2026): BIGNUM_ERROR_BAD_BASE/PARSE/EMPTY вынесены
 *                          в `bignum_helpers.h`; bignum.c → bignum_common.c.
 *   - rev. 8 (18.06.2026): Сигнатуры для основныx функций ядра перенесены в bignum_common.h
 */
#ifndef BIGNUM_H
#define BIGNUM_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define BIGNUM_CAPACITY 32   /* 32 * 64 = 2048 бит */
#define BIGNUM_WORD_SIZE 8   /* sizeof(uint64_t) байт  */

/**
 * @brief Коды возврата ядра bignum.
 *
 * Коды, специфичные для парсинга строк (BIGNUM_ERROR_BAD_BASE,
 * BIGNUM_ERROR_PARSE, BIGNUM_ERROR_EMPTY), объявлены в `bignum_helpers.h`.
 */
typedef enum {
    BIGNUM_SUCCESS         =  0,
    BIGNUM_ERROR_NULL_ARG  = -1,
    BIGNUM_ERROR_OVERFLOW  = -2
} bignum_status_t;

/**
 * @brief Структура для представления большого беззнакового целого числа.
 */
typedef struct {
/** Массив 64-битных "слов" для хранения числа. */
    uint64_t words[BIGNUM_CAPACITY];
/**
     * Количество используемых слов.
     * Для числа 0 значение len равно 0.
     * Для всех остальных чисел гарантируется, что words[len-1] != 0
     * (число нормализовано).
     */
    size_t   len;
} bignum_t;


#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_H */
/**
 * @file bignum_abs.h
 * @brief Public API for canonical normalization of an unsigned bignum_t.
 * @details bignum_t stores a non-negative fixed-capacity integer and therefore
 * has no sign bit to change. bignum_abs validates the record, removes zero words
 * above the most significant non-zero word, clears the unused tail, and commits
 * the canonical length. The caller owns the record and allocates its storage;
 * the function allocates no memory and uses no mutable global state. The API is
 * safe for concurrent calls only when each thread owns a distinct record.
 */
#ifndef BIGNUM_ABS_H
#define BIGNUM_ABS_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports validation or completion status for bignum_abs.
 * @details A success status means the caller-owned record was normalized. Both
 * error statuses guarantee that no store was made to the record.
 */
typedef enum bignum_abs_status {
    BIGNUM_ABS_SUCCESS = 0, /**< Normalization completed; the record is canonical and its unused words are zero. */
    BIGNUM_ABS_ERROR_NULL_ARG = -1, /**< The required caller-owned record pointer was NULL; no output exists and retry is valid after supplying storage. */
    BIGNUM_ABS_ERROR_LENGTH = -2 /**< The input length exceeded BIGNUM_CAPACITY; the record is unchanged and retry is valid after repairing the length. */
} bignum_abs_status_t;

/**
 * @brief Normalizes the absolute value of an unsigned bignum in place.
 * @details Since bignum_t represents only non-negative values, the numeric value
 * is preserved. The function first validates num and its length. On valid input
 * it scans from the declared high word, trims zero words, clears the unused
 * suffix, and writes the normalized length last. There is no allocation and the
 * operation is constant-space; the running time is O(BIGNUM_CAPACITY) worst case.
 * @param num [in,out] Caller-owned, writable bignum_t storage. It must be
 * non-NULL, valid for the complete call, and not concurrently modified. The
 * input and output are the same object; no ownership is transferred.
 * @return BIGNUM_ABS_SUCCESS when num is canonical; BIGNUM_ABS_ERROR_NULL_ARG
 * when num is NULL; or BIGNUM_ABS_ERROR_LENGTH when num->len is too large.
 * @pre num is either NULL or points to a writable bignum_t whose storage remains
 * live for the duration of the call.
 * @post On success, num->len is at most BIGNUM_CAPACITY, zero has len == 0,
 * words at indices len..BIGNUM_CAPACITY-1 are zero, and the represented value
 * is unchanged. On either error, num is byte-for-byte unchanged.
 * @warning The function is thread-safe for independent records only; callers
 * must synchronize access when threads share the same bignum_t.
 * @complexity O(BIGNUM_CAPACITY) time and O(1) auxiliary space.
 */
bignum_abs_status_t bignum_abs(bignum_t *num);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_ABS_H */
/**
 * @file    bignum_add_bignum.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    29.07.2026
 *
 * @brief   Модуль для сложения больших беззнаковых целых чисел.
 * @ingroup bignum_arithmetic
 *
 * @details
 *   Определяет API для функции bignum_add_bignum, включая типы данных,
 *   коды состояния и прототипы функций.
 *
 * @history
 *   - rev. 0 (29.07.2026): Первоначальное создание API.
 *
 * @see     bignum.h
 * @since   1.0.2
 */

#ifndef BIGNUM_ADD_BIGNUM_H
#define BIGNUM_ADD_BIGNUM_H

#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка
#ifndef BIGNUM_CAPACITY
#  error "bignum.h must define BIGNUM_CAPACITY"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Коды состояния для функции bignum_add_bignum.
 */
typedef enum {
    BIGNUM_ADD_BIGNUM_SUCCESS                 =  0, /**< Успешное выполнение. */
    BIGNUM_ADD_BIGNUM_ERROR_NULL_PTR          = -1, /**< Один из входных указателей `NULL`. */
    BIGNUM_ADD_BIGNUM_ERROR_CAPACITY_EXCEEDED = -2, /**< Длина операнда (a или b) превышает `BIGNUM_CAPACITY`. */
    BIGNUM_ADD_BIGNUM_ERROR_BUFFER_OVERLAP    = -3, /**< Обнаружено недопустимое перекрытие буферов. */
    BIGNUM_ADD_BIGNUM_ERROR_OVERFLOW          = -4  /**< Переполнение: результат не помещается в `BIGNUM_CAPACITY`. */
} bignum_add_bignum_status_t;

/**
 * @brief Выполняет сложение двух больших беззнаковых целых чисел.
 *
 * @details
 *   ### Алгоритм
 *   1.  **Валидация:** Проверяются входные указатели `result`, `a`, `b` на `NULL`.
 *   2.  **Проверка длины:** Проверяется, что `a->len` и `b->len` не превышают `BIGNUM_CAPACITY`.
 *   3.  **Проверка перекрытия:** Проверяется, что диапазоны памяти `result`, `a` и `b`
 *       не пересекаются (при этом in-place операции `result == a` или `result == b` разрешены).
 *   4.  **Сложение:** Выполняется пословное сложение `a + b` с распространением переноса (carry).
 *   5.  **Обработка хвоста:** Если один операнд длиннее другого, оставшиеся слова
 *       копируются с учетом возможного продолжения переноса.
 *   6.  **Переполнение:** Если после сложения всех слов остается перенос, и текущая длина
 *       равна `BIGNUM_CAPACITY`, возвращается ошибка `BIGNUM_ADD_BIGNUM_ERROR_OVERFLOW`.
 *       Иначе длина результата увеличивается на 1, и перенос записывается в старшее слово.
 *   7.  **Нормализация:** Длина результата устанавливается корректно (для сложения нулей).
 *
 *   ### Потокобезопасность
 *   Функция является потокобезопасной, так как не использует глобальное или
 *   статическое состояние.
 *
 * @param[out] result Указатель на структуру `bignum_t` для записи суммы.
 * @param[in]  a      Указатель на `bignum_t`, первое слагаемое.
 * @param[in]  b      Указатель на `bignum_t`, второе слагаемое.
 *
 * @return bignum_add_bignum_status_t Код состояния операции.
 * @retval BIGNUM_ADD_BIGNUM_SUCCESS                 Успешное выполнение.
 * @retval BIGNUM_ADD_BIGNUM_ERROR_NULL_PTR          Один из входных указателей `NULL`.
 * @retval BIGNUM_ADD_BIGNUM_ERROR_CAPACITY_EXCEEDED Длина операнда (a или b) превышает `BIGNUM_CAPACITY`.
 * @retval BIGNUM_ADD_BIGNUM_ERROR_BUFFER_OVERLAP    Обнаружено перекрытие буферов.
 * @retval BIGNUM_ADD_BIGNUM_ERROR_OVERFLOW          Результат превышает максимальную вместимость.
 */
bignum_add_bignum_status_t bignum_add_bignum(bignum_t *result, const bignum_t *a, const bignum_t *b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_ADD_BIGNUM_H */
/**
 * @file    bignum_add_u64.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    29.07.2026
 *
 * @brief   Модуль для сложения большого беззнакового целого числа и 64-битного скаляра.
 * @ingroup bignum_arithmetic
 *
 * @details
 *   Определяет API для функции bignum_add_u64, включая типы данных,
 *   коды состояния и прототипы функций.
 *
 * @history
 *   - rev. 0 (29.07.2026): Первоначальное создание API.
 *
 * @see     bignum.h
 * @since   1.1.2
 */

#ifndef BIGNUM_ADD_U64_H
#define BIGNUM_ADD_U64_H

#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Коды состояния для функции bignum_add_u64.
 */
typedef enum {
    BIGNUM_ADD_U64_SUCCESS                 =  0, /**< Успешное выполнение. */
    BIGNUM_ADD_U64_ERROR_NULL_PTR          = -1, /**< Один из входных указателей `NULL`. */
    BIGNUM_ADD_U64_ERROR_CAPACITY_EXCEEDED = -2, /**< Длина операнда `a` превышает `BIGNUM_CAPACITY`. */
    BIGNUM_ADD_U64_ERROR_BUFFER_OVERLAP    = -3, /**< Обнаружено недопустимое перекрытие буферов. */
    BIGNUM_ADD_U64_ERROR_OVERFLOW          = -4  /**< Переполнение: результат не помещается в `BIGNUM_CAPACITY`. */
} bignum_add_u64_status_t;

/**
 * @brief Выполняет сложение большого беззнакового целого числа и 64-битного числа.
 *
 * @details
 *   ### Алгоритм
 *   1.  **Валидация:** Проверяются входные указатели `result` и `a` на `NULL`.
 *   2.  **Проверка длины:** Проверяется, что `a->len` не превышает `BIGNUM_CAPACITY`.
 *   3.  **Проверка перекрытия:** Проверяется, что диапазоны памяти `result` и `a`
 *       не пересекаются (при этом in-place операция `result == a` разрешена).
 *   4.  **Сложение:** Выполняется сложение `b` с младшим словом `a`.
 *   5.  **Распространение переноса:** Если возник перенос (carry), он прибавляется
 *       к следующим словам `a`, пока не затухнет или пока не закончатся слова.
 *   6.  **Переполнение:** Если после обработки всех слов `a` перенос остался,
 *       длина результата увеличивается на 1. Если при этом длина превышает
 *       `BIGNUM_CAPACITY`, возвращается ошибка `BIGNUM_ADD_U64_ERROR_OVERFLOW`.
 *   7.  **Нормализация:** Длина результата устанавливается корректно (удаляются ведущие нули,
 *       если они были в исходном числе `a`, а `b` было равно 0).
 *
 *   ### Потокобезопасность
 *   Функция является потокобезопасной, так как не использует глобальное или
 *   статическое состояние.
 *
 * @param[out] result Указатель на структуру `bignum_t` для записи суммы.
 * @param[in]  a      Указатель на `bignum_t`, первое слагаемое.
 * @param[in]  b      64-битное беззнаковое целое, второе слагаемое.
 *
 * @return bignum_add_u64_status_t Код состояния операции.
 * @retval BIGNUM_ADD_U64_SUCCESS                 Успешное выполнение.
 * @retval BIGNUM_ADD_U64_ERROR_NULL_PTR          Один из входных указателей `NULL`.
 * @retval BIGNUM_ADD_U64_ERROR_CAPACITY_EXCEEDED Длина операнда `a` превышает `BIGNUM_CAPACITY`.
 * @retval BIGNUM_ADD_U64_ERROR_BUFFER_OVERLAP    Обнаружено перекрытие буферов.
 * @retval BIGNUM_ADD_U64_ERROR_OVERFLOW          Результат превышает максимальную вместимость.
 */
bignum_add_u64_status_t bignum_add_u64(bignum_t *result, const bignum_t *a, const uint64_t b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_ADD_U64_H */
/* ------------------------------------------------------------------ */
/**
 * @file    bignum_and.h
 * @brief   Побитовое И двух normalized bignum_t.
 * @version 0.1.0
 * @details
 *   Revision 0.1.0: typed API and C/ASM implementation for bitwise AND.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_AND_H
#define BIGNUM_AND_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BIGNUM_AND_SUCCESS                 = 0,
    BIGNUM_AND_ERROR_NULL_PTR          = -1,
    BIGNUM_AND_ERROR_CAPACITY_EXCEEDED = -2,
    BIGNUM_AND_ERROR_BUFFER_OVERLAP    = -3
} bignum_and_status_t;

/**
 * @brief Computes `result = a & b` for normalized 2048-bit bignum objects.
 *
 * The result is normalized and its unused words are cleared. `result == a`
 * and `result == b` are supported exact in-place aliases. Any partial overlap
 * between the three complete bignum_t objects is rejected and leaves result
 * unchanged. Inputs with len greater than BIGNUM_CAPACITY are rejected before
 * memory mutation. A zero-length operand produces a normalized zero result.
 *
 * @param[out] result Destination object.
 * @param[in]  a First operand.
 * @param[in]  b Second operand.
 * @return Typed operation status.
 */
bignum_and_status_t bignum_and(
    bignum_t *result,
    const bignum_t *a,
    const bignum_t *b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_AND_H */

/* SPDX-License-Identifier: MIT */
/**
 * @file bignum_bit_clear.h
 * @brief Public API for clearing one logical bit in a fixed-capacity bignum_t.
 * @details The module owns no storage and performs no allocation. The caller
 * owns a writable bignum_t containing little-endian 64-bit words. A valid call
 * clears the selected bit, canonicalizes len, and clears all physical words at
 * or above len. Validation precedes publication, so every error is atomic.
 * The operation is safe for independent records; same-record writers require
 * external synchronization.
 */
#ifndef BIGNUM_BIT_CLEAR_H
#define BIGNUM_BIT_CLEAR_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Reports the result of one bit-clear operation. */
typedef enum bignum_bit_clear_status {
    BIGNUM_BIT_CLEAR_SUCCESS = 0, /**< Clear completed or was a valid no-op; output is canonical. */
    BIGNUM_BIT_CLEAR_ERROR_NULL_ARG = -1, /**< `num` is NULL; no object is accessed or modified. */
    BIGNUM_BIT_CLEAR_ERROR_INDEX = -2, /**< `bit_index` is outside the fixed domain; output is unchanged. */
    BIGNUM_BIT_CLEAR_ERROR_LENGTH = -3 /**< `num->len` exceeds capacity; output is unchanged and retry is allowed. */
} bignum_bit_clear_status_t;

/**
 * @brief Clears one logical bit in a caller-owned bignum record.
 * @details The function validates the pointer, bit domain, and record length,
 * copies the record to a local candidate, clears the selected mask when its
 * word is logically present, removes leading zero words, clears the physical
 * tail, and publishes the candidate. Clearing beyond the current logical len
 * is a successful canonicalizing no-op.
 * @param[in,out] num Caller-allocated writable bignum_t; borrowed for the call
 * and mutated in place only on success. NULL produces the named null status.
 * @param[in] bit_index Zero-based bit position in [0, BIGNUM_CAPACITY * 64),
 * measured in bits; the operation never addresses outside the fixed array.
 * @return BIGNUM_BIT_CLEAR_SUCCESS on canonical success, or the named error
 * status. Every error leaves the complete record unchanged.
 * @pre When non-NULL, `num` points to writable storage for one bignum_t.
 * @post On success, all words at indexes >= num->len are zero and the selected
 * logical bit is clear; other logical bits retain their prior values.
 * @warning Do not free or concurrently modify `num` during the call. Distinct
 * records may be processed concurrently. No allocation or ownership transfer occurs.
 * @complexity O(BIGNUM_CAPACITY) worst-case time and O(BIGNUM_CAPACITY) auxiliary
 * space in the C11 reference; the assembly path uses O(1) extra registers.
 */
bignum_bit_clear_status_t bignum_bit_clear(bignum_t *num, size_t bit_index);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_BIT_CLEAR_H */
/**
 * @file bignum_bit_set.h
 * @brief Public API for assigning one bit in a fixed-capacity bignum_t.
 * @details The module owns no storage and performs no allocation. The caller
 * owns a writable bignum_t whose little-endian 64-bit words represent the
 * value. On valid input the selected bit is assigned, unrelated logical bits
 * are preserved, len is normalized, and every physical word at or above len is
 * cleared. Validation precedes mutation, so every error is transactional.
 * @note This API is reentrant and safe for independent records. A caller must
 * synchronize concurrent writers that target the same record.
 */
#ifndef BIGNUM_BIT_SET_H
#define BIGNUM_BIT_SET_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of one bit assignment.
 * @details The status is named rather than represented by a primitive integer.
 * On every error the caller-owned record is byte-for-byte unchanged and may be
 * retried after correcting the input. Success guarantees canonical output.
 */
typedef enum bignum_bit_set_status {
    BIGNUM_BIT_SET_SUCCESS = 0, /**< Assignment completed; output is canonical. */
    BIGNUM_BIT_SET_ERROR_NULL_ARG = -1, /**< `num` is NULL; no object is accessed and no output exists. */
    BIGNUM_BIT_SET_ERROR_INDEX = -2, /**< `bit_index` is outside the fixed bit domain; output is unchanged. */
    BIGNUM_BIT_SET_ERROR_LENGTH = -3, /**< `num->len` exceeds capacity; output is unchanged and retry is allowed. */
    BIGNUM_BIT_SET_ERROR_VALUE = -4 /**< `bit_value` is not exactly 0 or 1; output is unchanged and retry is allowed. */
} bignum_bit_set_status_t;

/**
 * @brief Assigns one logical bit in a caller-owned bignum record.
 * @details The function computes word and intra-word positions from
 * `bit_index`, validates all inputs, applies a set or clear mask, removes
 * leading zero words, and clears the physical tail. Clearing a bit outside the
 * current logical length is a successful no-op. Setting a bit may extend len
 * through the selected word, but never beyond BIGNUM_CAPACITY.
 * @param[in,out] num Caller-allocated writable record; borrowed for the call,
 * unchanged on error, and mutated in place on success. NULL is accepted only
 * to produce BIGNUM_BIT_SET_ERROR_NULL_ARG.
 * @param[in] bit_index Zero-based bit position in
 * [0, BIGNUM_CAPACITY * 64); the unit is one bit.
 * @param[in] bit_value Exact assignment value, either 0 or 1; other integers
 * are rejected.
 * @return BIGNUM_BIT_SET_SUCCESS on canonical in-place assignment, or the
 * named error status describing the rejected input. No output mutation occurs
 * for any error status.
 * @pre When non-NULL, `num` points to writable storage for one bignum_t.
 * @post On success, all words at indexes >= num->len are zero and all logical
 * bits other than bit_index retain their prior value.
 * @warning The record must not be freed or concurrently modified during the
 * call. This operation is safe without global synchronization for distinct
 * records. No heap allocation, ownership transfer, or cleanup call is needed.
 * @complexity O(BIGNUM_CAPACITY) worst-case time for canonical tail cleanup and
 * O(1) auxiliary space.
 */
bignum_bit_set_status_t bignum_bit_set(bignum_t *num, size_t bit_index, int bit_value);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_BIT_SET_H */
/**
 * @file bignum_bit_test.h
 * @brief Public API for querying one bit of a fixed-capacity bignum.
 * @details The module reads an unsigned 2048-bit `bignum_t` without modifying it.
 * A bit index is numbered from zero at the least-significant bit of `words[0]`.
 * The API is caller-owned, allocation-free, reentrant, and safe for concurrent
 * calls when callers obey the core object's ordinary data-race rules.
 *
 * @version 1.0.0
 * @since 1.0.0
 */
#ifndef BIGNUM_BIT_TEST_H
#define BIGNUM_BIT_TEST_H

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of a bit-query operation.
 * @details A successful call writes exactly one value, `0` or `1`, to the
 * caller-provided output. Failure statuses leave the output value unchanged.
 */
typedef enum bignum_bit_test_status {
    BIGNUM_BIT_TEST_SUCCESS = 0, /**< Query completed; output is exactly 0 or 1. */
    BIGNUM_BIT_TEST_ERROR_NULL_ARG = -1, /**< `num` or `bit_value` is NULL; output is unchanged. */
    BIGNUM_BIT_TEST_ERROR_INDEX = -2, /**< `bit_index` is outside the fixed capacity; output is unchanged. */
    BIGNUM_BIT_TEST_ERROR_LENGTH = -3 /**< `num->len` exceeds capacity; output is unchanged. */
} bignum_bit_test_status_t;

/**
 * @brief Reads one bit from an unsigned fixed-capacity bignum.
 * @details The word index is `bit_index / 64` and the intra-word offset is
 * `bit_index % 64`. A valid index at or above the normalized `len` reads as
 * zero, so leading zero capacity and a zero value are handled without a
 * special caller-side branch. The function does not normalize, allocate, or
 * modify the input object.
 * @param[in] num Caller-owned bignum record; must be non-NULL and have
 * normalized `len <= BIGNUM_CAPACITY`.
 * @param[in] bit_index Zero-based bit position; valid range is
 * `[0, BIGNUM_CAPACITY * 64)`.
 * @param[out] bit_value Caller-allocated integer receiving `0` or `1` only on
 * success; its previous value is preserved on failure.
 * @return A named `bignum_bit_test_status_t` status code.
 * @pre `num` and `bit_value` point to live storage for the duration of the call.
 * @post On success, `*bit_value` equals the selected bit and `num` is unchanged.
 * @warning The function is a read-only query; callers must synchronize a
 * concurrent writer of `num`.
 * @thread_safety Reentrant and safe for independent concurrent calls.
 * @complexity O(1) time and O(1) auxiliary space.
 */
bignum_bit_test_status_t bignum_bit_test(const bignum_t *num, size_t bit_index,
                                         int *bit_value);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_BIT_TEST_H */
/**
 * @file    bignum_cmp.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    20.11.2025
 *
 * @brief Публичный заголовочный файл для модуля сравнения больших чисел (bignum_t).
 *
 * @details Этот модуль предоставляет функцию для сравнения двух беззнаковых
 *          больших целых чисел, представленных в структуре bignum_t.
 *          Он является частью библиотеки для работы с математикой bignum.
 *
 * @history
 *   - rev. 1 (05.08.2025): Первоначальное создание документа, определение API.
 *   - rev. 2 (05.08.2025): По результатам ревью:
 *                         - Защита от включения (`include guard`) переименована в BIGNUM_CMP_H.
 *                         - Добавлена обработка NULL-аргументов (возврат INT_MIN).
 *                         - Добавлены макросы для семантической версии.
 *   - rev. 3 (20.11.2025): Removed version control functions.
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 */

#ifndef bignum_cmp_H
#define bignum_cmp_H

#include <stddef.h>
#include <stdint.h>
#include <limits.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Коды состояния для функции bignum_cmp.
 */
typedef enum {
    BIGNUM_CMP_GREATER         =  1, /**< `1`, если `a > b` */
    BIGNUM_CMP_EQ              =  0, /**< `0`, если `a == b` */
    BIGNUM_CMP_LESS            = -1, /**< `-1`, если `a < b` */
    BIGNUM_CMP_ERROR_NULL      = INT_MIN  /**< `INT_MIN`, если один из входных указателей (`a` или `b`) равен `NULL`.  */
} bignum_cmp_status_t;

/**
 * @brief Сравнивает два больших беззнаковых числа.
 *
 * @details
 * ### Алгоритм
 * 1.  Проверяются входные указатели `a` и `b` на `NULL`. Если хотя бы один
 *     из них `NULL`, возвращается `INT_MIN` для индикации ошибки.
 * 2.  Сравнивается количество использованных "слов" (`words`) в каждом числе.
 *     - Если `a->words > b->words`, число `a` больше, возвращается `1`.
 *     - Если `a->words < b->words`, число `b` больше, возвращается `-1`.
 * 3.  Если количество "слов" одинаково, производится пословное сравнение,
 *     начиная со старшего "слова" (most significant word) и двигаясь к младшему.
 * 4.  Цикл проходит от `words - 1` до `0`. На каждой итерации:
 *     - Если `a->limbs[i] > b->limbs[i]`, число `a` больше, возвращается `1`.
 *     - Если `a->limbs[i] < b->limbs[i]`, число `b` больше, возвращается `-1`.
 * 5.  Если все "слова" равны, числа равны, возвращается `0`.
 *
 * @param[in] a Указатель на первое большое число (левый операнд).
 * @param[in] b Указатель на второе большое число (правый операнд).
 *
 * @return int Возвращает:
 *         - BIGNUM_CMP_GREATER `1`, если `a > b`.
 *         - BIGNUM_CMP_EQ `0`, если `a == b`.
 *         - BIGNUM_CMP_LESS `-1`, если `a < b`.
 *         - BIGNUM_CMP_ERROR_NULL `INT_MIN`, если один из входных указателей (`a` или `b`) равен `NULL`.
 *
 */
bignum_cmp_status_t bignum_cmp(const bignum_t *a, const bignum_t *b);

#ifdef __cplusplus
}
#endif

#endif /* bignum_cmp_H */
/**
 * @file    bignum_common.h
 * @author  git@bayborodov.com
 * @version 1.0.8
 * @date    17.06.2026
 *
 * @brief   Основные функции ядра для арифметики с большими числами.
 *
 * @details
 *   Структура `bignum_t` является основой для всех высокоточных
 *   целочисленных вычислений в алгоритме форматирования `double`.
 *   Размер массива `words` (32 * 64 = 2048 бита) выбран для покрытия
 *   всех крайних случаев, возникающих при преобразовании `double`.
 *
 *
 * @history
 *   - rev. 1 (01.08.2025): Первоначальное создание.
 *   - rev. 2 (03.10.2025): Дополнение для github.
 *   - rev. 3 (17.06.2026): Добавлены функции инициализации, нормализации.
 *   - rev. 4 (17.06.2026): Реализация вынесена в bignum_common.c.
 *   - rev. 5 (17.06.2026): Реализован парсинг hex/dec;
 *                          bignum_is_zero перенесён в .c.
 *   - rev. 6 (17.06.2026): Парсинг вынесен в `bignum_helpers`.
 *   - rev. 7 (17.06.2026): BIGNUM_ERROR_BAD_BASE/PARSE/EMPTY вынесены
 *                          в `bignum_helpers.h`; bignum.c → bignum_common.c.
 *   - rev. 8 (18.06.2026): Сигнатуры для основныx функций ядра перенесены в bignum_common.h
 */
#ifndef BIGNUM_COMMON_H
#define BIGNUM_COMMON_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------------------------------------------ */
/*  Основные функции ядра — реализация в bignum_common.[c.|.asm]      */
/* ------------------------------------------------------------------ */

/** Обнуляет bignum_t (len = 0, words = 0). */
void bignum_init(bignum_t *x);

/** Инициализирует bignum_t из 64-битного числа. */
void bignum_init_u64(bignum_t *b, uint64_t val);

/**
 * @brief Инициализирует bignum_t из массива uint64_t (младшее слово — first).
 * @return BIGNUM_SUCCESS, BIGNUM_ERROR_NULL_ARG, BIGNUM_ERROR_OVERFLOW.
 */
int bignum_init_from_array(bignum_t *dst,
                           const uint64_t *src,
                           size_t src_len);

/** Возвращает 1, если x == NULL или x->len == 0, иначе 0. */
int bignum_is_zero(const bignum_t *x);

/**
 * @brief Обнуляет «хвост» массива words от индекса start (включительно)
 *        до конца статического буфера BIGNUM_CAPACITY.
 */
void bignum_clear_tail(bignum_t *x, size_t start);

/**
 * @brief Удаляет старшие нулевые слова и обнуляет хвост.
 *        Должна вызываться после любой арифметической операции,
 *        чтобы гарантировать отсутствие неинициализированных данных.
 */
void bignum_normalize(bignum_t *x);

/*
 * Замечание: парсинг строк (hex/dec) живёт в модуле `bignum_helpers` —
 * подключайте его явно через
 *   #include "bignum_helpers.h"
 * если нужна функция `bignum_init_from_string`.
 */

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_COMMON_H */
/* ------------------------------------------------------------------ */
/**
 * @file    bignum_copy.h
 * @brief   Копирование bignum_t.
 * @version 0.2.0
 * @details
 *   Revision 0.1.0: initial public API for copying a normalized bignum_t.
 *   Revision 0.2.0: typed status result and validation contract aligned with
 *                  the bignum-init-from-array template.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_COPY_H
#define BIGNUM_COPY_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Результаты выполнения bignum_copy. */
typedef enum {
    BIGNUM_COPY_SUCCESS        = 0,
    BIGNUM_COPY_ERROR_NULL_ARG = -1,
    BIGNUM_COPY_ERROR_OVERFLOW = -2
} bignum_copy_status_t;

/**
 * @brief Копирует значение bignum_t из src в dest.
 *
 * Массив хранится в little-endian порядке слов. При успешном вызове
 * копируются `src->len` и только используемые слова `src->words[0 ..
 * src->len - 1]`. Неиспользуемый хвост destination не очищается. При
 * `src->len == 0` копируется нулевая длина без копирования words.
 *
 * При NULL-аргументе функция возвращает `BIGNUM_COPY_ERROR_NULL_ARG` и не
 * изменяет destination. При `src->len > BIGNUM_CAPACITY` функция возвращает
 * `BIGNUM_COPY_ERROR_OVERFLOW` и не изменяет destination. Самокопирование
 * `dest == src` является безопасной успешной no-op операцией.
 *
 * Объекты должны быть корректными normalized `bignum_t`; области dest и src
 * не должны частично перекрываться. Поведение при partial overlap не
 * определено в соответствии с `restrict` contract и использованием
 * `memcpy`/`rep movsq`.
 *
 * @param[out] dest Destination bignum_t.
 * @param[in]  src  Source bignum_t.
 * @return `BIGNUM_COPY_SUCCESS`, `BIGNUM_COPY_ERROR_NULL_ARG` или
 *         `BIGNUM_COPY_ERROR_OVERFLOW`.
 */
bignum_copy_status_t bignum_copy(
    bignum_t *restrict dest,
    const bignum_t *restrict src);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_COPY_H */

/* SPDX-License-Identifier: MIT */
/**
 * @file bignum_crt.h
 * @brief Pairwise Chinese Remainder Theorem API for bignum_t values.
 *
 * @details
 * This module combines two congruences into the unique normalized value x in
 * [0, m1*m2). The first release accepts borrowed normalized bignum records
 * whose residues and moduli each occupy at most one 64-bit word. The C11
 * reference and the production YASM path share this public contract but may
 * use different internal algorithms and instruction sequences.
 *
 * All input records remain caller-owned and read-only. The destination is
 * caller-allocated and is published only after complete validation and
 * reconstruction. The operation has no mutable global state and is reentrant
 * for independent records. The implementation is O(log min(m1,m2)) arithmetic
 * iterations and uses O(1) temporary storage.
 */
#ifndef BIGNUM_CRT_H
#define BIGNUM_CRT_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of pairwise CRT validation or reconstruction.
 * @details
 * A success status guarantees that `result` contains a normalized solution.
 * Every failure status guarantees that the destination record is unchanged;
 * callers may correct the input and retry. Inputs and their storage remain
 * caller-owned for every status.
 */
typedef enum bignum_crt_status {
    BIGNUM_CRT_SUCCESS            =  0, /**< Congruences combined; result is normalized and valid. */
    BIGNUM_CRT_ERROR_NULL_ARG     = -1, /**< A required pointer was NULL; result was not dereferenced or changed. */
    BIGNUM_CRT_ERROR_BAD_LENGTH   = -2, /**< An input len exceeded one word; result is unchanged and retry is possible. */
    BIGNUM_CRT_ERROR_MODULUS_ZERO = -3, /**< A modulus represented zero; result is unchanged and a non-zero modulus is required. */
    BIGNUM_CRT_ERROR_NOT_COPRIME  = -4, /**< The moduli were not coprime; result is unchanged and pairwise CRT is undefined. */
    BIGNUM_CRT_ERROR_OVERLAP      = -5, /**< Destination storage overlaps an input record; result is unchanged. */
    BIGNUM_CRT_ERROR_CAPACITY     = -6  /**< The reconstructed value cannot fit the destination representation; result is unchanged. */
} bignum_crt_status_t;

/**
 * @brief Combines two pairwise-coprime modular congruences.
 * @details
 * The routine validates pointers, non-overlap, one-word record lengths,
 * non-zero moduli, and coprimality. It then reduces residues, computes the
 * inverse of m1 modulo m2 with extended Euclid, calculates the Garner factor,
 * and publishes r1 + m1*factor as a normalized one- or two-word result.
 *
 * @param[out] result Caller-allocated destination record. It must be writable,
 *                    non-NULL, and disjoint from every input record.
 * @param[in] residue1 Borrowed first residue record; `len` is zero or one and
 *                      its first word is interpreted modulo modulus1.
 * @param[in] modulus1 Borrowed non-zero first modulus; `len` is zero or one,
 *                      but a zero-length representation is rejected as zero.
 * @param[in] residue2 Borrowed second residue record; `len` is zero or one and
 *                      its first word is interpreted modulo modulus2.
 * @param[in] modulus2 Borrowed non-zero second modulus; `len` is zero or one
 *                      and must be coprime with modulus1.
 * @return `BIGNUM_CRT_SUCCESS` when result satisfies both congruences;
 *         otherwise the applicable named error status and an unchanged result.
 * @pre All records point to accessible storage, inputs are not concurrently
 *      mutated, and the destination does not overlap any input record.
 * @post On success result is normalized and satisfies both congruences; on
 *       failure every byte of result has its prior value.
 * @warning The one-word modulus restriction is a contract boundary. Invalid
 *          lengths, zero moduli, non-coprime moduli, NULL pointers, and aliasing
 *          are rejected before publication.
 * @par Thread safety
 * Reentrant and thread-safe for independent records; a single record must not
 * be concurrently mutated by another thread without external synchronization.
 * @par Complexity
 * O(log min(m1,m2)) word divisions and O(1) temporary storage.
 */
bignum_crt_status_t bignum_crt(
    bignum_t *result,
    const bignum_t *residue1,
    const bignum_t *modulus1,
    const bignum_t *residue2,
    const bignum_t *modulus2);

/**
 * @brief Executes the portable C11 CRT reference implementation.
 * @details It has the same validation, ownership, transactional, result, and
 * thread-safety contract as bignum_crt and exists for baseline measurements,
 * coverage, and differential verification; callers should use bignum_crt for
 * the selected production implementation.
 * @param[out] result Caller-allocated output record; unchanged on failure.
 * @param[in] residue1 Borrowed first residue record, with len at most one.
 * @param[in] modulus1 Borrowed non-zero first modulus, with len at most one.
 * @param[in] residue2 Borrowed second residue record, with len at most one.
 * @param[in] modulus2 Borrowed non-zero coprime second modulus, with len at most one.
 * @return Named bignum_crt_status_t; success publishes a normalized result.
 * @pre Pointers are valid, storage is non-overlapping, and records are not
 *      concurrently mutated.
 * @post Success satisfies both congruences; every failure preserves result.
 * @warning This reference uses compiler-supported 128-bit intermediates.
 * @par Thread safety Reentrant for independent records and caller-owned storage.
 */
bignum_crt_status_t bignum_crt_c11(
    bignum_t *result,
    const bignum_t *residue1,
    const bignum_t *modulus1,
    const bignum_t *residue2,
    const bignum_t *modulus2);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_CRT_H */
/**
 * @file    bignum_div_bignum.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    10.04.2026
 *
 * @brief   Публичный API для деления больших целых bignum_t.
 *
 * @details
 *   Определяет API для функции bignum_div_bignum, включая типы данных,
 *   коды состояния и прототипы функций. Нормализация (удаление ведущих нулей)
 *   выполняется автоматически.
 *
 *   Функция является потокобезопасной при условии, что разные потоки
 *   работают с разными, не пересекающимися объектами `bignum_t`.
 *
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 * @history
 *   - rev. 1 (07.04.2026): Первоначальное создание API.
 *   - rev. 2 (10.04.2026): Уточнение кодов состояния и сигнатуры с учетом bignum_t.
 */

#ifndef BIGNUM_DIV_BIGNUM_H
#define BIGNUM_DIV_BIGNUM_H

#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Коды состояния для функции bignum_div_bignum.
 */
typedef enum {
    BIGNUM_DIV_BIGNUM_OK                    =  0,
    BIGNUM_DIV_BIGNUM_ERR_NULL_PTR          = -1,
    BIGNUM_DIV_BIGNUM_ERR_DIVISION_BY_ZERO  = -2,
    BIGNUM_DIV_BIGNUM_ERR_BUFFER_OVERLAP    = -3,
    BIGNUM_DIV_BIGNUM_ERR_BAD_LENGTH        = -4,/** @brief Ошибка: длина входного числа n->len превышает BIGNUM_CAPACITY. */
    BIGNUM_DIV_BIGNUM_ERR_OVERFLOW          = -5 /**< Результат не помещается в BIGNUM_CAPACITY */
} bignum_div_bignum_status_t;


/**
 * @brief Деление больших беззнаковых целых: quotient = dividend / divisor, remainder = dividend % divisor.
 *
 * @details
 *   Поддерживает нормализацию входных данных, проверку ошибок и потокобезопасность.
 *   Частное и остаток автоматически нормализуются.
 *
 * @param[in]  dividend Указатель на делимое (bignum_t).
 * @param[in]  divisor  Указатель на делитель (bignum_t).
 * @param[out] quotient Указатель на структуру для записи частного (bignum_t).
 * @param[out] remainder Указатель на структуру для записи остатка (bignum_t).
 *
 * @return Код состояния bignum_div_bignum_status_t.
 * ABI: соответствует SysV x86-64; см. реализацию YASM.
 */
bignum_div_bignum_status_t bignum_div_bignum(const bignum_t *dividend,
                                             const bignum_t *divisor,
                                             bignum_t *quotient,
                                             bignum_t *remainder);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_DIV_BIGNUM_H */
/**
 * @file    bignum_div_u64.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    26.11.2025
 *
 * @brief   Публичный заголовочный файл для модуля деления большого числа на uint64_t.
 *
 * @details
 *   Определяет API для функции bignum_div_u64, включая типы данных,
 *   коды состояния и прототипы функций.
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 * @history
 *   - rev. 1 (02.08.2025): Первоначальное создание API.
 *   - rev. 2 (02.08.2025): По результатам ревью:
 *                         - Добавлен новый код ошибки BIGNUM_DIV_U64_ERR_BAD_LENGTH.
 *                         - Обновлена документация для отражения проверки n->len.
 *   - rev. 3 (26.11.2025): Removed version control functions.
 */

#ifndef BIGNUM_DIV_U64_H
#define BIGNUM_DIV_U64_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Коды состояния для функции bignum_div_u64.
 */
typedef enum {
    BIGNUM_DIV_U64_OK                    =  0,
    BIGNUM_DIV_U64_ERR_NULL_PTR          = -1,
    BIGNUM_DIV_U64_ERR_DIVISION_BY_ZERO  = -2,
    BIGNUM_DIV_U64_ERR_BUFFER_OVERLAP    = -3,
/** @brief Ошибка: длина входного числа n->len превышает BIGNUM_CAPACITY. */
    BIGNUM_DIV_U64_ERR_BAD_LENGTH        = -4
} bignum_div_u64_status_t;

/**
 * @brief Выполняет деление большого беззнакового целого числа на 64-битное число.
 *
 * @details
 *   ### Алгоритм
 *   1.  **Валидация:** Проверяются входные указатели на `NULL`, делитель на ноль,
 *       а также буферы `q` и `n` на перекрытие.
 *   2.  **Проверка длины:** Проверяется, что `n->len` не превышает `BIGNUM_CAPACITY`.
 *   3.  **Инициализация:** Буфер результата `q` и остаток `rem` обнуляются.
 *   4.  **Длинное деление:** Выполняется пословное деление, начиная со старшего
 *       слова `n`, с использованием 128/64-битной эмуляции для каждого шага.
 *       Остаток от каждой итерации переносится в следующую.
 *   5.  **Нормализация:** Длина результата `q->len` устанавливается корректно,
 *       удаляя ведущие нули.
 *
 * @param[out] q      Указатель на структуру `bignum_t` для записи частного.
 * @param[in]  n      Указатель на `bignum_t`, представляющую делимое.
 * @param[in]  d      64-битный делитель.
 * @param[out] rem    Указатель на `uint64_t` для записи остатка.
 *
 * @return bignum_div_status_t Код состояния операции.
 * @retval BIGNUM_DIV_U64_OK                    Успешное выполнение.
 * @retval BIGNUM_DIV_U64_ERR_NULL_PTR          Один из входных указателей `NULL`.
 * @retval BIGNUM_DIV_U64_ERR_DIVISION_BY_ZERO  Делитель `d` равен нулю.
 * @retval BIGNUM_DIV_U64_ERR_BUFFER_OVERLAP    Обнаружено перекрытие буферов `q` и `n`.
 * @retval BIGNUM_DIV_U64_ERR_BAD_LENGTH        Длина `n->len` превышает `BIGNUM_CAPACITY`.
 */
bignum_div_u64_status_t bignum_div_u64(bignum_t *q, const bignum_t *n, const uint64_t d, uint64_t *rem);

// --- API для отладки

/**
 * @brief Преобразует код состояния bignum_div_u64 в строку.
 *
 * @param[in] status Код состояния для преобразования.
 * @return const char* Строковое представление кода состояния.
 */
const char* bignum_div_status_to_string(bignum_div_u64_status_t status);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_DIV_U64_H */
/**
 * @file include/bignum_fast_div.h
 * @brief Declares the fixed-capacity bignum quotient/remainder API.
 * @details This public boundary defines caller-allocated inputs and outputs for
 * normalized unsigned bignums. The implementation uses binary long division,
 * never allocates memory, and commits outputs only after successful arithmetic.
 * All pointer parameters are borrowed or caller-owned; no ownership is
 * transferred. The operation is conditionally thread-safe: independent calls
 * may run concurrently when their bignum objects do not overlap.
 * @warning `quotient` and `remainder` must be distinct objects. Invalid input
 * leaves both output objects unchanged.
 */
#ifndef BIGNUM_FAST_DIV_H
#define BIGNUM_FAST_DIV_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports validation and arithmetic outcomes for fast division.
 * @details A success status guarantees both caller-allocated outputs contain
 * normalized values. Every error status guarantees that output objects are
 * unchanged; callers may correct the inputs and retry.
 */
typedef enum bignum_fast_div_status {
    BIGNUM_FAST_DIV_SUCCESS = 0, /**< Division completed; quotient and remainder are valid. */
    BIGNUM_FAST_DIV_ERROR_NULL_ARG = -1, /**< A required pointer was NULL; outputs are unchanged and retry is safe after correction. */
    BIGNUM_FAST_DIV_ERROR_LENGTH = -2, /**< An input length or unused-word invariant was invalid; outputs are unchanged. */
    BIGNUM_FAST_DIV_ERROR_DIVIDE_BY_ZERO = -3, /**< The divisor represented zero; outputs are unchanged and retry requires a non-zero divisor. */
    BIGNUM_FAST_DIV_ERROR_ALIAS = -4 /**< Quotient and remainder were the same object; outputs are unchanged and distinct destinations are required. */
} bignum_fast_div_status_t;

/**
 * @brief Divides a normalized bignum by a normalized non-zero divisor.
 * @details The algorithm scans the dividend from its most significant bit,
 * shifts a private remainder workspace, conditionally subtracts the divisor,
 * and sets the corresponding quotient bit. It then normalizes and commits both
 * outputs. Worst-case complexity is O(W^2) time and O(W) temporary space for
 * W capacity limbs. The function performs no allocation and does not retain
 * pointers after return.
 * @param quotient [out] Caller-allocated distinct destination; unchanged on error and owned by the caller.
 * @param remainder [out] Caller-allocated distinct destination; unchanged on error and owned by the caller.
 * @param dividend [in] Borrowed normalized dividend; ownership remains with caller and input is not modified.
 * @param divisor [in] Borrowed normalized non-zero divisor; ownership remains with caller and input is not modified.
 * @return `BIGNUM_FAST_DIV_SUCCESS` on committed outputs, or a named error status with outputs unchanged.
 * @pre `quotient` and `remainder` point to writable objects and are distinct; inputs satisfy normalized representation when valid.
 * @post On success, `dividend = quotient * divisor + remainder` and `remainder < divisor`; all outputs are normalized.
 * @warning Concurrent calls are safe only for non-overlapping bignum objects. No output is committed on failure.
 */
bignum_fast_div_status_t bignum_fast_div(
    bignum_t *quotient,
    bignum_t *remainder,
    const bignum_t *dividend,
    const bignum_t *divisor);

/**
 * @brief Executes the portable C11 reference implementation.
 * @details This differential-testing and baseline symbol has exactly the public
 * API contract, ownership model, status mapping, and complexity described for
 * bignum_fast_div. It is not required by the production YASM path.
 * @param quotient [out] Caller-allocated output, unchanged on failure.
 * @param remainder [out] Caller-allocated output, unchanged on failure.
 * @param dividend [in] Borrowed normalized input, never modified.
 * @param divisor [in] Borrowed normalized non-zero input, never modified.
 * @return A named bignum_fast_div_status_t; success commits both outputs.
 * @pre Output objects are distinct and writable.
 * @post Successful outputs satisfy the quotient/remainder identity and normalization invariant.
 * @warning Thread-safe only when concurrently used objects do not overlap.
 */
bignum_fast_div_status_t bignum_fast_div_c11(
    bignum_t *quotient,
    bignum_t *remainder,
    const bignum_t *dividend,
    const bignum_t *divisor);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_FAST_DIV_H */
/**
 * @file bignum_from_hex.h
 * @brief Public API for parsing a hexadecimal string into bignum_t.
 * @details The parser accepts an optional `0x`/`0X` prefix, ASCII hexadecimal
 * digits and leading zeroes. It rejects signs, whitespace and invalid digits.
 * Validation and conversion use a private candidate, so every error preserves
 * the complete caller-owned destination record byte-for-byte.
 */
#ifndef BIGNUM_FROM_HEX_H
#define BIGNUM_FROM_HEX_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of a hexadecimal parse.
 * @details Errors are returned before candidate publication and therefore do
 * not modify the destination record.
 */
typedef enum bignum_from_hex_status {
    BIGNUM_FROM_HEX_SUCCESS = 0, /**< Parsed successfully; dst is canonical and normalized. */
    BIGNUM_FROM_HEX_ERROR_NULL_ARG = -1, /**< dst or text is NULL; dst is unchanged. */
    BIGNUM_FROM_HEX_ERROR_EMPTY = -2, /**< No hexadecimal digit was supplied; dst is unchanged. */
    BIGNUM_FROM_HEX_ERROR_INVALID = -3, /**< A sign, whitespace, prefix or digit is invalid; dst is unchanged. */
    BIGNUM_FROM_HEX_ERROR_OVERFLOW = -4 /**< More than 2048 significant bits were supplied; dst is unchanged. */
} bignum_from_hex_status_t;

/**
 * @brief Parses a hexadecimal C string into a caller-owned bignum record.
 * @details The accepted grammar is `[0x|0X]` followed by one or more ASCII
 * digits `0-9`, `a-f` or `A-F`; leading zeroes are allowed. No sign,
 * whitespace, separator or trailing character is accepted. The first input
 * digit becomes the most-significant nibble and the result is stored in the
 * little-endian `bignum_t` word array. A zero value has `len == 0`.
 *
 * The complete input is validated for syntax and capacity before `dst` is
 * published. This makes malformed and overflowing calls transactional.
 * @param[out] dst Caller-owned destination record; must be writable.
 * @param[in] text NUL-terminated ASCII hexadecimal text; borrowed for the call.
 * @return `BIGNUM_FROM_HEX_SUCCESS` on canonical publication, or a named error
 * status with `dst` unchanged.
 * @pre `dst` and `text` point to live storage and `text` is NUL-terminated.
 * @post Success writes normalized words with zero tail and a valid `len`.
 * @warning The input string must not be concurrently modified during parsing.
 * @par Thread safety Reentrant for independent destination records; no global mutable state.
 * @par Complexity O(strlen(text)) time and O(BIGNUM_CAPACITY) local storage.
 */
bignum_from_hex_status_t bignum_from_hex(bignum_t *dst, const char *text);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_FROM_HEX_H */
/**
 * @file bignum_gcd.h
 * @brief Typed API for the greatest common divisor of bignum_t values.
 * @version 1.0.0
 * @date 2026-08-22
 *
 * @details
 * Computes the greatest common divisor of two normalized, non-negative
 * bignum_t operands using Stein's binary Euclidean algorithm. The public
 * operation is transactional: result is written only after validation and
 * arithmetic complete successfully. The caller owns all records; the module
 * performs no allocation and uses no mutable global state.
 *
 * Calls with independent records are thread-safe. `result` must not overlap
 * either input, while the two read-only inputs may refer to the same record.
 */
#ifndef BIGNUM_GCD_H
#define BIGNUM_GCD_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports validation and arithmetic outcome for bignum_gcd.
 * @details A successful status guarantees that result contains a normalized
 * gcd. Every failure status guarantees that the caller-owned result is
 * unchanged. The function does not allocate memory and can be retried after
 * the caller corrects its inputs.
 */
typedef enum bignum_gcd_status {
    BIGNUM_GCD_SUCCESS = 0, /**< Inputs were valid and result contains gcd(a,b). */
    BIGNUM_GCD_ERROR_NULL_ARG = -1, /**< A required pointer was NULL; result is unchanged. */
    BIGNUM_GCD_ERROR_BAD_LENGTH = -2, /**< An input len exceeded BIGNUM_CAPACITY; result is unchanged. */
    BIGNUM_GCD_ERROR_OVERLAP = -3, /**< result overlaps an input record; result is unchanged. */
    BIGNUM_GCD_ERROR_CAPACITY = -4 /**< Internal result publication would exceed capacity; result is unchanged. */
} bignum_gcd_status_t;

/**
 * @brief Computes the greatest common divisor of two bignum_t values.
 * @details Copies the borrowed inputs into private records, removes common
 * powers of two, repeatedly removes remaining even factors and subtracts the
 * smaller odd value from the larger value. The saved common power of two is
 * restored before the normalized result is published. Zero operands are
 * handled directly: gcd(a,0)=a, gcd(0,b)=b, and gcd(0,0)=0.
 *
 * @param[out] result Caller-allocated output record. It is borrowed for the
 *        duration of the call and must not overlap either input.
 * @param[in] a Caller-owned normalized non-negative first operand. Its storage
 *        remains owned by the caller and is not modified.
 * @param[in] b Caller-owned normalized non-negative second operand. Its storage
 *        remains owned by the caller and is not modified; it may alias a.
 * @return bignum_gcd_status_t BIGNUM_GCD_SUCCESS on success, or the named
 *         validation/capacity status explaining why result was not changed.
 * @pre result, a and b are valid pointers; each input len is at most
 *      BIGNUM_CAPACITY; result does not overlap a or b.
 * @post On success result is normalized and represents gcd(a,b), including
 *       result->len == 0 for gcd(0,0). On failure result is byte-for-byte
 *       unchanged.
 * @warning The API accepts only non-negative normalized bignum_t values. A
 *          caller must synchronize concurrent access to the same record.
 * @par Complexity
 * O(n^2) worst-case word operations for n-word inputs and O(1) private storage
 * relative to the fixed BIGNUM_CAPACITY.
 * @par Thread safety
 * Safe for concurrent calls when all calls use independent result and input
 * records; no mutable global state is used.
 */
bignum_gcd_status_t bignum_gcd(
    bignum_t *result,
    const bignum_t *a,
    const bignum_t *b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_GCD_H */
/* ------------------------------------------------------------------ */
/**
 * @file    bignum_init_pow10.h
 * @brief   Инициализация bignum_t значением 10^k.
 * @version 0.1.0
 * @details
 *   Реализация рассчитана на нормализованный 2048-битный bignum_t.
 *   Revision 0.1.0: typed API and initial contract.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_INIT_POW10_H
#define BIGNUM_INIT_POW10_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Результаты выполнения bignum_init_pow10. */
typedef enum {
    BIGNUM_INIT_POW10_SUCCESS              = 0,
    BIGNUM_INIT_POW10_ERROR_NULL_ARG       = -1,
    BIGNUM_INIT_POW10_ERROR_INVALID_EXPONENT = -2,
    BIGNUM_INIT_POW10_ERROR_OVERFLOW       = -3
} bignum_init_pow10_status_t;

/**
 * @brief Инициализирует bignum_t значением 10 в степени k.
 *
 * При успешном вызове `dst` полностью перезаписывается нормализованным
 * представлением 10^k. Поддерживаются k >= 0; для k == 0 результат равен 1.
 * Для 2048-битной ёмкости максимальная представимая десятичная степень равна
 * 616. Отрицательная степень является ошибкой входа, а степень, результат
 * которой не помещается в BIGNUM_CAPACITY слов, возвращает overflow.
 *
 * При любой ошибке `dst` не изменяется. Аргумент dst не должен быть NULL.
 * Функция не выделяет динамическую память и потокобезопасна при независимых
 * объектах назначения.
 *
 * @param[out] dst Объект, который следует инициализировать.
 * @param[in]  k   Неотрицательная десятичная степень.
 * @return Статус операции.
 */
bignum_init_pow10_status_t bignum_init_pow10(
    bignum_t *restrict dst,
    int k);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_INIT_POW10_H */

/* SPDX-License-Identifier: MIT */
/**
 * @file bignum_inverse.h
 * @brief Public typed API for modular multiplicative inverse of bignum_t values.
 * @version 0.2.2
 * @date 2026-08-22
 * @details Computes x in the canonical range 0 <= x < m such that
 * a*x == 1 (mod m), when gcd(a,m) == 1. The reference and assembly
 * implementations use the extended Euclidean algorithm over normalized unsigned
 * bignum_t records. The API is caller-allocated, allocation-free and publishes
 * the destination only after validation and successful computation.
 *
 * Inputs are borrowed and never modified. The destination is caller-owned and
 * must not overlap either input. Independent calls may execute concurrently.
 */
#ifndef BIGNUM_INVERSE_H
#define BIGNUM_INVERSE_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of modular inverse computation.
 * @details A successful status guarantees that result is normalized and in the
 * range [0,m). Every failure status guarantees that result remains unchanged.
 */
typedef enum bignum_inverse_status {
    BIGNUM_INVERSE_SUCCESS = 0, /**< Computation completed; result is the canonical inverse. */
    BIGNUM_INVERSE_ERROR_NULL_ARG = -1, /**< result, a or modulus is NULL; result is unchanged. */
    BIGNUM_INVERSE_ERROR_BAD_LENGTH = -2, /**< An input len exceeds BIGNUM_CAPACITY; retry after normalization. */
    BIGNUM_INVERSE_ERROR_OVERLAP = -3, /**< result overlaps an input; separate caller storage is required. */
    BIGNUM_INVERSE_ERROR_MODULUS_ZERO = -4, /**< modulus is zero; no modular ring is defined and result is unchanged. */
    BIGNUM_INVERSE_ERROR_NO_INVERSE = -5, /**< gcd(a,modulus) is not one or modulus <= 1; result is unchanged. */
    BIGNUM_INVERSE_ERROR_INTERNAL = -6 /**< A validated arithmetic invariant failed; no partial result is published. */
} bignum_inverse_status_t;

/**
 * @brief Computes the modular multiplicative inverse of a modulo modulus.
 * @details The implementation reduces a modulo modulus, runs the extended
 * Euclidean recurrence `(r0,r1) <- (r1,r0-q*r1)` and updates the signed Bezout
 * coefficient for a. If the final gcd is one, the coefficient is normalized
 * into [0, modulus); otherwise NO_INVERSE is returned. Temporary records are
 * private and the caller destination is written only on SUCCESS. Complexity is
 * O(n^2) arithmetic operations with fixed O(n) record storage for the public
 * contract, where n is the number of 64-bit words.
 * @param[out] result Caller-allocated output record. It must be non-NULL and
 * non-overlapping with a and modulus; unchanged on every failure status.
 * @param[in] a Borrowed non-negative input value. It is never modified and may
 * alias modulus because both inputs are copied before arithmetic begins.
 * @param[in] modulus Borrowed positive modulus. Zero returns MODULUS_ZERO; one
 * and non-coprime inputs return NO_INVERSE. It is never modified.
 * @return A named bignum_inverse_status_t value describing publication state.
 * @pre All three pointers are valid, result does not overlap either input, and
 * input lengths are at most BIGNUM_CAPACITY.
 * @post On SUCCESS, result is normalized, satisfies result < modulus and
 * a*result is congruent to one modulo modulus. On failure, result is unchanged.
 * @warning The operation is unsigned; negative a is not representable. The
 * assembly entry point follows System V AMD64 and uses the same record layout.
 * @par Thread safety
 * Safe for concurrent calls when each call uses independent non-overlapping
 * records; the function has no mutable global state.
 */
bignum_inverse_status_t bignum_inverse(
    bignum_t *result,
    const bignum_t *a,
    const bignum_t *modulus);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_INVERSE_H */
/**
 * @file bignum_is_prime.h
 * @brief Public API for Miller--Rabin primality testing of bignum_t values.
 * @details
 * The module accepts a caller-owned, non-negative fixed-capacity bignum_t and
 * writes a Boolean probable-primality result to caller-owned storage. No
 * allocation, ownership transfer, input mutation, or mutable global state is
 * used. Independent calls are reentrant and safe to execute concurrently.
 */
#ifndef BIGNUM_IS_PRIME_H
#define BIGNUM_IS_PRIME_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports validation and execution outcomes of bignum_is_prime.
 * @details
 * A success status guarantees that the output pointer was written with either
 * zero or one. Every error status leaves the caller-owned output unchanged;
 * errors are deterministic and may be retried after the input contract is
 * corrected. No status indicates allocation or ownership transfer.
 */
typedef enum bignum_is_prime_status {
    BIGNUM_IS_PRIME_SUCCESS = 0, /**< [out] Predicate completed; output is 0 or 1 and inputs remain unchanged. */
    BIGNUM_IS_PRIME_ERROR_NULL_ARG = -1, /**< [out] num or is_prime is NULL; no output can be written; correct the pointer and retry. */
    BIGNUM_IS_PRIME_ERROR_BAD_LENGTH = -2, /**< [out] num->len exceeds BIGNUM_CAPACITY; output is unchanged; repair the record and retry. */
    BIGNUM_IS_PRIME_ERROR_ROUNDS = -3 /**< [out] rounds is zero; output is unchanged; provide a positive round count and retry. */
} bignum_is_prime_status_t;

/**
 * @brief Tests whether a bignum_t is probably prime.
 * @details
 * The function copies and normalizes the input, rejects values below two,
 * even values, and small-prime divisors, then performs square-and-multiply
 * Miller--Rabin rounds using fixed bases. One-word values use deterministic
 * 64-bit bases; larger values use bounded fixed-capacity arithmetic. The
 * result is probabilistic for general multiword inputs and becomes stronger
 * as the positive round count increases. Time is O(rounds * n^3) for the
 * bounded binary multiply path and space is O(n), where n is word count.
 * @param[in] num Borrowed caller-owned input record; non-NULL, non-negative,
 *                and len must be in [0, BIGNUM_CAPACITY]. The record is not
 *                modified and may alias no output storage.
 * @param[in] rounds Positive number of Miller--Rabin rounds; units are rounds.
 *                   The caller owns this scalar and retains it after return.
 * @param[out] is_prime Caller-allocated int receiving 1 for probable prime or
 *                       0 for composite only on BIGNUM_IS_PRIME_SUCCESS. The
 *                       pointed storage is borrowed and unchanged on errors.
 * @return BIGNUM_IS_PRIME_SUCCESS, BIGNUM_IS_PRIME_ERROR_NULL_ARG,
 *         BIGNUM_IS_PRIME_ERROR_BAD_LENGTH, or BIGNUM_IS_PRIME_ERROR_ROUNDS.
 * @pre num and is_prime are non-NULL; num->len does not exceed capacity;
 *      rounds is positive; caller retains both objects for the call duration.
 * @post On success, *is_prime is exactly zero or one and num is byte-for-byte
 *       unchanged. On error, *is_prime is unchanged.
 * @warning The predicate reports probable-prime for general multiword input;
 *          it is not a proof of primality. The fixed-capacity representation
 *          does not permit values with len greater than BIGNUM_CAPACITY.
 * @thread_safety Safe for concurrent calls when each call's output storage is
 *                independent; the input is borrowed read-only.
 */
bignum_is_prime_status_t bignum_is_prime(
    const bignum_t *num,
    size_t rounds,
    int *is_prime);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_IS_PRIME_H */
/**
 * @file bignum_isqrt.h
 * @brief Typed API for the integer square root of a bignum_t.
 * @version 1.0.0
 * @date 2026-08-22
 *
 * @details
 * Computes floor(sqrt(x)) for a non-negative, normalized bignum_t value.
 * The operation is transactional: result is published only after complete
 * validation and successful arithmetic. No dynamic allocation or mutable
 * global state is used.
 *
 * The result record must not overlap x. Independent calls are thread-safe.
 */
#ifndef BIGNUM_ISQRT_H
#define BIGNUM_ISQRT_H


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of an integer-square-root operation.
 * @details The API is transactional: every non-success status leaves the
 * caller-owned result record unchanged. The input is never modified.
 */
typedef enum bignum_isqrt_status {
    BIGNUM_ISQRT_SUCCESS = 0, /**< Input accepted; result is normalized floor(sqrt(x)). */
    BIGNUM_ISQRT_ERROR_NULL_ARG = -1, /**< result or x is NULL; result is unchanged and retry is safe after fixing the pointer. */
    BIGNUM_ISQRT_ERROR_BAD_LENGTH = -2, /**< len exceeds capacity or top active word is zero; result is unchanged. */
    BIGNUM_ISQRT_ERROR_OVERLAP = -3, /**< Complete result and input byte ranges overlap; result is unchanged. */
    BIGNUM_ISQRT_ERROR_ARITHMETIC = -4 /**< A dependency or bounded Newton guard failed; result is unchanged and retry is valid only after fixing the arithmetic/input condition. */
} bignum_isqrt_status_t;

/**
 * @brief Computes floor(sqrt(x)).
 *
 * @param[out] result Caller-owned output record; unchanged on failure.
 * @param[in] x Caller-owned normalized non-negative input; never modified.
 * @return BIGNUM_ISQRT_SUCCESS, or a named validation/arithmetic error.
 * @pre result and x are valid bignum_t records and do not overlap.
 * @post On success, result is normalized and represents floor(sqrt(x));
 *       result->len is zero exactly when x is zero.
 * @warning Input records with len > BIGNUM_CAPACITY are rejected. The public
 *          API accepts normalized representations; malformed nonzero values
 *          are rejected rather than silently repaired.
 * @par Complexity
 * O(n^2) word operations for the fixed-width Newton iterations and division.
 * @par Thread safety
 * Safe for concurrent calls using independent records; no mutable global state.
 */
bignum_isqrt_status_t bignum_isqrt(bignum_t *result, const bignum_t *x);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_ISQRT_H */
/**
 * @file bignum_jacobi.h
 * @brief Public API for the Jacobi symbol of bounded unsigned integers.
 * @details
 * The module evaluates (a/n) for non-negative bignum_t operands using the
 * standard binary Jacobi algorithm. The modulus must be positive and odd.
 * Inputs are caller-owned borrowed values and are never modified. The result
 * is written to caller-owned storage only after successful validation and is
 * one of -1, 0, or 1. The operation performs no allocation and is safe for
 * concurrent calls when callers do not concurrently mutate the input objects.
 * The optimized implementation preserves the System V AMD64 ABI and has the
 * same status and output contract as the C11 reference implementation.
 * @warning The function does not interpret a zero or even modulus as a valid
 * Jacobi modulus; such input returns a named error and leaves the output
 * unchanged. The input bignum_t values must be normalized.
 */
#ifndef BIGNUM_JACOBI_H
#define BIGNUM_JACOBI_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports validation and evaluation status for bignum_jacobi.
 * @details A successful status guarantees that the output symbol was written.
 * Every error status leaves the caller-owned output object unchanged and is
 * suitable for correction and retry with valid operands.
 */
typedef enum bignum_jacobi_status {
    BIGNUM_JACOBI_SUCCESS = 0, /**< Symbol computed; output is exactly -1, 0, or 1. */
    BIGNUM_JACOBI_ERROR_NULL_ARG = -1, /**< An input or output pointer is NULL; output is unchanged. */
    BIGNUM_JACOBI_ERROR_MODULUS = -2 /**< Modulus is zero or even; output is unchanged and no symbol is defined. */
} bignum_jacobi_status_t;

/**
 * @brief Computes the Jacobi symbol of two non-negative bounded integers.
 * @details The binary algorithm repeatedly removes factors of two from the
 * numerator, applies the supplementary law for 2, swaps numerator/modulus,
 * applies quadratic reciprocity, and reduces the numerator modulo the modulus.
 * The result is 0 when the final numerator is zero and the remaining modulus
 * is greater than one; otherwise it is the accumulated sign when the modulus
 * reaches one. Time complexity is O(W^2) in the fixed capacity W-word model;
 * temporary storage is O(W) and is stack-local.
 * @param[in] a Borrowed normalized non-negative numerator; it is not modified.
 * @param[in] n Borrowed normalized positive odd modulus; it is not modified.
 * @param[out] symbol Caller-allocated integer receiving -1, 0, or 1 on success.
 *                    It may alias neither input and remains unchanged on error.
 * @return BIGNUM_JACOBI_SUCCESS, BIGNUM_JACOBI_ERROR_NULL_ARG, or
 *         BIGNUM_JACOBI_ERROR_MODULUS.
 * @pre `a`, `n`, and `symbol` are valid pointers; `a` and `n` are normalized.
 * @post Inputs are unchanged. On success `*symbol` is a Jacobi value.
 * @warning The modulus must be odd; this API does not silently normalize it.
 * @thread_safety Safe for independent immutable inputs and distinct outputs.
 */
bignum_jacobi_status_t bignum_jacobi(const bignum_t *a, const bignum_t *n, int *symbol);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_JACOBI_H */
#ifndef BIGNUM_KARATSUBA_MUL_H
#define BIGNUM_KARATSUBA_MUL_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BIGNUM_KARATSUBA_MUL_SUCCESS = 0,
    BIGNUM_KARATSUBA_MUL_ERROR_NULL_ARG = -1,
    BIGNUM_KARATSUBA_MUL_ERROR_INVALID = -2,
    BIGNUM_KARATSUBA_MUL_ERROR_OVERFLOW = -3,
    BIGNUM_KARATSUBA_MUL_ERROR_ALIAS = -4
} bignum_karatsuba_mul_status_t;

/**
 * @brief Multiplies two normalized unsigned bignums.
 * @details The product is published only after complete validation and
 * computation. Inputs and output must be distinct. Products exceeding the
 * fixed BIGNUM_CAPACITY are rejected without modifying output. The C11
 * implementation is the reference implementation; the YASM implementation
 * exposes the same ABI and status contract.
 * @param[out] out Writable destination bignum.
 * @param[in] a First normalized operand.
 * @param[in] b Second normalized operand.
 * @return Named operation status.
 */
bignum_karatsuba_mul_status_t bignum_karatsuba_mul(
    bignum_t *out,
    const bignum_t *a,
    const bignum_t *b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_KARATSUBA_MUL_H */
/**
 * @file bignum_mod_exp.h
 * @brief Typed API for modular exponentiation of bignum_t values.
 * @version 2.0.0
 * @date 2026-08-21
 *
 * @details Computes result = base^exp mod modulus for normalized, non-negative
 * bignum_t operands. The production implementation is a standalone x86-64
 * YASM routine using binary reduction, overflow-safe modular arithmetic and
 * right-to-left square-and-multiply. It does not allocate memory or call C
 * helpers. The operation is transactional: result is modified only after all
 * validation and arithmetic steps complete successfully.
 *
 * Inputs and output are caller-owned fixed-size records. Calls on independent
 * records are thread-safe; the API uses no mutable global state. Exact aliasing
 * and partial overlap are rejected so destination preservation remains atomic.
 */
#ifndef BIGNUM_MOD_EXP_H
#define BIGNUM_MOD_EXP_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of modular exponentiation validation or execution.
 * @details Every return is a named status. On SUCCESS the destination is a
 * normalized value in [0, modulus). On every error the destination is unchanged
 * byte-for-byte, so the caller may correct inputs and retry with the same output.
 */
typedef enum bignum_mod_exp_status {
    BIGNUM_MOD_EXP_SUCCESS            =  0, /**< Valid operation completed; result is normalized and published. */
    BIGNUM_MOD_EXP_ERROR_NULL_ARG     = -1, /**< A required pointer is NULL; destination is unchanged and retry is possible after fixing it. */
    BIGNUM_MOD_EXP_ERROR_MODULUS_ZERO = -2, /**< modulus->len is zero; destination is unchanged and a non-zero modulus is required. */
    BIGNUM_MOD_EXP_ERROR_OVERLAP      = -3, /**< result overlaps base, exp or modulus, including exact alias; destination is unchanged. */
    BIGNUM_MOD_EXP_ERROR_BAD_LENGTH   = -4, /**< An operand len exceeds BIGNUM_CAPACITY; destination is unchanged and inputs must be normalized/fixed. */
    BIGNUM_MOD_EXP_ERROR_CAPACITY     = -5, /**< An intermediate cannot fit BIGNUM_CAPACITY words; destination is unchanged and the call is not retryable without smaller inputs. */
    BIGNUM_MOD_EXP_ERROR_DEPENDENCY   = -6  /**< Internal arithmetic invariant failed; destination is unchanged and the failure requires investigation, not input mutation. */
} bignum_mod_exp_status_t;

/**
 * @brief Computes result = base^exp modulo modulus.
 * @details The routine validates all pointers, lengths, modulus non-zero state
 * and complete 264-byte record overlap before zeroing private stack records.
 * It reduces base by scanning input bits MSB-first, multiplies with bounded
 * double-and-add, and scans exponent bits LSB-first for right-to-left
 * square-and-multiply. Every successful intermediate is normalized; no 2N-word
 * product or heap allocation is used. Time is O(n^2 * log(exp)) in the bounded
 * word model and auxiliary stack space is O(n), where n is the maximum operand
 * length and n <= BIGNUM_CAPACITY.
 *
 * @param[out] result Caller-allocated, writable bignum_t destination. It must
 *                    be non-NULL and must not overlap any input record.
 * @param[in] base Caller-owned borrowed normalized non-negative base record;
 *                  its bytes remain owned by the caller and are never freed.
 * @param[in] exp Caller-owned borrowed normalized non-negative exponent record;
 *                 zero length represents exponent zero.
 * @param[in] modulus Caller-owned borrowed normalized non-zero modulus record.
 *                     The mathematical result is always less than this value.
 * @return A named bignum_mod_exp_status_t. SUCCESS publishes a normalized
 *         result; every error leaves result byte-for-byte unchanged.
 * @pre All input records have len <= BIGNUM_CAPACITY and valid little-endian
 *      words; modulus is non-zero; result storage is writable for one record.
 * @post On SUCCESS, result is normalized and 0 <= result < modulus. On error,
 *       result retains its complete pre-call representation.
 * @warning Exact aliasing and partial overlap are forbidden even though some
 *          internal arithmetic could technically tolerate it; this preserves
 *          the transactional API guarantee. Inputs are borrowed only for the
 *          duration of the call.
 * @par Thread safety
 * Safe for concurrent calls when records are not shared by calls.
 */
bignum_mod_exp_status_t bignum_mod_exp(
    bignum_t *result,
    const bignum_t *base,
    const bignum_t *exp,
    const bignum_t *modulus);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_MOD_EXP_H */
/**
 * @file bignum_mod_inv.h
 * @brief Public typed API for modular multiplicative mod_inv of bignum_t values.
 * @version 0.2.2
 * @date 2026-08-22
 * @details Computes x in the canonical range 0 <= x < m such that
 * a*x == 1 (mod m), when gcd(a,m) == 1. The reference and assembly
 * implementations use the extended Euclidean algorithm over normalized unsigned
 * bignum_t records. The API is caller-allocated, allocation-free and publishes
 * the destination only after validation and successful computation.
 *
 * Inputs are borrowed and never modified. The destination is caller-owned and
 * must not overlap either input. Independent calls may execute concurrently.
 */
#ifndef BIGNUM_MOD_INV_H
#define BIGNUM_MOD_INV_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of modular mod_inv computation.
 * @details A successful status guarantees that result is normalized and in the
 * range [0,m). Every failure status guarantees that result remains unchanged.
 */
typedef enum bignum_mod_inv_status {
    BIGNUM_MOD_INV_SUCCESS = 0, /**< Computation completed; result is the canonical mod_inv. */
    BIGNUM_MOD_INV_ERROR_NULL_ARG = -1, /**< result, a or modulus is NULL; result is unchanged. */
    BIGNUM_MOD_INV_ERROR_BAD_LENGTH = -2, /**< An input len exceeds BIGNUM_CAPACITY; retry after normalization. */
    BIGNUM_MOD_INV_ERROR_OVERLAP = -3, /**< result overlaps an input; separate caller storage is required. */
    BIGNUM_MOD_INV_ERROR_MODULUS_ZERO = -4, /**< modulus is zero; no modular ring is defined and result is unchanged. */
    BIGNUM_MOD_INV_ERROR_NO_INVERSE = -5, /**< gcd(a,modulus) is not one or modulus <= 1; result is unchanged. */
    BIGNUM_MOD_INV_ERROR_INTERNAL = -6 /**< A validated arithmetic invariant failed; no partial result is published. */
} bignum_mod_inv_status_t;

/**
 * @brief Computes the modular multiplicative mod_inv of a modulo modulus.
 * @details The implementation reduces a modulo modulus, runs the extended
 * Euclidean recurrence `(r0,r1) <- (r1,r0-q*r1)` and updates the signed Bezout
 * coefficient for a. If the final gcd is one, the coefficient is normalized
 * into [0, modulus); otherwise NO_INVERSE is returned. Temporary records are
 * private and the caller destination is written only on SUCCESS. Complexity is
 * O(n^2) arithmetic operations with fixed O(n) record storage for the public
 * contract, where n is the number of 64-bit words.
 * @param[out] result Caller-allocated output record. It must be non-NULL and
 * non-overlapping with a and modulus; unchanged on every failure status.
 * @param[in] a Borrowed non-negative input value. It is never modified and may
 * alias modulus because both inputs are copied before arithmetic begins.
 * @param[in] modulus Borrowed positive modulus. Zero returns MODULUS_ZERO; one
 * and non-coprime inputs return NO_INVERSE. It is never modified.
 * @return A named bignum_mod_inv_status_t value describing publication state.
 * @pre All three pointers are valid, result does not overlap either input, and
 * input lengths are at most BIGNUM_CAPACITY.
 * @post On SUCCESS, result is normalized, satisfies result < modulus and
 * a*result is congruent to one modulo modulus. On failure, result is unchanged.
 * @warning The operation is unsigned; negative a is not representable. The
 * assembly entry point follows System V AMD64 and uses the same record layout.
 * @par Thread safety
 * Safe for concurrent calls when each call uses independent non-overlapping
 * records; the function has no mutable global state.
 */
bignum_mod_inv_status_t bignum_mod_inv(
    bignum_t *result,
    const bignum_t *a,
    const bignum_t *modulus);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_MOD_INV_H */
/**
 * @file bignum_mod_sqrt.h
 * @brief Modular square-root API for bounded bignum_t values.
 * @details Computes a root modulo a positive odd prime using a stack-only,
 *          reentrant implementation. Inputs are borrowed and never modified.
 */
#ifndef BIGNUM_MOD_SQRT_H
#define BIGNUM_MOD_SQRT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BIGNUM_MOD_SQRT_SUCCESS = 0,
    BIGNUM_MOD_SQRT_ERROR_NULL_ARG = -1,
    BIGNUM_MOD_SQRT_ERROR_MODULUS = -2,
    BIGNUM_MOD_SQRT_ERROR_NOT_RESIDUE = -3,
    BIGNUM_MOD_SQRT_ERROR_OVERFLOW = -4
} bignum_mod_sqrt_status_t;

/**
 * @brief Compute a modular square root of @p a modulo @p modulus.
 * @param[in] a Non-negative borrowed operand.
 * @param[in] modulus Positive odd prime modulus.
 * @param[out] root Caller-owned output receiving a value in [0, modulus).
 * @return A named bignum_mod_sqrt_status_t value.
 * @retval BIGNUM_MOD_SQRT_SUCCESS A root was written.
 * @retval BIGNUM_MOD_SQRT_ERROR_NULL_ARG A required pointer was NULL.
 * @retval BIGNUM_MOD_SQRT_ERROR_MODULUS The modulus was zero or even.
 * @retval BIGNUM_MOD_SQRT_ERROR_NOT_RESIDUE No root exists modulo modulus.
 * @retval BIGNUM_MOD_SQRT_ERROR_OVERFLOW An operand exceeds capacity.
 * @note Inputs are not modified; root is unchanged on every error path.
 */
bignum_mod_sqrt_status_t bignum_mod_sqrt(const bignum_t *a,
                                          const bignum_t *modulus,
                                          bignum_t *root);
#ifdef __cplusplus
}
#endif
#endif /* BIGNUM_MOD_SQRT_H */
/**
 * @file bignum_mod.h
 * @brief Public API for exact fixed-capacity bignum remainder.
 * @details Computes the non-negative remainder of a canonical unsigned dividend
 * modulo a canonical non-zero modulus. The caller owns all records. The input
 * records are read-only; the destination is written only after success.
 */
#ifndef BIGNUM_MOD_H
#define BIGNUM_MOD_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of bignum_mod.
 * @details The status is returned by value; no status path allocates memory or
 * publishes a partial remainder. Callers may retry after correcting the input.
 */
typedef enum bignum_mod_status {
    BIGNUM_MOD_SUCCESS = 0, /**< Exact normalized remainder was written; inputs remain unchanged. */
    BIGNUM_MOD_ERROR_NULL_ARG = -1, /**< A required pointer was NULL; the destination is unchanged and retry is safe after repair. */
    BIGNUM_MOD_ERROR_DIV_ZERO = -2, /**< The normalized modulus is zero; the destination is unchanged and retry requires a non-zero modulus. */
    BIGNUM_MOD_ERROR_LENGTH = -3 /**< An input length exceeds BIGNUM_CAPACITY; the destination is unchanged and retry requires valid lengths. */
} bignum_mod_status_t;

/**
 * @brief Computes the exact unsigned remainder of one bignum by another.
 * @details The implementation copies and normalizes both inputs, scans the
 * dividend from its most significant bit, shifts a private candidate, and
 * subtracts the modulus when the candidate is large enough. The operation is
 * linear in dividend bit length with fixed-capacity word work per bit.
 * @param[in] dividend Borrowed caller-owned record; `NULL` is invalid and
 * lengths greater than BIGNUM_CAPACITY are rejected.
 * @param[in] modulus Borrowed caller-owned record; zero after normalization is
 * invalid and dirty words above `len` are ignored.
 * @param[out] remainder Caller-allocated record; it may alias either input and
 * is written only after successful completion.
 * @return A named bignum_mod_status_t result; success publishes a valid output,
 * while every error leaves the destination byte-for-byte unchanged.
 * @pre All non-NULL records point to readable/writable storage of bignum_t size.
 * @post On success the result is normalized, has a zero tail, and is less than
 * the normalized modulus. Neither input is modified.
 * @warning The fixed-capacity operation does not allocate memory and does not
 * accept a zero modulus or out-of-range length. It is thread-safe only when
 * concurrently invoked calls do not share a writable destination record.
 */
bignum_mod_status_t bignum_mod(const bignum_t *dividend,
                                const bignum_t *modulus,
                                bignum_t *remainder);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_MOD_H */
#ifndef BIGNUM_MONTGOMERY_MUL_H
#define BIGNUM_MONTGOMERY_MUL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BIGNUM_MONTGOMERY_MUL_SUCCESS = 0,
    BIGNUM_MONTGOMERY_MUL_ERROR_NULL_ARG = -1,
    BIGNUM_MONTGOMERY_MUL_ERROR_LENGTH = -2,
    BIGNUM_MONTGOMERY_MUL_ERROR_MODULUS = -3,
    BIGNUM_MONTGOMERY_MUL_ERROR_RANGE = -4
} bignum_montgomery_mul_status_t;

/**
 * Computes out = a*b*R^-1 mod modulus, where R=2^(64*k) and
 * k=modulus->len. The modulus must be normalized, odd, and non-zero.
 * Inputs must be normalized and strictly smaller than modulus.
 * The output is committed only after successful validation and reduction;
 * on every error out is left byte-for-byte unchanged. Inputs may alias out.
 */
bignum_montgomery_mul_status_t bignum_montgomery_mul(
    bignum_t *restrict out,
    const bignum_t *a,
    const bignum_t *b,
    const bignum_t *modulus);

/** C11 reference entry point used for differential testing and benchmarks. */
bignum_montgomery_mul_status_t bignum_montgomery_mul_c11(
    bignum_t *restrict out,
    const bignum_t *a,
    const bignum_t *b,
    const bignum_t *modulus);

#ifdef __cplusplus
}
#endif
#endif /* BIGNUM_MONTGOMERY_MUL_H */
/**
 * @file    bignum_mul_bignum.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    26.11.2025
 *
* @brief   API для модуля умножения двух больших беззнаковых целых чисел.
 *
 * @details
 *   Эта библиотека предоставляет функцию для выполнения умножения
 *   двух чисел в формате `bignum_t`.
 *
 *   Структура bignum_t (ожидаемая):
 *   - offset 0:  uint64_t words[BIGNUM_CAPACITY] - массив слов числа.
 *   - offset 48: int32_t  len - количество используемых слов.
 *
 * @history
 *   - rev. 1 (02.08.2025): Создание версии 0.0.2. Добавлен новый код ошибки
 *                         BIGNUM_MUL_ERROR_OVERFLOW для явной сигнализации
 *                         о переполнении емкости результата.
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 */

#ifndef BIGNUM_MUL_BIGNUM_H
#define BIGNUM_MUL_BIGNUM_H

#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Коды состояния для функции bignum_mul_bignum.
 */
typedef enum {
    BIGNUM_MUL_BIGNUM_SUCCESS         =  0, /**< Успешное выполнение. */
    BIGNUM_MUL_BIGNUM_ERROR_NULL_ARG  = -1, /**< Ошибка: один из входных указателей равен NULL. */
/**
     * @brief Ошибка: переполнение емкости.
     * @details Сумма длин входных чисел (a->len + b->len) превышает
     *          емкость структуры bignum_t (BIGNUM_CAPACITY). Результат
     *          гарантированно не поместится.
     */
    BIGNUM_MUL_BIGNUM_ERROR_OVERFLOW  = -2  /**< Ошибка: переполнение емкости. */
} bignum_mul_bignum_status_t;



/**
 * @brief Умножает два больших числа a и b, помещая результат в res.
 *
 * @details
 * **Алгоритм:**
 * 1.  Проверка входных указателей на NULL.
 * 2.  Проверка на потенциальное переполнение емкости результата.
 * 3.  Инициализация временного буфера для хранения 128-битных промежуточных
 *     произведений.
 * 4.  Выполнение умножения "в столбик" (schoolbook multiplication).
 * 5.  Нормализация результата с распространением переносов.
 * 6.  Запись финального значения и его длины в структуру `res`.
 *
 * @param res   Указатель на структуру для хранения результата.
 *              Не должен пересекаться в памяти с `a` или `b`.
 * @param a     Указатель на первый множитель.
 * @param b     Указатель на второй множитель.
 *
 * @return Код состояния `bignum_mul_bignum_status_t`.
 */
bignum_mul_bignum_status_t bignum_mul_bignum(bignum_t* res, const bignum_t* a, const bignum_t* b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_MUL_BIGNUM_H */
/**
 * @file bignum_mul_pow10.h
 * @brief In-place multiplication of a fixed-capacity bignum by a power of ten.
 *
 * @details
 * The operation computes `num = num * 10^pow10_exponent` for an unsigned
 * normalized bignum_t with 32 little-endian 64-bit words. The destination is
 * caller-allocated and caller-owned. The implementation uses a private
 * candidate so overflow or invalid input leaves every destination byte
 * unchanged. It has no mutable global state and is reentrant for independent
 * records.
 */
#ifndef BIGNUM_MUL_POW10_H
#define BIGNUM_MUL_POW10_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports multiplication and validation outcomes.
 * @details Success publishes a normalized result. Every error preserves the
 * complete destination record, allowing callers to correct input and retry.
 */
typedef enum bignum_mul_pow10_status {
    BIGNUM_MUL_POW10_SUCCESS = 0, /**< Multiplication completed and result is normalized. */
    BIGNUM_MUL_POW10_ERROR_NULL_ARG = -1, /**< num was NULL; no storage was accessed or changed. */
    BIGNUM_MUL_POW10_ERROR_OVERFLOW = -2, /**< Product exceeded BIGNUM_CAPACITY; destination is unchanged. */
    BIGNUM_MUL_POW10_ERROR_BAD_LENGTH = -3 /**< num->len exceeded capacity; destination is unchanged. */
} bignum_mul_pow10_status_t;

/**
 * @brief Multiply a bignum in place by an exact power of ten.
 * @details A zero exponent is a successful no-op. For non-zero values the
 * decimal exponent must be at most 616; larger exponents necessarily exceed
 * the 2048-bit representation. The C11 reference may multiply by ten per
 * step, while YASM uses 10^19 chunks; both paths implement this contract.
 *
 * @param[in,out] num Caller-allocated bignum_t, normalized or zero-length;
 *                    no ownership is transferred and all bytes are preserved
 *                    on error.
 * @param[in] pow10_exponent Non-negative decimal exponent in the range
 *                           [0, SIZE_MAX]; units are powers of ten.
 * @return `BIGNUM_MUL_POW10_SUCCESS` on publication,
 *         `BIGNUM_MUL_POW10_ERROR_NULL_ARG` for NULL,
 *         `BIGNUM_MUL_POW10_ERROR_BAD_LENGTH` for len > BIGNUM_CAPACITY, or
 *         `BIGNUM_MUL_POW10_ERROR_OVERFLOW` when the product does not fit.
 * @pre num points to writable storage and is not concurrently mutated.
 * @post Success leaves a normalized exact product in num; failure leaves every
 *       byte of num unchanged.
 * @warning This operation is not saturating and never truncates high words.
 * @par Thread safety Safe for independent objects; external synchronization is
 * required if another thread accesses the same num concurrently.
 * @par Complexity C11 is O(exponent * len); YASM is O(ceil(exponent/19) * len).
 */
bignum_mul_pow10_status_t bignum_mul_pow10(
    bignum_t *num,
    size_t pow10_exponent);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_MUL_POW10_H */
/**
 * @file bignum_mul_pow5.h
 * @brief Public API for multiplying a fixed-capacity bignum by a power of five.
 * @details The operation is in-place and transactional: a successful exact product
 * is published, while every error leaves the complete destination record unchanged.
 */
#ifndef BIGNUM_MUL_POW5_H
#define BIGNUM_MUL_POW5_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Classify the result of bignum_mul_pow5.
 * @details Status values are stable ABI values and are independent of bignum-core.
 */
typedef enum bignum_mul_pow5_status {
    BIGNUM_MUL_POW5_SUCCESS = 0,              /**< Exact product was published. */
    BIGNUM_MUL_POW5_ERROR_NULL_ARG = -1,     /**< The destination pointer was NULL. */
    BIGNUM_MUL_POW5_ERROR_OVERFLOW = -2,     /**< The exact product does not fit. */
    BIGNUM_MUL_POW5_ERROR_BAD_LENGTH = -3    /**< `num->len` exceeds capacity. */
} bignum_mul_pow5_status_t;

/**
 * @brief Multiply a bignum in place by the exact value 5^k.
 * @details The function validates the record, computes through private fixed-size
 * storage, and publishes only after every carry fits in BIGNUM_CAPACITY words.
 * A zero-length value is zero and remains zero for every exponent. An exponent
 * of zero is an exact no-op after length validation. No heap allocation, silent
 * truncation, saturation, or external synchronization is used.
 * @param[in,out] num Caller-owned writable bignum record; NULL is rejected and
 *        ownership remains with the caller.
 * @param[in] pow5_exponent Non-negative exponent k; no ownership is transferred.
 * @return BIGNUM_MUL_POW5_SUCCESS on publication;
 *         BIGNUM_MUL_POW5_ERROR_NULL_ARG for NULL;
 *         BIGNUM_MUL_POW5_ERROR_BAD_LENGTH for invalid length;
 *         BIGNUM_MUL_POW5_ERROR_OVERFLOW when the exact result cannot be represented.
 * @pre `num` is writable and is not concurrently accessed by another thread.
 * @post On success `num` represents exactly its old value multiplied by 5^k and
 * is normalized. On error every byte of `num` is unchanged.
 * @par Thread safety Reentrant for independent bignum objects.
 * @par Complexity The C11 reference is O(k*len); YASM uses 27-digit chunks and
 * is O(ceil(k/27)*len), with O(BIGNUM_CAPACITY) private stack storage.
 */
bignum_mul_pow5_status_t bignum_mul_pow5(
    bignum_t *num,
    size_t pow5_exponent);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_MUL_POW5_H */
/**
 * @file    bignum_mul_u64.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    27.11.2025
 *
 * @brief   API для умножения bignum_t на uint64_t.
 *
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 * @history
 *   - rev. 1 (01.08.2025): Первоначальное создание.
 *   - rev. 2 (01.08.2025): Добавлена полная Doxygen-документация для функции.
 */

#ifndef BIGNUM_MUL_U64_H
#define BIGNUM_MUL_U64_H

#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Коды состояния для функции bignum_mul_u64.
 */
typedef enum {
    BIGNUM_MUL_U64_SUCCESS         =  0, /**< Успешное выполнение. */
    BIGNUM_MUL_U64_ERROR_NULL_ARG  = -1, /**< Ошибка: один из входных указателей равен NULL. */
    BIGNUM_MUL_U64_ERROR_OVERFLOW  = -2  /**< Ошибка: переполнение емкости. */
} bignum_mul_u64_status_t;

/**
 * @brief Умножает большое число (bignum_t) на 64-битное целое.
 *
 * @param[out] res Указатель на структуру для хранения результата. Может совпадать с `a`.
 * @param[in]  a   Указатель на множимое (bignum_t).
 * @param[in]  b   Множитель (uint64_t).
 *
 * @return bignum_mul_u64_status_t (0 в случае успеха, -1 в случае переполнения).
 */
bignum_mul_u64_status_t bignum_mul_u64(bignum_t *res, const bignum_t *a, uint64_t b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_MUL_U64_H */
/**
 * @file    bignum_not.h
 * @brief   Typed public API for bitwise inversion of a bignum_t.
 * @version 0.1.0
 *
 * @details
 *   Computes the word-wise bitwise complement of a normalized fixed-capacity
 *   bignum_t. Only logical words in the input are inverted; an input with
 *   len == 0 represents normalized zero and produces normalized zero. The
 *   destination tail is always cleared on success.
 *
 *   Exact in-place aliasing (`result == input`) is supported. Partial overlap
 *   between two distinct complete bignum_t objects is rejected before any
 *   destination write. No dynamic allocation or global mutable state is used.
 */
/* SPDX-License-Identifier: MIT */

#ifndef BIGNUM_NOT_H
#define BIGNUM_NOT_H

#include <stddef.h>
#include <stdint.h>



#ifdef __cplusplus
extern "C" {
#endif

/** @brief Result codes returned by bignum_not. */
typedef enum {
    BIGNUM_NOT_SUCCESS                 = 0,  /**< Operation completed successfully. */
    BIGNUM_NOT_ERROR_NULL_PTR          = -1, /**< A required pointer is NULL. */
    BIGNUM_NOT_ERROR_CAPACITY_EXCEEDED = -2, /**< input->len exceeds BIGNUM_CAPACITY. */
    BIGNUM_NOT_ERROR_BUFFER_OVERLAP    = -3  /**< Distinct complete objects overlap. */
} bignum_not_status_t;

/**
 * @brief Computes the normalized word-wise bitwise complement of `input`.
 *
 * @details The operation inverts each logical word in `input` from index zero
 * through `input->len - 1`, treats a zero-length input as normalized zero, and
 * clears every destination word outside the resulting logical length. Leading
 * zero words are removed before publication. Exact aliases `result == input`
 * are valid; partial overlap between distinct complete objects is rejected.
 *
 * @param[out] result Caller-allocated destination; unchanged on error.
 * @param[in]  input Caller-owned operand; its complete object is read only
 *                   unless it is the exact destination alias.
 * @return A named `bignum_not_status_t` status.
 * @pre Every non-NULL pointer denotes a complete bignum_t object and
 *      `input->len <= BIGNUM_CAPACITY`.
 * @post On success, result is normalized, has a cleared tail, and contains the
 *       word-wise complement of the logical input. On error, result is unchanged.
 * @warning Partial overlap is unsupported; use an exact alias or independent
 *          complete bignum_t objects.
 * @par Thread safety Safe for independent objects; no global mutable state exists.
 * @par Complexity O(BIGNUM_CAPACITY) worst-case time; O(1) auxiliary memory in
 *       ASM and one fixed bignum_t temporary in the C reference implementation.
 */
bignum_not_status_t bignum_not(
    bignum_t *result,
    const bignum_t *input);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_NOT_H */
/**
 * @file    bignum_or.h
 * @brief   Побитовое ИЛИ двух normalized bignum_t.
 * @version 0.1.0
 * @details
 *   Revision 0.1.0: typed API and C/ASM implementation for bitwise OR.
 *   The operation computes the word-wise inclusive OR of two fixed-capacity
 *   bignum objects without dynamic allocation. Successful output is normalized
 *   and all unused storage words are cleared.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_OR_H
#define BIGNUM_OR_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of a bignum_or operation.
 * @details A successful return guarantees that result contains the normalized
 * inclusive OR. Error returns occur before result mutation.
 */
typedef enum {
    BIGNUM_OR_SUCCESS                 = 0,  /**< Operation completed; result is normalized and valid. */
    BIGNUM_OR_ERROR_NULL_PTR          = -1, /**< A required pointer is NULL; result is unchanged. */
    BIGNUM_OR_ERROR_CAPACITY_EXCEEDED = -2, /**< An input len exceeds BIGNUM_CAPACITY; result is unchanged. */
    BIGNUM_OR_ERROR_BUFFER_OVERLAP    = -3  /**< Partial object overlap was detected; result is unchanged. */
} bignum_or_status_t;

/**
 * @brief Computes `result = a | b` for normalized 2048-bit bignum objects.
 * @details The implementation computes through a stack temporary in the C
 * reference path and through register/word operations in the ASM path. The
 * output length starts at the larger input length because OR preserves words
 * present in either operand, then leading zero words are removed. The complete
 * destination storage is cleared before the normalized result is published.
 * Exact aliases `result == a`, `result == b`, and `a == b` are supported.
 * Partial overlap between any two distinct complete objects is rejected.
 *
 * @param[out] result Caller-allocated destination object; unchanged on error.
 * @param[in]  a Caller-owned first operand; its complete object is read only.
 * @param[in]  b Caller-owned second operand; its complete object is read only.
 * @return A named bignum_or_status_t value describing success or the rejected
 *         input condition.
 * @pre Every non-NULL pointer refers to a complete bignum_t object, and each
 *      input len is at most BIGNUM_CAPACITY.
 * @post On success, result equals the normalized bitwise OR and has zeroed tail
 *       words. On error, result storage is not modified.
 * @warning Partial overlap is not a supported aliasing mode; use exact aliases
 *          or independent complete bignum_t objects.
 * Thread-safety: safe for independent objects; no global mutable state exists.
 * Complexity: O(BIGNUM_CAPACITY) worst-case time and O(1) auxiliary memory in
 *             the ASM path; the C reference uses one stack bignum_t temporary.
 */
bignum_or_status_t bignum_or(
    bignum_t *result,
    const bignum_t *a,
    const bignum_t *b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_OR_H */

/* SPDX-License-Identifier: MIT */
/**
 * @file bignum_pow.h
 * @brief Public API for exact exponentiation of a fixed-capacity bignum_t.
 * @details bignum_pow computes base raised to a non-negative uint64_t exponent
 * without modular reduction. The caller owns both records and allocates their
 * storage. The operation uses no global mutable state and publishes the result
 * only after the complete power fits in BIGNUM_CAPACITY words. On NULL input,
 * malformed base length, or arithmetic overflow, the destination is unchanged.
 * Calls on independent records are thread-safe.
 */
#ifndef BIGNUM_POW_H
#define BIGNUM_POW_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of exact fixed-capacity exponentiation.
 * @details Every non-success status preserves the caller-owned destination.
 */
typedef enum bignum_pow_status {
    BIGNUM_POW_SUCCESS = 0, /**< Exact power completed; result is canonical and represents base^exponent. */
    BIGNUM_POW_ERROR_NULL_ARG = -1, /**< Base or result was NULL; no storage was accessed and the destination is unchanged. */
    BIGNUM_POW_ERROR_LENGTH = -2, /**< base->len exceeded BIGNUM_CAPACITY; no arithmetic was attempted and the destination is unchanged. */
    BIGNUM_POW_ERROR_OVERFLOW = -3 /**< The exact power required more than BIGNUM_CAPACITY words; the destination is unchanged and a smaller exponent may be retried. */
} bignum_pow_status_t;

/**
 * @brief Computes an exact non-negative integer power into caller storage.
 * @details The implementation uses exponentiation by squaring. It validates the
 * base length, treats words above base->len as logically zero, and keeps every
 * intermediate in a private fixed-capacity candidate. Multiplication detects
 * any non-zero word above the representable capacity before publication.
 * @param base [in] Caller-owned input bignum_t; may not be NULL and remains
 * valid for the complete call. Its length must not exceed BIGNUM_CAPACITY.
 * @param exponent [in] Non-negative exponent in the range 0..UINT64_MAX.
 * @param result [out] Caller-owned writable destination; may not alias base and
 * remains unchanged on every error. No ownership is transferred.
 * @return A named bignum_pow_status_t: SUCCESS, ERROR_NULL_ARG,
 * ERROR_LENGTH, or ERROR_OVERFLOW.
 * @pre base and result point to live bignum_t records and do not overlap.
 * @post On success, result is canonical, zero has len == 0, one has len == 1
 * with words[0] == 1, and result represents base^exponent. On error, result is
 * byte-for-byte unchanged.
 * @warning The function is thread-safe only when concurrently used records do
 * not overlap; callers must synchronize shared records.
 * @complexity O(log exponent * BIGNUM_CAPACITY^2) time and O(BIGNUM_CAPACITY)
 * auxiliary space.
 */
bignum_pow_status_t bignum_pow(const bignum_t *base, uint64_t exponent,
                                bignum_t *result);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_POW_H */
/**
 * @file benchmark_framework.h
 * @brief Public include bridge for the benchmark-framework distribution.
 * @details
 * The current dist artifact publishes this header at
 * `libs/benchmark-framework/dist/benchmark_framework.h`. A legacy local cache
 * may still expose the equivalent API as `dist/include/benchmark_core.h`.
 * This bridge contains no framework implementation and only normalizes the
 * distribution's packaging location for project-owned benchmark sources.
 */
#ifndef BIGNUM_PRIME_GEN_BENCHMARK_FRAMEWORK_BRIDGE_H
#define BIGNUM_PRIME_GEN_BENCHMARK_FRAMEWORK_BRIDGE_H

#if defined(__has_include)
#  if __has_include("../libs/benchmark-framework/dist/benchmark_framework.h")
#    include "../libs/benchmark-framework/dist/benchmark_framework.h"
#  else
#    include "../libs/benchmark-framework/dist/include/benchmark_core.h"
#  endif
#else
#  include "../libs/benchmark-framework/dist/benchmark_framework.h"
#endif

#endif /* BIGNUM_PRIME_GEN_BENCHMARK_FRAMEWORK_BRIDGE_H */
/**
 * @file bignum_prime_gen.h
 * @brief Generate a probable prime with an exact requested bit length.
 * @details The module samples candidates through bignum_random and validates
 * them with bignum_is_prime. The caller owns all input/output storage; no heap
 * allocation or mutable global state is used. The C11 implementation is the
 * correctness reference and the assembly implementation preserves this API.
 */
#ifndef BIGNUM_PRIME_GEN_H
#define BIGNUM_PRIME_GEN_H

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of exact-bit-length probable-prime generation.
 * @details A failure leaves the complete caller-owned output record unchanged.
 */
typedef enum bignum_prime_gen_status {
    BIGNUM_PRIME_GEN_SUCCESS = 0, /**< Candidate passed Miller--Rabin; `out` contains the normalized prime. */
    BIGNUM_PRIME_GEN_ERROR_NULL_ARG = -1, /**< `out` is NULL; no output exists and no retry can correct the call. */
    BIGNUM_PRIME_GEN_ERROR_BITS = -2, /**< `bits` is outside [2, BIGNUM_CAPACITY * 64]; output is unchanged. */
    BIGNUM_PRIME_GEN_ERROR_ROUNDS = -3, /**< `rounds` is zero; output is unchanged and caller must provide a positive value. */
    BIGNUM_PRIME_GEN_ERROR_ATTEMPTS = -4, /**< `max_attempts` is zero; output is unchanged and caller may retry with a bound. */
    BIGNUM_PRIME_GEN_ERROR_RANDOM = -5, /**< Random dependency failed; output is unchanged and the call may be retried. */
    BIGNUM_PRIME_GEN_ERROR_PRIMALITY = -6 /**< No candidate passed or primality dependency failed; output is unchanged. */
} bignum_prime_gen_status_t;

/**
 * @brief Generates a probable prime with exactly `bits` significant bits.
 * @param[out] out Destination record written only on success.
 * @param[in] bits Requested bit length in the inclusive range
 *                 [2, BIGNUM_CAPACITY * 64].
 * @param[in] rounds Positive Miller--Rabin round count passed to
 *                    bignum_is_prime.
 * @param[in] max_attempts Positive upper bound on candidate attempts.
 * @return A named bignum_prime_gen_status_t value; only SUCCESS writes `*out`.
 * @details Each candidate has its highest requested bit and least significant
 * bit set, so accepted output has the requested bit length and is odd. The
 * random source is rejection-based and candidates are tested independently.
 * If no prime is found before `max_attempts`, the output is unchanged and
 * BIGNUM_PRIME_GEN_ERROR_PRIMALITY is returned.
 * @pre `out` is a live caller-owned bignum_t; all scalar parameters are valid.
 * @post On success, `out` is normalized, odd, and has exactly `bits` bits.
 * @thread_safety Independent calls are reentrant and safe concurrently; one output record must not be shared concurrently.
 * @complexity O(max_attempts * primality_cost(bits)) time and O(1) caller-visible storage.
 */
bignum_prime_gen_status_t bignum_prime_gen(
    bignum_t *out,
    size_t bits,
    size_t rounds,
    size_t max_attempts);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_PRIME_GEN_H */
/**
 * @file bignum_random.h
 * @brief Public API for a cryptographically random bounded bignum.
 * @details The module samples a uniformly distributed unsigned `bignum_t` from
 * the half-open interval `[0, upper_bound)`. It uses the Linux `getrandom(2)`
 * urandom source and rejection sampling rather than a remainder reduction, so
 * a non-power-of-two bound does not introduce modulo bias. The API owns no
 * caller memory and modifies the caller-owned output only after a valid sample
 * has been produced. The C11 reference retains no entropy bytes. The x86-64
 * YASM path may retain unused raw `getrandom(2)` bytes in ELF thread-local
 * storage to amortize future syscalls; they are never shared across threads and
 * are discarded when the process identifier changes after `fork()`.
 *
 * The implementation targets Linux on x86-64. The `upper_bound` input must use
 * the normalized fixed-capacity representation defined by `bignum.h`: zero has
 * `len == 0`, and a nonzero value has `1 <= len <= BIGNUM_CAPACITY` with a
 * nonzero most-significant word. Independent calls are reentrant; concurrent
 * access to an object that another thread writes still requires caller-side
 * synchronization.
 *
 * @version 0.1.1
 */
#ifndef BIGNUM_RANDOM_H
#define BIGNUM_RANDOM_H



#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the outcome of bounded cryptographic random sampling.
 * @details Success is the only status that writes `*out`. Every failure status
 * preserves the complete caller-owned output record, enabling a caller to
 * retry after correcting an input or a transient entropy-source failure.
 */
typedef enum bignum_random_status {
    BIGNUM_RANDOM_SUCCESS = 0, /**< A uniform sample was stored in `*out`; `0 <= *out < *upper_bound`. */
    BIGNUM_RANDOM_ERROR_NULL_ARG = -1, /**< `out` or `upper_bound` is NULL; no object is dereferenced and `*out` is unchanged when valid. */
    BIGNUM_RANDOM_ERROR_RANGE = -2, /**< `upper_bound` represents zero; the interval is empty and `*out` is unchanged. */
    BIGNUM_RANDOM_ERROR_LENGTH = -3, /**< `upper_bound->len` exceeds fixed capacity; `*out` is unchanged. */
    BIGNUM_RANDOM_ERROR_NORMALIZATION = -4, /**< A nonzero bound has a zero most-significant word; `*out` is unchanged. */
    BIGNUM_RANDOM_ERROR_ENTROPY = -5, /**< Linux `getrandom(2)` could not refill required entropy; `*out` is unchanged and the call may be retried. */
    BIGNUM_RANDOM_ERROR_ALIAS = -6 /**< `out` aliases `upper_bound`; both records are unchanged. */
} bignum_random_status_t;

/**
 * @brief Samples a cryptographically random unsigned bignum in `[0, upper_bound)`.
 * @details The function validates the bound before obtaining entropy. It derives
 * the bit length of the normalized upper bound, obtains exactly enough random
 * bytes from Linux `getrandom(2)`, masks unused high bits, and rejects any
 * candidate not strictly smaller than `upper_bound`. Repeating those steps until
 * acceptance gives every value in the requested interval the same probability.
 * Candidate storage is automatic and is copied to `*out` only after acceptance.
 * The YASM implementation may consume already obtained bytes from its current
 * thread's private cache; it refills that cache exclusively through `getrandom`
 * and invalidates it after a process-ID change.
 *
 * @param[out] out Caller-owned writable destination. It must point to one live
 * `bignum_t`; it may not alias `upper_bound`. The function writes a normalized
 * value only with `BIGNUM_RANDOM_SUCCESS` and otherwise preserves all bytes.
 * @param[in] upper_bound Caller-owned borrowed normalized positive bound. The
 * pointed-to object is read only for the duration of the call and is never
 * modified. A zero bound, a length above `BIGNUM_CAPACITY`, an unnormalized
 * nonzero bound, or aliasing with `out` is rejected.
 * @return A named `bignum_random_status_t` status. On success `*out` is a
 * normalized uniformly distributed value in `[0, *upper_bound)`; on failure
 * `*out` and `*upper_bound` are unchanged.
 * @pre `out` and `upper_bound` designate non-overlapping live `bignum_t`
 * objects for the whole call when neither is NULL. Equal pointers are detected
 * and rejected with `BIGNUM_RANDOM_ERROR_ALIAS`.
 * @post Successful output has `len == 0` exactly when its sampled value is
 * zero; otherwise `words[len - 1] != 0` and all words at or above `len` are
 * zero.
 * @warning The call may block before Linux initializes its urandom source.
 * Rejection sampling has data-dependent iteration count and must not be used
 * where that timing is itself secret-sensitive.
 * @thread_safety Reentrant and safe for independent concurrent calls. The C11
 * path uses no mutable state. The YASM path has mutable ELF thread-local cache
 * state only; it has no shared global state, file descriptor, or heap allocation.
 * @complexity O(BIGNUM_CAPACITY) time and O(BIGNUM_CAPACITY) automatic space
 * per candidate. The YASM cache refill is O(BIGNUM_CAPACITY) bytes and is
 * amortized across later calls; expected candidate count is below two for any
 * positive bound at the selected bit length.
 */
bignum_random_status_t bignum_random(bignum_t *out, const bignum_t *upper_bound);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_RANDOM_H */
/**
 * @file    bignum_shift_left.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    07.11.2025
 *
 * @brief   Публичный API для логического сдвига bignum_t влево.
 *
 * @details
 *   Выполняет in-place (на месте) логический сдвиг большого числа на
 *   заданное количество бит. Нормализация (удаление ведущих нулей)
 *   выполняется автоматически.
 *
 *   Функция является потокобезопасной при условии, что разные потоки
 *   работают с разными, не пересекающимися объектами `bignum_t`.
 *
 *   **Алгоритм:**
 *    1. Проверка аргументов.
 *    2. Нулевой сдвиг — быстрый выход.
 *    3. Разбиение `shift_amount` на сдвиг по словам (`word_shift`) и битам (`bit_shift`).
 *    4. Проверка на переполнение (при выходе старшего бита за BIGNUM_CAPACITY).
 *    5. Сдвиг по словам, затем побитовый сдвиг с переносами между словами.
 *    6. Обновление `len` и нормализация результата.
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 * @history
 *   - rev. 1 (02.08.2025): Первоначальное создание API.
 *   - rev. 2 (02.08.2025): API улучшен по результатам аудита: добавлены
 *                         макросы версий, `restrict`, `size_t`, улучшены
 *                         Doxygen-комментарии и include guards.
 *   - rev. 3 (07.11.2025): Removed version control functions.
 */

#ifndef BIGNUM_SHIFT_LEFT_H
#define BIGNUM_SHIFT_LEFT_H

#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Коды состояния для функции bignum_shift_left.
 */
typedef enum {
    BIGNUM_SHIFT_SUCCESS         =  0, /**< Успех. Сдвиг выполнен. */
    BIGNUM_SHIFT_ERROR_NULL_ARG  = -1, /**< Указатель `num` равен NULL. */
    BIGNUM_SHIFT_ERROR_OVERFLOW  = -2  /**< Сдвиг привел к потере значащих бит (переполнению). */
} bignum_shift_status_t;

/**
 * @brief      Выполняет логический сдвиг большого числа влево.
 *
 * @param[in,out] num           Указатель на число для модификации. Размер внутреннего
 *                              буфера определяется BIGNUM_CAPACITY.
 * @param[in]     shift_amount  Количество бит для сдвига влево.
 *
 * @return
 *   - `BIGNUM_SHIFT_SUCCESS` (0) – сдвиг выполнен успешно.
 *   - `BIGNUM_SHIFT_ERROR_NULL_ARG` (-1) – передан NULL вместо числа.
 *   - `BIGNUM_SHIFT_ERROR_OVERFLOW` (-2) – сдвиг привёл к переполнению ёмкости.
 *
 * @details
 *   **Алгоритм:**
 *   1.  Проверка аргументов на NULL.
 *   2.  Если `shift_amount` равен 0, немедленно вернуть успех.
 *   3.  Вычислить сдвиг в целых словах (`word_shift`) и в битах внутри слова (`bit_shift`).
 *   4.  Проверить, не приведет ли сдвиг к переполнению (самый старший бит
 *       сдвигается за пределы емкости `BIGNUM_CAPACITY`).
 *   5.  Выполнить сдвиг по словам, перемещая данные в старшие позиции.
 *   6.  Выполнить побитовый сдвиг внутри слов, распространяя биты-переносы
 *       из младших слов в старшие.
 *   7.  Обновить поле `len` и нормализовать результат (удалить ведущие нули).
 *
 * @param[in,out] num           Указатель на число для модификации.
 * @param[in]     shift_amount  Количество бит для сдвига влево.
 *
 * @return     Код состояния `bignum_shift_status_t`.
 */
bignum_shift_status_t bignum_shift_left(bignum_t* restrict num, size_t shift_amount);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_SHIFT_LEFT_H */
/**
 * @file    bignum_shift_right.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    10.11.2025
 *
 * @brief   Публичный API для логического сдвига bignum_t вправо.
 *
 * @details
 *   Выполняет in-place (на месте) логический сдвиг большого числа на
 *   заданное количество бит. Нормализация (удаление ведущих нулей)
 *   выполняется автоматически.
 *
 *   Функция является потокобезопасной при условии, что разные потоки
 *   работают с разными, не пересекающимися объектами `bignum_t`.
 *
 *   **Алгоритм:**
 *    1. Проверка аргументов.
 *    2. Нулевой сдвиг — быстрый выход.
 *    3. Разбиение `shift_amount` на сдвиг по словам (`word_shift`) и битам (`bit_shift`).
 *    4. Если сдвиг по словам превышает длину числа, обнулить его и вернуть BIGNUM_SHIFT_RIGHT_ZEROED.
 *    5. Сдвиг по словам, затем побитовый сдвиг с переносами между словами.
 *    6. Обновление `len` и нормализация результата.
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 * @history
 *   - rev. 1 (10.08.2025): Первоначальное создание API по аналогии с bignum_shift_left.
 *   - rev. 2 (10.08.2025): Улучшен API по результатам ревью: добавлен код возврата
 *                         BIGNUM_SHIFT_RIGHT_ZEROED, уточнена документация
 *                         потокобезопасности и поведения при обнулении.
 *   - rev. 3 (10.11.2025): Removed version control functions.
 */

#ifndef BIGNUM_SHIFT_RIGHT_H
#define BIGNUM_SHIFT_RIGHT_H

#include <stddef.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Коды состояния для функции bignum_shift_right.
 */
typedef enum {
    BIGNUM_SHIFT_RIGHT_SUCCESS         =  0, /**< Успех. Сдвиг выполнен. */
    BIGNUM_SHIFT_RIGHT_ERROR_NULL_ARG  = -1, /**< Указатель `num` равен NULL. */
    BIGNUM_SHIFT_RIGHT_ZEROED          =  1, /**< Сдвиг больше длины числа, результат обнулен. */
} bignum_shift_right_status_t;

/**
 * @brief      Выполняет логический сдвиг большого числа вправо.
 *
 * @details
 *   **Алгоритм:**
 *   1.  Проверка аргументов на NULL.
 *   2.  Если `shift_amount` равен 0, немедленно вернуть успех.
 *   3.  Вычислить сдвиг в целых словах (`word_shift`) и в битах внутри слова (`bit_shift`).
 *   4.  Если `word_shift` больше или равен `len`, обнулить число и вернуть `BIGNUM_SHIFT_RIGHT_ZEROED`.
 *   5.  Выполнить сдвиг по словам, перемещая данные в младшие позиции.
 *   6.  Выполнить побитовый сдвиг внутри слов, распространяя биты-переносы
 *       из старших слов в младшие.
 *   7.  Обновить поле `len` и нормализовать результат (удалить ведущие нули).
 *
 * @param[in,out] num           Указатель на число для модификации.
 * @param[in]     shift_amount  Количество бит для сдвига вправо.
 *
 * @return     Код состояния `bignum_shift_right_status_t`.
 *   - `BIGNUM_SHIFT_RIGHT_SUCCESS` (0) – сдвиг выполнен успешно.
 *   - `BIGNUM_SHIFT_RIGHT_ERROR_NULL_ARG` (-1) – передан NULL вместо числа.
 *   - `BIGNUM_SHIFT_RIGHT_ZEROED` (1) – сдвиг был настолько велик, что все значащие
 *     биты были потеряны и число стало равно 0.
 */
bignum_shift_right_status_t bignum_shift_right(bignum_t* restrict num, size_t shift_amount);


#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_SHIFT_RIGHT_H */
/* ------------------------------------------------------------------ */
/**
 * @file    bignum_swap.h
 * @brief   In-place обмен двух значений bignum_t.
 * @version 0.1.0
 * @details
 *   Revision 0.1.0: initial typed API and in-place swap contract.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_SWAP_H
#define BIGNUM_SWAP_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Результаты выполнения bignum_swap. */
typedef enum {
    BIGNUM_SWAP_SUCCESS        = 0,
    BIGNUM_SWAP_ERROR_NULL_ARG = -1,
    BIGNUM_SWAP_ERROR_OVERFLOW = -2
} bignum_swap_status_t;

/**
 * @brief Обменивает два значения bignum_t in-place.
 *
 * При успешном вызове меняются местами все поля объектов, включая `len` и
 * весь фиксированный массив слов. Дополнительная динамическая память не
 * выделяется; реализации используют временные регистры.
 *
 * NULL-аргументы возвращают `BIGNUM_SWAP_ERROR_NULL_ARG`. Некорректные
 * значения `len > BIGNUM_CAPACITY` возвращают `BIGNUM_SWAP_ERROR_OVERFLOW` и
 * не изменяют ни один объект. Exact self-swap (`a == b`) является успешной
 * no-op операцией. Объекты должны быть корректными normalized bignum_t.
 *
 * Для non-identical объектов действует `restrict` contract: области объектов
 * не должны частично перекрываться. Partial overlap не поддерживается.
 *
 * @param[in,out] a Первый объект.
 * @param[in,out] b Второй объект.
 * @return `BIGNUM_SWAP_SUCCESS`, `BIGNUM_SWAP_ERROR_NULL_ARG` или
 *         `BIGNUM_SWAP_ERROR_OVERFLOW`.
 */
bignum_swap_status_t bignum_swap(
    bignum_t *restrict a,
    bignum_t *restrict b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_SWAP_H */

/* SPDX-License-Identifier: MIT */
/**
 * @file bignum_to_hex.h
 * @brief Public API for converting a fixed-capacity bignum_t to hexadecimal.
 * @details The conversion emits a canonical lowercase hexadecimal string with
 * no prefix and one digit (`0`) for zero. The caller owns the output buffer;
 * validation completes before any output byte is modified.
 */
#ifndef BIGNUM_TO_HEX_H
#define BIGNUM_TO_HEX_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of a bignum-to-hex conversion.
 * @details Every failure status is returned before publication, so the caller
 * output buffer remains byte-for-byte unchanged and the input record is read-only.
 */
typedef enum bignum_to_hex_status {
    BIGNUM_TO_HEX_SUCCESS = 0, /**< Conversion completed; output is canonical and NUL-terminated. */
    BIGNUM_TO_HEX_ERROR_NULL_ARG = -1, /**< `num` or `output` was NULL; no storage was accessed and retry is safe after fixing the pointer. */
    BIGNUM_TO_HEX_ERROR_LENGTH = -2, /**< `num->len` exceeded BIGNUM_CAPACITY; output is unchanged and retry is safe after fixing the record. */
    BIGNUM_TO_HEX_ERROR_BUFFER = -3 /**< output_size was too small; output is unchanged and retry is safe with a larger buffer. */
} bignum_to_hex_status_t;

/**
 * @brief Converts a caller-owned bignum record to canonical hexadecimal.
 * @details Words are interpreted little-endian, while emitted text is most-
 * significant digit first. Leading zero nibbles are omitted; zero emits `0`.
 * All validation precedes output writes, so every error preserves the output.
 * @param[in] num Borrowed bignum record with `len <= BIGNUM_CAPACITY`.
 * @param[out] output Caller-owned output buffer.
 * @param[in] output_size Output capacity in bytes, including the NUL byte.
 * @return `BIGNUM_TO_HEX_SUCCESS` on publication of canonical output;
 * `BIGNUM_TO_HEX_ERROR_NULL_ARG`, `BIGNUM_TO_HEX_ERROR_LENGTH` or
 * `BIGNUM_TO_HEX_ERROR_BUFFER` on validation failure, with output unchanged.
 * @pre `num` and `output` point to live storage for the duration of the call.
 * @post Success writes a canonical lowercase string and one trailing NUL.
 * @warning The input record must not be concurrently modified during conversion.
 * @par Thread safety Reentrant and safe for independent records; no global state.
 * @par Complexity O(BIGNUM_CAPACITY) worst-case time; C11 uses fixed local storage,
 * while the YASM path uses O(1) extra registers.
 */
bignum_to_hex_status_t bignum_to_hex(const bignum_t *num, char *output, size_t output_size);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_TO_HEX_H */

/**
 * @file    bignum_to_string.h
 * @brief   Конвертация беззнакового bignum_t в строку.
 * @version 0.5.0
 * @details
 *   Ревизия 0.5.0: public contract без изменений; ASM получил P1 paths
 *   для Base2 byte LUT, Base8 reservoir и Base16 byte-pair LUT.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_TO_STRING_H
#define BIGNUM_TO_STRING_H

#include <stddef.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Результаты выполнения операций bignum_to_string. */
typedef enum {
    BIGNUM_TO_STRING_SUCCESS        = 0,
    BIGNUM_TO_STRING_ERROR_NULL_ARG = -1,
    BIGNUM_TO_STRING_ERROR_BAD_BASE = -2,
    BIGNUM_TO_STRING_ERROR_NO_SPACE = -3,
    BIGNUM_TO_STRING_ERROR_INVALID   = -4
} bignum_to_string_status_t;

/**
 * @brief Вычисляет размер буфера для строкового представления bignum_t.
 * @details
 *   Возвращаемый размер включает завершающий нулевой байт. Поддерживаются
 *   основания 2, 8, 10 и 16. Для нулевого значения требуется два байта.
 *   Функция не изменяет src и не использует глобальное изменяемое состояние.
 *
 * @param[in]  src           Нормализованное беззнаковое число.
 * @param[in]  base          Основание строки: 2, 8, 10 или 16.
 * @param[out] required_size Минимальный размер буфера вместе с NUL.
 * @return Статус операции.
 */
bignum_to_string_status_t bignum_to_string_size(
    const bignum_t *src,
    int base,
    size_t *required_size);

/**
 * @brief Преобразует bignum_t в строку указанного основания.
 * @details
 *   Поддерживаются основания 2, 8, 10 и 16. Результат всегда беззнаковый,
 *   поскольку bignum_t не содержит знакового поля. Для Base16 используются
 *   строчные hexadecimal digits. При недостаточном буфере функция возвращает
 *   BIGNUM_TO_STRING_ERROR_NO_SPACE и не записывает частичный результат.
 *
 * @param[out] dst      Буфер назначения.
 * @param[in]  dst_size Размер dst в байтах.
 * @param[in]  src      Нормализованное беззнаковое число.
 * @param[in]  base     Основание строки: 2, 8, 10 или 16.
 * @return Статус операции.
 */
bignum_to_string_status_t bignum_to_string(
    char *dst,
    size_t dst_size,
    const bignum_t *src,
    int base);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_TO_STRING_H */

/* SPDX-License-Identifier: MIT */
/**
 * @file    bignum_xor.h
 * @brief   Побитовый XOR двух normalized bignum_t.
 * @version 0.1.0
 * @details
 *   Revision 0.1.0: typed API and C/ASM implementation for bitwise XOR.
 *   The operation computes the word-wise inclusive XOR of two fixed-capacity
 *   bignum objects without dynamic allocation. Successful output is normalized
 *   and all unused storage words are cleared.
 */
/* ------------------------------------------------------------------ */
#pragma once
#ifndef BIGNUM_XOR_H
#define BIGNUM_XOR_H

#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Reports the result of a bignum_xor operation.
 * @details A successful return guarantees that result contains the normalized
 * inclusive XOR. Error returns occur before result mutation.
 */
typedef enum {
    BIGNUM_XOR_SUCCESS                 = 0,  /**< Operation completed; result is normalized and valid. */
    BIGNUM_XOR_ERROR_NULL_PTR          = -1, /**< A required pointer is NULL; result is unchanged. */
    BIGNUM_XOR_ERROR_CAPACITY_EXCEEDED = -2, /**< An input len exceeds BIGNUM_CAPACITY; result is unchanged. */
    BIGNUM_XOR_ERROR_BUFFER_OVERLAP    = -3  /**< Partial object overlap was detected; result is unchanged. */
} bignum_xor_status_t;

/**
 * @brief Computes `result = a ^ b` for normalized 2048-bit bignum objects.
 * @details The implementation computes through a stack temporary in the C
 * reference path and through register/word operations in the ASM path. The
 * output length starts at the larger input length because XOR preserves words
 * present in either operand, then leading zero words are removed. The complete
 * destination storage is cleared before the normalized result is published.
 * Exact aliases `result == a`, `result == b`, and `a == b` are supported.
 * Partial overlap between any two distinct complete objects is rejected.
 *
 * @param[out] result Caller-allocated destination object; unchanged on error.
 * @param[in]  a Caller-owned first operand; its complete object is read only.
 * @param[in]  b Caller-owned second operand; its complete object is read only.
 * @return A named bignum_xor_status_t value describing success or the rejected
 *         input condition.
 * @pre Every non-NULL pointer refers to a complete bignum_t object, and each
 *      input len is at most BIGNUM_CAPACITY.
 * @post On success, result equals the normalized bitwise XOR and has zeroed tail
 *       words. On error, result storage is not modified.
 * @warning Partial overlap is not a supported aliasing mode; use exact aliases
 *          or independent complete bignum_t objects.
 * Thread-safety: safe for independent objects; no global mutable state exists.
 * Complexity: O(BIGNUM_CAPACITY) worst-case time and O(1) auxiliary memory in
 *             the ASM path; the C reference uses one stack bignum_t temporary.
 */
bignum_xor_status_t bignum_xor(
    bignum_t *result,
    const bignum_t *a,
    const bignum_t *b);

#ifdef __cplusplus
}
#endif

#endif /* BIGNUM_XOR_H */

/* SPDX-License-Identifier: MIT */
#ifndef BENCHMARK_FRAMEWORK_SINGLE_H
#define BENCHMARK_FRAMEWORK_SINGLE_H

/**
 * @file benchmark_core.h
 * @brief Публичный C11 adapter API воспроизводимого ST/MT benchmark-core.
 *
 * @details
 * Core отделяет универсальный lifecycle от предметной операции клиента. Алгоритм
 * запуска создаёт deterministic immutable dataset callback-ом initialize, перед
 * каждой измеряемой операцией получает independent mutable copy, вызывает operation
 * и складывает checksum. ST выполняет этот цикл последовательно; MT даёт каждому
 * worker отдельную state область и синхронизирует старт barriers, поэтому core не
 * передаёт один mutable record между потоками.
 */
#ifndef BENCHMARK_CORE_H
#define BENCHMARK_CORE_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Представляет именованный boolean result внутреннего и callback контракта.
 * @details
 * Тип устраняет неявные `0`/`1` в управляющих ветвях. Он применяется для состояний
 * ready/released/aborted и predicate results; это не статус ошибки функции.
 */
typedef enum {
    BENCHMARK_BOOLEAN_FALSE = 0, /**< Условие ложно или флаг не установлен. */
    BENCHMARK_BOOLEAN_TRUE = 1   /**< Условие истинно или флаг установлен. */
} benchmark_boolean_t;

/**
 * @brief Описывает все статусы, возвращаемые core lifecycle и его helpers.
 * @details
 * Значения позволяют adapter binary и internal cleanup различать ошибку аргументов,
 * allocation, clock, callback, worker и protocol без неименованных чисел. Public
 * runner возвращает этот enum; ISO C `main` mapping выполняет только project adapter.
 */
typedef enum {
    BENCHMARK_CORE_STATUS_SUCCESS = 0,       /**< Lifecycle завершён и protocol опубликован. */
    BENCHMARK_CORE_STATUS_HELP = 1,          /**< Запрошен `--help`; usage напечатан без benchmark run. */
    BENCHMARK_CORE_STATUS_ARGUMENT_ERROR = 2,/**< CLI, ENV, token или adapter contract некорректен. */
    BENCHMARK_CORE_STATUS_ALLOCATION_ERROR = 3, /**< Dataset/workspace/worker allocation не удался. */
    BENCHMARK_CORE_STATUS_CLOCK_ERROR = 4,   /**< CLOCK_MONOTONIC не предоставил timestamp. */
    BENCHMARK_CORE_STATUS_CALLBACK_ERROR = 5,/**< Adapter initialize/operation вернул неуспешный status. */
    BENCHMARK_CORE_STATUS_THREAD_ERROR = 6   /**< pthread create/join/synchronization lifecycle не удался. */
} benchmark_core_status_t;

/**
 * @brief Описывает result, который domain adapter возвращает core callback-ам.
 * @details
 * Adapter обязан явно сопоставить status предметной библиотеки с этими значениями.
 * Core интерпретирует только SUCCESS; все прочие codes приводят к CALLBACK_ERROR,
 * но сохраняют семантическое имя в project-owned adapter code.
 */
typedef enum {
    BENCHMARK_ADAPTER_STATUS_SUCCESS = 0, /**< Предметная операция завершилась успешно. */
    BENCHMARK_ADAPTER_STATUS_INPUT_ERROR = 1, /**< Workload/state нарушает adapter precondition. */
    BENCHMARK_ADAPTER_STATUS_OPERATION_ERROR = 2 /**< Предметная операция вернула runtime failure. */
} benchmark_adapter_status_t;

/**
 * @brief Описывает workload metadata, передаваемые project-owned adapter без изменений.
 * @details
 * Core разбирает CLI/ENV, печатает metadata в protocol и передаёт один immutable
 * descriptor callbacks. Он намеренно не приписывает предметную семантику строкам:
 * bignum adapter может трактовать operation_kind как shift path, а byte adapter —
 * как transform. seed, warmup и data_count образуют воспроизводимый lifecycle input.
 */
typedef struct {
    const char *data_mode; /**< Legacy mode или `custom`; protocol compatibility field. */
    const char *input_kind; /**< Domain-defined source input class, например `zero`. */
    const char *operation_kind; /**< Domain-defined operation path, например `bit`. */
    const char *measure_mode; /**< Declared timing boundary text: end-to-end/kernel-only. */
    const char *size_profile; /**< Domain-defined logical operand/state length profile. */
    const char *capacity_profile; /**< Domain-defined capacity boundary profile. */
    uint64_t seed; /**< Stable deterministic dataset seed. */
    uint64_t warmup; /**< Unmeasured operation count per ST run or MT worker. */
    size_t data_count; /**< Number of immutable source records in cyclic dataset. */
} benchmark_workload_t;

/**
 * @brief Создаёт один immutable source-state record deterministic dataset.
 * @param state Zeroed writable buffer размера benchmark_adapter_t::state_size.
 * @param sequence_index Стабильный индекс record при равных seed/workload.
 * @param workload Непосредственно переданный immutable workload descriptor.
 * @param adapter_context Project-owned opaque context.
 * @return Именованный benchmark_adapter_status_t result.
 * @details
 * Алгоритм callback-а должен полностью инициализировать `state` без shared mutable
 * state. Core вызывает initialize до warm-up/measurement, сохраняет source dataset
 * и использует memcpy для подготовки каждой in-place operation.
 */
typedef benchmark_adapter_status_t (*benchmark_initialize_fn)(
    void *state,
    uint64_t sequence_index,
    const benchmark_workload_t *workload,
    void *adapter_context);

/**
 * @brief Выполняет одну измеряемую in-place операцию над mutable state record.
 * @param state Независимая mutable копия одного source record.
 * @param iteration Logical ST iteration либо iteration соответствующего MT worker.
 * @param workload Непосредственно переданный immutable workload descriptor.
 * @param adapter_context Project-owned opaque context.
 * @return Именованный benchmark_adapter_status_t result.
 * @details
 * Алгоритм adapter-а не должен хранить result в shared global state. В kernel-only
 * mode core исключает preparation copy из elapsed interval; в end-to-end mode copy
 * входит в interval. Callback выбирает operation parameter детерминированно из
 * iteration/workload, если это необходимо конкретному bignum profile.
 */
typedef benchmark_adapter_status_t (*benchmark_operation_fn)(
    void *state,
    uint64_t iteration,
    const benchmark_workload_t *workload,
    void *adapter_context);

/**
 * @brief Производит deterministic observable checksum post-operation state.
 * @param state Read-only post-operation record.
 * @param iteration Logical iteration соответствующего вызова.
 * @param adapter_context Project-owned opaque context.
 * @return Наблюдаемое 64-bit значение для protocol checksum reduction.
 * @details
 * Алгоритм должен читать достаточную часть результата, чтобы operation не могла
 * быть удалена оптимизатором как ненаблюдаемая. Core смешивает callback values в
 * final checksum, который печатается в machine-readable completion line.
 */
typedef uint64_t (*benchmark_checksum_fn)(
    const void *state,
    uint64_t iteration,
    void *adapter_context);

/**
 * @brief Связывает concrete client operation с generic benchmark-core lifecycle.
 *
 * @details
 * Core валидирует все поля до allocation и запуска. state_size определяет один
 * opaque record; callbacks и success_code определяют предметный contract. Adapter
 * не владеет dataset memory: core создаёт и освобождает buffers, тогда как context
 * остаётся собственностью вызывающего project до возврата run function.
 */
typedef struct {
    const char *benchmark_name; /**< Stable protocol identifier без whitespace/equal sign. */
    size_t state_size; /**< Размер одного opaque mutable state record в bytes. */
    benchmark_adapter_status_t success_code; /**< Domain success code, обычно BENCHMARK_ADAPTER_STATUS_SUCCESS. */
    void *adapter_context; /**< Project-owned opaque context, живущий весь benchmark run. */
    benchmark_initialize_fn initialize; /**< Callback deterministic source-state initialization. */
    benchmark_operation_fn operation; /**< Callback измеряемой in-place операции. */
    benchmark_checksum_fn checksum; /**< Callback наблюдаемого post-operation checksum. */
} benchmark_adapter_t;

/**
 * @brief Запускает generic parameterized single-thread benchmark harness.
 * @param argc Число CLI arguments adapter binary.
 * @param argv CLI argument vector adapter binary.
 * @param adapter Валидный binding project operation к core callbacks.
 * @return Именованный benchmark_core_status_t lifecycle result.
 * @details
 * Алгоритм разбирает CLI и совместимый ENV, создаёт deterministic dataset, выполняет
 * warm-up, измеряет declared iteration count одним worker, публикует ровно одну
 * `benchmark=...` строку и затем `Benchmark finished.`. Legacy data-mode mapping
 * сохраняется для существующих Makefile workflows.
 */
benchmark_core_status_t benchmark_core_run_st(
    int argc,
    char **argv,
    const benchmark_adapter_t *adapter);

/**
 * @brief Запускает generic parameterized multi-thread benchmark harness.
 * @param argc Число CLI arguments adapter binary.
 * @param argv CLI argument vector adapter binary.
 * @param adapter Валидный binding project operation к core callbacks.
 * @return Именованный benchmark_core_status_t lifecycle result.
 * @details
 * Алгоритм требует кратность total iterations threads, создаёт per-worker dataset
 * copies и context, barrier-синхронизирует warm-up/start, собирает elapsed/checksum
 * каждого worker и публикует единый aggregate protocol. Потокобезопасность операции
 * достигается отсутствием shared mutable state между callback invocations.
 */
benchmark_core_status_t benchmark_core_run_mt(
    int argc,
    char **argv,
    const benchmark_adapter_t *adapter);

#ifdef __cplusplus
}
#endif

#endif
/**
 * @file json_lib.h
 * @brief Публичный C11-интерфейс самостоятельной библиотеки разбора и записи JSON.
 * @details
 * `json-lib` принимает UTF-8-совместимый JSON-текст, синтаксически проверяет его
 * и строит плоский массив токенов в префиксном порядке. Каждый токен хранит
 * диапазон исходного текста и индекс родителя; поэтому библиотека не формирует
 * рекурсивное дерево и позволяет безопасно выполнять навигацию только через
 * проверяемые индексы. Исходный текст и массив токенов принадлежат
 * `json_document_t` до вызова `json_document_destroy()`.
 *
 * Все публичные операции возвращают только `json_status_t`. Полезные значения,
 * включая предикаты, числовые преобразования и индексы токенов, передаются через
 * output-параметры. Единственная функция, которой разрешено возвращать `int`,
 * находится в исполняемых программах-потребителях и является ISO C точкой входа
 * `main`; она не входит в API этой библиотеки.
 *
 * @par Потоковая безопасность
 * Библиотека не содержит глобального изменяемого состояния. Независимые объекты
 * `json_document_t` и `json_writer_t` допускается использовать одновременно в
 * разных потоках. Один объект не должен использоваться конкурентно без внешней
 * синхронизации, потому что parse/destroy и writer-операции изменяют его поля.
 */
#ifndef JSON_LIB_H
#define JSON_LIB_H

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/**
 * @brief Представляет именованный итог выполнения операции json-lib.
 * @details
 * При `JSON_STATUS_SUCCESS` библиотека гарантирует инициализацию всех output-
 * параметров, описанных контрактом функции. При любом другом значении вызывающий
 * код не должен читать output-параметры, если функция не документирует обратное.
 */
typedef enum {
    JSON_STATUS_SUCCESS = 0,          /**< Операция завершена успешно; outputs записаны. */
    JSON_STATUS_ARGUMENT_ERROR = 1,   /**< Передан NULL, неинициализированный объект, неверный индекс или параметр. */
    JSON_STATUS_NOT_FOUND = 2,        /**< Запрошенный ключ объекта либо позиция элемента массива отсутствует. */
    JSON_STATUS_SYNTAX_ERROR = 3,     /**< Вход не удовлетворяет JSON-грамматике или ожидаемому primitive type. */
    JSON_STATUS_ALLOCATION_ERROR = 4, /**< Не удалось выделить память для текста, токенов или пути writer-а. */
    JSON_STATUS_CAPACITY_ERROR = 5,   /**< Требуемый размер массива токенов превышает безопасный предел `size_t`. */
    JSON_STATUS_IO_ERROR = 6          /**< Операция над файлом, stream, fsync, close либо atomic rename завершилась ошибкой. */
} json_status_t;

/**
 * @brief Представляет типизированный результат JSON-предиката.
 * @details
 * Тип исключает использование неименованных целочисленных литералов в API
 * `json_token_has_type()` и `json_token_equals()`.
 */
typedef enum {
    JSON_BOOLEAN_FALSE = 0, /**< Проверяемое условие не выполняется. */
    JSON_BOOLEAN_TRUE = 1   /**< Проверяемое условие выполняется. */
} json_boolean_t;

/**
 * @brief Описывает синтаксическую категорию сохранённого JSON-токена.
 * @details
 * Строковый токен хранит байты между кавычками в исходном JSON, то есть без
 * внешних `"`, но с исходными escape-последовательностями. Primitive-токен
 * представляет number, `true`, `false` или `null`.
 */
typedef enum {
    JSON_TOKEN_UNDEFINED = 0, /**< Внутренний незанятый слот; в успешном документе не выдаётся. */
    JSON_TOKEN_OBJECT = 1,    /**< Объект, открывающийся `{` и закрывающийся `}`. */
    JSON_TOKEN_ARRAY = 2,     /**< Массив, открывающийся `[` и закрывающийся `]`. */
    JSON_TOKEN_STRING = 3,    /**< Строка без внешних кавычек. */
    JSON_TOKEN_PRIMITIVE = 4  /**< JSON number, boolean literal или null literal. */
} json_token_type_t;

/**
 * @brief Хранит положение одного лексического JSON-токена и его место в иерархии.
 * @details
 * Диапазон `[start, end)` индексирует байты поля `json_document_t::text`.
 * `parent_index` образует плоское дерево: у root-токена он равен `-1`, у каждого
 * другого токена — индексу контейнера. Поле `child_count` относится только к
 * непосредственным потомкам array/object; у объекта учитываются как key strings,
 * так и соответствующие value tokens.
 */
typedef struct {
    json_token_type_t type;  /**< Синтаксическая категория токена. */
    size_t start;            /**< Включаемое смещение начала в `document->text`. */
    size_t end;              /**< Исключаемое смещение конца в `document->text`. */
    size_t child_count;      /**< Число непосредственных потомков контейнера. */
    ptrdiff_t parent_index;  /**< Индекс токена-родителя либо `-1` для корня. */
} json_token_t;

/**
 * @brief Владеет копией JSON-текста и результатом его разбора.
 * @details
 * Перед первым использованием объект должен быть инициализирован через
 * `json_document_init()`. Операции `json_document_parse_text()` и
 * `json_document_load_file()` заменяют ранее сохранённый документ только после
 * предварительного освобождения его ресурсов. Публичные поля предназначены для
 * read-only диагностики; пользователь не должен изменять их либо освобождать
 * принадлежащую объекту память напрямую.
 */
typedef struct {
    char *text;              /**< Owned NUL-terminated копия исходного JSON-текста. */
    json_token_t *tokens;    /**< Owned префиксный массив токенов, ссылающихся на `text`. */
    size_t text_size;        /**< Число байтов JSON до завершающего NUL. */
    size_t token_count;      /**< Число инициализированных элементов `tokens`. */
} json_document_t;

/**
 * @brief Владеет состоянием незавершённой atomic JSON file transaction.
 * @details
 * `json_writer_open()` создаёт временный файл в каталоге целевого файла.
 * `json_writer_write_raw()` и `json_writer_write_string()` записывают в этот
 * файл. `json_writer_commit()` выполняет flush, fsync и rename, после чего
 * временный путь атомарно становится целевым на одной файловой системе.
 * `json_writer_abort()` удаляет временный файл и возвращает объект в пустое
 * состояние; его безопасно вызывать после частично неудачного открытия.
 */
typedef struct {
    FILE *stream;             /**< Owned writable temporary stream до commit/abort. */
    char *temporary_path;     /**< Owned NUL-terminated путь временного файла. */
    char *target_path;        /**< Owned NUL-terminated путь публикуемого файла. */
} json_writer_t;

/**
 * @brief Инициализирует document как пустой объект без выделения памяти.
 * @param document Output-объект, который требуется инициализировать.
 * @return `JSON_STATUS_SUCCESS` либо `JSON_STATUS_ARGUMENT_ERROR`.
 * @details
 * Алгоритм записывает нулевой aggregate. После успеха документ можно передать
 * любой parse/load операции или безопасно освободить через
 * `json_document_destroy()`.
 */
json_status_t json_document_init(json_document_t *document);

/**
 * @brief Разбирает NUL-terminated JSON-текст, у которого корнем является object.
 * @param text Входной JSON-текст, не принадлежащий библиотеке.
 * @param document Инициализированный output-документ для результата разбора.
 * @param error Необязательный буфер человекочитаемой диагностики.
 * @param error_capacity Ёмкость `error` в байтах; ноль разрешён при `error == NULL`.
 * @return Именованный `json_status_t` результата разбора.
 * @details
 * Алгоритм копирует исходный текст, выполняет синтаксический recursive-descent
 * разбор в плоский префиксный token array и при заполнении capacity повторяет
 * разбор с удвоенной ёмкостью. Успешным считается только документ с root object;
 * при ошибке частично выделенные ресурсы освобождаются, а `document` возвращается
 * в пустое состояние.
 */
json_status_t json_document_parse_text(const char *text, json_document_t *document,
    char *error, size_t error_capacity);

/**
 * @brief Загружает JSON-файл и разбирает его как document с root object.
 * @param path Путь к читаемому regular file.
 * @param document Инициализированный output-документ.
 * @param error Необязательный буфер диагностики синтаксической ошибки.
 * @param error_capacity Ёмкость `error` в байтах.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм открывает файл в бинарном режиме, определяет длину, считывает ровно
 * это число байтов в NUL-terminated буфер и передаёт буфер
 * `json_document_parse_text()`. Ошибки доступа и чтения отличаются от ошибок
 * JSON-грамматики кодом `JSON_STATUS_IO_ERROR`.
 */
json_status_t json_document_load_file(const char *path, json_document_t *document,
    char *error, size_t error_capacity);

/**
 * @brief Освобождает все ресурсы document и повторно инициализирует его.
 * @param document Инициализированный document для освобождения.
 * @return `JSON_STATUS_SUCCESS` либо `JSON_STATUS_ARGUMENT_ERROR`.
 * @details
 * Алгоритм освобождает text и tokens независимо, после чего обнуляет aggregate.
 * Поэтому повторный вызов после успешного destroy не вызывает double free.
 */
json_status_t json_document_destroy(json_document_t *document);

/**
 * @brief Находит value token непосредственного поля JSON object по имени ключа.
 * @param document Успешно разобранный document.
 * @param object_index Индекс токена типа `JSON_TOKEN_OBJECT`.
 * @param key Искомое имя ключа без JSON-кавычек.
 * @param value_index Output-индекс токена значения.
 * @return `JSON_STATUS_SUCCESS`, `JSON_STATUS_NOT_FOUND` или error status.
 * @details
 * Алгоритм обходит непосредственные key/value пары объекта. Для каждого key
 * сравниваются raw bytes, а функция определения span пропускает всё поддерево
 * соответствующего value. Вложенные поля не участвуют в поиске.
 */
json_status_t json_object_get(const json_document_t *document, size_t object_index,
    const char *key, size_t *value_index);

/**
 * @brief Возвращает число непосредственных элементов JSON array.
 * @param document Успешно разобранный document.
 * @param array_index Индекс токена типа `JSON_TOKEN_ARRAY`.
 * @param element_count Output-число непосредственных элементов.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм проверяет тип токена и копирует сохранённое при разборе количество
 * прямых потомков массива без обхода вложенных поддеревьев.
 */
json_status_t json_array_size(const json_document_t *document, size_t array_index,
    size_t *element_count);

/**
 * @brief Возвращает индекс элемента JSON array по нулевой позиции.
 * @param document Успешно разобранный document.
 * @param array_index Индекс токена типа `JSON_TOKEN_ARRAY`.
 * @param element_position Нулевая позиция среди непосредственных элементов.
 * @param value_index Output-индекс найденного токена.
 * @return `JSON_STATUS_SUCCESS`, `JSON_STATUS_NOT_FOUND` или error status.
 * @details
 * Алгоритм последовательно обходит прямых потомков массива, пропуская целые
 * вложенные subtrees. Благодаря этому позиция не зависит от числа токенов в
 * предыдущем объекте или массиве.
 */
json_status_t json_array_get(const json_document_t *document, size_t array_index,
    size_t element_position, size_t *value_index);

/**
 * @brief Сравнивает синтаксический тип токена с ожидаемым типом.
 * @param document Успешно разобранный document.
 * @param token_index Индекс проверяемого токена.
 * @param expected_type Ожидаемая категория токена.
 * @param result Output-предикат равенства типов.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм валидирует документ и индекс, затем записывает `JSON_BOOLEAN_TRUE`
 * либо `JSON_BOOLEAN_FALSE`; несовпадение типа не является ошибкой операции.
 */
json_status_t json_token_has_type(const json_document_t *document, size_t token_index,
    json_token_type_t expected_type, json_boolean_t *result);

/**
 * @brief Проверяет raw text токена на точное побайтное равенство строке.
 * @param document Успешно разобранный document.
 * @param token_index Индекс сравниваемого токена.
 * @param expected_text Ожидаемый текст без JSON-кавычек для string token.
 * @param result Output-предикат равенства.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм сначала сравнивает длины, затем выполняет `memcmp()` по диапазону
 * `[start, end)`. Escape-последовательности string token не декодируются:
 * сравнение выполняется с raw JSON representation.
 */
json_status_t json_token_equals(const json_document_t *document, size_t token_index,
    const char *expected_text, json_boolean_t *result);

/**
 * @brief Копирует raw text токена в отдельную NUL-terminated строку.
 * @param document Успешно разобранный document.
 * @param token_index Индекс копируемого токена.
 * @param text Output-указатель на allocation, освобождаемую json_memory_free().
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм проверяет индекс, выделяет `token_length + 1` байт, копирует raw
 * диапазон токена и добавляет завершающий NUL. Для строк кавычки не копируются,
 * но JSON escape-последовательности остаются неизменными.
 */
json_status_t json_token_copy_text(const json_document_t *document, size_t token_index,
    char **text);

/**
 * @brief Преобразует JSON number primitive в `uint64_t` через output-параметр.
 * @param document Успешно разобранный document.
 * @param token_index Индекс токена типа `JSON_TOKEN_PRIMITIVE`.
 * @param value Output-беззнаковое 64-битное значение.
 * @return `JSON_STATUS_SUCCESS`, `JSON_STATUS_SYNTAX_ERROR` или error status.
 * @details
 * Алгоритм копирует raw primitive, применяет `strtoull()` с основанием десять и
 * принимает результат только при полном потреблении строки и отсутствии overflow.
 * Отрицательные, дробные и экспоненциальные JSON numbers отклоняются как
 * неподходящие для `uint64_t`.
 */
json_status_t json_token_to_u64(const json_document_t *document, size_t token_index,
    uint64_t *value);

/**
 * @brief Преобразует JSON number primitive в конечное значение `double`.
 * @param document Успешно разобранный document.
 * @param token_index Индекс токена типа `JSON_TOKEN_PRIMITIVE`.
 * @param value Output-конечное floating-point значение.
 * @return `JSON_STATUS_SUCCESS`, `JSON_STATUS_SYNTAX_ERROR` или error status.
 * @details
 * Алгоритм копирует raw primitive, применяет `strtod()` и требует полного
 * потребления текста, отсутствия range error и конечного результата. JSON
 * literals `true`, `false` и `null` не удовлетворяют этому контракту.
 */
json_status_t json_token_to_double(const json_document_t *document, size_t token_index,
    double *value);

/**
 * @brief Преобразует primitive literal `true` или `false` в json_boolean_t.
 * @param document Успешно разобранный document.
 * @param token_index Индекс токена типа `JSON_TOKEN_PRIMITIVE`.
 * @param value Output-значение `JSON_BOOLEAN_TRUE` или `JSON_BOOLEAN_FALSE`.
 * @return `JSON_STATUS_SUCCESS`, `JSON_STATUS_SYNTAX_ERROR` или error status.
 * @details
 * Алгоритм выполняет точные raw-text сравнения сначала с `true`, затем с `false`.
 * Number и `null` отклоняются без неявного приведения.
 */
json_status_t json_token_to_boolean(const json_document_t *document, size_t token_index,
    json_boolean_t *value);

/**
 * @brief Освобождает allocation, ранее возвращённый json-lib.
 * @param memory Nullable allocation, полученный через json_token_copy_text().
 * @return `JSON_STATUS_SUCCESS`.
 * @details
 * Алгоритм передаёт указатель `free()`. Нулевой указатель разрешён правилами C и
 * сохраняет единый status-returning ownership endpoint вместо void-return API.
 */
json_status_t json_memory_free(void *memory);

/**
 * @brief Открывает temporary stream для атомарной публикации JSON-файла.
 * @param target_path Конечный путь публикуемого файла.
 * @param writer Output-writer, который должен быть пустым или инициализированным.
 * @param error Необязательный буфер диагностики I/O ошибки.
 * @param error_capacity Ёмкость `error` в байтах.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм копирует конечный путь, формирует шаблон `<target>.tmp.XXXXXX`,
 * создаёт unique temporary file через `mkstemp()` и оборачивает его `FILE` stream.
 * Commit безопасен только если temporary file и target находятся в одном каталоге.
 */
json_status_t json_writer_open(const char *target_path, json_writer_t *writer,
    char *error, size_t error_capacity);

/**
 * @brief Записывает trusted raw JSON syntax fragment в temporary stream.
 * @param writer Открытый writer transaction.
 * @param text NUL-terminated фрагмент JSON syntax или предварительно проверенный literal.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм передаёт фрагмент `fputs()` без дополнительной валидации. Потребитель
 * отвечает за корректность punctuation и literals; для пользовательской строки
 * необходимо использовать json_writer_write_string().
 */
json_status_t json_writer_write_raw(json_writer_t *writer, const char *text);

/**
 * @brief Кодирует и записывает одну JSON string value с внешними кавычками.
 * @param writer Открытый writer transaction.
 * @param text Nullable NUL-terminated исходная строка; NULL кодируется как `""`.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм обрамляет значение кавычками, экранирует `"` и `\\`, а управляющие
 * байты U+0000..U+001F кодирует в форме `\\u00XX`. Неуправляющие байты копируются
 * без изменения; библиотека не выполняет Unicode normalization.
 */
json_status_t json_writer_write_string(json_writer_t *writer, const char *text);

/**
 * @brief Синхронизирует stream и атомарно публикует temporary JSON artifact.
 * @param writer Открытый writer transaction; после вызова будет пустым.
 * @param error Необязательный буфер диагностики I/O ошибки.
 * @param error_capacity Ёмкость `error` в байтах.
 * @return Именованный `json_status_t`.
 * @details
 * Алгоритм вызывает `fflush()`, `fsync()`, `fclose()` и затем `rename()`. Любая
 * ошибка приводит к освобождению transaction и удалению временного файла; при
 * успехе target-path заменяется единой filesystem операцией rename.
 */
json_status_t json_writer_commit(json_writer_t *writer, char *error,
    size_t error_capacity);

/**
 * @brief Прерывает transaction, закрывает stream и удаляет temporary file.
 * @param writer Открытый или частично инициализированный writer transaction.
 * @return `JSON_STATUS_SUCCESS`, `JSON_STATUS_IO_ERROR` либо
 * `JSON_STATUS_ARGUMENT_ERROR`.
 * @details
 * Алгоритм best-effort закрывает stream и unlink-ает temporary path, освобождает
 * пути и записывает нулевой aggregate. Ошибка close/unlink отражается как
 * `JSON_STATUS_IO_ERROR`, но не препятствует очистке ownership. Его следует
 * вызывать при любом ошибочном пути после успешного json_writer_open().
 */
json_status_t json_writer_abort(json_writer_t *writer);

#endif /* JSON_LIB_H */

#endif // BENCHMARK_FRAMEWORK_SINGLE_H

#endif // BIGNUM_SINGLE_H
