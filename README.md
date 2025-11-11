# bignum-lib

**Version: 0.2.0**

`bignum-lib` is a high-performance library for arbitrary-precision integer arithmetic, with core functions written in x86-64 assembly.

This is an aggregator project that combines several modules into a single, easy-to-use static library.

## Current Features (v0.2.0)

*   Core data structure `bignum_t`.
*   High-performance logical left shift (`bignum_shift_left`).
*   High-performance logical right shift (`bignum_shift_right`).

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
