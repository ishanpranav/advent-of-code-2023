// Licensed under the MIT License.

// Never Tell Me The Odds Part 2

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define BODY3_COLLECTION_CAPACITY 512

struct Body3
{
    long long x;
    long long y;
    long long z;
    int dx;
    int dy;
    int dz;
};

struct Body3Collection
{
    struct Body3 items[BODY3_COLLECTION_CAPACITY];
    int count;
};

typedef struct Body3* Body3;
typedef struct Body3Collection* Body3Collection;

void body3_collection(Body3Collection instance)
{
    instance->count = 0;
}

void body3_collection_add(Body3Collection instance, Body3 item)
{
    instance->items[instance->count] = *item;
    instance->count++;
}

bool math_gaussian_elimination(long double matrix[6][7], long double x[6])
{
    for (int k = 0; k < 6; k++)
    {
        int maxI = k;
        long double maxV = matrix[maxI][k];

        for (int i = k + 1; i < 6; i++)
        {
            if (fabsl(matrix[i][k]) > maxV)
            {
                maxV = matrix[i][k];
                maxI = i;
            }
        }

        if (!matrix[k][maxI])
        {
            return false;
        }

        if (maxI != k)
        {
            long double swap[7];

            memcpy(swap, matrix[k], sizeof swap);
            memcpy(matrix[k], matrix[maxI], sizeof swap);
            memcpy(matrix[maxI], swap, sizeof swap);
        }

        for (int i = k + 1; i < 6; i++)
        {
            long double r = matrix[i][k] / matrix[k][k];

            for (int j = k + 1; j < 7; j++)
            {
                matrix[i][j] -= r * matrix[k][j];
            }

            matrix[i][k] = 0;
        }
    }

    for (int i = 5; i >= 0; i--)
    {
        x[i] = matrix[i][6];

        for (int j = i + 1; j < 6; j++)
        {
            x[i] -= matrix[i][j] * x[j];
        }

        x[i] = x[i] / matrix[i][i];
    }
    
    return true;
}

static long long scan(Body3Collection bodies)
{
    for (int i = 0; i < bodies->count - 2; i++)
    {
        struct Body3 a = bodies->items[i];
        struct Body3 b = bodies->items[i + 1];
        struct Body3 c = bodies->items[i + 2];
        long double matrix[6][7] =
        {
            {
                a.dy - b.dy, -(a.dx - b.dx), 0, -(a.y - b.y), a.x - b.x, 0,
                (a.x * a.dy - a.y * a.dx) - (b.x * b.dy - b.y * b.dx)
            },
            {
                -(a.dz - b.dz), 0, a.dx - b.dx, a.z - b.z, 0, -(a.x - b.x),
                (a.z * a.dx - a.x * a.dz) - (b.z * b.dx - b.x * b.dz)
            },
            {
                0, a.dz - b.dz, -(a.dy - b.dy), 0, -(a.z - b.z), a.y - b.y,
                (a.y * a.dz - a.z * a.dy) - (b.y * b.dz - b.z * b.dy)
            },
            {
                b.dy - c.dy, -(b.dx - c.dx), 0, -(b.y - c.y), b.x - c.x, 0,
                (b.x * b.dy - b.y * b.dx) - (c.x * c.dy - c.y * c.dx)
            },
            {
                -(b.dz - c.dz), 0, b.dx - c.dx, b.z - c.z, 0, -(b.x - c.x),
                (b.z * b.dx - b.x * b.dz) - (c.z * c.dx - c.x * c.dz)
            },
            {
                0, b.dz - c.dz, -(b.dy - c.dy), 0, -(b.z - c.z), b.y - c.y,
                (b.y * b.dz - b.z * b.dy) - (c.y * c.dz - c.z * c.dy)
            }
        };
        long double x[6];

        if (math_gaussian_elimination(matrix, x))
        {
            return x[0] + x[1] + x[2];
        }
    }

    return -1;
}

int main(void)
{
    struct Body3 body;
    struct Body3Collection bodies;
    clock_t start = clock();

    body3_collection(&bodies);

    while (scanf("%lld, %lld, %lld @ %d, %d, %d\n",
        &body.x,
        &body.y,
        &body.z,
        &body.dx,
        &body.dy,
        &body.dz) > 0)
    {
        body3_collection_add(&bodies, &body);
    }

    long long sum = scan(&bodies);

    printf("24b %lld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);
}
