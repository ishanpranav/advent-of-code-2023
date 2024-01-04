// Licensed under the MIT License.

// Step Counter Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define COORDINATE_SET_BUCKETS 24593
#define DIMENSION 256
#define EXCEPTION_FORMAT "Error: Format.\n"
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define STATE_QUEUE_CAPACITY 2048

enum AddResult
{
    ADD_RESULT_ADDED,
    ADD_RESULT_NOT_ADDED,
    ADD_RESULT_OUT_OF_MEMORY
};

struct Coordinate
{
    int i;
    int j;
};

struct CoordinateSetEntry
{
    struct CoordinateSetEntry* nextEntry;
    struct Coordinate item;
};

struct CoordinateSetBucket
{
    struct CoordinateSetEntry* firstEntry;
    struct CoordinateSetBucket* nextBucket;
};

struct CoordinateSet
{
    struct CoordinateSetBucket* firstBucket;
    struct CoordinateSetBucket buckets[COORDINATE_SET_BUCKETS];
};

struct Matrix
{
    struct Coordinate origin;
    int rows;
    int columns;
    char items[DIMENSION * DIMENSION];
};

struct State
{
    struct Coordinate coordinate;
    int priority;
};

struct StateQueue
{
    struct State items[STATE_QUEUE_CAPACITY];
    int first;
    int last;
};

typedef enum AddResult AddResult;
typedef struct Coordinate* Coordinate;
typedef struct CoordinateSetEntry* CoordinateSetEntry;
typedef struct CoordinateSetBucket* CoordinateSetBucket;
typedef struct CoordinateSet* CoordinateSet;
typedef struct Matrix* Matrix;
typedef struct State* State;
typedef struct StateQueue* StateQueue;

void coordinate_empty(Coordinate result)
{
    result->i = -1;
    result->j = -1;
}

void coordinate_set(CoordinateSet instance)
{
    instance->firstBucket = NULL;

    for (int i = 0; i < COORDINATE_SET_BUCKETS; i++)
    {
        instance->buckets[i].firstEntry = NULL;
        instance->buckets[i].nextBucket = NULL;
    }
}

AddResult coordinate_set_add(CoordinateSet instance, Coordinate item)
{
    CoordinateSetEntry* p;
    unsigned int hash = (item->i * DIMENSION + item->j) %
        COORDINATE_SET_BUCKETS;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if (item->i == (*p)->item.i && item->j == (*p)->item.j)
        {
            return ADD_RESULT_NOT_ADDED;
        }
    }

    CoordinateSetEntry entry = malloc(sizeof * entry);

    if (!entry)
    {
        return ADD_RESULT_OUT_OF_MEMORY;
    }

    if (!instance->buckets[hash].firstEntry)
    {
        CoordinateSetBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    entry->item = *item;
    entry->nextEntry = NULL;
    *p = entry;

    return ADD_RESULT_ADDED;
}

void coordinate_set_clear(CoordinateSet instance)
{
    for (CoordinateSetBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        CoordinateSetEntry entry = bucket->firstEntry;

        while (entry)
        {
            CoordinateSetEntry nextEntry = entry->nextEntry;

            free(entry);

            entry = nextEntry;
        }

        bucket->firstEntry = NULL;
    }

    instance->firstBucket = NULL;
}

void matrix(Matrix instance, int n)
{
    coordinate_empty(&instance->origin);

    instance->rows = 0;
    instance->columns = n;
}

char matrix_get(Matrix instance, int i, int j)
{
    return instance->items[(instance->columns * i) + j];
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

void state(State instance, Coordinate coordinate)
{
    instance->coordinate = *coordinate;
    instance->priority = 0;
}

void state_queue(StateQueue instance)
{
    instance->first = -1;
    instance->last = -1;
}

State state_queue_enqueue(StateQueue instance)
{
    if (instance->first == -1)
    {
        instance->first = 0;
        instance->last = 0;
    }
    else if (instance->first && instance->last == STATE_QUEUE_CAPACITY - 1)
    {
        instance->last = 0;
    }
    else
    {
        instance->last++;
    }

    return instance->items + instance->last;
}

bool state_queue_try_dequeue(StateQueue instance, State result)
{
    if (instance->first == -1)
    {
        return false;
    }

    *result = instance->items[instance->first];

    if (instance->first == instance->last)
    {
        instance->first = -1;
        instance->last = -1;
    }
    else if (instance->first == STATE_QUEUE_CAPACITY - 1)
    {
        instance->first = 0;
    }
    else
    {
        instance->first++;
    }

    return true;
}

static void scan_hi(Matrix matrix, State current, StateQueue queue)
{
    int i = current->coordinate.i - 1;

    if (i < 0 || matrix_get(matrix, i, current->coordinate.j) == '#')
    {
        return;
    }

    State state = state_queue_enqueue(queue);

    state->coordinate.i = i;
    state->coordinate.j = current->coordinate.j;
    state->priority = current->priority + 1;
}

static void scan_lo(Matrix matrix, State current, StateQueue queue)
{
    int i = current->coordinate.i + 1;

    if (i >= matrix->rows || matrix_get(matrix, i, current->coordinate.j) == '#')
    {
        return;
    }

    State state = state_queue_enqueue(queue);

    state->coordinate.i = i;
    state->coordinate.j = current->coordinate.j;
    state->priority = current->priority + 1;
}

static void scan_left(Matrix matrix, State current, StateQueue queue)
{
    int j = current->coordinate.j - 1;

    if (j < 0 || matrix_get(matrix, current->coordinate.i, j) == '#')
    {
        return;
    }

    State state = state_queue_enqueue(queue);

    state->coordinate.i = current->coordinate.i;
    state->coordinate.j = j;
    state->priority = current->priority + 1;
}

static void scan_right(Matrix matrix, State current, StateQueue queue)
{
    int j = current->coordinate.j + 1;

    if (j >= matrix->columns ||
        matrix_get(matrix, current->coordinate.i, j) == '#')
    {
        return;
    }

    State state = state_queue_enqueue(queue);

    state->coordinate.i = current->coordinate.i;
    state->coordinate.j = j;
    state->priority = current->priority + 1;
}

int main(void)
{
    char buffer[DIMENSION + 2];
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

        char* token = strchr(buffer, 'S');

        if (token)
        {
            a.origin.i = a.rows - 1;
            a.origin.j = token - buffer;
        }
    }
    while (fgets(buffer, n + 2, stdin));

    if (a.origin.i < 0 || a.origin.j < 0)
    {
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    CoordinateSet visited = malloc(sizeof * visited);

    if (!visited)
    {
        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

        return 1;
    }

    int total = 0;
    struct StateQueue queue;
    struct State current;

    coordinate_set(visited);
    state_queue(&queue);
    state(state_queue_enqueue(&queue), &a.origin);

    while (state_queue_try_dequeue(&queue, &current))
    {
        switch (coordinate_set_add(visited, &current.coordinate))
        {
            case ADD_RESULT_ADDED: break;
            case ADD_RESULT_NOT_ADDED: continue;

            case ADD_RESULT_OUT_OF_MEMORY:
                fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                return 1;
        }

        if (current.priority % 2 == 0)
        {
            total++;
        }

        if (current.priority > 64)
        {
            continue;
        }

        scan_hi(&a, &current, &queue);
        scan_lo(&a, &current, &queue);
        scan_left(&a, &current, &queue);
        scan_right(&a, &current, &queue);
    }

    printf("21a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
    coordinate_set_clear(visited);
    free(visited);
}
