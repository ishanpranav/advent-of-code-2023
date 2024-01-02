// Licensed under the MIT License.

// Point of Incidence Part 2

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#define DIMENSION 32

typedef unsigned long BitVector;
typedef unsigned long* BitMatrix;

void bit_matrix_clear(BitMatrix instance)
{
    memset(instance, 0, DIMENSION * sizeof(unsigned long));
}

static bool mid(BitMatrix matrix, int index)
{
    int left = index;
    int right = index + 1;

    while (left >= 0 && matrix[right])
    {
        if (matrix[left] != matrix[right])
        {
            return false;
        }

        left--;
        right++;
    }

    return true;
}

static int realize(BitMatrix matrix)
{
    for (int i = 0; matrix[i + 1]; i++)
    {
        if (mid(matrix, i))
        {
            return i + 1;
        }
    }

    return 0;
}

static int realize_xy(BitMatrix x, BitMatrix y)
{
    int result = realize(y);

    if (result)
    {
        return 100 * result;
    }

    return realize(x);
}

int main(void)
{
    int i = 0;
    int total = 0;
    BitVector x[DIMENSION] = { 0 };
    BitVector y[DIMENSION] = { 0 };
    char buffer[DIMENSION + 2];
    clock_t start = clock();

    while (fgets(buffer, sizeof buffer, stdin))
    {
        if (buffer[0] == '\n')
        {
            i = 0;
            total += realize_xy(x, y);

            bit_matrix_clear(x);
            bit_matrix_clear(y);
            continue;
        }

        int j = 0;

        while (buffer[j])
        {
            switch (buffer[j])
            {
                case '#':
                    y[i] = (y[i] << 1) | 1;
                    x[j] = (x[j] << 1) | 1;
                    break;

                case '.':
                    y[i] <<= 1;
                    x[j] <<= 1;
                    break;
            }

            j++;
        }

        i++;
    }

    total += realize_xy(x, y);

    printf("13a %d %lf\n", total, (double)(clock() - start) / CLOCKS_PER_SEC);

    return 0;
}
