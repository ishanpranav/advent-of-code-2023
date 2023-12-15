// Author: Ishan Pranav
// Copyright (c) 2023 Ishan Pranav. All rights reserved.
// Licensed under the MIT License.

// Pipe Maze Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define COORDINATE_QUEUE_CAPACITY 4
#define DIMENSION 141

struct Coordinate
{
    int i;
    int j;
};

struct CoordinateQueue
{
    struct Coordinate items[COORDINATE_QUEUE_CAPACITY];
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
typedef struct CoordinateQueue* CoordinateQueue;
typedef struct Matrix* Matrix;

Coordinate coordinate_empty()
{
    struct Coordinate result =
    {
        .i = -1,
        .j = -1
    };

    return result;
}

void coordinate_queue(CoordinateQueue instance)
{
    instance->count = 0;
}

void coordinate_queue_enqueue(CoordinateQueue instance, Coordinate item)
{
    int count = instance->count;

    instance->items[count] = item;
    instance->count = count + 1;
}

bool coordinate_queue_try_dequeue(CoordinateQueue instance, Coordinate* result)
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
    instance->start = coordinate_empty();
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

static void scan_hi(Matrix matrix, CoordinateQueue queue, Coordinate coordinate)
{
    coordinate.i--;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '|':
    case '7':
    case 'F':
        coordinate_queue_enqueue(queue, coordinate);
        break;
    }
}

static void scan_lo(Matrix matrix, CoordinateQueue queue, Coordinate coordinate)
{
    coordinate.i++;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '|':
    case 'L':
    case 'J':
        coordinate_queue_enqueue(queue, coordinate);
        break;
    }
}

static void scan_left(Matrix matrix, CoordinateQueue queue, Coordinate coordinate)
{
    coordinate.j--;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '-':
    case 'L':
    case 'F':
        coordinate_queue_enqueue(queue, coordinate);
        break;
    }
}

static void scan_right(Matrix matrix, CoordinateQueue queue, Coordinate coordinate)
{
    coordinate.j++;

    char other = matrix_get(matrix, coordinate);

    switch (other)
    {
    case '-':
    case 'J':
    case '7':
        coordinate_queue_enqueue(queue, coordinate);
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
    struct CoordinateQueue queue = { 0 };

    coordinate_queue(&queue);
    coordinate_queue_enqueue(&queue, a.start);

    while (coordinate_queue_try_dequeue(&queue, &result))
    {
        total++;

        char current = matrix_get(&a, result);

        switch (current)
        {
        case 'S':
            scan_hi(&a, &queue, result);
            scan_lo(&a, &queue, result);
            scan_left(&a, &queue, result);
            scan_right(&a, &queue, result);
            break;

        case '|':
            scan_hi(&a, &queue, result);
            scan_lo(&a, &queue, result);
            break;

        case '-':
            scan_left(&a, &queue, result);
            scan_right(&a, &queue, result);
            break;

        case 'J':
            scan_hi(&a, &queue, result);
            scan_left(&a, &queue, result);
            break;

        case 'L':
            scan_hi(&a, &queue, result);
            scan_right(&a, &queue, result);
            break;

        case '7':
            scan_lo(&a, &queue, result);
            scan_left(&a, &queue, result);
            break;

        case 'F':
            scan_lo(&a, &queue, result);
            scan_right(&a, &queue, result);
            break;
        }

        matrix_set(&a, result, 0);
    }

    printf("%d : %lf\n", total / 2, (double)(clock() - start) / CLOCKS_PER_SEC);
    fclose(stream);

    return 0;
}
