# Advent of Code 2023

This is a collection of solutions to the
[Advent of Code 2023](https://adventofcode.com/2023) programming problems
implemented in the C programming language. I have done my best to minimize time
complexity and running time.

Some of the more complex problems were implemented in a
[higher-level programming language](ref/) before being converted to C. The
equivalent code is included here as a reference.

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

## Summary

**Note:** The times below are expressed as orders of magnitude, not precise
figures. For example, a time of 0.00010 seconds indicates an average running
time between 0.00005 seconds and 0.00050 seconds.

| Day |                     Problem                     |                             Domain                              |        Result         |  Time  | Implementation                                                                                                                                                                                                 |
| :-: | :---------------------------------------------: | :-------------------------------------------------------------: | :-------------------: | :----: | :------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
|  1  |           [Trebuchet?!](src/day01b.c)           |                         Strings, Tries                          |          Sum          | 0.0001 | Primitive [trie](https://en.wikipedia.org/wiki/Trie)                                                                                                                                                           |
|  2  |         [Cube Conundrum](src/day02b.c)          |                             Strings                             |          Sum          | 0.0001 |                                                                                                                                                                                                                |
|  3  |           [Gear Ratios](src/day03b.c)           |                       Dynamic programming                       |          Sum          | 0.0001 | Sliding window technique                                                                                                                                                                                       |
|  4  |          [Scratchcards](src/day04b.c)           |                              Sets                               |          Sum          | 0.0001 |                                                                                                                                                                                                                |
|  5  | [If You Give A Seed A Fertilizer](src/day05b.c) |                  Functions, Sorting algorithms                  |        Minimum        | 0.0001 | Function composition, System of linear equations                                                                                                                                                               |
|  6  |           [Wait For It](src/day06b.c)           |                        Physics, Algebra                         |        Product        | 0.0001 | [Quadratic formula](https://en.wikipedia.org/wiki/Quadratic_formula)                                                                                                                                           |
|  7  |           [Camel Cards](src/day07b.c)           |          Statistics, Dictionaries, Sorting algorithms           |          Sum          | 0.0001 | Frequency map with mode-tracking                                                                                                                                                                               |
|  8  |        [Haunted Wasteland](src/day08b.c)        |            Graph theory, Number theory, Dictionaries            | Least Common Multiple | 0.0001 | [Euclidean algorithm](https://en.wikipedia.org/wiki/Euclidean_algorithm), map-based graph, base-36 encoding                                                                                                    |
|  9  |       [Mirage Maintenance](src/day09b.c)        |                       Numerical analysis                        |          Sum          | 0.0001 | [Lagrange polynomial](https://en.wikipedia.org/wiki/Lagrange_polynomial)                                                                                                                                       |
| 10  |            [Pipe Maze](src/day10a.c)            |         Geometry, Graph theory, Pathfinding algorithms          |         Area          | 0.0001 | [Flood-fill](https://en.wikipedia.org/wiki/Flood_fill), matrix-based graph                                                                                                                                     |
| 11  |        [Cosmic Expansion](src/day11b.c)         |                            Geometry                             |          Sum          | 0.0001 | [Taxicab geometry](https://en.wikipedia.org/wiki/Taxicab_geometry)                                                                                                                                             |
| 12  |           [Hot Springs](src/day12b.c)           |       Automata theory, Regular expressions, Dictionaries        |          Sum          |  0.01  | [Non-deterministic finite automaton](https://en.m.wikipedia.org/wiki/Nondeterministic_finite_automaton), iterable dictionary                                                                                   |
| 13  |       [Point of Incidence](src/day13b.c)        |                        Binary arithmetic                        |          Sum          | 0.0001 | Bit array, bit matrix                                                                                                                                                                                          |
| 14  |    [Parabolic Reflector Dish](src/day14b.c)     |                      Strings, Dictionaries                      |          Sum          | 0.0001 | Character matrix, cycle detection                                                                                                                                                                              |
| 15  |          [Lens Library](src/day15b.c)           |       Cryptography, Hash functions, Dictionaries, Strings       |          Sum          | 0.0001 | Iterable ordered dictionary, Length-prefixed string                                                                                                                                                            |
| 16  |     [The Floor Will Be Lava](src/day16b.c)      |                   Stacks, Sets, Dictionaries                    |        Maximum        |  0.01  | Array stack, array set, array map, character matrix                                                                                                                                                            |
| 17  |         [Clumsy Crucible](src/day17b.c)         |      Graph theory, Pathfinding algorithms, Priority queues      |        Minimum        |  0.01  | [Dijkstra\'s algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm), [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer), state matrix                                                  |
| 18  |         [Lavaduct Lagoon](src/day18b.c)         |                            Geometry                             |         Area          | 0.0001 | [Shoelace formula](https://en.wikipedia.org/wiki/Shoelace_formula), [Pick\'s theorem](https://en.wikipedia.org/wiki/Pick%27s_theorem)                                                                          |
| 19  |             [Aplenty](src/day19b.c)             | Parsing algorithms, Recursion, Stacks, Functions, Dictionaries  |         Count         | 0.0001 | [Recursive descent parser](https://en.wikipedia.org/wiki/Recursive_descent_parser), dictionary                                                                                                                 |
| 20  |        [Pulse Propagation](src/day20b.c)        | Graph theory, Number theory, Dictionaries, Sets, Circuit design |        Product        |  0.01  | [Euclidean algorithm](https://en.wikipedia.org/wiki/Euclidean_algorithm), map-based graph, hash set, length-prefixed string                                                                                    |
| 21  |          [Step Counter](src/day21a.c)           |               Graph theory, Sets, Priority queues               |         Count         | 0.001  | [Breadth-first search](https://en.wikipedia.org/wiki/Breadth-first_search), hash set, [circular buffer](https://en.wikipedia.org/wiki/Circular_buffer), matrix-based graph                                     |
| 22  |           [Sand Slabs](src/day22b.c)            |    Geometry, Graph theory, Sorting algorithms, Stacks, Sets     |         Count         |  0.01  | Object-based graph, hash set                                                                                                                                                                                   |
| 24  |     [Never Tell Me The Odds](src/day24b.c)      |                     Physics, Linear algebra                     |          Sum          |  0.01  | [Line–line intersection](https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection), [Gaussian elimination](https://en.wikipedia.org/wiki/Gaussian_elimination)                                             |
| 25  |           [Snowverload](src/day25z.c)           |                          Graph theory                           |        Product        | 0.001  | [Max-flow min-cut theorem](https://en.wikipedia.org/wiki/Max-flow_min-cut_theorem), [Ford–Fulkerson algorithm](https://en.wikipedia.org/wiki/Ford%E2%80%93Fulkerson_algorithm), adjacency list, iterable graph |

## Constraints

I am working within the following constraints to ensure high code quality.

- Adhere to the [project style guide](cstyle.md).
- Final solutions must be implemented in the C programming language following
  the C99 standard.
  - Assume signed integer overflow is defined based on two\'s complement.
- All solutions must be standalone files with no user-defined headers and no
  external dependencies beyond the C standard library (`libc`) and the C
  mathematics library (`libm`). The first and second problems for a given day
  must be implemented separately.
- Bounds checking is not required for data structures whose capacity is defined
  by a macro.
- The return values of all C standard library functions must be checked, except
  for those returned from the following:
  - `fprintf`
  - `memcpy`
  - `printf`

## Omissions

- The solution must produce the correct result for the [examples](data/), not
  just the personalized puzzle input. This means that the efficient solutions to
  Day 10(b) and Day 21(b) are unfeasible. I have omitted both.
- By design, there is no efficient solution to Day 23(a) and (b). I have
  omitted both.
