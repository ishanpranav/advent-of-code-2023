// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Custom optimized implementation

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DIMENSION 141

typedef char* String;
typedef bool (*Validator)(String a, String b, int index, int offset);

static bool check(char value)
{
    return value != '.' && value != '\n' && value;
}

static bool realize_x(String a, String b, int index, int offset)
{
    if (index < DIMENSION && check(a[index]))
    {
        return true;
    }

    if (offset - 1 >= 0 && check(a[offset - 1]))
    {
        return true;
    }

    return false;
}

static bool realize_y(String a, String b, int index, int offset)
{
    for (int i = offset - 1; i <= index; i++)
    {
        if (check(b[i]))
        {
            return true;
        }
    }

    return false;
}

static bool realize_xy(String a, String b, int index, int offset)
{
    return realize_x(a, b, index, offset) || realize_y(a, b, index, offset);
}

static void aggregate(String a, String b, long* sum, Validator validator)
{
    int offset = -1;
    int number = 0;

    for (int i = 0; i < DIMENSION; i++)
    {
        char current = a[i];

        if (isdigit(current))
        {
            if (offset == -1)
            {
                offset = i;
            }

            number = (number * 10) + (current - '0');

            continue;
        }

        if (number && validator(a, b, i, offset))
        {
            *sum += number;
        }

        offset = -1;
        number = 0;
    }

    if (number && validator(a, b, DIMENSION - 1, offset))
    {
        *sum += number;
    }
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day03a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    char hi[DIMENSION + 1];
    clock_t start = clock();

    if (!fgets(hi, sizeof hi, stream))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    long sum = 0;
    char lo[DIMENSION + 1];

    while (fgets(lo, sizeof lo, stream))
    {
        aggregate(hi, lo, &sum, realize_xy);
        aggregate(lo, hi, &sum, realize_y);
        memcpy(hi, lo, sizeof hi);
    }

    aggregate(lo, hi, &sum, realize_x);
    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
