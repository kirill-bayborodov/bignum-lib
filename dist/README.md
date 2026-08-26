# bignum-lib

[![C/ASM CI](https://github.com/kirill-bayborodov/bignum-lib/actions/workflows/ci.yml/badge.svg)](https://github.com/kirill-bayborodov/bignum-lib/actions/workflows/ci.yml)
[![Latest release](https://img.shields.io/github/v/release/kirill-bayborodov/bignum-lib?label=release)](https://github.com/kirill-bayborodov/bignum-lib/releases/latest)

`bignum-lib` is a unified static-library distribution for the completed `bignum-*` family. It aggregates independently released C11 reference and x86-64 YASM modules behind one generated public header, `dist/bignum.h`, and one archive, `dist/libbignum.a`.

## Scope and invariants

The repository is an aggregator, not a replacement for the individual module repositories. Each operation remains owned by its upstream repository and is included here as a pinned Git submodule. The aggregate preserves the fixed-capacity unsigned `bignum_t` representation, little-endian 64-bit words, normalized lengths, caller-owned storage, and the transactional contracts documented by each operation module.

The build uses the existing repository Makefile and CI workflow without modification. The root `libs/bignum-common` checkout is retained as infrastructure because the protected Makefile requires its base declarations first when constructing the single-file header. It is not presented as an operation in the module inventory. The template repository and the `bignum-sub-*` operation repositories are not included.

## Included operation modules

The following completed operation repositories are included under `libs/` and pinned to released commits where a release exists: `bignum-abs`, `bignum-add-bignum`, `bignum-add-u64`, `bignum-and`, `bignum-bit-clear`, `bignum-bit-set`, `bignum-bit-test`, `bignum-copy`, `bignum-core`, `bignum-crt`, `bignum-div-bignum`, `bignum-div-u64`, `bignum-fast-div`, `bignum-from-hex`, `bignum-gcd`, `bignum-inverse`, `bignum-is-prime`, `bignum-isqrt`, `bignum-jacobi`, `bignum-karatsuba-mul`, `bignum-mod`, `bignum-mod-exp`, `bignum-mod-inv`, `bignum-mod-sqrt`, `bignum-montgomery-mul`, `bignum-mul-pow10`, `bignum-mul-pow5`, `bignum-mul-u64`, `bignum-not`, `bignum-or`, `bignum-pow`, `bignum-prime-gen`, `bignum-random`, `bignum-shift-left`, `bignum-shift-right`, `bignum-swap`, `bignum-to-hex`, `bignum-to-string`, and `bignum-xor`.

The newer duplicate utility implementations (`bignum-clear-tail`, `bignum-init`, `bignum-init-from-array`, `bignum-init-from-string`, `bignum-init-pow10`, `bignum-init-u64`, `bignum-is-zero`, and `bignum-normalize`) are preserved under `vendor/compat/` as pinned source submodules for auditability, but are not linked into the aggregate. Their status-returning APIs conflict with the legacy common-base declarations when both are concatenated by the protected Makefile. This keeps all completed source repositories available while making the distributed ABI unambiguous.

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
