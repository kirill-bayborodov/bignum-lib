# bignum-lib

[![C/ASM CI](https://github.com/kirill-bayborodov/bignum-lib/actions/workflows/ci.yml/badge.svg)](https://github.com/kirill-bayborodov/bignum-lib/actions/workflows/ci.yml)
[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-lib?label=release)](https://github.com/kirill-bayborodov/bignum-lib/releases/latest)


`bignum-lib` is a high-performance library for arbitrary-precision integer arithmetic, with core functions written in x86-64 assembly.

This is an aggregator project that combines several modules into a single, easy-to-use static library.

## Current Features 

*   Core data structure `bignum_t` and type definitions used by all other bignum modules.(`bignum_common`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-common?label=release)](https://github.com/kirill-bayborodov/bignum-common/releases/latest)
*   logical left shift (`bignum_shift_left`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-shift-left?label=release)](https://github.com/kirill-bayborodov/bignum-shift-left/releases/latest)
*   logical right shift (`bignum_shift_right`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-shift-right?label=release)](https://github.com/kirill-bayborodov/bignum-shift-right/releases/latest)
*   logical compare (`bignum_cmp`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-cmp?label=release)](https://github.com/kirill-bayborodov/bignum-cmp/releases/latest)
*   dividing a large number by uint64_t (`bignum_div_u64`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-div-u64?label=release)](https://github.com/kirill-bayborodov/bignum-div-u64/releases/latest)
*   multiplication of two large numbers (`bignum_mul_bignum`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-mul-bignum?label=release)](https://github.com/kirill-bayborodov/bignum-mul-bignum/releases/latest)
*   multiplication of bignum_t by uint64_t (`bignum_mul_u64`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-mul-u64?label=release)](https://github.com/kirill-bayborodov/bignum-mul-u64/releases/latest)
*   subtraction function for large integers (bignum) (`bignum_sub`).[![GitHub release (latest by date)](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-sub?label=release)](https://github.com/kirill-bayborodov/bignum-sub/releases/latest)

## Prerequisites

*   `git`
*   `make`
*   `gcc`
*   `yasm`

## How to Build and Use

**1. Clone the repository with submodules:**
```bash
git clone --recurse-submodules https://github.com/kirill-bayborodov/bignum-lib.git
cd bignum-lib
```

**2. Build the library:**
This command will compile all modules and create the static library `libbignum.a` in the `dist/` directory.
```bash
make build
```

**3. Install (Optional ):**
This will copy the library and all necessary public headers into the `dist/` directory, creating a self-contained package.
```bash
make install
```

**4. Link with your application:**
To use the library, you need to tell your compiler where to find the headers (`-I`) and the library itself (`-L` and `-l`).

```bash
# Assuming your project structure is:
# my_app/
# ├── main.c
# └── bignum-lib/  <-- our library folder

# Compile your application:
gcc main.c -I./bignum-lib/dist -L./bignum-lib/dist -lbignum -o my_app -no-pie
```

**Example `main.c`:**
```c
#include <stdio.h>
#include "bignum.h" // Include the main library header

int main() {
    bignum_t num = { .len = 1, .words = {123} };
    bignum_shift_left(&num, 2);
    printf("Shifted value: %llu\n", num.words[0]); // Should print 492
    return 0;
}
```
