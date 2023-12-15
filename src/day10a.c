// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Pipe Maze Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 141

struct Coordinate
{
    int i;
    int j;
};

struct CoordinateStack
{
    struct Coordinate item;
    bool empty;
};

struct Matrix
{
    struct Coordinate origin;
    int rows;
    int columns;
    char items[(DIMENSION - 1) * (DIMENSION - 1)];
};

typedef char* String;
typedef char* Row;
typedef struct Coordinate Coordinate;
typedef struct CoordinateStack* CoordinateStack;
typedef struct Matrix* Matrix;

void coordinate_stack(CoordinateStack instance)
{
    instance->empty = true;
}

void coordinate_stack_push(CoordinateStack instance, Coordinate item)
{
    instance->item = item;
    instance->empty = false;
}

bool coordinate_stack_try_pop(CoordinateStack instance, Coordinate* result)
{
    if (instance->empty)
    {
        return false;
    }

    *result = instance->item;
    instance->empty = true;

    return true;
}

void matrix(Matrix instance, int n)
{
    Coordinate empty =
    {
        .i = -1,
        .j = -1
    };

    instance->origin = empty;
    instance->rows = 0;
    instance->columns = n;
}

char matrix_get(Matrix instance, Coordinate coordinate)
{
    return instance->items[(instance->columns * coordinate.i) + coordinate.j];
}

void matrix_set(Matrix instance, Coordinate coordinate, char item)
{
    instance->items[(instance->columns * coordinate.i) + coordinate.j] = item;
}

Row matrix_add_row(Matrix instance)
{
    int m = instance->rows;

    instance->rows = m + 1;

    return instance->items + (instance->columns * m);
}

static bool scan_hi(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.i--;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '|':
    case '7':
    case 'F':
        coordinate_stack_push(stack, coordinate);
        return true;
    }

    return false;
}

static bool scan_lo(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.i++;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '|':
    case 'L':
    case 'J':
        coordinate_stack_push(stack, coordinate);
        return true;
    }

    return false;
}

static bool scan_left(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.j--;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '-':
    case 'L':
    case 'F':
        coordinate_stack_push(stack, coordinate);
        return true;
    }

    return false;
}

static bool scan_right(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.j++;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '-':
    case 'J':
    case '7':
        coordinate_stack_push(stack, coordinate);
        return true;
    }

    return false;
}

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

    char buffer[DIMENSION + 1] = { 0 };
    clock_t start = clock();

    if (!fgets(buffer, sizeof buffer, stream))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int n = strlen(buffer) - 1;

    if (n < 1)
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    struct Matrix a;

    matrix(&a, n);

    do
    {
        memcpy(matrix_add_row(&a), buffer, n);

        char* token = strchr(buffer, 'S');

        if (token)
        {
            Coordinate origin =
            {
                .i = a.rows - 1,
                .j = token - buffer
            };

            a.origin = origin;
        }
    }
    while (fgets(buffer, sizeof buffer, stream));

    if (!a.rows || a.origin.i < 0 || a.origin.j < 0)
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int total = 1;
    Coordinate current = a.origin;
    struct CoordinateStack stack;

    coordinate_stack(&stack);

    if (scan_hi(&a, &stack, current) ||
        scan_lo(&a, &stack, current) ||
        scan_left(&a, &stack, current) ||
        scan_right(&a, &stack, current))
    {
    }

    while (coordinate_stack_try_pop(&stack, &current))
    {
        total++;

        switch (matrix_get(&a, current))
        {
        case '|':
            if (scan_hi(&a, &stack, current) || scan_lo(&a, &stack, current))
            {
            }
            break;

        case '-':
            if (scan_left(&a, &stack, current) ||
                scan_right(&a, &stack, current))
            {
            }
            break;

        case 'J':
            if (scan_hi(&a, &stack, current) ||
                scan_left(&a, &stack, current))
            {
            }
            break;

        case 'L':
            if (scan_hi(&a, &stack, current) || scan_right(&a, &stack, current)) 
            {
            }
            break;

        case '7':
            if (scan_lo(&a, &stack, current) || scan_left(&a, &stack, current))
            {
            }
            break;

        case 'F':
            if (scan_lo(&a, &stack, current) || scan_right(&a, &stack, current))
            {
            }
            break;
        }

        matrix_set(&a, current, 0);
    }

    printf("%d : %lf\n", total / 2, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
