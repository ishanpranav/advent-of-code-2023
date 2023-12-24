// Licensed under the MIT License.

// Clumsy Crucible Part 1

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DIMENSION 101
#define STATE_PRIORITY_QUEUE_ARITY 2
#define STATE_PRIORITY_QUEUE_CAPACITY 10000
#define STATE_SET_BUCKETS 10000

enum Exception
{
    EXCEPTION_NONE = 0,
    EXCEPTION_INVALID_OPERATION,
    EXCEPTION_OUT_OF_MEMORY
};

struct Direction
{
    int di;
    int dj;
};

struct Matrix
{
    int rows;
    int columns;
    char items[(DIMENSION - 1) * (DIMENSION - 1)];
};

struct State
{
    struct Direction direction;
    int i;
    int j;
    int length;
};

struct StatePriorityQueueElement
{
    struct State value;
    int priority;
};

struct StatePriorityQueue
{
    struct StatePriorityQueueElement items[STATE_PRIORITY_QUEUE_CAPACITY];
    int count;
};

struct StateSetElement
{
    struct StateSetElement* nextElement;
    struct State value;
};

struct StateSetBucket
{
    struct StateSetElement* firstElement;
    struct StateSetBucket* nextBucket;
};

struct StateSet
{
    struct StateSetBucket* firstBucket;
    struct StateSetBucket buckets[STATE_SET_BUCKETS];
};

typedef char* Row;
typedef enum Exception Exception;
typedef struct Direction* Direction;
typedef struct Matrix* Matrix;
typedef struct State* State;
typedef struct StatePriorityQueueElement* StatePriorityQueueElement;
typedef struct StatePriorityQueue* StatePriorityQueue;
typedef struct StateSetElement* StateSetElement;
typedef struct StateSetBucket* StateSetBucket;
typedef struct StateSet* StateSet;

bool direction_equals(Direction instance, Direction other)
{
    return instance->di == other->di && instance->dj == other->dj;
}

void direction_get_left(Direction instance, Direction result)
{
    switch (instance->di)
    {
        case -1:
            result->di = 0;
            result->dj = -1;
            break;

        case 1:
            result->di = 0;
            result->dj = 1;
            break;

        case 0:
            switch (instance->dj)
            {
                case -1:
                    result->di = 1;
                    result->dj = 0;
                    break;

                case 1:
                    result->di = -1;
                    result->dj = 0;
                    break;
            }
            break;
    }
}

void direction_get_right(Direction instance, Direction result)
{
    switch (instance->di)
    {
        case -1:
            result->di = 0;
            result->dj = 1;
            break;

        case 1:
            result->di = 0;
            result->dj = -1;
            break;

        case 0:
            switch (instance->dj)
            {
                case -1:
                    result->di = -1;
                    result->dj = 0;
                    break;

                case 1:
                    result->di = 1;
                    result->dj = 0;
                    break;
            }
            break;
    }
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

void state_priority_queue(StatePriorityQueue instance)
{
    instance->count = 0;
}

void state_priority_queue_move_up(
    StatePriorityQueue instance,
    State item,
    int priority,
    int index)
{
    while (index > 0)
    {
        int parentIndex = (index - 1) / STATE_PRIORITY_QUEUE_ARITY;

        StatePriorityQueueElement parent = instance->items + parentIndex;

        if (priority < parent->priority)
        {
            instance->items[index] = *parent;
            index = parentIndex;
        }
        else
        {
            break;
        }
    }

    instance->items[index].value = *item;
    instance->items[index].priority = priority;
}

void state_priority_queue_move_down(
    StatePriorityQueue instance,
    State item,
    int priority,
    int index)
{
    int i;

    while ((i = ((index * STATE_PRIORITY_QUEUE_ARITY) + 1)) < instance->count)
    {
        StatePriorityQueueElement minChild = instance->items + i;
        int minChildIndex = i;
        int childIndexUpperBound;
        
        if(i + STATE_PRIORITY_QUEUE_ARITY < instance->count)
        {
            childIndexUpperBound = i + STATE_PRIORITY_QUEUE_ARITY;
        }
        else
        {
            childIndexUpperBound = instance->count;
        }
        
        while (++i < childIndexUpperBound)
        {
            StatePriorityQueueElement nextChild = instance->items + i;

            if (nextChild->priority < minChild->priority)
            {
                minChild = nextChild;
                minChildIndex = i;
            }
        }

        if (priority <= minChild->priority)
        {
            break;
        }

        instance->items[index] = *minChild;
        index = minChildIndex;
    }

    instance->items[index].value = *item;
    instance->items[index].priority = priority; 
}

void state_priority_queue_enqueue(
    StatePriorityQueue instance,
    State item,
    int priority)
{
    int count = instance->count;

    instance->count = count + 1;

    state_priority_queue_move_up(instance, item, priority, count);
}

bool state_priority_queue_try_dequeue(
    StatePriorityQueue instance,
    State result,
    int* priority)
{
    if (!instance->count)
    {
        return false;
    }

    *result = instance->items[0].value;
    *priority = instance->items[0].priority;

    int lastIndex = --instance->count;

    if (lastIndex > 0)
    {
        state_priority_queue_move_down(
            instance, 
            &instance->items[lastIndex].value,
            instance->items[lastIndex].priority,
            0);
    }

    return true;
}

Exception state_set_add(StateSet instance, State item)
{
    StateSetElement* p;
    unsigned int hash = 0 % STATE_SET_BUCKETS;

    for (p = &instance->buckets[hash].firstElement; *p; p = &(*p)->nextElement)
    {
        if ((*p)->value.i == item->i &&
            (*p)->value.j == item->j &&
            (*p)->value.length == item->length &&
            direction_equals(&(*p)->value.direction, &item->direction))
        {
            return EXCEPTION_INVALID_OPERATION;
        }
    }

    StateSetElement element = calloc(1, sizeof(struct StateSetElement));

    if (!element)
    {
        return EXCEPTION_OUT_OF_MEMORY;
    }

    if (!instance->buckets[hash].firstElement)
    {
        StateSetBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    element->value = *item;
    *p = element;

    return EXCEPTION_NONE;
}

void state_set_clear(StateSet instance)
{
    for (StateSetBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        StateSetElement element = bucket->firstElement;

        while (element)
        {
            StateSetElement nextElement = element->nextElement;

            free(element);

            element = nextElement;
        }

        bucket->firstElement = NULL;
    }

    instance->firstBucket = NULL;
}

static void next(
    State state,
    int priority,
    Matrix matrix,
    StatePriorityQueue queue,
    Direction direction)
{
    struct State next;

    next.i = state->i + direction->di;
    next.j = state->j + direction->dj;

    if (next.i < 0 || next.i >= matrix->rows ||
        next.j < 0 || next.j >= matrix->columns)
    {
        return;
    }

    if (direction_equals(&state->direction, direction))
    {
        next.length = state->length + 1;
    }
    else
    {
        next.length = 0;
    }

    state_priority_queue_enqueue(
        queue,
        &next,
        priority + matrix_get(matrix, next.i, next.j));
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

    if (n < 3)
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    struct Matrix a;

    matrix(&a, n);

    do
    {
        matrix_add_row(&a, buffer);
    }
    while (fgets(buffer, n + 2, stdin));

    if (a.rows < 2)
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    struct StateSet visited;
    struct StatePriorityQueue queue;
    struct State current;
    struct State first =
    {
        .i = 0,
        .j = 1
    };
    struct State second =
    {
        .i = 1,
        .j = 0
    };
    int min = 0;
    int priority;

    state_priority_queue(&queue);
    state_priority_queue_enqueue(&queue, &first, matrix_get(&a, 0, 1));
    state_priority_queue_enqueue(&queue, &second, matrix_get(&a, 1, 0));

    while (state_priority_queue_try_dequeue(&queue, &current, &priority))
    {
        switch (state_set_add(&visited, &current))
        {
            case EXCEPTION_INVALID_OPERATION: continue;

            case EXCEPTION_OUT_OF_MEMORY:
                fprintf(stderr, "Error: Out of memory.\n");
                return 1;

            default: break;
        }

        if (current.i == a.rows - 1 && current.j == a.columns - 1)
        {
            break;
        }

        if (current.length < 2)
        {
            next(&current, priority, &a, &queue, &current.direction);
        }

        struct Direction left;
        struct Direction right;

        direction_get_left(&current.direction, &left);
        direction_get_right(&current.direction, &right);
        next(&current, priority, &a, &queue, &left);
        next(&current, priority, &a, &queue, &right);
    }

    printf("17a %d %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);
    state_set_clear(&visited);

    return 0;
}
