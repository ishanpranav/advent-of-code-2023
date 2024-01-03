// Licensed under the MIT License.

// Clumsy Crucible Part 1

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define COORDINATE_QUEUE_CAPACITY 32768
#define DIMENSION 256
#define EXCEPTION_FORMAT "Error: Format.\n"
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define STEP_MIN 1
#define STEP_MAX 4

enum
{
    LOCAL_MAX = INT_MAX / 2
};

enum Direction
{
    DIRECTION_NONE = 0,
    DIRECTION_HORIZONTAL,
    DIRECTION_VERTICAL
};

struct Coordinate
{
    int i;
    int j;
    enum Direction direction;
};

struct CoordinateQueue
{
    struct Coordinate items[COORDINATE_QUEUE_CAPACITY];
    int first;
    int last;
};

struct State
{
    int priority;
    int hi;
    int lo;
    int left;
    int right;
};

struct StateMatrix
{
    struct State items[DIMENSION * DIMENSION];
    int rows;
    int columns;
};

typedef enum Direction Direction;
typedef struct Coordinate* Coordinate;
typedef struct CoordinateQueue* CoordinateQueue;
typedef struct State* State;
typedef struct StateMatrix* StateMatrix;

int math_min(int a, int b)
{
    if (a < b)
    {
        return a;
    }

    return b;
}

void coordinate_queue(CoordinateQueue instance)
{
    instance->first = -1;
    instance->last = -1;
}

void coordinate_queue_enqueue(CoordinateQueue instance, Coordinate item)
{
    if (instance->first == -1)
    {
        instance->first = 0;
        instance->last = 0;
    }
    else if (instance->first && instance->last == COORDINATE_QUEUE_CAPACITY - 1)
    {
        instance->last = 0;
    }
    else
    {
        instance->last++;
    }

    instance->items[instance->last] = *item;
}

bool coordinate_queue_try_dequeue(CoordinateQueue instance, Coordinate result)
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
    else if (instance->first == COORDINATE_QUEUE_CAPACITY - 1)
    {
        instance->first = 0;
    }
    else
    {
        instance->first++;
    }

    return true;
}

void state(State instance, int priority)
{
    instance->priority = priority;
    instance->hi = LOCAL_MAX;
    instance->lo = LOCAL_MAX;
    instance->left = LOCAL_MAX;
    instance->right = LOCAL_MAX;
}

void state_matrix(StateMatrix instance, int columns)
{
    instance->rows = 0;
    instance->columns = columns;
}

State state_matrix_get(StateMatrix instance, int i, int j)
{
    return instance->items + (i * instance->columns) + j;
}

static void scan_hi(
    StateMatrix matrix,
    Coordinate current,
    int priority,
    CoordinateQueue queue)
{
    struct Coordinate coordinate;

    coordinate.j = current->j;
    coordinate.direction = DIRECTION_VERTICAL;

    for (int k = 1; k < STEP_MAX; k++)
    {
        coordinate.i = current->i - k;

        if (coordinate.i < 0)
        {
            return;
        }

        State state = state_matrix_get(matrix, coordinate.i, coordinate.j);

        priority += state->priority;

        if (k < STEP_MIN || state->lo <= priority)
        {
            continue;
        }

        state->lo = priority;

        coordinate_queue_enqueue(queue, &coordinate);
    }
}

static void scan_lo(
    StateMatrix matrix,
    Coordinate current,
    int priority,
    CoordinateQueue queue)
{
    struct Coordinate coordinate;

    coordinate.j = current->j;
    coordinate.direction = DIRECTION_VERTICAL;

    for (int k = 1; k < STEP_MAX; k++)
    {
        coordinate.i = current->i + k;

        if (coordinate.i >= matrix->rows)
        {
            return;
        }

        State state = state_matrix_get(matrix, coordinate.i, coordinate.j);

        priority += state->priority;

        if (k < STEP_MIN || state->hi <= priority)
        {
            continue;
        }

        state->hi = priority;

        coordinate_queue_enqueue(queue, &coordinate);
    }
}

static void scan_left(
    StateMatrix matrix,
    Coordinate current,
    int priority,
    CoordinateQueue queue)
{
    struct Coordinate coordinate;

    coordinate.i = current->i;
    coordinate.direction = DIRECTION_HORIZONTAL;

    for (int k = 1; k < STEP_MAX; k++)
    {
        coordinate.j = current->j - k;

        if (coordinate.j < 0)
        {
            return;
        }

        State state = state_matrix_get(matrix, coordinate.i, coordinate.j);

        priority += state->priority;

        if (k < STEP_MIN || state->right <= priority)
        {
            continue;
        }

        state->right = priority;

        coordinate_queue_enqueue(queue, &coordinate);
    }
}

static void scan_right(
    StateMatrix matrix,
    Coordinate current,
    int priority,
    CoordinateQueue queue)
{
    struct Coordinate coordinate;

    coordinate.i = current->i;
    coordinate.direction = DIRECTION_HORIZONTAL;

    for (int k = 1; k < STEP_MAX; k++)
    {
        coordinate.j = current->j + k;

        if (coordinate.j >= matrix->columns)
        {
            return;
        }

        State state = state_matrix_get(matrix, coordinate.i, coordinate.j);

        priority += state->priority;

        if (k < STEP_MIN || state->left <= priority)
        {
            continue;
        }

        state->left = priority;

        coordinate_queue_enqueue(queue, &coordinate);
    }
}

int main(void)
{
    StateMatrix matrix = malloc(sizeof * matrix);

    if (!matrix)
    {
        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

        return 1;
    }

    CoordinateQueue queue = malloc(sizeof * queue);

    if (!queue)
    {
        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

        return 1;
    }

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

    state_matrix(matrix, n);

    do
    {
        matrix->rows++;

        for (int j = 0; j < n; j++)
        {
            if (!buffer[j])
            {
                fprintf(stderr, EXCEPTION_FORMAT);

                return 1;
            }

            State newState = state_matrix_get(matrix, matrix->rows - 1, j);

            state(newState, buffer[j] - '0');
        }
    }
    while (fgets(buffer, n + 2, stdin));

    State initialState = state_matrix_get(matrix, 0, 0);

    initialState->hi = 0;
    initialState->lo = 0;
    initialState->left = 0;
    initialState->right = 0;

    struct Coordinate current = { 0 };

    coordinate_queue(queue);
    coordinate_queue_enqueue(queue, &current);

    while (coordinate_queue_try_dequeue(queue, &current))
    {
        if (current.direction != DIRECTION_VERTICAL)
        {
            State currentState = state_matrix_get(matrix, current.i, current.j);
            int priority = math_min(currentState->left, currentState->right);

            scan_hi(matrix, &current, priority, queue);
            scan_lo(matrix, &current, priority, queue);
        }

        if (current.direction != DIRECTION_HORIZONTAL)
        {
            State currentState = state_matrix_get(matrix, current.i, current.j);
            int priority = math_min(currentState->hi, currentState->lo);

            scan_left(matrix, &current, priority, queue);
            scan_right(matrix, &current, priority, queue);
        }
    }

    State finalState = state_matrix_get(
        matrix,
        matrix->rows - 1,
        matrix->columns - 1);
    int min = math_min(finalState->hi, finalState->lo);

    min = math_min(min, finalState->left);
    min = math_min(min, finalState->right);

    printf("17a %d %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);
    free(matrix);
    free(queue);
}
