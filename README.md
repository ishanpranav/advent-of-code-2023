# Advent of Code 2023

This is a collection of solutions to the [Advent of Code 2023](https://adventofcode.com/2023) programming problems implemented in the C programming languages. I have done my best to minimize time complexity while keeping the average running time on the order of 1 ms.

Some of the more difficult problems were implemented in a higher-level programming language before being converted to C. The equivalent code is included here as a reference.

## Constraints

I am adhering to the following constraints to ensure high code quality.

- Final solutions must be implemented in the C programming language following the C99 standard
- All solutions must be standalone files with no user-defined headers and no external dependencies beyond the C standard library (`libc`) and the C mathematics library (`libm`). The first and second problems for a given day must be implemented separately.
- Dynamic memory allocation is prohibited (i.e., no calls to `malloc`, `calloc`, `realloc`, `aligned_alloc`, or `free`).
- Bounds checking is not required for data structures whose capacity is defined by a macro.
- The first and second problems for a given day may share the same data structures and algorithms, but the second problem may only add to (and never remove from) the public API of the first.
- When calling a function from the C standard library, the return value must always be checked, except for the return values from the following functions:
  - `printf`
- Only the `main` entry point can do the following:
  - Write to the standard output stream (i.e., call `printf`)
  - Write to the standard error stream (i.e., call `fprintf` with `stderr` as the first argument)
  - Terminate the program

## Acknowledgements

Some of the most challenging problems in this repository were solved with help from Iris Lim. Others are based on the implementations in [this](https://github.com/Hudinid/AdventOfCode2023) repository by Brian Hu and [this](https://github.com/snoopboopsnoop/advent-of-code-2023) repository by Walter Cheng. Please see the relevant file headers for more details.
