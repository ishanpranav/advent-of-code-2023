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
typedef struct Matrix* Matrix;

void coordinate_empty(struct Coordinate* result)
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

char matrix_get(Matrix instance, struct Coordinate coordinate)
{
    return instance->items[(instance->columns * coordinate.i) + coordinate.j];
}

void matrix_set(Matrix instance, struct Coordinate coordinate, char item)
{
    instance->items[(instance->columns * coordinate.i) + coordinate.j] = item;
}

Row matrix_new_row(Matrix instance)
{
    int m = instance->rows;

    instance->rows = m + 1;

    return instance->items + (instance->columns * m);
}

static bool scan_hi(
    Matrix matrix,
    struct Coordinate* previous,
    struct Coordinate current)
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

static bool scan_lo(
    Matrix matrix,
    struct Coordinate* previous,
    struct Coordinate current)
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

static bool scan_left(
    Matrix matrix,
    struct Coordinate* previous,
    struct Coordinate current)
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

static bool scan_right(
    Matrix matrix,
    struct Coordinate* previous,
    struct Coordinate current)
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

static bool scan(
    Matrix matrix,
    struct Coordinate* previous,
    struct Coordinate current)
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

int main()
{
    char buffer[DIMENSION + 1] = { 0 };
    clock_t start = clock();

    if (!fgets(buffer, sizeof buffer, stdin))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    int n = strlen(buffer) - 1;

    if (n < 1)
    {
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
            a.origin.i = a.rows - 1;
            a.origin.j = token - buffer;
        }
    }
    while (fgets(buffer, n + 2, stdin));

    if (!a.rows || a.origin.i < 0 || a.origin.j < 0)
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    struct Coordinate current = a.origin;
    struct Coordinate previous;

    coordinate_empty(&previous);

    if (!scan_hi(&a, &previous, current) &&
        !scan_lo(&a, &previous, current) &&
        !scan_left(&a, &previous, current) &&
        !scan_right(&a, &previous, current))
    {
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
            coordinate_empty(&previous);
        }

        matrix_set(&a, current, 0);
    }

    total /= 2;

    printf("%d : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
