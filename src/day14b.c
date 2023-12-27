// Licensed under the MIT License.

// Parabolic Reflector Dish Part 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DICTIONARY_BUCKETS 197
#define DIMENSION 101
#define ITERATIONS 1000000000

enum
{
    KEY_SIZE = (DIMENSION - 1) * (DIMENSION - 1)
};

enum Exception
{
    EXCEPTION_NONE = 0,
    EXCEPTION_OUT_OF_MEMORY
};

struct DictionaryEntry
{
    struct DictionaryEntry* nextEntry;
    long value;
    char key[KEY_SIZE];
};

struct DictionaryBucket
{
    struct DictionaryEntry* firstEntry;
    struct DictionaryBucket* nextBucket;
};

struct Dictionary
{
    struct DictionaryBucket* firstBucket;
    struct DictionaryBucket buckets[DICTIONARY_BUCKETS];
};

struct Matrix
{
    int rows;
    int columns;
    char items[KEY_SIZE];
};

typedef char* Row;
typedef enum Exception Exception;
typedef struct DictionaryEntry* DictionaryEntry;
typedef struct DictionaryBucket* DictionaryBucket;
typedef struct Dictionary* Dictionary;
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

void matrix_to_char_array(Matrix instance, char result[])
{
    memcpy(result, instance->items, instance->rows * instance->columns);
}

Exception dictionary_replace(
    Dictionary instance,
    char key[],
    long* existingValue,
    long newValue)
{
    DictionaryEntry* p;
    unsigned int hash = 7;

    for (int i = 0; i < KEY_SIZE; i++)
    {
        hash = (hash * 31) + key[i];
    }

    hash %= DICTIONARY_BUCKETS;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (memcmp(key, (*p)->key, KEY_SIZE) == 0)
        {
            *existingValue = (*p)->value;
            (*p)->value = newValue;

            return EXCEPTION_NONE;
        }
    }

    DictionaryEntry entry = calloc(1, sizeof(struct DictionaryEntry));

    if (!entry)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    if (!instance->buckets[hash].firstEntry)
    {
        DictionaryBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    memcpy(entry->key, key, KEY_SIZE);

    entry->value = newValue;
    *p = entry;

    return EXCEPTION_NONE;
}

void dictionary_clear(Dictionary instance)
{
    for (DictionaryBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        DictionaryEntry entry = bucket->firstEntry;

        while (entry)
        {
            DictionaryEntry nextEntry = entry->nextEntry;

            free(entry);

            entry = nextEntry;
        }

        bucket->firstEntry = NULL;
    }

    instance->firstBucket = NULL;
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

static void roll_lo(Matrix matrix)
{
    for (int i = matrix->rows - 1; i >= 0; i--)
    {
        for (int j = 0; j < matrix->columns; j++)
        {
            if (matrix_get(matrix, i, j) != 'O')
            {
                continue;
            }

            for (int k = i + 1;
                k < matrix->rows && matrix_get(matrix, k, j) == '.';
                k++)
            {
                matrix_set(matrix, k, j, 'O');
                matrix_set(matrix, k - 1, j, '.');
            }
        }
    }
}

static void roll_left(Matrix matrix)
{
    for (int i = 0; i < matrix->rows; i++)
    {
        for (int j = 0; j < matrix->columns; j++)
        {
            if (matrix_get(matrix, i, j) != 'O')
            {
                continue;
            }

            for (int k = j - 1; k >= 0 && matrix_get(matrix, i, k) == '.'; k--)
            {
                matrix_set(matrix, i, k, 'O');
                matrix_set(matrix, i, k + 1, '.');
            }
        }
    }
}

static void roll_right(Matrix matrix)
{
    for (int i = matrix->rows - 1; i >= 0; i--)
    {
        for (int j = matrix->columns - 1; j >= 0; j--)
        {
            if (matrix_get(matrix, i, j) != 'O')
            {
                continue;
            }

            for (int k = j + 1;
                k < matrix->columns && matrix_get(matrix, i, k) == '.';
                k++)
            {
                matrix_set(matrix, i, k, 'O');
                matrix_set(matrix, i, k - 1, '.');
            }
        }
    }
}

static void roll(Matrix matrix)
{
    roll_hi(matrix);
    roll_left(matrix);
    roll_lo(matrix);
    roll_right(matrix);
}

static Exception roll_many(Matrix matrix, Dictionary cache)
{
    long i = 0;

    while (i < ITERATIONS)
    {
        long previous = -1;
        char key[KEY_SIZE];

        roll(matrix);
        matrix_to_char_array(matrix, key);

        Exception exception = dictionary_replace(cache, key, &previous, i);

        if (exception)
        {
            return exception;
        }

        if (previous != -1)
        {
            i = ITERATIONS - ((ITERATIONS - previous) % (i - previous)) + 1;

            break;
        }

        i++;
    }

    while (i < ITERATIONS)
    {
        roll(matrix);

        i++;
    }

    return EXCEPTION_NONE;
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
    struct Dictionary cache;

    matrix(&a, n);

    do
    {
        matrix_add_row(&a, buffer);
    }
    while (fgets(buffer, n + 2, stdin));

    switch (roll_many(&a, &cache))
    {
        case EXCEPTION_OUT_OF_MEMORY:
            fprintf(stderr, "Error: Out of memory.\n");
            return 1;

        default: break;
    }

    long total = scan(&a);

    printf("14b %ld %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    dictionary_clear(&cache);

    return 0;
}
