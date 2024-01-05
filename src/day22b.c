// Licensed under the MIT License.

// Sand Slabs Part 2

#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define BRICK_BUFFER_CAPACITY 2048
#define BRICK_CHILDREN_CAPACITY 10
#define BRICK_PARENTS_CAPACITY 10
#define BRICK_SET_BUCKETS 3079
#define BUFFER_SIZE 32
#define DELIMITERS ",~"
#define EXCEPTION_OUT_OF_MEMORY "Error: Out of memory.\n"
#define SIZE_X 10
#define SIZE_Y 10

enum AddResult
{
    ADD_RESULT_ADDED,
    ADD_RESULT_NOT_ADDED,
    ADD_RESULT_OUT_OF_MEMORY
};

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

struct BrickSetEntry
{
    struct BrickSetEntry* nextEntry;
    struct Brick* item;
};

struct BrickSetBucket
{
    struct BrickSetEntry* firstEntry;
    struct BrickSetBucket* nextBucket;
};

struct BrickSet
{
    struct BrickSetBucket* firstBucket;
    struct BrickSetBucket buckets[BRICK_SET_BUCKETS];
    int count;
};

typedef const void* Object;
typedef enum AddResult AddResult;
typedef struct Point* Point;
typedef struct Brick* Brick;
typedef struct BrickCollection* BrickCollection;
typedef struct BrickSetEntry* BrickSetEntry;
typedef struct BrickSetBucket* BrickSetBucket;
typedef struct BrickSet* BrickSet;

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

unsigned int brick_get_hash_code(Brick instance)
{
    return ((unsigned int)instance->p.z * SIZE_X * SIZE_Y) +
        ((unsigned int)instance->p.y * SIZE_X) +
        (unsigned int)instance->p.x;
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

Brick brick_collection_remove_first(BrickCollection instance)
{
    if (!instance->count)
    {
        return NULL;
    }

    instance->count--;

    return instance->items[instance->count];
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

AddResult brick_set_add(BrickSet instance, Brick item)
{
    BrickSetEntry* p;
    unsigned int hash = brick_get_hash_code(item) % BRICK_SET_BUCKETS;

    for (p = &instance->buckets[hash].firstEntry; *p; p = &(*p)->nextEntry)
    {
        if ((*p)->item == item)
        {
            return ADD_RESULT_NOT_ADDED;
        }
    }

    BrickSetEntry entry = malloc(sizeof * entry);

    if (!entry)
    {
        return ADD_RESULT_OUT_OF_MEMORY;
    }

    if (!instance->buckets[hash].firstEntry)
    {
        BrickSetBucket first = instance->firstBucket;

        instance->buckets[hash].nextBucket = first;
        instance->firstBucket = instance->buckets + hash;
    }

    entry->item = item;
    entry->nextEntry = NULL;
    *p = entry;
    instance->count++;

    return ADD_RESULT_ADDED;
}

bool brick_set_contains(BrickSet instance, Brick item)
{
    unsigned int hash = brick_get_hash_code(item) % BRICK_SET_BUCKETS;

    for (BrickSetEntry entry = instance->buckets[hash].firstEntry;
        entry;
        entry = entry->nextEntry)
    {
        if (entry->item == item)
        {
            return true;
        }
    }

    return false;
}

bool brick_set_is_superset(BrickSet instance, BrickCollection other)
{
    if (instance->count < other->count)
    {
        return false;
    }

    for (Brick* p = other->items; p < other->items + other->count; p++)
    {
        if (!brick_set_contains(instance, *p))
        {
            return false;
        }
    }

    return true;
}

void brick_set_clear(BrickSet instance)
{
    for (BrickSetBucket bucket = instance->firstBucket;
        bucket;
        bucket = bucket->nextBucket)
    {
        BrickSetEntry entry = bucket->firstEntry;

        while (entry)
        {
            BrickSetEntry nextEntry = entry->nextEntry;

            free(entry);

            entry = nextEntry;
        }

        bucket->firstEntry = NULL;
    }

    instance->firstBucket = NULL;
    instance->count = 0;
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
            fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

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

    long total = 0;
    struct BrickSet visited = { 0 };
    struct BrickCollection stack;
    struct Brick* stackBuffer[BRICK_BUFFER_CAPACITY];

    brick_collection(&stack, stackBuffer);

    for (Brick* q = supported.items; q < supported.items + supported.count; q++)
    {
        brick_collection_clear(&stack);

        for (Brick* child = (*q)->children.items;
            child < (*q)->children.items + (*q)->children.count;
            child++)
        {
            if ((*child)->parents.count == 1)
            {
                brick_collection_add(&stack, *child);
            }
        }

        Brick current;

        while ((current = brick_collection_remove_first(&stack)))
        {
            switch (brick_set_add(&visited, current))
            {
                case ADD_RESULT_ADDED: break;
                case ADD_RESULT_NOT_ADDED: continue;

                case ADD_RESULT_OUT_OF_MEMORY:
                    fprintf(stderr, EXCEPTION_OUT_OF_MEMORY);

                    return 1;
            }

            for (Brick* child = current->children.items;
                child < current->children.items + current->children.count;
                child++)
            {
                if (brick_set_is_superset(&visited, &(*child)->parents))
                {
                    brick_collection_add(&stack, *child);
                }
            }
        }

        total += visited.count;

        brick_set_clear(&visited);
    }

    printf("22b %ld %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    for (Brick* p = bricks.items; p < bricks.items + bricks.count; p++)
    {
        finalize_brick(*p);
        free(*p);
    }

    return 0;
}
