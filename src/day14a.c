// Licensed under the MIT License.

// The Floor Will Be Lava Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 101
#define EXCEPTION_FORMAT "Error: Format.\n"

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

void matrix_set(Matrix instance, int i, int j, char value)
{
    instance->items[(instance->columns * i) + j] = value;
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

static long scan(Matrix matrix)
{
    long result = 0;

    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->columns; j++)
        {
            if (matrix_get(matrix, i, j) == 'O')
            {
                result += matrix->rows - i;
            }
        }
    }

    return result;
}

static void roll_hi(Matrix matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->columns; j++)
        {
            if (matrix_get(matrix, i, j) != 'O')
            {
                continue;
            }

            for (int k = i - 1; k >= 0 && matrix_get(matrix, k, j) == '.'; k--)
            {
                matrix_set(matrix, k, j, 'O');
                matrix_set(matrix, k + 1, j, '.');
            }
        }
    }
}

int main(void)
{
    char buffer[DIMENSION + 1];
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
    }
    while (fgets(buffer, n + 2, stdin));

    roll_hi(&a);

    long total = scan(&a);

    printf("14a %ld %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
