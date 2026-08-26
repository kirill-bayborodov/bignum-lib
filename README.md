# bignum-lib

[![C/ASM CI](https://github.com/kirill-bayborodov/bignum-lib/actions/workflows/ci.yml/badge.svg)](https://github.com/kirill-bayborodov/bignum-lib/actions/workflows/ci.yml)
[![Latest release](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-lib?label=release)](https://github.com/kirill-bayborodov/bignum-lib/releases/latest)

`bignum-lib` is a unified static-library distribution for the completed `bignum-*` family. It aggregates independently released C11 reference and x86-64 YASM modules behind one generated public header, `dist/bignum.h`, and one archive, `dist/libbignum.a`.

## Scope and invariants

The repository is an aggregator, not a replacement for the individual module repositories. Each operation remains owned by its upstream repository and is included here as a pinned Git submodule. The aggregate preserves the fixed-capacity unsigned `bignum_t` representation, little-endian 64-bit words, normalized lengths, caller-owned storage, and the transactional contracts documented by each operation module.

The build uses the existing repository Makefile and CI workflow without modification. The root `libs/bignum-common` checkout is retained as infrastructure because the protected Makefile requires its base declarations first when constructing the single-file header. It is not presented as an operation in the module inventory. The template repository and the `bignum-sub-*` operation repositories are not included.

## Included operation modules

The table below lists every completed operation submodule included in the unified distribution. Each description is condensed from the corresponding upstream README, and each module name links to its source repository [1]. The pinned revision is the tag or commit recorded by the root `.gitmodules` file.

| Module | Repository path | Short description from upstream README | Pinned revision |
|---|---|---|---|
| [`bignum-abs`](https://github.com/kirill-bayborodov/bignum-abs) | `libs/bignum-abs` | Canonicalizes the absolute value of an unsigned `bignum_t`. | `v0.1.0` |
| [`bignum-add-bignum`](https://github.com/kirill-bayborodov/bignum-add-bignum) | `libs/bignum-add-bignum` | Adds two arbitrary-precision integers. | `v1.1.5` |
| [`bignum-add-u64`](https://github.com/kirill-bayborodov/bignum-add-u64) | `libs/bignum-add-u64` | Adds a `uint64_t` to an arbitrary-precision integer. | `v1.1.5` |
| [`bignum-and`](https://github.com/kirill-bayborodov/bignum-and) | `libs/bignum-and` | Computes bitwise `result = a & b` for normalized values. | `8bd8fc4` |
| [`bignum-bit-clear`](https://github.com/kirill-bayborodov/bignum-bit-clear) | `libs/bignum-bit-clear` | Clears one logical bit in a fixed-capacity value. | `v0.1.0` |
| [`bignum-bit-set`](https://github.com/kirill-bayborodov/bignum-bit-set) | `libs/bignum-bit-set` | Assigns one logical bit while preserving all other bits. | `v0.1.1` |
| [`bignum-bit-test`](https://github.com/kirill-bayborodov/bignum-bit-test) | `libs/bignum-bit-test` | Queries one logical bit without modifying the operand. | `v1.0.1` |
| [`bignum-cmp`](https://github.com/kirill-bayborodov/bignum-cmp) | `libs/bignum-cmp` | Performs a logical comparison of arbitrary-precision integers. | `v1.1.5` |
| [`bignum-copy`](https://github.com/kirill-bayborodov/bignum-copy) | `libs/bignum-copy` | Copies a normalized `bignum_t` value between caller-owned objects. | `v0.0.0` |
| [`bignum-core`](https://github.com/kirill-bayborodov/bignum-core) | `libs/bignum-core` | Provides the low-level arbitrary-precision unsigned integer type. | `v0.0.1` |
| [`bignum-crt`](https://github.com/kirill-bayborodov/bignum-crt) | `libs/bignum-crt` | Reconstructs a value from two pairwise-coprime congruences. | `v0.1.1-1-g6edbcde` |
| [`bignum-div-bignum`](https://github.com/kirill-bayborodov/bignum-div-bignum) | `libs/bignum-div-bignum` | Divides one arbitrary-precision integer by another. | `v1.1.5` |
| [`bignum-div-u64`](https://github.com/kirill-bayborodov/bignum-div-u64) | `libs/bignum-div-u64` | Divides an arbitrary-precision integer by `uint64_t`. | `v1.1.5` |
| [`bignum-fast-div`](https://github.com/kirill-bayborodov/bignum-fast-div) | `libs/bignum-fast-div` | Performs fixed-capacity division by a normalized non-zero divisor. | `v0.1.0-1-g186f9bf` |
| [`bignum-from-hex`](https://github.com/kirill-bayborodov/bignum-from-hex) | `libs/bignum-from-hex` | Parses a NUL-terminated hexadecimal string into `bignum_t`. | `v0.1.1-1-gb164dc4` |
| [`bignum-gcd`](https://github.com/kirill-bayborodov/bignum-gcd) | `libs/bignum-gcd` | Computes the greatest common divisor using Stein's algorithm. | `v1.0.0` |
| [`bignum-inverse`](https://github.com/kirill-bayborodov/bignum-inverse) | `libs/bignum-inverse` | Computes a modular inverse with an optimized assembly path and fallback. | `v0.2.3` |
| [`bignum-is-prime`](https://github.com/kirill-bayborodov/bignum-is-prime) | `libs/bignum-is-prime` | Tests probable primality using Miller–Rabin. | `v0.0.2-1-g81464f2` |
| [`bignum-isqrt`](https://github.com/kirill-bayborodov/bignum-isqrt) | `libs/bignum-isqrt` | Computes the integer square root of a non-negative value. | `v1.0.0` |
| [`bignum-jacobi`](https://github.com/kirill-bayborodov/bignum-jacobi) | `libs/bignum-jacobi` | Computes the Jacobi symbol for bounded operands. | `v0.0.2` |
| [`bignum-karatsuba-mul`](https://github.com/kirill-bayborodov/bignum-karatsuba-mul) | `libs/bignum-karatsuba-mul` | Multiplies two unsigned values using a Karatsuba-capable implementation. | `v0.1.0` |
| [`bignum-mod`](https://github.com/kirill-bayborodov/bignum-mod) | `libs/bignum-mod` | Computes an exact unsigned fixed-capacity remainder. | `v0.1.1` |
| [`bignum-mod-exp`](https://github.com/kirill-bayborodov/bignum-mod-exp) | `libs/bignum-mod-exp` | Computes `base^exp mod modulus`. | `v2.0.0` |
| [`bignum-mod-inv`](https://github.com/kirill-bayborodov/bignum-mod-inv) | `libs/bignum-mod-inv` | Computes a modular inverse with optimized assembly and fallback. | `v1.0.0` |
| [`bignum-mod-sqrt`](https://github.com/kirill-bayborodov/bignum-mod-sqrt) | `libs/bignum-mod-sqrt` | Computes a modular square root for bounded values. | `v0.1.0` |
| [`bignum-montgomery-mul`](https://github.com/kirill-bayborodov/bignum-montgomery-mul) | `libs/bignum-montgomery-mul` | Performs Montgomery multiplication of fixed-capacity values. | `v0.1.0` |
| [`bignum-mul-bignum`](https://github.com/kirill-bayborodov/bignum-mul-bignum) | `libs/bignum-mul-bignum` | Multiplies two arbitrary-precision integers. | `v1.1.5` |
| [`bignum-mul-pow10`](https://github.com/kirill-bayborodov/bignum-mul-pow10) | `libs/bignum-mul-pow10` | Multiplies a value in place by `10^k`. | `v0.1.1` |
| [`bignum-mul-pow5`](https://github.com/kirill-bayborodov/bignum-mul-pow5) | `libs/bignum-mul-pow5` | Multiplies a value in place by `5^k`. | `v0.1.1` |
| [`bignum-mul-u64`](https://github.com/kirill-bayborodov/bignum-mul-u64) | `libs/bignum-mul-u64` | Multiplies a normalized value by a `uint64_t`. | `v1.1.5` |
| [`bignum-not`](https://github.com/kirill-bayborodov/bignum-not) | `libs/bignum-not` | Computes bitwise `result = ~input`. | `v1.0.1` |
| [`bignum-or`](https://github.com/kirill-bayborodov/bignum-or) | `libs/bignum-or` | Computes bitwise `result = a \| b`. | `v1.0.0` |
| [`bignum-pow`](https://github.com/kirill-bayborodov/bignum-pow) | `libs/bignum-pow` | Computes exact non-modular exponentiation. | `v0.1.0` |
| [`bignum-prime-gen`](https://github.com/kirill-bayborodov/bignum-prime-gen) | `libs/bignum-prime-gen` | Generates probable primes with an exact requested bit length. | `v0.0.0` |
| [`bignum-random`](https://github.com/kirill-bayborodov/bignum-random) | `libs/bignum-random` | Samples a cryptographically random value from `[0, n)`. | `v0.1.2` |
| [`bignum-shift-left`](https://github.com/kirill-bayborodov/bignum-shift-left) | `libs/bignum-shift-left` | Performs a logical left shift. | `v1.1.5` |
| [`bignum-shift-right`](https://github.com/kirill-bayborodov/bignum-shift-right) | `libs/bignum-shift-right` | Performs a logical right shift. | `v1.1.5` |
| [`bignum-swap`](https://github.com/kirill-bayborodov/bignum-swap) | `libs/bignum-swap` | Exchanges two normalized values in place. | `v0.0.0` |
| [`bignum-to-hex`](https://github.com/kirill-bayborodov/bignum-to-hex) | `libs/bignum-to-hex` | Serializes a value to canonical lowercase hexadecimal. | `v0.1.1` |
| [`bignum-to-string`](https://github.com/kirill-bayborodov/bignum-to-string) | `libs/bignum-to-string` | Converts a magnitude to a base-2, 8, 10, or 16 string. | `v0.0.0` |
| [`bignum-xor`](https://github.com/kirill-bayborodov/bignum-xor) | `libs/bignum-xor` | Computes bitwise `result = a ^ b`. | `v1.0.0` |

## Compatibility-preserved modules

These completed utility modules are kept under `vendor/compat/` as pinned source submodules. They are documented here for completeness but are not linked into the aggregate because their status-returning APIs conflict with the legacy common-base declarations when both are concatenated by the protected Makefile.

| Module | Repository path | Short description from upstream README | Pinned revision |
|---|---|---|---|
| [`bignum-clear-tail`](https://github.com/kirill-bayborodov/bignum-clear-tail) | `vendor/compat/bignum-clear-tail` | Clears the unused tail of a fixed-capacity value. | `v0.0.0` |
| [`bignum-init`](https://github.com/kirill-bayborodov/bignum-init) | `vendor/compat/bignum-init` | Initializes an arbitrary-precision integer value. | `v0.0.0` |
| [`bignum-init-from-array`](https://github.com/kirill-bayborodov/bignum-init-from-array) | `vendor/compat/bignum-init-from-array` | Initializes a value from an array of 64-bit words. | `v0.0.0` |
| [`bignum-init-from-string`](https://github.com/kirill-bayborodov/bignum-init-from-string) | `vendor/compat/bignum-init-from-string` | Initializes a value from a textual non-negative integer. | `v0.0.0` |
| [`bignum-init-pow10`](https://github.com/kirill-bayborodov/bignum-init-pow10) | `vendor/compat/bignum-init-pow10` | Initializes a normalized value with `10^k`. | `v0.0.0` |
| [`bignum-init-u64`](https://github.com/kirill-bayborodov/bignum-init-u64) | `vendor/compat/bignum-init-u64` | Initializes a value from a 64-bit unsigned integer. | `v0.0.1` |
| [`bignum-is-zero`](https://github.com/kirill-bayborodov/bignum-is-zero) | `vendor/compat/bignum-is-zero` | Checks whether a value represents zero. | `v0.0.0` |
| [`bignum-normalize`](https://github.com/kirill-bayborodov/bignum-normalize) | `vendor/compat/bignum-normalize` | Normalizes a value and clears its unused words. | `v0.0.0` |


## Public interface

The generated aggregate header is available as `dist/bignum.h` after a build. It contains the base type and the public declarations exported by the linked operation modules. Individual module headers remain available in their respective submodules for consumers who need a narrower dependency surface.

Every operation module owns its own status type and contract. Callers should inspect the operation-specific header or generated declaration for validation rules, aliasing restrictions, overflow behavior, and whether an error leaves the destination unchanged.

## Prerequisites

A Linux x86-64 environment with `git`, `make`, GCC, YASM, GNU binutils, and POSIX shell tools is required. The build is designed for System V AMD64 ABI targets. The benchmark framework distribution is vendored at `libs/benchmark-framework/dist/` for reproducible local builds; CI may refresh that distribution according to the protected workflow.

## Clone and initialize

Clone the repository and initialize all nested dependencies:

```bash
git clone https://github.com/kirill-bayborodov/bignum-lib.git
cd bignum-lib
git submodule update --init --recursive --jobs 8
```

The recursive checkout is required because operation repositories retain their own core or common dependencies and benchmark distributions.

## Build and distribution

Build all source submodules in release configuration:

```bash
make build CONFIG=release
```

Create the unified distribution, generate `dist/bignum.h`, archive the linked object files into `dist/libbignum.a`, and compile/link the aggregate integration runner:

```bash
make dist CONFIG=release
```

A successful distribution build ends with `Distribution created successfully in dist/`. The release directory contains the single header, static archive, integration runner source, and distribution README/license files.

## Integration test

The integration runner is compiled against the generated single header and linked against the aggregate archive by `make dist`. It checks the core representation and exercises representative retained operations, including addition, multiplication, division, shifts, bit operations, logical operations, copying, hexadecimal parsing, and multiplication by powers of 5 and 10.

Run the standalone runner after a successful distribution build with:

```bash
./dist/test_bignum_runner
```

The runner must report `All integration tests passed!`. Module-specific tests remain in the upstream submodules and are executed by their own workflows.

## C11 and YASM boundary

Where a module provides both implementations, C11 is the portable reference used for correctness, coverage, and baseline measurements. The selected production symbol may be implemented in YASM, with the same public signature and externally visible contract. The aggregate does not merge implementation internals or require C11 and YASM to use identical algorithms.

## Reproducibility and review

Root `.gitmodules` records every aggregate dependency. A release review must verify that all gitlinks resolve, excluded `bignum-sub-*` operation modules and `bignum-template` are absent from `libs/`, generated header and archive are reproducible, and the integration runner passes. The review must also confirm that no CI workflow or root Makefile was modified.

## Protected files and proposed improvements

The root `Makefile` and `.github/workflows/ci.yml` are protected project inputs and are not edited by this repository update. The current Makefile assumes one base header and a homogeneous generated-header layout; the compatibility modules under `vendor/compat/` document the duplicate-API limitation exposed by that assumption. A future maintenance change could make module selection explicit and distinguish operation headers from benchmark-only bridge headers, but that change must be proposed and reviewed separately before touching the protected files.

## License and contribution

Each submodule retains its upstream license and contribution policy. Changes to an operation belong in that operation's repository. Aggregator changes should be limited to dependency pins, integration coverage, generated-distribution documentation, and release metadata. Pull requests should include the exact build command, integration-test output, affected submodule list, and confirmation that protected files remain unchanged.
