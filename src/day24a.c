// Licensed under the MIT License.

// Never Tell Me The Odds Part 1

#include <stdbool.h>
#include <stdio.h>
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

struct Point2
{
    long double x;
    long double y;
};

struct Line2
{
    struct Point2 p;
    struct Point2 q;
};

typedef struct Body3* Body3;
typedef struct Body3Collection* Body3Collection;
typedef struct Point2* Point2;
typedef struct Line2* Line2;

void body3_collection(Body3Collection instance)
{
    instance->count = 0;
}

void body3_collection_add(Body3Collection instance, Body3 item)
{
    instance->items[instance->count] = *item;
    instance->count++;
}

bool line2_point_intersection(Line2 l, Line2 m, Point2 result)
{
    long double ldx = l->p.x - l->q.x;
    long double ldy = l->p.y - l->q.y;
    long double mdx = m->p.x - m->q.x;
    long double mdy = m->p.y - m->q.y;
    long double r = l->p.x * l->q.y - l->p.y * l->q.x;
    long double s = m->p.x * m->q.y - m->p.y * m->q.x;
    long double x = r * mdx - s * ldx;
    long double y = r * mdy - s * ldy;
    long double b = ldx * mdy - mdx * ldy;

    if (!b)
    {
        return false;
    }

    result->x = x / b;
    result->y = y / b;

    return true;
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

    int total = 0;

    for (int i = 0; i < bodies.count - 1; i++)
    {
        for (int j = i + 1; j < bodies.count; j++)
        {
            struct Line2 l;
            struct Line2 m;
            struct Point2 p;
            int ldx = bodies.items[i].dx;
            int mdx = bodies.items[j].dx;

            l.p.x = bodies.items[i].x;
            l.p.y = bodies.items[i].y;
            l.q.x = l.p.x + ldx;
            l.q.y = l.p.y + bodies.items[i].dy;
            m.p.x = bodies.items[j].x;
            m.p.y = bodies.items[j].y;
            m.q.x = m.p.x + mdx;
            m.q.y = m.p.y + bodies.items[j].dy;

            if (line2_point_intersection(&l, &m, &p) &&
                p.x >= 2e14 && p.x <= 4e14 && p.y >= 2e14 && p.y <= 4e14 &&
                ((p.x > l.p.x && ldx > 0) || (p.x < l.p.x && ldx < 0)) &&
                ((p.x > m.p.x && mdx > 0) || (p.x < m.p.x && mdx < 0)))
            {
                total++;
            }
        }
    }

    printf("24a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
}
