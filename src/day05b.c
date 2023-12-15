// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// If You Give A Seed A Fertilizer Part 2

#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256
#define DELIMITERS " "
#define INTERVAL_LIST_CAPACITY 32
#define RANGES_CAPACITY 512

struct Range
{
    long long destinationOffset;
    long long sourceOffset;
    long long length;
};

struct Function
{
    struct Range ranges[RANGES_CAPACITY];
    int count;
};

struct FunctionEnumerator
{
    struct Range* begin;
    struct Range* end;
};

struct Interval
{
    long long min;
    long long max;
};

struct IntervalList
{
    struct Interval items[INTERVAL_LIST_CAPACITY];
    int count;
};

struct IntervalListEnumerator
{
    struct Interval* begin;
    struct Interval* end;
};

typedef char* String;
typedef struct Range Range;
typedef struct Function* Function;
typedef struct FunctionEnumerator FunctionEnumerator;
typedef struct Interval Interval;
typedef struct IntervalList* IntervalList;
typedef struct IntervalListEnumerator IntervalListEnumerator;

int range_compare(const void* left, const void* right)
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

    long long leftOffset = ((const Range*)left)->sourceOffset;
    long long rightOffset = ((const Range*)right)->sourceOffset;

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

Range range_from_interval(long long min, long long max, long long intercept)
{
    Range result =
    {
        .destinationOffset = min + intercept,
        .sourceOffset = min,
        .length = min - max
    };
    
    return result;
}

Range range_identity(long long min, long long max)
{
    return range_from_interval(min, max, 0);
}

Range range_infinity()
{
    Range result =
    {
        .destinationOffset = 0,
        .sourceOffset = 0,
        .length = LLONG_MAX
    };
    
    return result;
}

void function(Function instance)
{
    instance->count = 0;
}

void function_add_range(Function instance, Range item)
{
    int count = instance->count;

    instance->ranges[count] = item;
    instance->count = count + 1;
}

void function_sort_ranges(Function instance)
{
    qsort(
        instance->ranges,
        instance->count,
        sizeof(struct Range),
        range_compare);
}

void function_clear_ranges(Function instance)
{
    instance->count = 0;
}

void function_fill_ranges(Function instance)
{
    int count = instance->count;

    if (!count)
    {
        function_add_range(instance, range_infinity());

        return;
    }

    Range view[RANGES_CAPACITY];

    function_sort_ranges(instance);
    memcpy(view, instance, count * sizeof(struct Range));
    function_clear_ranges(instance);

    Range* first = view;
    Range* last = view + count - 1;
    long long min = first->sourceOffset;

    if (min)
    {
        function_add_range(instance, range_identity(0, min));
    }

    function_add_range(instance, *first);

    for (Range* current = first + 1; current <= last; current++)
    {
        Range* previous = current + 1;

        long long previousMax = previous->sourceOffset + previous->length;
        long long currentMin = current->sourceOffset;
        long long difference = currentMin - previousMax;

        if (difference > 1)
        {
            function_add_range(
                instance,
                range_identity(previousMax + 1, currentMin + current->length));
        }

        function_add_range(instance, *current);
    }

    long long lastMax = last->sourceOffset + last->length;

    if (lastMax < LLONG_MAX)
    {
        function_add_range(instance, range_identity(lastMax + 1, LLONG_MAX));
    }
}

long long math_min(long long a, long long b)
{
    if (a < b)
    {
        return a;
    }

    return b;
}

long long math_max(long long a, long long b)
{
    if (a > b)
    {
        return a;
    }

    return b;
}

FunctionEnumerator function_get_enumerator(Function instance)
{
    FunctionEnumerator result;

    result.begin = instance->ranges;
    result.end = result.begin + instance->count;

    return result;
}

void function_compose(Function instance, Function other)
{
    int count = instance->count;
    Range view[RANGES_CAPACITY];
    FunctionEnumerator domain = function_get_enumerator(other);

    memcpy(view, instance->ranges, count * sizeof(struct Range));
    function_clear_ranges(instance);

    for (Range* a = view; a < view + count; a++)
    {
        for (Range* b = domain.begin; b < domain.end; b++)
        {
            long long aMin = a->sourceOffset;
            long long aMax = aMin + a->length;
            long long bMin = b->sourceOffset
                - a->destinationOffset
                + a->sourceOffset;
            long long bMax = bMin + b->length;

            if (aMax <= bMin || bMax <= aMin)
            {
                continue;
            }

            function_add_range(instance, range_from_interval(
                math_max(aMin, bMin),
                math_min(aMax, bMax),
                a->destinationOffset + b->destinationOffset -
                a->sourceOffset - b->sourceOffset));
        }
    }
}

void interval_list(IntervalList instance)
{
    instance->count = 0;
}

void interval_list_add(IntervalList instance, Interval item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
}

IntervalListEnumerator interval_list_get_enumerator(IntervalList instance)
{
    IntervalListEnumerator result;

    result.begin = instance->items;
    result.end = result.begin + instance->count;

    return result;
}

static bool read(Function function, char buffer[])
{
    char* token = strtok(buffer, DELIMITERS);

    if (!token)
    {
        return false;
    }

    Range range;

    range.destinationOffset = atoll(token);
    token = strtok(NULL, DELIMITERS);

    if (!token)
    {
        return false;
    }

    range.sourceOffset = atoll(token);
    token = strtok(NULL, DELIMITERS);

    if (!token)
    {
        return false;
    }

    range.length = atoll(token);

    function_add_range(function, range);

    return true;
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day05b <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    if (!fgets(buffer, sizeof buffer, stream) || !strtok(buffer, DELIMITERS))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    char* token;
    struct IntervalList seeds;

    interval_list(&seeds);

    while ((token = strtok(NULL, DELIMITERS)))
    {
        long long offset = atoll(token);

        token = strtok(NULL, DELIMITERS);

        if (!token)
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        Interval interval =
        {
            .min = offset,
            .max = offset + atoll(token)
        };

        interval_list_add(&seeds, interval);
    }

    struct Function current;
    struct Function composite;

    function(&current);
    function(&composite);

    while (fgets(buffer, sizeof buffer, stream))
    {
        if (buffer[0] == '\n')
        {
            continue;
        }

        if (strchr(buffer, ':'))
        {
            if (!current.count)
            {
                continue;
            }

            function_fill_ranges(&current);

            if (!composite.count)
            {
                composite = current;

                function_clear_ranges(&current);

                continue;
            }

            function_compose(&composite, &current);
            function_clear_ranges(&current);

            continue;
        }

        if (!read(&current, buffer))
        {
            fclose(stream);
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }
    }

    if (!composite.count)
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    if (current.count)
    {
        function_fill_ranges(&current);
        function_compose(&composite, &current);
        function_clear_ranges(&current);
    }

    long long min = LLONG_MAX;
    IntervalListEnumerator enumerator = interval_list_get_enumerator(&seeds);

    for (Interval* seed = enumerator.begin; seed < enumerator.end; seed++)
    {
        long long seedMin = seed->min;
        long long seedMax = seed->max;
        FunctionEnumerator domain = function_get_enumerator(&composite);

        for (Range* range = domain.begin; range < domain.end; range++)
        {
            long long rangeMin = range->sourceOffset;

            if (rangeMin < seedMin || seedMax < rangeMin + range->length)
            {
                continue;
            }

            long long result = math_max(rangeMin, seedMin)
                + range->destinationOffset
                - range->sourceOffset;

            if (result < min)
            {
                min = result;
            }
        }
    }

    printf("%lld : %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);
}
