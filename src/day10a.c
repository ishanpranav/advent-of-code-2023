// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Custom implementation using Lagrange interpolation

#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 128

typedef char* String;

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day10a <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    long sum = 0;
    char buffer[BUFFER_SIZE];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stream))
    {
    }

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
