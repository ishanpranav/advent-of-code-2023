// Licensed under the MIT License.

// If You Give A Seed A Fertilizer Part 2

// This code relies on two's complement for signed integer overflow, which leads
// to undefined behavior in the general case.

// Compile with:                                    -fno-strict-overflow -fwrapv

#include <limits.h> 
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256
#define DELIMITERS " "
#define EXCEPTION_FORMAT "Error: Format.\n"
#define FUNCTION_RANGES_CAPACITY 512
#define INTERVAL_LIST_CAPACITY 32

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

typedef const void* Object;
typedef char* String;
typedef struct Range* Range;
typedef struct Range* RangeArray;
typedef struct Function* Function;
typedef struct Interval* Interval;
typedef struct IntervalList* IntervalList;

void range_from_interval(
    long long min,
    long long max,
    long long intercept,
    Range result)
{
    result->destinationOffset = min + intercept;
    result->sourceOffset = min;
    result->length = max - min;
}

void range_identity(long long min, long long max, Range result)
{
    range_from_interval(min, max, 0, result);
}

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

void function_add_range(Function instance, Range item)
{
    instance->ranges[instance->count] = *item;
    instance->count++;
}

void function_sort_ranges(Function instance)
{
    qsort(
        instance->ranges,
        instance->count,
        sizeof * instance->ranges,
        range_compare);
}

void function_get_ranges(Function instance, RangeArray results)
{
    memcpy(
        results,
        instance->ranges, 
        instance->count * sizeof * instance->ranges);
}

void function_fill_ranges(Function instance)
{
    int count = instance->count;

    if (!count)
    {
        Range infinity = function_new_range(instance);

        infinity->sourceOffset = 0;
        infinity->destinationOffset = 0;
        infinity->length = LLONG_MAX;

        return;
    }

    struct Range view[FUNCTION_RANGES_CAPACITY];

    function_sort_ranges(instance);
    function_get_ranges(instance, view);
    function(instance);

    Range first = view;
    Range last = view + count - 1;
    long long min = first->sourceOffset;

    if (min)
    {
        range_identity(LLONG_MIN, min, function_new_range(instance));
    }

    function_add_range(instance, first);

    for (Range current = first + 1; current <= last; current++)
    {
        Range previous = current - 1;

        long long previousMax = previous->sourceOffset + previous->length;
        long long currentMin = current->sourceOffset;
        long long difference = currentMin - previousMax;

        if (difference > 1)
        {
            range_identity(
                previousMax + 1,
                currentMin + current->length,
                function_new_range(instance));
        }

        function_add_range(instance, current);
    }

    long long lastMax = last->sourceOffset + last->length;

    if (lastMax < LLONG_MAX)
    {
        range_identity(lastMax + 1, LLONG_MAX, function_new_range(instance));
    }
}

void function_compose(Function instance, Function other)
{
    struct Range view[FUNCTION_RANGES_CAPACITY];
    Range last = view + instance->count - 1;

    function_get_ranges(instance, view);
    function(instance);

    for (Range a = view; a <= last; a++)
    {
        for (Range b = other->ranges; b < other->ranges + other->count; b++)
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

            range_from_interval(
                math_max(aMin, bMin),
                math_min(aMax, bMax),
                a->destinationOffset - a->sourceOffset +
                b->destinationOffset - b->sourceOffset,
                function_new_range(instance));
        }
    }
}

void interval_list(IntervalList instance)
{
    instance->count = 0;
}

void interval_list_add(IntervalList instance, Interval item)
{
    instance->items[instance->count] = *item;
    instance->count++;
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
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    String token;
    struct IntervalList seeds;

    interval_list(&seeds);

    while ((token = strtok(NULL, DELIMITERS)))
    {
        long long offset = atoll(token);

        token = strtok(NULL, DELIMITERS);

        if (!token)
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }

        struct Interval interval =
        {
            .min = offset,
            .max = offset + atoll(token)
        };

        interval_list_add(&seeds, &interval);
    }

    struct Function current;
    struct Function composite;

    function(&current);
    function(&composite);

    while (fgets(buffer, sizeof buffer, stdin))
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

                function(&current);

                continue;
            }

            function_compose(&composite, &current);
            function(&current);

            continue;
        }

        if (!read(&current, buffer))
        {
            fprintf(stderr, EXCEPTION_FORMAT);

            return 1;
        }
    }

    if (!composite.count)
    {
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    if (current.count)
    {
        function_fill_ranges(&current);
        function_compose(&composite, &current);
        function(&current);
    }

    long long min = LLONG_MAX;

    for (Interval seed = seeds.items; seed < seeds.items + seeds.count; seed++)
    {
        long long seedMin = seed->min;
        long long seedMax = seed->max;

        for (Range range = composite.ranges;
            range < composite.ranges + composite.count;
            range++)
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
    
    printf("05b %lld %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);
}
