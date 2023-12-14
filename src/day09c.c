// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Custom implementation using Lagrange interpolation

#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 256

typedef char* String;

double lagrange_
int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day8a <path>\n");

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
    int solution = 0;

    while (fgets(buffer, sizeof buffer, stream))
    {

    }

    printf("%d : %lf\n", solution, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
