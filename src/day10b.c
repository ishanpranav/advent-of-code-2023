// Licensed under the MIT License.

// Pipe Maze Part 2

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 141
#define POLYGON_CAPACITY 16384

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

struct Polygon
{
    struct Coordinate items[POLYGON_CAPACITY];
    int count;
};

typedef char* Row;
typedef char* String;
typedef struct Coordinate* Coordinate;
typedef struct Matrix* Matrix;
typedef struct Polygon* Polygon;

void coordinate_empty(Coordinate result)
{
    result->i = -1;
    result->j = -1;
}

void matrix(Matrix instance, int columns)
{
    coordinate_empty(&instance->origin);

    instance->rows = 0;
    instance->columns = columns;
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

void matrix_add_row(Matrix instance, String value)
{
    int m = instance->rows;

    instance->rows = m + 1;

    memcpy(instance->items + (instance->columns * m), value, instance->columns);
}

void polygon(Polygon instance, Coordinate item)
{
    instance->items[0] = *item;
    instance->count = 1;
}

void polygon_add(Polygon instance, Coordinate item)
{
    instance->items[instance->count] = *item;
    instance->count++;
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

int math_shoelace_formula_area(Polygon polygon)
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
        matrix_add_row(&a, buffer);

        char* token = strchr(buffer, 'S');

        if (token)
        {
            a.origin.i = a.rows - 1,
                a.origin.j = token - buffer;
        }
    }
    while (fgets(buffer, n + 2, stdin));

    if (!a.rows || a.origin.i < 0 || a.origin.j < 0)
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    struct Polygon path;
    struct Coordinate current = a.origin;
    struct Coordinate previous;

    polygon(&path, &a.origin);
    coordinate_empty(&previous);

    if (!scan_hi(&a, &previous, &current) &&
        !scan_lo(&a, &previous, &current) &&
        !scan_left(&a, &previous, &current) &&
        !scan_right(&a, &previous, &current))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    while (previous.i >= 0 && previous.j >= 0)
    {
        polygon_add(&path, &current);

        current = previous;

        if (!scan(&a, &previous, &current))
        {
            coordinate_empty(&previous);
        }

        matrix_set(&a, &current, 0);
    }

    int total = math_pick_theorem_i(
        math_shoelace_formula_area(&path),
        path.count);

    printf("10b %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
