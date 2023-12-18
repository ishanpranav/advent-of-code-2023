// Licensed under the MIT License.

// Pipe Maze Part 2

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 141
#define COORDINATE_LIST_CAPACITY 16384

struct Coordinate
{
    int i;
    int j;
};

struct CoordinateList
{
    struct Coordinate items[COORDINATE_LIST_CAPACITY];
    int count;
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
typedef struct CoordinateList* CoordinateList;
typedef struct Matrix* Matrix;

void coordinate_empty(struct Coordinate* result)
{
    result->i = -1;
    result->j = -1;
}

void coordinate_list(CoordinateList instance, struct Coordinate item)
{
    instance->items[0] = item;
    instance->count = 1;
}

void coordinate_list_add(CoordinateList instance, struct Coordinate item)
{
    instance->items[instance->count] = item;
    instance->count++;
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

int math_shoelace_formula_area(CoordinateList polygon)
{
    int result = 0;
    int lastIndex = polygon->count - 1;
    struct Coordinate first = polygon->items[0];

    for (int i = 0; i < lastIndex; i++)
    {
        struct Coordinate current = polygon->items[i];
        struct Coordinate next = polygon->items[i + 1];

        result += (current.i + next.i) * (current.j - next.j);
    }

    struct Coordinate last = polygon->items[lastIndex];

    return (result + (last.i + first.i) * (last.j - first.j)) / 2;
}

int math_pick_theorem_i(int a, int b)
{
    return a - (b / 2) + 1;
}

int main(int count, String args[])
{
    if (count != 2)
    {
        printf("Usage: day10b <path>\n");

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
            a.origin.i = a.rows - 1,
            a.origin.j = token - buffer;
        }
    }
    while (fgets(buffer, n + 2, stream));

    if (!a.rows || a.origin.i < 0 || a.origin.j < 0)
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    struct CoordinateList path;
    struct Coordinate current = a.origin;
    struct Coordinate previous;

    coordinate_list(&path, a.origin);
    coordinate_empty(&previous);

    if (!scan_hi(&a, &previous, current) &&
        !scan_lo(&a, &previous, current) &&
        !scan_left(&a, &previous, current) &&
        !scan_right(&a, &previous, current))
    {
        fclose(stream);
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    while (previous.i >= 0 && previous.j >= 0)
    {
        coordinate_list_add(&path, current);

        current = previous;

        if (!scan(&a, &previous, current))
        {
            coordinate_empty(&previous);
        }

        matrix_set(&a, current, 0);
    }

    int total = math_pick_theorem_i(
        math_shoelace_formula_area(&path),
        path.count);

    printf("%d : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
