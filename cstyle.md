# C language style guide

## Indentation

- Use spaces instead of tabs.
- The indentation size is four spaces.
- Indent `case` labels.
- Do not put multiple statements on a single line.
- Do not put multiple assignments on a single line.
- Do not leave whitespace at the end of lines.

## Wrapping

- The line limit is 80 characters.
- Split long strings to meet the character limit.

## Bracing

- Every brace (`{`) or brace-semicolon pair (`};`) should have its own line,
  except during empty initialization or during array initialization when all array
  items are placed on a single line.
- Always use braces with control structures, even when optional.
- Separate functions and control structures by exactly one blank line.

## Spacing

Defer to the Linux Kernel Coding Style
[Section 3.1: Spacing](https://www.kernel.org/doc/html/v4.10/process/coding-style.html)
for spacing considerations.

## Naming

- Use `UPPER_SNAKE_CASE` for global constants, constant macros, and enumeration
  members.
- Use `PascalCase` for struct names, enumeration names, and type definitions.
- Use `lower_snake_case` for global functions.
- Use `lowercase` for static functions.
- Use `camelCase` for local variables, struct fields, and function arguments.

Always use descriptive names. Abbreviated names are forbidden except in the
following approved use cases. If a value is arbitrary, use the corresponding
abbreviation based on its type. If several values are abitrary, begin with the
corresponding type-based abbreviation and continue alphabetically. If doing so
is impractical, use numeric suffixes instead.

| Letter | Appropriate use                                                                              |
| :----: | :------------------------------------------------------------------------------------------- |
|  `a`   | arbitrary object, area                                                                       |
|  `b`   | blue color component, boundary points in Pick\'s Theorem                                     |
|  `e`   | arbitrary edge                                                                               |
|  `dx`  | change in an arbitrary dimension                                                             |
|  `g`   | green color component, arbitrary graph                                                       |
|  `hi`  | larger pointer, point above, upper buffer                                                    |
|  `i`   | abitrary counter, matrix index in an arbitrary dimension, interior points in Pick\'s Theorem |
|  `k`   | term number, arbitrary constant                                                              |
|  `l`   | arbitrary line                                                                               |
|  `lo`  | smaller pointer, point below, lower buffer                                                   |
|  `m`   | matrix size in an arbitrary dimension                                                        |
| `max`  | inclusive maximum, exclusive interval upper bound                                            |
| `mid`  | middle pointer, middle buffer                                                                |
| `min`  | inclusive minimum, inclusive interval lower bound                                            |
|  `n`   | count, arbitrary integer                                                                     |
|  `p`   | arbitrary pointer, arbitrary point                                                           |
|  `q`   | arbitrary rational number                                                                    |
|  `r`   | red color component, arbitrary real number                                                   |
|  `u`   | arbitrary vertex                                                                             |
|  `x`   | vector in an arbitrary dimension, position in an arbitrary dimension                         |

## Type definitions

- Always use type definitions for enumeration types.
- Always use type definitions to obscure pointers to structs.

### Macros

- Only use macros to define constants.
- When multiple related integer constants are defined, use an enumeration
  instead.
