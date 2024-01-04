#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define BUFFER_SIZE 32
#define DELIMITERS ",~"
#define LINE_3_COLLECTION_CAPACITY 8
#define MAX_X 10
#define MAX_Y 10
#define MAX_Z 10

struct Point3
{
    int x;
    int y;
    int z;
};

struct Line3
{
    struct Point3 p;
    struct Point3 q;
};

struct Line3Collection
{
    struct Line3 items[LINE_3_COLLECTION_CAPACITY];
    int count;
};

struct Space3
{
    int sizeX;
    int sizeY;
    int sizeZ;
    int items[(MAX_Z + 1) * (MAX_Y + 1) * (MAX_X + 1)];
};

typedef const void* Object;
typedef struct Point3* Point3;
typedef struct Line3* Line3;
typedef struct Line3Collection* Line3Collection;
typedef struct Space3* Space3;

int math_max(int a, int b)
{
    if (a > b)
    {
        return a;
    }

    return b;
}

int line_3_compare(Object left, Object right)
{
    const struct Line3* l = (const struct Line3*)left;
    const struct Line3* m = (const struct Line3*)right;

    return l->p.z - m->p.z;
}

void line_3_collection(Line3Collection instance)
{
    instance->count = 0;
}

void line_3_collection_add(Line3Collection instance, Line3 item)
{
    instance->items[instance->count] = *item;
    instance->count++;
}

void line_3_collection_sort(Line3Collection instance)
{
    qsort(
        instance->items,
        instance->count,
        sizeof(struct Line3),
        line_3_compare);
}

void space_3(Space3 space, int sizeX, int sizeY)
{
    space->sizeX = sizeX;
    space->sizeY = sizeY;
    space->sizeZ = 1;
}

int main(void)
{
    int maxX = 0;
    int maxY = 0;
    clock_t start = clock();
    struct Line3Collection lines;

    line_3_collection(&lines);

    for (;;)
    {
        struct Line3 l;
        int scan = scanf(
            "%d,%d,%d~%d,%d,%d\n",
            &l.p.x,
            &l.p.y,
            &l.p.z,
            &l.q.x,
            &l.q.y,
            &l.q.z);

        if (scan == EOF)
        {
            break;
        }

        if (scan < 6)
        {
            fprintf(stderr, "Error: Format.\n");

            return 1;
        }

        int x = math_max(l.p.x, l.q.x);

        if (x > maxX)
        {
            maxX = x;
        }

        int y = math_max(l.p.y, l.q.y);

        if (y > maxY)
        {
            maxY = y;
        }

        line_3_collection_add(&lines, &l);
    }

    struct Space3 space = { 0 };

    space_3(&space);
    
    space.sizeX = maxX + 1;
    space.sizeY = maxY + 1;
    space.sizeZ = 1;

    for (int y = 0; y < space.sizeY; y++)
    {
        int yOffset = y * space.sizeX;

        for (int x = 0; x < space.sizeX; x++)
        {
            space.items[yOffset + x] = -1;
        }
    }

    line_3_collection_sort(&lines);

    for (Line3 l = lines.items; l < lines.items + lines.count; l++)
    {
        int supportZ = -1;

        printf("rest = set()\n");

        struct Point3 min;
        struct Point3 max;

        for (int z = 0; z < space.sizeZ; z++)
        {
            int zOffset = z * space.sizeX * space.sizeY;

            for (int y = 0; y < space.sizeY; y++)
            {
                int yOffset = y * space.sizeX;

                for (int x = 0; x < space.sizeX; x++)
                {
                    if ()
                }
            }
        }
    }

    printf("22a %d %lf\n", 0, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
