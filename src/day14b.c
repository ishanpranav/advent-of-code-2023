// Licensed under the MIT License.

// The Floor Will Be Lava Part 2

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define CACHE_SIZE 1024
#define DIMENSION 101
#define ITERATIONS 1000000000

struct CacheLine
{
    char key[(DIMENSION - 1) * (DIMENSION - 1)];
    int value;
};

struct Cache
{
    struct CacheLine lines[CACHE_SIZE];
    struct CacheLine* end;
};

struct Matrix
{
    int rows;
    int columns;
    char items[(DIMENSION - 1) * (DIMENSION - 1)];
};

struct String
{
    char* buffer;
    int length;
};

typedef char* Row;
typedef struct CacheLine* CacheLine;
typedef struct Cache* Cache;
typedef struct Matrix* Matrix;
typedef struct String* String;

void cache(Cache instance)
{
    instance->end = instance->lines;
}

void cache_add(Cache instance, String key, int value)
{
    CacheLine result = instance->end;
    CacheLine next = result + 1;

    if (next == instance->lines + CACHE_SIZE)
    {
        instance->end = instance->lines;
    }
    else
    {
        instance->end = next;
    }

    return result;
}

bool cache_try_get_value(Cache instance, String key, int* value)
{
    for (CacheLine line = instance->lines; line < instance->end; line++)
    {
        if (memcmp(line, key->buffer, key->length) == 0)
        {
            *value = line->value;

            return true;
        }
    }

    return false;
}

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

void matrix_swap(Matrix instance, int i1, int j1, int i2, int j2)
{
    int index1 = (i1 * instance->columns) + j1;
    int index2 = (i2 * instance->columns) + j2;
    char swap = instance->items[index1];

    instance->items[index1] = instance->items[index2];
    instance->items[index2] = swap;
}

void matrix_to_string(Matrix instance, struct String* result)
{
    result->buffer = instance->items;
    result->length = instance->rows * instance->columns;
}

static int find_hi(Matrix matrix, int i, int j)
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

static int find_lo(Matrix matrix, int i, int j)
{
    for (int row = i + 1; i < matrix->rows; row++)
    {
        if (matrix_get(matrix, row, j) != '.')
        {
            return row;
        }
    }

    return matrix->rows;
}

static int find_left(Matrix matrix, int i, int j)
{
    for (int column = j - 1; column >= 0; column--)
    {
        if (matrix_get(matrix, i, column) != '.')
        {
            return column;
        }
    }

    return -1;
}

static int find_right(Matrix matrix, int i, int j)
{
    for (int column = j + 1; column < matrix->columns; column++)
    {
        if (matrix_get(matrix, i, column) != '.')
        {
            return column;
        }
    }

    return matrix->columns;
}

static void roll_hi(Matrix matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->columns; j++)
        {
            if (matrix_get(matrix, i, j) == 'O')
            {
                matrix_swap(matrix, i, j, find_hi(matrix, i, j) + 1, j);
            }
        }
    }
}

static void roll_lo(Matrix matrix)
{
    for (int i = matrix->rows - 1; i >= 0; i--)
    {
        for (int j = 0; j < matrix->columns; j++)
        {
            if (matrix_get(matrix, i, j) == 'O')
            {
                matrix_swap(matrix, i, j, find_lo(matrix, i, j) - 1, j);
            }
        }
    }
}

static void roll_left(Matrix matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->columns; ++j)
        {
            if (matrix_get(matrix, i, j) == 'O')
            {
                matrix_swap(matrix, i, j, i, find_left(matrix, i, j) + 1);
            }
        }
    }
}

static void roll_right(Matrix matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = matrix->columns - 1; j >= 0; j--)
        {
            if (matrix_get(matrix, i, j) == 'O')
            {
                matrix_swap(matrix, i, j, i, find_right(matrix, i, j) - 1);
            }
        }
    }
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

    struct Matrix a;
    struct Cache visited;

    matrix(&a, n);

    do
    {
        memcpy(matrix_new_row(&a), buffer, n);
    }
    while (fgets(buffer, n + 2, stdin));

    for (int i = 0; i < ITERATIONS; i++)
    {
        roll_hi(&a);
        roll_left(&a);
        roll_lo(&a);
        roll_right(&a);

        int value;
        struct String key;

        matrix_to_string(&a, &key);

        if (cache_try_get_value(&visited, &key, &value))
        {
            i = ITERATIONS - ((ITERATIONS - value) % (i - value));
        }

        CacheLine line = cache_new_line(&visited);

        memcpy(line->key, key.buffer, key.length);

        line->value = 0;
    }

    long total = scan(&a);

    printf("%ld : %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
