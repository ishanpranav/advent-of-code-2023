// Licensed under the MIT License.

// Pipe Maze Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 141
#define EXCEPTION_FORMAT "Error: Format.\n"

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

typedef char* Row;
typedef struct Coordinate* Coordinate;
typedef struct Matrix* Matrix;

void coordinate_empty(Coordinate result)
{
    result->i = -1;
    result->j = -1;
}

void matrix(Matrix instance, int n)
{
    coordinate_empty(&instance->origin);

    instance->rows = 0;
    instance->columns = n;
}

char matrix_get(Matrix instance, Coordinate coordinate)
{
    return instance->items[(instance->columns * coordinate->i) + coordinate->j];
}

void matrix_set(Matrix instance, Coordinate coordinate, char value)
{
    int index = (instance->columns * coordinate->i) + coordinate->j;

    instance->items[index] = value;
}

void matrix_add_row(Matrix instance, char values[])
{
    int m = instance->rows;

    instance->rows = m + 1;

    memcpy(
        instance->items + (instance->columns * m),
        values,
        instance->columns);
}

static bool scan_hi(Matrix matrix, Coordinate previous, Coordinate current)
{
    struct Coordinate coordinate =
    {
        .i = current->i - 1,
        .j = current->j
    };

    char other = matrix_get(matrix, &coordinate);

    switch (other)
    {
        case '|':
        case '7':
        case 'F':
            *previous = coordinate;

            return true;

        default: return false;
    }
}

static bool scan_lo(Matrix matrix, Coordinate previous, Coordinate current)
{
    struct Coordinate coordinate =
    {
        .i = current->i + 1,
        .j = current->j
    };

    char other = matrix_get(matrix, &coordinate);

    switch (other)
    {
        case '|':
        case 'L':
        case 'J':
            *previous = coordinate;

            return true;

        default: return false;
    }
}

static bool scan_left(Matrix matrix, Coordinate previous, Coordinate current)
{
    struct Coordinate coordinate =
    {
        .i = current->i,
        .j = current->j - 1
    };

    char other = matrix_get(matrix, &coordinate);

    switch (other)
    {
        case '-':
        case 'L':
        case 'F':
            *previous = coordinate;

            return true;

        default: return false;
    }
}

static bool scan_right(Matrix matrix, Coordinate previous, Coordinate current)
{
    struct Coordinate coordinate =
    {
        .i = current->i,
        .j = current->j + 1
    };

    char other = matrix_get(matrix, &coordinate);

    switch (other)
    {
        case '-':
        case 'J':
        case '7':
            *previous = coordinate;

            return true;

        default: return false;
    }
}

static bool scan(Matrix matrix, Coordinate previous, Coordinate current)
{
    switch (matrix_get(matrix, current))
    {
        case '|': return
            scan_hi(matrix, previous, current) ||
            scan_lo(matrix, previous, current);

        case '-': return
            scan_left(matrix, previous, current) ||
            scan_right(matrix, previous, current);

        case 'J': return
            scan_hi(matrix, previous, current) ||
            scan_left(matrix, previous, current);

        case 'L': return
            scan_hi(matrix, previous, current) ||
            scan_right(matrix, previous, current);

        case '7': return
            scan_lo(matrix, previous, current) ||
            scan_left(matrix, previous, current);

        case 'F': return
            scan_lo(matrix, previous, current) ||
            scan_right(matrix, previous, current);

        default: return false;
    }
}

int main(void)
{
    char buffer[DIMENSION + 1] = { 0 };
    clock_t start = clock();

    if (!fgets(buffer, sizeof buffer, stdin))
    {
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    int n = strlen(buffer) - 1;

    if (n < 1)
    {
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    struct Matrix a;

    matrix(&a, n);

    do
    {
        matrix_add_row(&a, buffer);

        char* token = strchr(buffer, 'S');

        if (token)
        {
            a.origin.i = a.rows - 1;
            a.origin.j = token - buffer;
        }
    }
    while (fgets(buffer, n + 2, stdin));

    if (!a.rows || a.origin.i < 0 || a.origin.j < 0)
    {
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    struct Coordinate current = a.origin;
    struct Coordinate previous;

    coordinate_empty(&previous);

    if (!scan_hi(&a, &previous, &current) &&
        !scan_lo(&a, &previous, &current) &&
        !scan_left(&a, &previous, &current) &&
        !scan_right(&a, &previous, &current))
    {
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    int total = 1;

    while (previous.i >= 0 && previous.j >= 0)
    {
        current = previous;
        total++;

        if (!scan(&a, &previous, &current))
        {
            coordinate_empty(&previous);
        }

        matrix_set(&a, &current, 0);
    }

    total /= 2;

    printf("10a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
