// Licensed under the MIT License.

// Sand Slabs Part 1

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BRICK_BUFFER_CAPACITY 2048
#define BRICK_CHILDREN_CAPACITY 10
#define BRICK_PARENTS_CAPACITY 10
#define BUFFER_SIZE 32
#define DELIMITERS ",~"

struct Point
{
    int x;
    int y;
    int z;
};

struct Brick;

struct BrickCollection
{
    struct Brick** items;
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
typedef struct Brick* Brick;
typedef struct BrickCollection* BrickCollection;

int math_min(int a, int b)
{
    if (a < b)
    {
        return a;
    }

    return b;
}

void brick_collection(BrickCollection, Brick items[]);

bool brick(Brick instance, Point p, Point q)
{
    Brick* parents = malloc(BRICK_PARENTS_CAPACITY * sizeof * parents);

    if (!parents)
    {
        return false;
    }

    Brick* children = malloc(BRICK_CHILDREN_CAPACITY * sizeof * children);

    if (!children)
    {
        free(parents);

        return false;
    }

    instance->p = *p;
    instance->q = *q;
    instance->floor = math_min(p->z, q->z);

    brick_collection(&instance->parents, parents);
    brick_collection(&instance->children, children);

    return true;
}

void finalize_brick(Brick instance)
{
    free(instance->parents.items);
    free(instance->children.items);
}

void brick_collection(BrickCollection instance, Brick items[])
{
    instance->count = 0;
    instance->items = items;
}

void brick_collection_add(BrickCollection instance, Brick item)
{
    instance->items[instance->count] = item;
    instance->count++;
}

static int brick_collection_compare_items(Object left, Object right)
{
    const struct Brick* leftBrick = *(const struct Brick**)left;
    const struct Brick* rightBrick = *(const struct Brick**)right;

    return leftBrick->p.z - rightBrick->p.z;
}

void brick_collection_sort(BrickCollection instance)
{
    qsort(
        instance->items,
        instance->count,
        sizeof * instance->items,
        brick_collection_compare_items);
}

void brick_collection_clear(BrickCollection instance)
{
    instance->count = 0;
}

static bool scan(BrickCollection instance)
{
    for (Brick* p = instance->items; p < instance->items + instance->count; p++)
    {
        if ((*p)->parents.count < 2)
        {
            return false;
        }
    }

    return true;
}

int main(void)
{
    struct Point p;
    struct Point q;
    struct Brick* brickBuffer[BRICK_BUFFER_CAPACITY];
    struct Brick* supportedBuffer[BRICK_BUFFER_CAPACITY];
    struct BrickCollection bricks;
    struct BrickCollection supported;
    int floor = INT_MAX;
    clock_t start = clock();

    brick_collection(&bricks, brickBuffer);
    brick_collection(&supported, supportedBuffer);

    while (scanf("%d,%d,%d~%d,%d,%d\n", &p.x, &p.y, &p.z, &q.x, &q.y, &q.z) > 0)
    {
        Brick current = malloc(sizeof * current);

        if (!brick(current, &p, &q))
        {
            fprintf(stderr, "Error: Out of memory.\n");

            return 1;
        }

        brick_collection_add(&bricks, current);

        if (current->floor < floor)
        {
            floor = current->floor;
        }
    }

    brick_collection_sort(&bricks);

    for (Brick* p = bricks.items; p < bricks.items + bricks.count; p++)
    {
        if ((*p)->floor == floor)
        {
            brick_collection_add(&supported, *p);

            continue;
        }

        brick_collection_clear(&(*p)->parents);

        (*p)->floor = 1;

        for (Brick* q = supported.items;
            q < supported.items + supported.count;
            q++)
        {
            if ((*q)->p.x <= (*p)->q.x && (*p)->p.x <= (*q)->q.x &&
                (*q)->p.y <= (*p)->q.y && (*p)->p.y <= (*q)->q.y)
            {
                int z = (*q)->floor + abs((*q)->p.z - (*q)->q.z) + 1;

                if (z > (*p)->floor)
                {
                    brick_collection_clear(&(*p)->parents);

                    (*p)->floor = z;
                }

                if (z == (*p)->floor)
                {
                    brick_collection_add(&(*p)->parents, *q);
                }
            }
        }

        brick_collection_add(&supported, *p);

        for (Brick* parent = (*p)->parents.items;
            parent < (*p)->parents.items + (*p)->parents.count;
            parent++)
        {
            brick_collection_add(&(*parent)->children, *p);
        }
    }

    int total = 0;

    for (Brick* q = supported.items; q < supported.items + supported.count; q++)
    {
        if (scan(&(*q)->children))
        {
            total++;
        }
    }

    printf("22a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    for (Brick* p = bricks.items; p < bricks.items + bricks.count; p++)
    {
        finalize_brick(*p);
        free(*p);
    }

    return 0;
}
