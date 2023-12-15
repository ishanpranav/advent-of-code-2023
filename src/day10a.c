// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Pipe Maze Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define COORDINATE_STACK_CAPACITY 4
#define DIMENSION 141

struct Coordinate
{
    int i;
    int j;
};

struct CoordinateStack
{
    struct Coordinate items[COORDINATE_STACK_CAPACITY];
    int count;
};

struct Matrix
{
    struct Coordinate start;
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
    instance->count = 0;
}

void coordinate_stack_push(CoordinateStack instance, Coordinate item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
}

bool coordinate_stack_try_pop(CoordinateStack instance, Coordinate* result)
{
    if (!instance->count)
    {
        return false;
    }

    int index = instance->count - 1;

    *result = instance->items[index];
    instance->count = index;

    return true;
}

void matrix(Matrix instance)
{
    Coordinate empty =
    {
        .i = -1,
        .j = -1
    };
    
    instance->start = empty;
    instance->rows = 0;
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

static void scan_hi(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.i--;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '|':
    case '7':
    case 'F':
        coordinate_stack_push(stack, coordinate);
        break;
    }
}

static void scan_lo(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.i++;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '|':
    case 'L':
    case 'J':
        coordinate_stack_push(stack, coordinate);
        break;
    }
}

static void scan_left(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.j--;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '-':
    case 'L':
    case 'F':
        coordinate_stack_push(stack, coordinate);
        break;
    }
}

static void scan_right(Matrix matrix, CoordinateStack stack, Coordinate coordinate)
{
    coordinate.j++;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '-':
    case 'J':
    case '7':
        coordinate_stack_push(stack, coordinate);
        break;
    }
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

    struct Matrix a;
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

    matrix(&a);

    a.columns = n;

    do
    {
        memcpy(matrix_add_row(&a), buffer, n);

        char* token = strchr(buffer, 'S');

        if (token)
        {
            Coordinate start =
            {
                .i = a.rows - 1,
                .j = token - buffer
            };

            a.start = start;
        }
    }
    while (fgets(buffer, sizeof buffer, stream));

    if (!a.rows || a.start.i < 0 || a.start.j < 0)
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int total = 0;
    Coordinate result;
    struct CoordinateStack stack = { 0 };

    coordinate_stack(&stack);
    coordinate_stack_push(&stack, a.start);

    while (coordinate_stack_try_pop(&stack, &result))
    {
        total++;

        char current = matrix_get(&a, result);

        switch (current)
        {
        case 'S':
            scan_hi(&a, &stack, result);
            scan_lo(&a, &stack, result);
            scan_left(&a, &stack, result);
            scan_right(&a, &stack, result);
            break;

        case '|':
            scan_hi(&a, &stack, result);
            scan_lo(&a, &stack, result);
            break;

        case '-':
            scan_left(&a, &stack, result);
            scan_right(&a, &stack, result);
            break;

        case 'J':
            scan_hi(&a, &stack, result);
            scan_left(&a, &stack, result);
            break;

        case 'L':
            scan_hi(&a, &stack, result);
            scan_right(&a, &stack, result);
            break;

        case '7':
            scan_lo(&a, &stack, result);
            scan_left(&a, &stack, result);
            break;

        case 'F':
            scan_lo(&a, &stack, result);
            scan_right(&a, &stack, result);
            break;
        }

        matrix_set(&a, result, 0);
    }

    printf("%d : %lf\n", total / 2, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
