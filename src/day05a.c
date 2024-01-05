// Licensed under the MIT License.

// If You Give A Seed A Fertilizer Part 1

#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256
#define DELIMITERS " "
#define FUNCTION_RANGES_CAPACITY 64
#define LIST_CAPACITY 32

struct Range
{
    long long destinationOffset;
    long long sourceOffset;
    long long length;
};

struct Function
{
    struct Range ranges[FUNCTION_RANGES_CAPACITY];
    int count;
};

struct List
{
    long long items[LIST_CAPACITY];
    int count;
};

typedef const void* Object;
typedef char* String;
typedef struct Range* Range;
typedef struct Function* Function;
typedef struct List* List;

int range_compare(Object left, Object right)
{
    if (!left && !right)
    {
        return 0;
    }

    if (!left)
    {
        return -1;
    }

    if (!right)
    {
        return 1;
    }

    long long leftOffset = ((const struct Range*)left)->sourceOffset;
    long long rightOffset = ((const struct Range*)right)->sourceOffset;

    if (leftOffset < rightOffset)
    {
        return -1;
    }

    if (leftOffset > rightOffset)
    {
        return 1;
    }

    return 0;
}

void function(Function instance)
{
    instance->count = 0;
}

Range function_new_range(Function instance)
{
    Range result = instance->ranges + instance->count;

    instance->count++;

    return result;
}

void function_sort_ranges(Function instance)
{
    qsort(
        instance->ranges,
        instance->count,
        sizeof * instance->ranges,
        range_compare);
}

void list(List instance)
{
    instance->count = 0;
}

void list_add(List instance, long long item)
{
    instance->items[instance->count] = item;
    instance->count++;
}

static Range search(Function function, long long value)
{
    Range lo = function->ranges;
    Range hi = lo + function->count - 1;

    while (lo <= hi)
    {
        Range current = lo + ((hi - lo) / 2);
        long long other = current->sourceOffset;

        if (other == value)
        {
            return current;
        }

        if (other > value)
        {
            hi = current - 1;
        }
        else
        {
            lo = current + 1;
        }
    }

    return hi;
}

static void realize(Function f, List seeds)
{
    if (f->count == 0)
    {
        return;
    }

    function_sort_ranges(f);

    for (long long* p = seeds->items; p < seeds->items + seeds->count; p++)
    {
        long long input = *p;
        Range range = search(f, input);
        long long offset = range->sourceOffset;

        if (input >= offset && input < offset + range->length)
        {
            *p = input - offset + range->destinationOffset;
        }
    }

    function(f);
}

static bool read(Function function, char buffer[])
{
    String token = strtok(buffer, DELIMITERS);

    if (!token)
    {
        return false;
    }

    Range range = function_new_range(function);

    range->destinationOffset = atoll(token);
    token = strtok(NULL, DELIMITERS);

    if (!token)
    {
        return false;
    }

    range->sourceOffset = atoll(token);
    token = strtok(NULL, DELIMITERS);

    if (!token)
    {
        return false;
    }

    range->length = atoll(token);

    return true;
}

int main(void)
{
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    if (!fgets(buffer, sizeof buffer, stdin) || !strtok(buffer, DELIMITERS))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    String token;
    struct List seeds;
    struct Function current;

    list(&seeds);
    function(&current);

    while ((token = strtok(NULL, DELIMITERS)))
    {
        list_add(&seeds, atoll(token));
    }

    while (fgets(buffer, sizeof buffer, stdin))
    {
        if (buffer[0] == '\n')
        {
            continue;
        }

        if (strchr(buffer, ':'))
        {
            realize(&current, &seeds);

            continue;
        }

        read(&current, buffer);
    }

    realize(&current, &seeds);

    long long min = LLONG_MAX;
    
    for (long long* p = seeds.items; p < seeds.items + seeds.count; p++)
    {
        long long value = *p;

        if (value < min)
        {
            min = value;
        }
    }

    printf("05a %lld %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);
}
