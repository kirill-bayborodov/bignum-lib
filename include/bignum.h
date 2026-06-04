#ifndef BIGNUM_SINGLE_H
#define BIGNUM_SINGLE_H

/* --- Included from libs/bignum-common/include/bignum.h --- */
/**
 * @file    bignum.h
 * @author  git@bayborodov.com
 * @version 1.0.2
 * @date    03.10.2025
 *
 * @brief   Определение базовой структуры для арифметики с большими числами.
 *
 * @details
 *   Эта структура `bignum_t` является основой для всех высокоточных
 *   целочисленных вычислений в алгоритме форматирования `double`.
 *   Размер массива `words` (32 * 64 = 2048 бита) выбран для покрытия
 *   всех крайних случаев, возникающих при преобразовании `double`.
 *
 * @history
 *   - rev. 1 (01.08.2025): Первоначальное создание.
 *   - rev. 2 (03.10.2025): Дополнение для github
 */

#include <stdint.h>
#include <stddef.h>

#define BIGNUM_CAPACITY 32 // 32 * (8*8) =  2048 бит
#define BIGNUM_WORD_SIZE 8 // sizeof(uint64_t) байт

/**
 * @brief Структура для представления большого беззнакового целого числа.
 */
typedef struct {
    /** Массив 64-битных "слов" для хранения числа. */
    uint64_t words[BIGNUM_CAPACITY];
    /** Количество используемых слов. words[len-1] не может быть 0, кроме числа 0. */
    size_t len;
} bignum_t;

typedef enum {
    BIGNUM_SUCCESS = 0,
    BIGNUM_ERROR_NULL_ARG = -1,
    BIGNUM_ERROR_OVERFLOW = -2
} bignum_status_t;



/* --- Included from libs/bignum-cmp/include/bignum_cmp.h --- */
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
 *                         - Добавлена обработка NULL-аргументов (возврат INT_MIN).
 *                         - Добавлены макросы для семантической версии.
 *   - rev. 3 (20.11.2025): Removed version control functions.
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 */


#include <stddef.h>
#include <stdint.h>
#include <limits.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка
#ifndef BIGNUM_CAPACITY
#  error "bignum.h must define BIGNUM_CAPACITY"
#endif

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


/* --- Included from libs/bignum-div-u64/include/bignum_div_u64.h --- */
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


#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

// Проверка на наличие определения BIGNUM_CAPACITY из общего заголовка
#ifndef BIGNUM_CAPACITY
#  error "bignum.h must define BIGNUM_CAPACITY"
#endif

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


/* --- Included from libs/bignum-mul-bignum/include/bignum_mul_bignum.h --- */
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


/* --- Included from libs/bignum-mul-u64/include/bignum_mul_u64.h --- */
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
 * @brief Коды состояния для функции bignum_mul_u64.
 */
typedef enum {
    BIGNUM_MUL_U64_SUCCESS         =  0, /**< Успешное выполнение. */
    BIGNUM_MUL_U64_ERROR_NULL_ARG  = -1, /**< Ошибка: один из входных указателей равен NULL. */
    BIGNUM_MUL_U64_ERROR_OVERFLOW  = -2  /**< Ошибка: переполнение емкости. */
    /**
     * @brief Ошибка: переполнение емкости.
     * @details Сумма длин входных чисел (a->len + b->len) превышает
     *          емкость структуры bignum_t (BIGNUM_CAPACITY). Результат
     *          гарантированно не поместится.
     */
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


/* --- Included from libs/bignum-shift-left/include/bignum_shift_left.h --- */
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


/* --- Included from libs/bignum-shift-right/include/bignum_shift_right.h --- */
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


/* --- Included from libs/bignum-sub/include/bignum_sub.h --- */
/**
 * @file    bignum_sub.h
 * @author  git@bayborodov.com
 * @version 1.0.0
 * @date    28.11.2025
 *
 * @brief   Модуль для вычитания больших беззнаковых целых чисел.
 * @ingroup bignum_arithmetic
 *
 * @attention
 *   Проект предполагает, что пути к заголовочным файлам (таким как `bignum_t.h`)
 *   управляются через флаги компилятора (например, `-Iinclude`), поэтому
 *   используются прямые включения (`#include "bignum_t.h"`).
 *
 * @history
 *   - rev. 1 (05.08.2025): Первоначальное создание (v0.0.1).
 *   - rev. 2 (05.08.2025): Переход на API v0.0.2 с фиксированным буфером.
 *   - rev. 3 (05.08.2025): Первая попытка запрета перекрытия буферов (v0.0.3).
 *   - rev. 4 (05.08.2025): API переработан, добавлены именованные коды ошибок.
 *   - rev. 5 (05.08.2025): API финализирован, добавлены inline-функции и API отладки.
 *   - rev. 6 (05.08.2025): Доработка документации и контракта API.
 *   - rev. 7 (05.08.2025): Восстановлена полная история. Добавлен include <stddef.h>.
 *                         Финализирована документация Doxygen.
 *   - rev. 8 (06.08.2025): Переход к версии 0.0.4 с сильной декомпозиции эталонной функции
 *   - rev. 9 (07.08.2025): Переход к версии 0.0.5 с оптимизацией вычислительной функции
 *   - rev. 10(08.08.2025): Переход к версии 0.0.6 композитной ассемблерной с оптимизацией вычислительной функции
 *
 * @see     bignum.h
 * @since   1.0.0
 *
 */


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
 * @brief Коды состояния для функции bignum_sub.
 */

typedef enum {
    BIGNUM_SUB_SUCCESS = 0,                  /** Успешное выполнение. **/
    BIGNUM_SUB_ERROR_NULL_PTR = -1,          /** Один из входных указателей `NULL`. **/ 
    BIGNUM_SUB_ERROR_NEGATIVE_RESULT = -2,   /** Результат отрицательный (`a < b`). **/ 
    BIGNUM_SUB_ERROR_CAPACITY_EXCEEDED = -3, /** Длина операнда (a или b) превышает `BIGNUM_CAPACITY`. **/
    BIGNUM_SUB_ERROR_BUFFER_OVERLAP = -4     /** Обнаружено перекрытие буферов. **/
} bignum_sub_status_t;


/**
 * @brief Выполняет вычитание двух больших беззнаковых целых чисел.
 *
 * @details
 *   ### Алгоритм
 *   1.  Проверяются входные указатели `result`, `a`, `b` на `NULL`.
 *   2.  Проверяется, что длины операндов `a->len` и `b->len` не превышают `BIGNUM_CAPACITY`.
 *   3.  С помощью `bignum_sub_ranges_overlap` проверяется, что диапазоны памяти `result`, `a` и `b` не пересекаются.
 *      Размер буфера для `bignum_t` должен вычисляться как `sizeof(uint64_t) * BIGNUM_CAPACITY`.
 *   4.  Проверяется, что `a->len >= b->len`. Если это не так, `bignum_cmp` не вызывается.
 *   5.  С помощью `bignum_cmp` проверяется, что `a >= b`.
 *   6.  Выполняется пословное вычитание `a - b` с распространением заимствования.
 *   7.  Длина результата нормализуется (удаляются ведущие нули).
 *
 *   ### Потокобезопасность
 *   Функция является потокобезопасной, так как не использует глобальное или
 *   статическое состояние. Всю ответственность за синхронизацию доступа к
 *   одним и тем же объектам `bignum_t` из разных потоков несет вызывающий код.
 *
 * @param[out] result Указатель на структуру `bignum_t` для записи результата.
 * @param[in]  a      Указатель на `bignum_t`, представляющую уменьшаемое.
 * @param[in]  b      Указатель на `bignum_t`, представляющую вычитаемое.
 *
 * @return bignum_sub_status_t Код состояния операции.
 * @retval BIGNUM_SUB_SUCCESS Успешное выполнение.
 * @retval BIGNUM_SUB_ERROR_NULL_PTR Один из входных указателей `NULL`.
 * @retval BIGNUM_SUB_ERROR_NEGATIVE_RESULT Результат отрицательный (`a < b`).
 * @retval BIGNUM_SUB_ERROR_CAPACITY_EXCEEDED Длина операнда (a или b) превышает `BIGNUM_CAPACITY`.
 * @retval BIGNUM_SUB_ERROR_BUFFER_OVERLAP Обнаружено перекрытие буферов.
 */
bignum_sub_status_t bignum_sub(bignum_t *result, const bignum_t *a, const bignum_t *b);

#ifdef __cplusplus
}
#endif


#endif // BIGNUM_SINGLE_H
