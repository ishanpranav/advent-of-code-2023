// Licensed under the MIT License.

// Step Counter Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define COORDINATE_SET_CAPACITY 8192
#define DIMENSION 256
#define EXCEPTION_FORMAT "Error: Format.\n"
#define STATE_QUEUE_CAPACITY 2048

struct Coordinate
{
    int i;
    int j;
};

struct CoordinateSet
{
    struct Coordinate items[COORDINATE_SET_CAPACITY];
    int count;
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

typedef struct Coordinate* Coordinate;
typedef struct CoordinateSet* CoordinateSet;
typedef struct Matrix* Matrix;
typedef struct State* State;
typedef struct StateQueue* StateQueue;

void coordinate_empty(Coordinate result)
{
    result->i = -1;
    result->j = -1;
}

bool coordinate_is_empty(Coordinate instance)
{
    return instance->i < 0 || instance->j < 0;
}

void coordinate_set(CoordinateSet instance)
{
    instance->count = 0;
}

bool coordinate_set_try_add(CoordinateSet instance, Coordinate item)
{
    for (Coordinate other = instance->items;
        other < instance->items + instance->count;
        other++)
    {
        if (item->i == other->i && item->j == other->j)
        {
            return false;
        }
    }

    instance->items[instance->count] = *item;
    instance->count++;

    return true;
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

static long scan(Matrix matrix, long steps)
{
    long total = 0;
    struct CoordinateSet visited;
    struct StateQueue queue;
    struct State current;

    coordinate_set(&visited);
    state_queue(&queue);
    state(state_queue_enqueue(&queue), &matrix->origin);

    while (state_queue_try_dequeue(&queue, &current))
    {
        if (!coordinate_set_try_add(&visited, &current.coordinate))
        {
            continue;
        }

        if (current.priority % 2 == steps % 2)
        {
            total++;
        }

        if (current.priority > steps)
        {
            continue;
        }

        scan_hi(matrix, &current, &queue);
        scan_lo(matrix, &current, &queue);
        scan_left(matrix, &current, &queue);
        scan_right(matrix, &current, &queue);
    }

    return total;
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

    if (coordinate_is_empty(&a.origin))
    {
        fprintf(stderr, EXCEPTION_FORMAT);

        return 1;
    }

    long total = scan(&a, 64);

    printf("21a %ld %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
}
