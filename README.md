# samesies

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue?logo=c%2B%2B)
![CMake](https://img.shields.io/badge/build-CMake-064F8C?logo=cmake)
[![CI](https://github.com/pandalinoox/samesies/actions/workflows/ci.yml/badge.svg)](https://github.com/pandalinoox/samesies/actions/workflows/ci.yml)
[![Release](https://img.shields.io/github/v/release/pandalinoox/samesies)](https://github.com/pandalinoox/samesies/releases)

A small command-line tool that finds duplicate files in a directory.
Built as a learning project to get comfortable with C++.

## Build

Requires CMake 3.28+ and a C++20 compiler.

```sh
cmake -S . -B build
cmake --build build
```

Tests use [Criterion](https://github.com/Snaipe/Criterion):

```sh
sudo apt install libcriterion-dev pkg-config
ctest --test-dir build --output-on-failure
```

Skip tests with `-DSAMESIES_BUILD_TESTS=OFF`.

## Usage

```sh
samesies [DIRECTORY] [OPTIONS]
```

| Option         | Description                                |
| -------------- | ------------------------------------------ |
| `-d`           | Delete duplicates (asks which file to remove per group) |
| `-h`, `--help` | Show help                                  |

```sh
samesies              # scan current directory
samesies ./files      # scan ./files
samesies ./files -d   # scan and delete interactively
```

## How it works

1. Group files by size.
2. Hash same-size files in 4 KiB chunks with FNV-1a, dropping files that diverge early.
3. Report the files that survive as duplicates.

> FNV-1a is a 64-bit non-cryptographic hash, so a collision could in theory report two different files as duplicates. Fine for learning, check before deleting anything important.
