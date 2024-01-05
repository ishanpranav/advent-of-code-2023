#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BRICK_CHILDREN_CAPACITY 4
#define BRICK_PARENTS_CAPACITY 4
#define BRICKS_CAPACITY 2048
#define BUFFER_SIZE 32
#define DELIMITERS ",~"
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"

struct Point
{
    int x;
    int y;
    int z;
};

struct Brick;

struct BrickCollection
{
    struct Brick* items;
    int count;
};

struct Brick
{
    struct BrickCollection parents;
    struct BrickCollection children;
    struct Point p;
    struct Point q;
    int floor;
};

typedef const void* Object;
typedef struct Point* Point;
typedef struct BrickCollection* BrickCollection;
typedef struct Brick* Brick;

int math_min(int a, int b)
{
    if (a < b)
    {
        return a;
    }

    return b;
}

bool brick_collection(BrickCollection instance, int capacity);
void finalize_brick_collection(BrickCollection instance);

bool brick(Brick instance, Point p, Point q)
{
    if (!brick_collection(&instance->parents, BRICK_PARENTS_CAPACITY))
    {
        return false;
    }

    if (!brick_collection(&instance->children, BRICK_CHILDREN_CAPACITY))
    {
        finalize_brick_collection(&instance->parents);

        return false;
    }

    instance->p = *p;
    instance->q = *q;
    instance->floor = math_min(p->z, q->z);

    return true;
}

int brick_compare(Object left, Object right)
{
    const struct Brick* l = (const struct Brick*)left;
    const struct Brick* m = (const struct Brick*)right;

    return l->p.z - m->p.z;
}

bool brick_collection(BrickCollection instance, int capacity)
{
    instance->count = 0;
    instance->items = malloc(capacity * sizeof * instance->items);

    return instance->items;
}

void finalize_brick_collection(BrickCollection instance)
{
    free(instance->items);
}

Brick brick_collection_new_brick(BrickCollection instance)
{
    Brick result = instance->items + instance->count;

    instance->count++;

    return result;
}

void brick_collection_sort(BrickCollection instance)
{
    qsort(
        instance->items,
        instance->count,
        sizeof * instance->items,
        brick_compare);
}

int main(void)
{
    struct BrickCollection bricks;
    clock_t start = clock();

    if (!brick_collection(&bricks, BRICKS_CAPACITY))
    {
        fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

        return 1;
    }

    int floor = INT_MAX;

    for (;;)
    {
        struct Point p;
        struct Point q;
        int scan = scanf(
            "%d,%d,%d~%d,%d,%d\n",
            &p.x,
            &p.y,
            &p.z,
            &q.x,
            &q.y,
            &q.z);

        if (scan == EOF)
        {
            break;
        }

        if (scan < 6)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        Brick current = brick_collection_new_brick(&bricks);

        if (!brick(current, &p, &q))
        {
            fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

            return 1;
        }

        if (current->floor < floor)
        {
            floor = current->floor;
        }
    }

    brick_collection_sort(&bricks);
    printf("22a %d %lf\n", 0, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
