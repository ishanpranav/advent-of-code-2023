// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Brute-force with binary search
// Time complexity: O(n * m * log(p)) where
//      n is the number of seeds,
//      m is the number of functions, and
//      p is the number of ranges in the largest map.

#include <limits.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 256
#define DELIMITERS " "
#define LIST_CAPACITY 32
#define RANGES_CAPACITY 64

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

struct List
{
    long long items[LIST_CAPACITY];
    int count;
};

struct ListEnumerator
{
    long long* begin;
    long long* end;
};

typedef char* String;
typedef struct Range Range;
typedef struct Function* Function;
typedef struct List* List;
typedef struct ListEnumerator ListEnumerator;

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

Range* function_binary_search(Function instance, long long value)
{
    Range* begin = instance->ranges;
    Range* lo = instance->ranges;
    Range* hi = lo + instance->count - 1;

    while (lo <= hi)
    {
        Range* current = lo + ((hi - lo) / 2);
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

    if (hi > begin)
    {
        return hi;
    }

    return begin;
}

long long function_transform(Function instance, long long input)
{
    if (instance->count == 0)
    {
        return input;
    }

    Range* range = function_binary_search(instance, input);
    long long offset = range->sourceOffset;

    if (input >= offset && input < offset + range->length)
    {
        return input - offset + range->destinationOffset;
    }

    return input;
}

void list(List instance)
{
    instance->count = 0;
}

void list_add(List instance, long long item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
}

ListEnumerator list_get_enumerator(List instance)
{
    ListEnumerator result;

    result.begin = instance->items;
    result.end = result.begin + instance->count;

    return result; 
}

static void transform(Function function, List seeds)
{
    function_sort_ranges(function);
    
    ListEnumerator enumerator = list_get_enumerator(seeds);

    for (long long* p = enumerator.begin; p < enumerator.end; p++)
    {
        *p = function_transform(function, *p);
    }

    function_clear_ranges(function);
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day5a <path>\n");

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

    struct List seeds;
    struct Function current;

    list(&seeds);
    function(&current);

    for (char* token = strtok(NULL, DELIMITERS);
        token && seeds.count < LIST_CAPACITY;
        token = strtok(NULL, DELIMITERS))
    {
        list_add(&seeds, atoll(token));
    }

    while (fgets(buffer, sizeof buffer, stream))
    {
        if (strchr(buffer, ':'))
        {
            transform(&current, &seeds);

            continue;
        }

        if (buffer[0] == '\n')
        {
            continue;
        }

        Range range =
        {
            .destinationOffset = atoll(strtok(buffer, DELIMITERS)),
            .sourceOffset = atoll(strtok(NULL, DELIMITERS)),
            .length = atoll(strtok(NULL, DELIMITERS))
        };

        function_add_range(&current, range);
    }

    transform(&current, &seeds);

    long long min = LLONG_MAX;
    ListEnumerator enumerator = list_get_enumerator(&seeds);

    for (long long* p = enumerator.begin; p < enumerator.end; p++)
    {
        long long value = *p;

        if (value < min)
        {
            min = value;
        }
    }

    printf("%lld : %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);
}
