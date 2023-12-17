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

typedef const void* Object;
typedef char* String;
typedef struct Function* Function;
typedef struct IntervalList* IntervalList;

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

void range_from_interval(
    long long min,
    long long max,
    long long intercept,
    struct Range* result)
{
    result->destinationOffset = min + intercept;
    result->sourceOffset = min;
    result->length = max - min;
}

void range_identity(long long min, long long max, struct Range* result)
{
    range_from_interval(min, max, 0, result);
}

void function(Function instance)
{
    instance->count = 0;
}

void function_add_range(Function instance, struct Range item)
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
        struct Range infinity =
        {
            .sourceOffset = 0,
            .destinationOffset = 0,
            .length = LLONG_MAX
        };

        function_add_range(instance, infinity);

        return;
    }

    struct Range view[RANGES_CAPACITY];

    function_sort_ranges(instance);
    memcpy(view, instance->ranges, count * sizeof(struct Range));
    function_clear_ranges(instance);

    struct Range* first = view;
    struct Range* last = view + count - 1;
    long long min = first->sourceOffset;

    if (min)
    {
        struct Range identity;

        range_identity(LLONG_MIN, min, &identity);
        function_add_range(instance, identity);
    }

    function_add_range(instance, *first);

    for (struct Range* current = first + 1; current <= last; current++)
    {
        struct Range* previous = current - 1;

        long long previousMax = previous->sourceOffset + previous->length;
        long long currentMin = current->sourceOffset;
        long long difference = currentMin - previousMax;

        if (difference > 1)
        {
            struct Range identity;

            range_identity(
                previousMax + 1,
                currentMin + current->length,
                &identity);
            function_add_range(instance, identity);
        }

        function_add_range(instance, *current);
    }

    long long lastMax = last->sourceOffset + last->length;

    if (lastMax < LLONG_MAX)
    {
        struct Range identity;

        range_identity(lastMax + 1, LLONG_MAX, &identity);
        function_add_range(instance, identity);
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

void function_get_enumerator(
    Function instance, 
    struct FunctionEnumerator* result)
{
    result->begin = instance->ranges;
    result->end = result->begin + instance->count;
}

void function_compose(Function instance, Function other)
{
    int count = instance->count;
    struct Range view[RANGES_CAPACITY];
    struct Range* last = view + count - 1;
    struct FunctionEnumerator image;
    
    function_get_enumerator(other, &image);
    memcpy(view, instance->ranges, count * sizeof(struct Range));
    function_clear_ranges(instance);

    for (struct Range* a = view; a <= last; a++)
    {
        for (struct Range* b = image.begin; b < image.end; b++)
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

            struct Range range;

            range_from_interval(
                math_max(aMin, bMin),
                math_min(aMax, bMax),
                a->destinationOffset - a->sourceOffset +
                b->destinationOffset - b->sourceOffset,
                &range);
            function_add_range(instance, range);
        }
    }
}

void interval_list(IntervalList instance)
{
    instance->count = 0;
}

void interval_list_add(IntervalList instance, struct Interval item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
}

void interval_list_get_enumerator(
    IntervalList instance,
    struct IntervalListEnumerator* result)
{
    result->begin = instance->items;
    result->end = result->begin + instance->count;
}

static bool read(Function function, char buffer[])
{
    String token = strtok(buffer, DELIMITERS);

    if (!token)
    {
        return false;
    }

    struct Range range;

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

    String token;
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

        struct Interval interval =
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
    struct IntervalListEnumerator domain;

    interval_list_get_enumerator(&seeds, &domain);

    for (struct Interval* seed = domain.begin; seed < domain.end; seed++)
    {
        long long seedMin = seed->min;
        long long seedMax = seed->max;
        struct FunctionEnumerator image;
        
        function_get_enumerator(&composite, &image);

        for (struct Range* range = image.begin; range < image.end; range++)
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
