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
typedef struct Coordinate Coordinate;
typedef struct CoordinateList* CoordinateList;
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

void coordinate_list(CoordinateList instance, Coordinate item)
{
    instance->items[0] = item;
    instance->count = 1;
}

void coordinate_list_add(CoordinateList instance, Coordinate item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
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

int math_shoelace_formula_area(CoordinateList polygon)
{
    int result = 0;
    int lastIndex = polygon->count - 1;
    Coordinate first = polygon->items[0];

    for (int i = 0; i < lastIndex; i++)
    {
        Coordinate current = polygon->items[i];
        Coordinate next = polygon->items[i + 1];

        result += (current.i + next.i) * (current.j - next.j);
    }

    Coordinate last = polygon->items[lastIndex];

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
            Coordinate origin =
            {
                .i = a.rows - 1,
                .j = token - buffer
            };

            a.origin = origin;
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
    Coordinate current = a.origin;
    Coordinate previous = coordinate_empty();

    coordinate_list(&path, a.origin);

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
            previous = coordinate_empty();
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
