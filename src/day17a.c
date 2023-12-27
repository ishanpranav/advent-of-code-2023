// Licensed under the MIT License.

// Clumsy Crucible Part 1

#include <limits.h>
#include <stdio.h>
#include <time.h>
#define DIMENSION 101

struct Coordinate
{
    int i;
    int j;
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
    int rows;
    int columns;
    struct State items[(DIMENSION - 1) * (DIMENSION - 1)];
};

typedef struct Coordinate* Coordinate;
typedef struct State* State;
typedef struct State* StateArray;
typedef struct StateMatrix* StateMatrix;

void state(State current, int priority)
{
    current->priority = priority;
    current->hi = INT_MAX;
    current->lo = INT_MAX;
    current->left = INT_MAX;
    current->right = INT_MAX;
}

void state_matrix(StateMatrix instance, int columns)
{
    instance->rows = 0;
    instance->columns = columns;
}

State state_matrix_get(StateMatrix instance, int i, int j)
{
    return instance->items + (instance->columns * i) + j;
}

void state_matrix_set(StateMatrix instance, int i, int j, State value)
{
    instance->items[(instance->columns * i) + j] = *value;
}

void state_matrix_add_row(StateMatrix instance)
{
    instance->rows++;
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

    struct StateMatrix matrix;

    state_matrix(&matrix, n);

    do
    {
        state_matrix_add_row(&matrix);

        for (int j = 0; j < n; j++)
        {
            struct State current;

            state(&current, state_matrix_get(&matrix, matrix.rows - 1, j));
            state_matrix_set(&matrix, matrix.rows - 1, j, &current);
        }
    }
    while (fgets(buffer, n + 2, stdin));

    State initial = state_matrix_get(&matrix, 0, 0);


    initial->hi = 0;
    initial->lo = 0;
    initial->left = 0;
    initial->right = 0;



    int min = 0;

    printf("17a %d %lf\n", min, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
