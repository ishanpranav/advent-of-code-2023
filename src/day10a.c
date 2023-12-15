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
typedef struct Matrix* Matrix;

Coordinate coordinate_empty()
{
    Coordinate result =
    {
        .i = -1,
        .j = -1
    };

    return result;
}

void matrix(Matrix instance, int n)
{
    instance->origin = coordinate_empty();
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

Row matrix_new_row(Matrix instance)
{
    int m = instance->rows;

    instance->rows = m + 1;

    return instance->items + (instance->columns * m);
}

static bool scan_hi(Matrix matrix, Coordinate* previous, Coordinate current)
{
    current.i--;

    char other = matrix_get(matrix, current);

    switch (other)
    {
    case '|':
    case '7':
    case 'F':
        *previous = current;

        return true;
    
    default: return false;
    }
}

static bool scan_lo(Matrix matrix, Coordinate* previous, Coordinate current)
{
    current.i++;

    char other = matrix_get(matrix, current);

    switch (other)
    {
    case '|':
    case 'L':
    case 'J':
        *previous = current;

        return true;
    
    default: return false;
    }
}

static bool scan_left(Matrix matrix, Coordinate* previous, Coordinate current)
{
    current.j--;

    char other = matrix_get(matrix, current);

    switch (other)
    {
    case '-':
    case 'L':
    case 'F':
        *previous = current;

        return true;
    
    default: return false;
    }
}

static bool scan_right(Matrix matrix, Coordinate* previous, Coordinate current)
{
    current.j++;

    char other = matrix_get(matrix, current);

    switch (other)
    {
    case '-':
    case 'J':
    case '7':
        *previous = current;

        return true;
    
    default: return false;
    }
}

static bool scan(Matrix a, Coordinate* previous, Coordinate current)
{
    switch (matrix_get(a, current))
    {
    case '|': return 
        scan_hi(a, previous, current) || 
        scan_lo(a, previous, current);

    case '-': return
        scan_left(a, previous, current) ||
        scan_right(a, previous, current);

    case 'J': return
        scan_hi(a, previous, current) ||
        scan_left(a, previous, current);

    case 'L': return
        scan_hi(a, previous, current) ||
        scan_right(a, previous, current);

    case '7': return
        scan_lo(a, previous, current) ||
        scan_left(a, previous, current);

    case 'F': return
        scan_lo(a, previous, current) ||
        scan_right(a, previous, current);

    default: return false;
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
        memcpy(matrix_new_row(&a), buffer, n);

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

    Coordinate current = a.origin;
    Coordinate previous = coordinate_empty();

    if (!scan_hi(&a, &previous, current) &&
        !scan_lo(&a, &previous, current) &&
        !scan_left(&a, &previous, current) &&
        !scan_right(&a, &previous, current))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int total = 1;
    
    while (previous.i >= 0 && previous.j >= 0)
    {
        current = previous;
        total++;
        
        if (!scan(&a, &previous, current))
        {
            previous = coordinate_empty();
        }

        matrix_set(&a, current, 0);
    }

    printf("%d : %lf\n", total / 2, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
