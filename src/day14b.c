// Licensed under the MIT License.

// Parabolic Reflector Dish Part 2

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUCKETS 197
#define DIMENSION 101
#define ITERATIONS 1000000000

enum
{
    KEY_SIZE = (DIMENSION - 1) * (DIMENSION - 1)
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
    struct DictionaryBucket buckets[BUCKETS];
};

struct Matrix
{
    int rows;
    int columns;
    char items[KEY_SIZE];
};

typedef char* Row;
typedef struct DictionaryEntry* DictionaryEntry;
typedef struct DictionaryBucket* DictionaryBucket;
typedef struct Dictionary* Dictionary;
typedef struct Matrix* Matrix;

unsigned int char_array_get_hash_code(char instance[])
{
    unsigned int result = 7;

    for (int i = 0; i < KEY_SIZE; i++)
    {
        result = (result * 31) + instance[i];
    }

    return result;
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

void matrix_to_char_array(Matrix instance, char value[])
{
    memcpy(value, instance->items, instance->rows * instance->columns);
}

bool dictionary_set(Dictionary instance, char key[], long value)
{
    DictionaryEntry* p;
    int hash = char_array_get_hash_code(key) % BUCKETS;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (memcmp(key, (*p)->key, KEY_SIZE) == 0)
        {
            (*p)->value = value;

            return true;
        }
    }

    DictionaryEntry entry = calloc(1, sizeof(struct DictionaryEntry));

    if (!entry)
    {
        return false;
    }

    if (!instance->buckets[hash].firstEntry)
    {
        DictionaryBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    memcpy(entry->key, key, KEY_SIZE);

    entry->value = value;
    *p = entry;

    return true;
}

bool dictionary_try_get_value(Dictionary instance, char key[], long* result)
{
    int hash = char_array_get_hash_code(key) % BUCKETS;

    for (DictionaryEntry entry = instance->buckets[hash].firstEntry;
        entry;
        entry = entry->nextEntry)
    {
        if (memcmp(key, entry->key, KEY_SIZE) == 0)
        {
            *result = entry->value;

            return true;
        }
    }

    return false;
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
    Dictionary cache = calloc(1, sizeof(struct Dictionary));

    matrix(&a, n);

    do
    {
        matrix_add_row(&a, buffer);
    }
    while (fgets(buffer, n + 2, stdin));

    for (long i = 0; i < ITERATIONS; i++)
    {
        long value;
        char key[KEY_SIZE];

        roll_hi(&a);
        roll_left(&a);
        roll_lo(&a);
        roll_right(&a);
        matrix_to_char_array(&a, key);

        if (dictionary_try_get_value(cache, key, &value))
        {
            i = ITERATIONS - ((ITERATIONS - value) % (i - value));
        }
        else
        {
            dictionary_set(cache, key, i);
        }
    }

    long total = scan(&a);

    printf("14b %ld %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    dictionary_clear(cache);

    return 0;
}
