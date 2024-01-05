// Licensed under the MIT License.

// Never Tell Me The Odds Part 1

#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#define BODY_3_COLLECTION_CAPACITY 512

struct Body3
{
    long long x;
    long long y;
    long long z;
    long long dx;
    long long dy;
    long long dz;
};

struct Body3Collection
{
    struct Body3 items[BODY_3_COLLECTION_CAPACITY];
    int count;
};

struct Point2
{
    double x;
    double y;
};

struct Line2
{
    struct Point2 p;
    struct Point2 q;
};

typedef struct Body3* Body3;
typedef struct Body3Collection* Body3Collection;
typedef struct Rational* Rational;
typedef struct Point2* Point2;
typedef struct Line2* Line2;

void body_3_collection(Body3Collection instance)
{
    instance->count = 0;
}

void body_3_collection_add(Body3Collection instance, Body3 item)
{
    instance->items[instance->count] = *item;
    instance->count++;
}

bool line_2_point_intersection(Line2 l, Line2 m, Point2 result)
{
    double x1 = l->p.x;
    double x2 = l->q.x;
    double x3 = m->p.x;
    double x4 = m->q.x;
    double y1 = l->p.y;
    double y2 = l->q.y;
    double y3 = m->p.y;
    double y4 = m->q.y;
    double r = x1 * y2 - y1 * x2;
    double s = x3 * y4 - y3 * x4;
    double ldx = x1 - x2;
    double ldy = y1 - y2;
    double mdx = x3 - x4;
    double mdy = y3 - y4;
    double x = r * mdx - s * ldx;
    double y = r * mdy - s * ldy;
    double b = ldx * mdy - mdx * ldy;

    if (b == 0)
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

    body_3_collection(&bodies);

    while (scanf("%lld, %lld, %lld @ %lld, %lld, %lld\n",
        &body.x,
        &body.y,
        &body.z,
        &body.dx,
        &body.dy,
        &body.dz) > 0)
    {
        body_3_collection_add(&bodies, &body);
    }

    double min = 200000000000000;
    double max = 400000000000001;
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

            if (line_2_point_intersection(&l, &m, &p) &&
                p.x >= min && p.x < max && p.y >= min && p.y < max &&
                ((p.x > l.p.x && ldx > 0) || (p.x < l.p.x && ldx < 0)) &&
                ((p.x > m.p.x && mdx > 0) || (p.x < m.p.x && mdx < 0)))
            {
                total++;
            }
        }
    }

    printf("24a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);
}
