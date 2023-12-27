// Licensed under the MIT License.

// Clumsy Crucible Part 2

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define COORDINATE_PRIORITY_QUEUE_CAPACITY 65536
#define DIMENSION 256
#define STEP_MIN 4
#define STEP_MAX 11

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
    enum Direction obstacle;
};

struct CoordinatePriorityQueueElement
{
    struct CoordinatePriorityQueueElement* next;
    struct Coordinate value;
};

struct CoordinatePriorityQueue
{
    struct Coordinate items[COORDINATE_PRIORITY_QUEUE_CAPACITY];
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
typedef struct CoordinatePriorityQueue* CoordinatePriorityQueue;
typedef struct CoordinatePriorityQueueElement* CoordinatePriorityQueueElement;
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

void coordinate_priority_queue(CoordinatePriorityQueue instance)
{
    instance->first = -1;
    instance->last = -1;
}

void coordinate_priority_queue_enqueue(
    CoordinatePriorityQueue instance,
    Coordinate value,
    int priority)
{
    if (instance->first == -1)
    {
        instance->first = 0;
        instance->last = 0;
    }
    else if (
        instance->first > 0 &&
        instance->last == COORDINATE_PRIORITY_QUEUE_CAPACITY - 1)
    {
        instance->last = 0;
    }
    else
    {
        instance->last++;
    }

    instance->items[instance->last] = *value;
}

bool coordinate_priority_queue_try_dequeue(
    CoordinatePriorityQueue instance,
    Coordinate result)
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
    else if (instance->first == COORDINATE_PRIORITY_QUEUE_CAPACITY - 1)
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
    instance->hi = INT_MAX / 2;
    instance->lo = INT_MAX / 2;
    instance->left = INT_MAX / 2;
    instance->right = INT_MAX / 2;
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
    CoordinatePriorityQueue priorityQueue)
{
    struct Coordinate coordinate;
    State initialState = state_matrix_get(matrix, current->i, current->j);
    int priority = math_min(initialState->left, initialState->right);

    coordinate.j = current->j;
    coordinate.obstacle = DIRECTION_VERTICAL;

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

        coordinate_priority_queue_enqueue(priorityQueue, &coordinate, priority);
    }
}

static void scan_lo(
    StateMatrix matrix,
    Coordinate current,
    CoordinatePriorityQueue priorityQueue)
{
    struct Coordinate coordinate;
    State initialState = state_matrix_get(matrix, current->i, current->j);
    int priority = math_min(initialState->left, initialState->right);

    coordinate.j = current->j;
    coordinate.obstacle = DIRECTION_VERTICAL;

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

        coordinate_priority_queue_enqueue(priorityQueue, &coordinate, priority);
    }
}

static void scan_left(
    StateMatrix matrix,
    Coordinate current,
    CoordinatePriorityQueue priorityQueue)
{
    struct Coordinate coordinate;
    State initialState = state_matrix_get(matrix, current->i, current->j);
    int priority = math_min(initialState->hi, initialState->lo);

    coordinate.i = current->i;
    coordinate.obstacle = DIRECTION_HORIZONTAL;

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

        coordinate_priority_queue_enqueue(priorityQueue, &coordinate, priority);
    }
}

static void scan_right(
    StateMatrix matrix,
    Coordinate current,
    CoordinatePriorityQueue priorityQueue)
{
    State initialState = state_matrix_get(matrix, current->i, current->j);
    int priority = math_min(initialState->hi, initialState->lo);

    struct Coordinate coordinate;

    coordinate.i = current->i;
    coordinate.obstacle = DIRECTION_HORIZONTAL;

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

        coordinate_priority_queue_enqueue(priorityQueue, &coordinate, priority);
    }
}

int main()
{
    char buffer[DIMENSION + 1];
    StateMatrix matrix = malloc(sizeof(struct StateMatrix));
    CoordinatePriorityQueue priorityQueue =
        malloc(sizeof(struct CoordinatePriorityQueue));
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

    state_matrix(matrix, n);

    do
    {
        matrix->rows++;

        for (int j = 0; j < n; j++)
        {
            if (!buffer[j])
            {
                fprintf(stderr, "Error: Format.\n");

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

    coordinate_priority_queue(priorityQueue);
    coordinate_priority_queue_enqueue(priorityQueue, &current, 0);

    while (coordinate_priority_queue_try_dequeue(priorityQueue, &current))
    {
        if (current.obstacle != DIRECTION_VERTICAL)
        {
            scan_hi(matrix, &current, priorityQueue);
            scan_lo(matrix, &current, priorityQueue);
        }

        if (current.obstacle != DIRECTION_HORIZONTAL)
        {
            scan_left(matrix, &current, priorityQueue);
            scan_right(matrix, &current, priorityQueue);
        }
    }

    State finalState = state_matrix_get(
        matrix,
        matrix->rows - 1,
        matrix->columns - 1);
    int min = finalState->hi;

    if (finalState->lo < min) 
    {
        min = finalState->lo;
    }

    if (finalState->left < min) 
    {
        min = finalState->left;
    }

    if (finalState->right < min)
    {
        min = finalState->right;
    }
    
    printf("17b %d %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);
    free(matrix);
    free(priorityQueue);
}
