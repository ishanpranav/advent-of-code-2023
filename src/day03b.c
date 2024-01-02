// Licensed under the MIT License.

// Gear Ratios Part 2

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define CASES 8
#define DIMENSION 141

struct Window
{
    char* hi;
    char* mid;
    char* lo;
    int index;
};

typedef char* String;
typedef int (*Case)(struct Window*);
typedef struct Window* Window;

static int scan_left(String value, int index)
{
    if (isdigit(value[index]))
    {
        return 0;
    }

    int number = 0;
    int place = 1;

    for (int j = index - 1; j >= 0; j--)
    {
        char current = value[j];

        if (!isdigit(current))
        {
            break;
        }

        number += (current - '0') * place;
        place *= 10;
    }

    return number;
}

static int scan_right(String value, int index)
{
    if (isdigit(value[index]))
    {
        return 0;
    }

    int number = 0;

    for (int j = index + 1; j < DIMENSION; j++)
    {
        if (!isdigit(value[j]))
        {
            break;
        }

        number = (number * 10) + (value[j] - '0');
    }

    return number;
}

static int scan_center(String value, int index)
{
    char current = value[index];

    if (!isdigit(current))
    {
        return 0;
    }

    bool hasLeft = index - 1 >= 0 && isdigit(value[index - 1]);
    bool hasRight = index + 1 < DIMENSION && isdigit(value[index + 1]);

    if (hasLeft && hasRight)
    {
        return scan_right(value, index - 2);
    }

    if (hasLeft)
    {
        return scan_left(value, index + 1);
    }

    if (hasRight)
    {
        return scan_right(value, index - 1);
    }

    return current - '0';
}

int window_hi_left(Window instance)
{
    return scan_left(instance->hi, instance->index);
}

int window_hi_center(Window instance)
{
    return scan_center(instance->hi, instance->index);
}

int window_hi_right(Window instance)
{
    return scan_right(instance->hi, instance->index);
}

int window_mid_left(Window instance)
{
    return scan_left(instance->mid, instance->index);
}

int window_mid_right(Window instance)
{
    return scan_right(instance->mid, instance->index);
}

int window_lo_left(Window instance)
{
    return scan_left(instance->lo, instance->index);
}

int window_lo_center(Window instance)
{
    return scan_center(instance->lo, instance->index);
}

int window_lo_right(Window instance)
{
    return scan_right(instance->lo, instance->index);
}

static void aggregate(String hi, String mid, String lo, Case cases[], long* sum)
{
    for (int i = 0; i < DIMENSION && mid[i]; i++)
    {
        if (mid[i] != '*')
        {
            continue;
        }

        int count = 0;
        int numbers[CASES] = { 0 };
        struct Window window =
        {
            .hi = hi,
            .mid = mid,
            .lo = lo,
            .index = i
        };

        for (int j = 0; j < CASES; j++)
        {
            int number = cases[j](&window);

            if (number)
            {
                count++;
            }

            if (count > 2)
            {
                break;
            }

            numbers[j] = number;
        }

        if (count != 2)
        {
            continue;
        }

        long product = 1;

        for (int j = 0; j < CASES; j++)
        {
            int number = numbers[j];

            if (number)
            {
                product *= number;
            }
        }

        *sum += product;
    }
}

int main(void)
{
    char hi[DIMENSION + 1] = { 0 };
    char mid[DIMENSION + 1] = { 0 };
    char lo[DIMENSION + 1] = { 0 };
    clock_t start = clock();

    if (!fgets(mid, sizeof mid, stdin) || !fgets(lo, sizeof lo, stdin))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    long sum = 0;
    Case cases[CASES] =
    {
        window_hi_left,
        window_hi_center,
        window_hi_right,
        window_mid_left,
        window_mid_right,
        window_lo_left,
        window_lo_center,
        window_lo_right
    };

    do
    {
        aggregate(hi, mid, lo, cases, &sum);
        memcpy(hi, mid, sizeof hi);
        memcpy(mid, lo, sizeof mid);
    }
    while (fgets(lo, sizeof lo, stdin));

    memset(lo, 0, sizeof lo);
    aggregate(hi, mid, lo, cases, &sum);
    printf("03b %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
