// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

#include <stdio.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DIRECTIONS_CAPACITY 512

enum Direction
{
    LEFT = 'L',
    RIGHT = 'R'
};

typedef char* String;
typedef enum Direction Direction;

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day7b <path>\n");

        return 1;
    }

    FILE* stream = fopen(args[1], "r");

    if (!stream)
    {
        fprintf(stderr, "Error: I/O.\n");

        return 1;
    }

    int sum = 0;
    char buffer[BUFFER_SIZE];
    Direction directions[DIRECTIONS_CAPACITY];
    clock_t start = clock();

    printf("%ld : %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
