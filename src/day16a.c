// Licensed under the MIT License.

// The Floor Will Be Lava Part 1

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define COORDINATE_STACK_CAPACITY 128
#define DIMENSION 128
#define EXCEPTION_FORMAT "Error: Format.\n"

enum Direction
{
    DIRECTION_HI,
    DIRECTION_LO,
    DIRECTION_LEFT,
    DIRECTION_RIGHT,
    DIRECTION_NONE
};

struct Coordinate
{
    int i;
    int j;
    enum Direction direction;
};

struct CoordinateStack
{
    int count;
    struct Coordinate items[COORDINATE_STACK_CAPACITY];
};

struct Table
{
    int rows;
    int columns;
    char matrix[DIMENSION * DIMENSION];
    bool set[DIMENSION * DIMENSION][DIRECTION_NONE];
};

typedef enum Direction Direction;
typedef struct Coordinate* Coordinate;
typedef struct CoordinateStack* CoordinateStack;
typedef struct Table* Table;

void coordinate_stack(CoordinateStack instance)
{
    instance->count = 0;
}

void coordinate_stack_push(CoordinateStack instance, Coordinate item)
{
    instance->items[instance->count] = *item;
    instance->count++;
}

bool coordinate_stack_try_pop(CoordinateStack instance, Coordinate result)
{
    if (!instance->count)
    {
        return false;
    }

    instance->count--;
    *result = instance->items[instance->count];

    return true;
}

void table(Table instance, int columns)
{
    instance->rows = 0;
    instance->columns = columns;
}

char table_matrix_get(Table instance, Coordinate coordinate)
{
    int index = (instance->columns * coordinate->i) + coordinate->j;

    return instance->matrix[index];
}

void table_matrix_add_row(Table instance, char values[])
{
    int m = instance->rows;

    instance->rows = m + 1;

    memcpy(
        instance->matrix + (instance->columns * m),
        values,
        instance->columns);

    for (Direction direction = 0; direction < DIRECTION_NONE; direction++)
    {
        memset(
            instance->set[m] + direction,
            false,
            instance->columns * sizeof(bool));
    }
}

bool table_set_add(Table instance, Coordinate coordinate)
{
    int index = (coordinate->i * instance->columns) + coordinate->j;

    if (instance->set[index][coordinate->direction])
    {
        return false;
    }

    instance->set[index][coordinate->direction] = true;

    return true;
}

static int scan(Table table, Coordinate current)
{
    Direction hiRight[DIRECTION_NONE];
    Direction loRight[DIRECTION_NONE];
    struct CoordinateStack stack;

    table_set_add(table, current);
    coordinate_stack(&stack);
    coordinate_stack_push(&stack, current);

    hiRight[DIRECTION_HI] = DIRECTION_RIGHT;
    hiRight[DIRECTION_LO] = DIRECTION_LEFT;
    hiRight[DIRECTION_LEFT] = DIRECTION_LO;
    hiRight[DIRECTION_RIGHT] = DIRECTION_HI;
    loRight[DIRECTION_HI] = DIRECTION_LEFT;
    loRight[DIRECTION_LO] = DIRECTION_RIGHT;
    loRight[DIRECTION_LEFT] = DIRECTION_HI;
    loRight[DIRECTION_RIGHT] = DIRECTION_LO;

    while (coordinate_stack_try_pop(&stack, current))
    {
        switch (table_matrix_get(table, current))
        {
            case '/':
                current->direction = hiRight[current->direction];
                break;

            case '\\':
                current->direction = loRight[current->direction];
                break;

            case '-':
            {
                if (current->direction == DIRECTION_LEFT ||
                    current->direction == DIRECTION_RIGHT)
                {
                    break;
                }

                current->direction = DIRECTION_RIGHT;

                struct Coordinate next =
                {
                    .i = current->i,
                    .j = current->j,
                    .direction = DIRECTION_LEFT
                };

                coordinate_stack_push(&stack, &next);
            }
            break;

            case '|':
            {
                if (current->direction == DIRECTION_HI ||
                    current->direction == DIRECTION_LO)
                {
                    break;
                }

                current->direction = DIRECTION_HI;

                struct Coordinate next =
                {
                    .i = current->i,
                    .j = current->j,
                    .direction = DIRECTION_LO
                };

                coordinate_stack_push(&stack, &next);
            }
            break;
        }

        switch (current->direction)
        {
            case DIRECTION_HI:
                current->i--;
                break;

            case DIRECTION_LO:
                current->i++;
                break;

            case DIRECTION_LEFT:
                current->j--;
                break;

            case DIRECTION_RIGHT:
                current->j++;
                break;

            default: break;
        }

        if (current->i < 0 || current->i >= table->rows ||
            current->j < 0 || current->j >= table->columns ||
            !table_set_add(table, current))
        {
            continue;
        }

        coordinate_stack_push(&stack, current);
    }

    int result = 0;

    for (int k = 0; k < table->rows * table->columns; k++)
    {
        for (Direction direction = 0; direction < DIRECTION_NONE; direction++)
        {
            if (table->set[k][direction])
            {
                result++;

                break;
            }
        }
    }

    return result;
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

    struct Table data;
    struct Coordinate current =
    {
        .direction = DIRECTION_RIGHT
    };

    table(&data, n);

    do
    {
        table_matrix_add_row(&data, buffer);
    }
    while (fgets(buffer, n + 2, stdin));

    int total = scan(&data, &current);

    printf("16a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
