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
- Do not split long strings to meet the character limit.

## Bracing

- Every brace (`{`) or brace-semicolon pair (`};`) should have its own line,
except during empty initialization or during array initialization when all array
items are placed on a single line.
- Always use braces with control structures, even when optional.
- Separate functions and controls structures by exactly one blank line.

## Spacing

Defer to the Linux Kernel Coding Style
[Section 3.1: Spacing](https://www.kernel.org/doc/html/v4.10/process/coding-style.html) for spacing considerations.

## Naming

- Use `UPPER_SNAKE_CASE` for global constants, constant macros, and enumeration
members.
- Use `PascalCase` for struct names, enumeration names, and type definitions.
- Use `lower_snake_case` for global functions.
- Use `lowercase` for static functions.
- Use `camelCase` for local variables, struct fields, and function arguments.

Always use descriptive names. Abbreviated names are forbidden except for the
following reserved use cases. If a value is arbitrary, use the corresponding
abbreviation based on its type. If several values are abitrary, begin with the
corresponding type-based abbreviation and continue alphabetically. If doing so
is impractical, use numeric suffixes instead.

|Letter|Approved uses|
|:----:|-------------|
|`a`   |arbitrary object|
|`b`   |blue color component|
|`d`   |change|
|`g`   |green color component|
|`hi`  |larger pointer, point above, upper buffer|
|`i`   |abitrary counter, matrix index in an arbitrary dimension|
|`k`   |term number|
|`m`   |matrix size in an arbitrary dimension|
|`n`   |count|
|`p`   |arbitrary pointer|
|`r`   |red color component|
|`x`   |position, vector in an arbitrary dimension, position in an arbitrary dimension|
|`lo`  |smaller pointer, point below, lower buffer|

## Type definitions

- Always use type definitions for enumeration types.
- Use type definitions to obscure pointers to structs with pass-by-reference
semantics (classes).
- Never use type definitions for structs with pass-by-value semantics, not even
to obscure pointers.

### Reference semantics

In the example below, the `Person` class is a complex structure with
pass-by-reference semantics. A type defintion is required.

```c
#include <stdbool.h>
#include <stdlib.h>
#define REFERENCES_CAPACITY 10

struct Person
{
    char* name;
    struct Person* references[REFERENCES_CAPACITY];
    int referenceCount;
    int graduationYear;
    float gpa;
    bool hasReferral;
};

typedef struct Person* Person;

void person(Person instance)
{
    instance->name = NULL;
    instance->referenceCount = 0;
    instance->graduationYear = 2026;
    instance->gpa = 4;
    instance->hasReferral = false;
}

bool person_try_add_reference(Person instance, Person reference)
{
    int count = instance->referenceCount;

    if (count == REFERENCES_CAPACITY)
    {
        return false;
    }

    instance->references[count] = reference;
    instance->referenceCount = count + 1;

    return true;
}
```

### Value semantics

In the example below, the `Suit` enumeration is an enumeration with
pass-by-value semantics. A type definition should be used because copying an
enumeration member is not more expensive than copying a pointer.

```c
enum Suit
{
    SUIT_SPADES,
    SUIT_DIAMONDS,
    SUIT_CLUBS,
    SUIT_HEARTS
};
```

In this example, the `Point` struct is a simple structure with pass-by-value
semantics. A type definition should not be used because it obscures the true
intent of the operations. Also, copying a structure is more expensive than
copying a pointer.

```c
struct Point
{
    double x;
    double y;
};

void point_add(struct Point a, struct Point b, struct Point* result)
{
    result->x = a.x + b.x;
    result->y = a.y + b.y;
}
```

### Macros

- Only use macros to define constants.
- When multiple related integer constants are defined, use an enumeration
instead.
