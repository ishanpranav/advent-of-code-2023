// Licensed under the MIT License.

// Gear Ratios Part 1

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define DIMENSION 141

struct View
{
    char* a;
    char* b;
    int index;
    int offset;
};

typedef char* String;
typedef struct View* View;
typedef bool (*Validator)(struct View* view);

static bool check(char value)
{
    return value != '.' && value != '\n' && value;
}

static bool realize_x(View view)
{
    if (view->index < DIMENSION && check(view->a[view->index]))
    {
        return true;
    }

    if (view->offset - 1 >= 0 && check(view->a[view->offset - 1]))
    {
        return true;
    }

    return false;
}

static bool realize_y(View view)
{
    for (int i = view->offset - 1; i <= view->index; i++)
    {
        if (check(view->b[i]))
        {
            return true;
        }
    }

    return false;
}

static bool realize_xy(View view)
{
    return realize_x(view) || realize_y(view);
}

static void aggregate(String a, String b, long* sum, Validator validator)
{
    int number = 0;
    struct View view =
    {
        .a = a,
        .b = b,
        .offset = -1
    };

    while (view.index < DIMENSION && a[view.index])
    {
        if (isdigit(a[view.index]))
        {
            if (view.offset == -1)
            {
                view.offset = view.index;
            }

            number = (number * 10) + (a[view.index] - '0');
            view.index++;

            continue;
        }

        if (number && validator(&view))
        {
            *sum += number;
        }

        number = 0; 
        view.offset = -1;
        view.index++;
    }

    view.index = DIMENSION - 1;

    if (number && validator(&view))
    {
        *sum += number;
    }
}

int main(void)
{
    char hi[DIMENSION + 1] = { 0 };
    clock_t start = clock();

    if (!fgets(hi, sizeof hi, stdin))
    {
        fprintf(stderr, "Error: Format.\n");

        return 1;
    }

    long sum = 0;
    char lo[DIMENSION + 1] = { 0 };

    while (fgets(lo, sizeof lo, stdin))
    {
        aggregate(hi, lo, &sum, realize_xy);
        aggregate(lo, hi, &sum, realize_y);
        memcpy(hi, lo, sizeof hi);
    }

    aggregate(lo, hi, &sum, realize_x);
    printf("03a %ld %lf\n", sum, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
