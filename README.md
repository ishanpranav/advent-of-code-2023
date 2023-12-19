# Advent of Code 2023

This is a collection of solutions to the
[Advent of Code 2023](https://adventofcode.com/2023) programming problems
implemented in the C programming language. I have done my best to minimize time
complexity while keeping the average running time on the order of 0.0001 s
(0.1 ms).

The exceptions are [day 12(a)](src/day12a.c), which runs on the order of
0.001 s (1 ms), and [day 12(b)](src/day12b.c), which runs on the order of 0.01 s
(10 ms). Despite my best efforts, the order of magnitude of these run times
cannot be improved.

Some of the more difficult problems were implemented in a higher-level
programming language before being converted to C. The equivalent code is
included here as a reference.

## Usage

These programs are cross-platform, having been tested on Windows and Linux
operating systems.

**Important:** Do not use Windows-style line endings (`CR LF`, i.e. `\r\n`).
Only use Unix-style line endings (`LF`, i.e. `\n`).

**Important:** I have tested each program on the example test cases and two
puzzle input files - not a large sample size! I have avoided assumptions about
the input, sometimes even at the cost of performance. However, to avoid memory
allocation, all buffers have fixed sizes. Ensure that all buffers (see `#define`
statements) have sufficient capacity before running. Not doing so could result
in a stack smashing, segmentation fault, or worse.

### GCC

```sh
gcc src/day01a.c -o day01a -std=c99 -O2 -lm -Wall -Wextra -pedantic

./day01a in.txt
```

### LLVM

```sh
clang src/day01a.c -o day01a -std=c99 -O2 -lm -Wall -Wextra -Wno-deprecated-declarations -pedantic

day01a in.txt
```

## Summary

|Day|Problem|Domain|Result|Implementation|
|:-:|:-----:|:----:|:----:|:-------|
| 1 |[Trebuchet?!](src/day01b.c)|Strings, Tries|Sum|Primitive [trie](https://en.wikipedia.org/wiki/Trie)|
| 2 |[Cube Conundrum](src/day02b.c)|Strings|Sum||
| 3 |[Gear Ratios](src/day03b.c)|Dynamic programming|Sum|Sliding window technique|
| 4 |[Scratchcards](src/day04b.c)|Hash sets|Sum||
| 5 |[If You Give A Seed A Fertilizer](src/day05b.c)|Functions, Sorting algorithms|Minimum|Function composition, System of linear equations|
| 6 |[Wait For It](src/day06b.c)|Physics, Algebra|Product|[Quadratic formula](https://en.wikipedia.org/wiki/Quadratic_formula)|
| 7 |[Camel Cards](src/day07b.c)|Statistics, Dictionaries, Sorting algorithms|Sum|Frequency map with mode-tracking|
| 8 |[Haunted Wasteland](src/day08b.c)|Graph theory, Number theory, Dictionaries|Least Common Multiple|[Euclidean algorithm](https://en.wikipedia.org/wiki/Euclidean_algorithm), map-based graph|
| 9 |[Mirage Maintenance](src/day09b.c)|Numerical analysis|Sum|[Lagrange polynomial](https://en.wikipedia.org/wiki/Lagrange_polynomial)|
| 10|[Pipe Maze](src/day10b.c)|Geometry, Graph theory, Pathfinding algorithms|Count|[Flood-fill](https://en.wikipedia.org/wiki/Flood_fill), [Shoelace formula](https://en.wikipedia.org/wiki/Shoelace_formula), [Pick\'s theorem](https://en.wikipedia.org/wiki/Pick%27s_theorem), matrix-based graph|
| 11|[Cosmic Expansion](src/day11b.c)|Geometry|Sum|[Taxicab geometry](https://en.wikipedia.org/wiki/Taxicab_geometry)|
| 12|[Hot Springs](src/day12b.c)|Automata theory, Regular expressions, Dictionaries|Sum|[Non-deterministic finite automaton](https://en.m.wikipedia.org/wiki/Nondeterministic_finite_automaton), iterable dictionary|
| 13|[Point of Incidence](src/day13b.c)|Binary arithmetic|Sum|Bit vector, Bit matrix|

## Constraints

I am adhering to the following constraints to ensure high code quality.

- Adhere to the [project style guide](cstyle.md).
- Final solutions must be implemented in the C programming language following 
the C99 standard.
- All solutions must be standalone files with no user-defined headers and no 
external dependencies beyond the C standard library (`libc`) and the C 
mathematics library (`libm`). The first and second problems for a given day must 
be implemented separately.
- Dynamic memory allocation is prohibited (i.e., no calls to `malloc`, `calloc`,
`realloc`, `aligned_alloc`, or `free`).
- Bounds checking is not required for data structures whose capacity is defined
by a macro.
- The first and second problems for a given day may share the same data
structures and algorithms, but the second problem may only add to (and never
remove from) the public API of the first.
- The return values of all C standard library functions must be checked, except
for those returned from the following:
  - `fclose` (when the mode is `"r"`)
  - `fprintf`
  - `printf`
- Only the `main` entry point can do the following:
  - Write to the standard output stream (i.e., call `printf`)
  - Write to the standard error stream (i.e., call `fprintf` with `stderr` as
  the first argument)
  - Terminate the program
