// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Optimized using precalculated quadratic equation
// Time complexity: O(1)

#include <math.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 64

typedef char* String;

static bool read(FILE* stream, long long* result)
{
    char buffer[BUFFER_SIZE];

    if (!fgets(buffer, sizeof buffer, stream))
    {
        return false;
    }

    long long number = 0;
    char current;

    for (char* p = buffer; (current = *p); p++)
    {
        if (!isdigit(current))
        {
            continue;
        }

        number = (number * 10) + (current - '0');
    }

    *result = number;

    return true;
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day6b <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    long long t;
    long long dx;
    clock_t start = clock();

    if (!read(stream, &t) || !read(stream, &dx))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    long long result = ceil((t + sqrt(t * t - 4 * dx)) / 2)
        - floor((t - sqrt(t * t - 4 * dx)) / 2)
        - 1;

    printf("%lld : %lf\n", result, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
