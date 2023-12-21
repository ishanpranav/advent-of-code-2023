// Licensed under the MIT License.

// The Floor Will Be Lava Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 101

struct Matrix
{
    int rows;
    int columns;
    char items[(DIMENSION - 1) * (DIMENSION - 1)];
}; 

typedef char* Row;
typedef struct Matrix* Matrix;

void matrix(Matrix instance, int columns)
{
    instance->rows = 0;
    instance->columns = columns;
}

char matrix_get(Matrix instance, int i, int j)
{
    return instance->items[(instance->columns * i) + j];
}

Row matrix_new_row(Matrix instance)
{
    int m = instance->rows;

    instance->rows = m + 1;

    return instance->items + (instance->columns * m);
}

void matrix_vertical_swap(Matrix instance, int i1, int i2, int j)
{
    i1 *= instance->columns;
    i2 *= instance->columns;

    char swap = instance->items[i2 + j];

    instance->items[i2 + j] = instance->items[i1 + j];
    instance->items[i1 + j] = swap;
}

static int find(Matrix matrix, int i, int j)
{
    for (int row = i - 1; row >= 0; row--)
    {
        if (matrix_get(matrix, row, j) != '.')
        {
            return row;
        }
    }

    return -1;
}

int main()
{
    char buffer[DIMENSION + 1];
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

    int total = 0;
    struct Matrix a;

    matrix(&a, n);

    do
    {
        memcpy(matrix_new_row(&a), buffer, n);
    }
    while (fgets(buffer, n + 2, stdin));

    for (int i = 1; i < a.rows; i++)
    {
        for (int j = 0; j < a.columns; j++)
        {
            if (matrix_get(&a, i, j) == 'O')
            {
                matrix_vertical_swap(&a, i, find(&a, i, j) + 1, j);
            }
        }
    }

    for (int i = 0; i < a.rows; i++)
    {
        for (int j = 0; j < a.columns; j++)
        {
            if (matrix_get(&a, i, j) == 'O')
            {
                total += a.rows - i;
            }
        }
    }

    printf("%d : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
